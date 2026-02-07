// IdleHandler.cpp
// PyAE - Python for After Effects
// アイドルハンドラの実装

#include "IdleHandler.h"
#include "PluginState.h"
#include "Logger.h"
#include "ErrorHandling.h"
#include "PySideLoader.h"
#include "PythonHost.h"
#include "SuiteManager.h"

#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace PyAE
{

    bool IdleHandler::Initialize()
    {
        if (m_initialized.load())
        {
            PYAE_LOG_WARNING("IdleHandler", "Already initialized");
            return true;
        }

        PYAE_LOG_INFO("IdleHandler", "Initializing IdleHandler...");

        m_initialized.store(true);

        PYAE_LOG_INFO("IdleHandler", "IdleHandler initialized");
        return true;
    }

    void IdleHandler::Shutdown()
    {
        if (!m_initialized.load())
        {
            return;
        }

        PYAE_LOG_INFO("IdleHandler", "Shutting down IdleHandler...");

        // タスクキューをシャットダウン
        m_taskQueue.Shutdown();

        // 残りのタスクをクリア
        m_taskQueue.Clear();

        m_initialized.store(false);

        PYAE_LOG_INFO("IdleHandler", "IdleHandler shutdown complete");
        PYAE_LOG_INFO("IdleHandler", "Total tasks processed: " + std::to_string(m_totalTasksProcessed.load()));
        PYAE_LOG_INFO("IdleHandler", "Total idle calls: " + std::to_string(m_totalIdleCalls.load()));
    }

    A_Err IdleHandler::OnIdle(AEGP_GlobalRefcon plugin_refconP,
                              AEGP_IdleRefcon refconP,
                              A_long *max_sleepPL)
    {
        // Debug: Log first few idle calls
        static int idleCallCount = 0;
        if (idleCallCount < 3) {
            PyAE::DebugOutput("IdleHandler::OnIdle called #" + std::to_string(idleCallCount));
        }
        idleCallCount++;

        if (!m_initialized.load())
        {
            return A_Err_NONE;
        }

        // シャットダウン中はスキップ
        if (PluginState::Instance().IsShuttingDown())
        {
            return A_Err_NONE;
        }

        // サスペンド中はスキップ
        if (m_suspended.load())
        {
            return A_Err_NONE;
        }

        m_totalIdleCalls.fetch_add(1);

        // 自動テストを処理
        if (idleCallCount <= 3) PyAE::DebugOutput("IdleHandler::OnIdle - ProcessAutoTest");
        ProcessAutoTest();

        // ペンディングタスクを処理
        if (idleCallCount <= 3) PyAE::DebugOutput("IdleHandler::OnIdle - ProcessPendingTasks");
        ProcessPendingTasks();
        if (idleCallCount <= 3) PyAE::DebugOutput("IdleHandler::OnIdle - ProcessPendingTasks done");

        // 次のアイドルまでの最大スリープ時間を設定
        if (max_sleepPL)
        {
            if (idleCallCount <= 3) PyAE::DebugOutput("IdleHandler::OnIdle - checking queue");
            // タスクがある場合は短いインターバル、なければ長いインターバル
            if (!m_taskQueue.Empty() || m_autoTestTask)
            {
                *max_sleepPL = static_cast<A_long>(m_idleInterval.count());
            }
            else
            {
                *max_sleepPL = 100; // 100ms
            }
            if (idleCallCount <= 3) PyAE::DebugOutput("IdleHandler::OnIdle - done");
        }

        return A_Err_NONE;
    }

    void IdleHandler::EnqueueTask(std::function<void()> task, TaskPriority priority,
                                   const std::string& description, const std::string& sourceLocation)
    {
        if (!m_initialized.load())
        {
            PYAE_LOG_WARNING("IdleHandler", "Cannot enqueue task: not initialized");
            return;
        }

        m_taskQueue.Push(std::move(task), priority, description, sourceLocation);
    }

    void IdleHandler::ProcessPendingTasks()
    {
        size_t tasksProcessed = 0;

        while (tasksProcessed < m_maxTasksPerIdle)
        {
            auto task = m_taskQueue.TryPop();
            if (!task)
            {
                break;
            }

            try
            {
                task->func();
                tasksProcessed++;
                m_totalTasksProcessed.fetch_add(1);
            }
            catch (const std::exception &e)
            {
                PYAE_LOG_ERROR("IdleHandler", std::string("Task threw exception: ") + e.what());
            }
            catch (...)
            {
                PYAE_LOG_ERROR("IdleHandler", "Task threw unknown exception");
            }
        }

        if (tasksProcessed > 0)
        {
            PYAE_LOG_DEBUG("IdleHandler", "Processed " + std::to_string(tasksProcessed) + " tasks");
        }

        // Process PySide6 Qt events if plugin is loaded
        if (PySideLoader::Instance().IsLoaded())
        {
            auto *plugin = PySideLoader::Instance().GetPlugin();
            if (plugin && plugin->IsInitialized())
            {
                plugin->ProcessEvents(10); // Process events for max 10ms
            }
        }
    }

    void IdleHandler::QueueAutoTest(
        const std::string &scriptPath,
        const std::string &moduleName,
        const std::string &functionName,
        const std::string &outputPath,
        bool autoExit,
        int timeout,
        int delay)
    {
        if (m_autoTestTask)
        {
            PYAE_LOG_WARNING("IdleHandler", "Auto test already queued");
            return;
        }

        m_autoTestTask = std::make_unique<AutoTestTask>();
        m_autoTestTask->scriptPath = scriptPath;
        m_autoTestTask->moduleName = moduleName;
        m_autoTestTask->functionName = functionName;
        m_autoTestTask->outputPath = outputPath;
        m_autoTestTask->autoExit = autoExit;
        m_autoTestTask->timeout = timeout;
        m_autoTestTask->delay = delay;
        m_autoTestTask->queuedTime = std::chrono::steady_clock::now();

        PYAE_LOG_INFO("IdleHandler", "Auto test queued: " + moduleName + "." + functionName);
        PYAE_LOG_INFO("IdleHandler", "  Script: " + scriptPath);
        PYAE_LOG_INFO("IdleHandler", "  Output: " + outputPath);
        PYAE_LOG_INFO("IdleHandler", "  Auto Exit: " + std::string(autoExit ? "Yes" : "No"));
        PYAE_LOG_INFO("IdleHandler", "  Timeout: " + std::to_string(timeout) + "s");
        PYAE_LOG_INFO("IdleHandler", "  Delay: " + std::to_string(delay) + "s");
    }

    void IdleHandler::ProcessAutoTest()
    {
        // 早期リターン: タスクがない、または既に実行済み
        if (!m_autoTestTask || m_autoTestTask->executed)
        {
            return;
        }

        // 遅延時間を確認
        auto elapsed = std::chrono::steady_clock::now() - m_autoTestTask->queuedTime;
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();

        if (elapsedSeconds < m_autoTestTask->delay)
        {
            // まだ待機時間中
            return;
        }

        // タイムアウトチェック
        if (m_autoTestTask->timeout > 0 && elapsedSeconds > m_autoTestTask->timeout)
        {
            PYAE_LOG_ERROR("IdleHandler", "Auto test timed out after " + std::to_string(elapsedSeconds) + "s");
            HandleTestCompletion(false);
            return;
        }

        // テストを実行
        m_autoTestTask->executed = true;
        ExecuteAutoTest();
    }

    void IdleHandler::ExecuteAutoTest()
    {
        if (!m_autoTestTask)
        {
            return;
        }

        PYAE_LOG_INFO("IdleHandler", "Executing auto test: " + m_autoTestTask->moduleName + "." + m_autoTestTask->functionName);

        bool success = false;
        std::string errorMsg;

        try
        {
            // Python ホストが初期化されているか確認
            auto &pythonHost = PythonHost::Instance();
            if (!pythonHost.IsInitialized())
            {
                errorMsg = "Python host not initialized";
                PYAE_LOG_ERROR("IdleHandler", errorMsg);
                HandleTestCompletion(false);
                return;
            }

            // 状態を取得
            auto &state = PluginState::Instance();

            // Python コードを構築
            std::string pythonCode;

            // tests ディレクトリを sys.path に追加
            std::filesystem::path testsDir = state.GetPluginDir() / "tests";
            pythonCode = "import sys\n";
            pythonCode += "import os\n";
            pythonCode += "sys.path.insert(0, r'" + testsDir.string() + "')\n";

            // Python経由で新規プロジェクトを作成（これで確実にプロジェクトが利用可能になる）
            // Import ae module with detailed error handling
            pythonCode += "try:\n";
            pythonCode += "    import ae\n";
            pythonCode += "    print('ae module imported successfully')\n";
            pythonCode += "except Exception as e:\n";
            pythonCode += "    import traceback\n";
            pythonCode += "    print(f'Failed to import ae: {e}')\n";
            pythonCode += "    print(traceback.format_exc())\n";
            pythonCode += "    raise\n";
            pythonCode += "print('Creating new project...')\n";
            pythonCode += "ae.new_project()\n";
            pythonCode += "proj = ae.Project.get_current()\n";
            pythonCode += "print(f'New project created: {proj.name}')\n";

            if (!m_autoTestTask->scriptPath.empty())
            {
                // スクリプトファイルから実行
                pythonCode += "sys.path.insert(0, r'" + std::filesystem::path(m_autoTestTask->scriptPath).parent_path().string() + "')\n";
                pythonCode += "import " + m_autoTestTask->moduleName + "\n";
                pythonCode += "result = " + m_autoTestTask->moduleName + "." + m_autoTestTask->functionName + "()\n";
            }
            else
            {
                // モジュールから直接実行
                pythonCode += "import " + m_autoTestTask->moduleName + "\n";

                // Check if function needs arguments from environment variable (safe version)
                pythonCode += "test_target = os.getenv('PYAE_TEST_TARGET', '')\n";
                pythonCode += "if test_target:\n";
                pythonCode += "    result = " + m_autoTestTask->moduleName + "." + m_autoTestTask->functionName + "(test_target)\n";
                pythonCode += "else:\n";
                pythonCode += "    result = " + m_autoTestTask->moduleName + "." + m_autoTestTask->functionName + "()\n";
            }

            // 結果を出力パスに書き込む
            pythonCode += "import json\n";
            pythonCode += "with open(r'" + m_autoTestTask->outputPath + "', 'w') as f:\n";
            pythonCode += "    json.dump({'success': bool(result), 'result': result}, f)\n";

            PYAE_LOG_INFO("IdleHandler", "Executing Python code:\n" + pythonCode);

            // GILスコープを最小限に - Python実行部分のみ
            bool executeSuccess;
            {
                ScopedGIL gil;
                executeSuccess = pythonHost.ExecuteString(pythonCode, errorMsg);
            }

            if (!executeSuccess)
            {
                PYAE_LOG_ERROR("IdleHandler", "Python execution failed: " + errorMsg);
                success = false;
            }
            else
            {
                PYAE_LOG_INFO("IdleHandler", "Python execution completed successfully");
                success = true;
            }
        }
        catch (const std::exception &e)
        {
            errorMsg = std::string("Exception during test execution: ") + e.what();
            PYAE_LOG_ERROR("IdleHandler", errorMsg);
            success = false;
        }
        catch (...)
        {
            errorMsg = "Unknown exception during test execution";
            PYAE_LOG_ERROR("IdleHandler", errorMsg);
            success = false;
        }

        // エラーがあれば出力ファイルに書き込む
        if (!success && !m_autoTestTask->outputPath.empty())
        {
            try
            {
                std::ofstream ofs(m_autoTestTask->outputPath);
                if (ofs)
                {
                    // JSONエスケープ処理
                    std::string escapedError = errorMsg;
                    size_t pos = 0;
                    // バックスラッシュをエスケープ
                    while ((pos = escapedError.find('\\', pos)) != std::string::npos)
                    {
                        escapedError.replace(pos, 1, "\\\\");
                        pos += 2;
                    }
                    // ダブルクォートをエスケープ
                    pos = 0;
                    while ((pos = escapedError.find('"', pos)) != std::string::npos)
                    {
                        escapedError.replace(pos, 1, "\\\"");
                        pos += 2;
                    }
                    // 改行をエスケープ
                    pos = 0;
                    while ((pos = escapedError.find('\n', pos)) != std::string::npos)
                    {
                        escapedError.replace(pos, 1, "\\n");
                        pos += 2;
                    }
                    // 復帰をエスケープ
                    pos = 0;
                    while ((pos = escapedError.find('\r', pos)) != std::string::npos)
                    {
                        escapedError.replace(pos, 1, "\\r");
                        pos += 2;
                    }

                    ofs << "{\"success\": false, \"error\": \"" << escapedError << "\"}" << std::endl;
                }
            }
            catch (...)
            {
                PYAE_LOG_ERROR("IdleHandler", "Failed to write error to output file");
            }
        }

        HandleTestCompletion(success);
    }

    void IdleHandler::HandleTestCompletion(bool success)
    {
        if (!m_autoTestTask)
        {
            return;
        }

        m_autoTestTask->completed = true;

        if (success)
        {
            PYAE_LOG_INFO("IdleHandler", "Auto test completed successfully");
        }
        else
        {
            PYAE_LOG_ERROR("IdleHandler", "Auto test failed");
        }

        // 自動終了フラグが立っていればExtendScript経由でQuit
        if (m_autoTestTask->autoExit)
        {
            PYAE_LOG_INFO("IdleHandler", "Auto exit requested, executing quit...");
            ExecuteQuitCommand();
        }

        // タスクをクリア
        m_autoTestTask.reset();
    }

    void IdleHandler::ExecuteQuitCommand()
    {
        try
        {
            auto &state = PluginState::Instance();
            const auto &suites = state.GetSuites();

            PYAE_LOG_INFO("IdleHandler", "Preparing to quit After Effects...");

            // Step 1: AEGP APIで新規プロジェクトを作成
            if (suites.projSuite)
            {
                PYAE_LOG_INFO("IdleHandler", "Creating new project via AEGP API...");

                AEGP_ProjectH oldProjH = nullptr;
                suites.projSuite->AEGP_GetProjectByIndex(0, &oldProjH);

                AEGP_ProjectH newProjH = nullptr;
                A_Err err = suites.projSuite->AEGP_NewProject(&newProjH);

                if (err == A_Err_NONE)
                {
                    PYAE_LOG_INFO("IdleHandler", "New project created successfully");

                    // Step 2: プロジェクトが切り替わったことを確認
                    AEGP_ProjectH currentProjH = nullptr;
                    err = suites.projSuite->AEGP_GetProjectByIndex(0, &currentProjH);

                    if (err == A_Err_NONE && currentProjH)
                    {
                        // プロジェクト名を取得して確認
                        A_char projName[AEGP_MAX_PROJ_NAME_SIZE] = {0};
                        suites.projSuite->AEGP_GetProjectName(currentProjH, projName);
                        PYAE_LOG_INFO("IdleHandler", std::string("Current project name: ") + projName);

                        // dirtyフラグを確認（新規プロジェクトはクリーンなはず）
                        A_Boolean isDirty = false;
                        suites.projSuite->AEGP_ProjectIsDirty(currentProjH, &isDirty);
                        PYAE_LOG_INFO("IdleHandler", std::string("Project is dirty: ") + (isDirty ? "Yes" : "No"));

                        // 新規プロジェクトの場合、dirtyフラグがfalseであることを確認
                        if (!isDirty)
                        {
                            PYAE_LOG_INFO("IdleHandler", "New project confirmed (dirty flag is false)");
                        }
                        else
                        {
                            PYAE_LOG_WARNING("IdleHandler", "Project is marked as dirty (unexpected for new project)");
                        }
                    }
                    else
                    {
                        PYAE_LOG_WARNING("IdleHandler", "Failed to get current project for verification");
                    }
                }
                else
                {
                    PYAE_LOG_ERROR("IdleHandler", "Failed to create new project, error: " + std::to_string(err));
                }
            }
            else
            {
                PYAE_LOG_WARNING("IdleHandler", "Project suite not available");
            }

            // Step 3: After Effectsを終了 (ExtendScriptで単純にquit)
            if (suites.utilitySuite)
            {
                PYAE_LOG_INFO("IdleHandler", "Executing quit command...");

                const char *quitScript = "app.quit();";
                AEGP_MemHandle resultH = nullptr;
                AEGP_MemHandle errorH = nullptr;

                A_Err err = suites.utilitySuite->AEGP_ExecuteScript(
                    state.GetPluginID(),
                    quitScript,
                    true,
                    &resultH,
                    &errorH);

                if (resultH)
                    suites.memorySuite->AEGP_FreeMemHandle(resultH);
                if (errorH)
                    suites.memorySuite->AEGP_FreeMemHandle(errorH);

                if (err == A_Err_NONE)
                {
                    PYAE_LOG_INFO("IdleHandler", "Quit command executed successfully");
                }
                else
                {
                    PYAE_LOG_WARNING("IdleHandler", "Quit command failed with error: " + std::to_string(err));
                }
            }
            else
            {
                PYAE_LOG_WARNING("IdleHandler", "Utility suite not available");
            }
        }
        catch (const std::exception &e)
        {
            PYAE_LOG_ERROR("IdleHandler", std::string("Failed to execute quit command: ") + e.what());
        }
        catch (...)
        {
            PYAE_LOG_ERROR("IdleHandler", "Unknown exception while executing quit command");
        }
    }


    // C関数: AEから呼ばれるアイドルフックコールバック
    A_Err PyAE_IdleHook(
        AEGP_GlobalRefcon plugin_refconP,
        AEGP_IdleRefcon refconP,
        A_long *max_sleepPL)
    {
        return IdleHandler::Instance().OnIdle(plugin_refconP, refconP, max_sleepPL);
    }

} // namespace PyAE
