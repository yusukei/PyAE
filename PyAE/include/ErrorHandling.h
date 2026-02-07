// ErrorHandling.h
// PyAE - Python for After Effects
// AEエラーハンドリングマクロとユーティリティ

#pragma once

// C++標準ライブラリを先にインクルード
#include <string>
#include <stdexcept>

// After Effects SDK
#include "AE_GeneralPlug.h"
#include "AE_Macros.h"

// PyAE
#include "Logger.h"

namespace PyAE {

// AEエラーコードを文字列に変換
inline std::string AEErrorToString(A_Err err) {
    switch (err) {
        case A_Err_NONE:                return "A_Err_NONE";
        case A_Err_GENERIC:             return "A_Err_GENERIC";
        case A_Err_STRUCT:              return "A_Err_STRUCT";
        case A_Err_PARAMETER:           return "A_Err_PARAMETER";
        case A_Err_ALLOC:               return "A_Err_ALLOC";
        case A_Err_MISSING_SUITE:       return "A_Err_MISSING_SUITE";
        default:                        return "A_Err_UNKNOWN (" + std::to_string(err) + ")";
    }
}

// AE例外クラス
class AEException : public std::runtime_error {
public:
    AEException(A_Err err, const std::string& context)
        : std::runtime_error(context + ": " + AEErrorToString(err))
        , m_error(err)
        , m_context(context)
    {}

    A_Err GetError() const { return m_error; }
    const std::string& GetContext() const { return m_context; }

private:
    A_Err m_error;
    std::string m_context;
};

// Python例外変換用
class PyAEException : public std::runtime_error {
public:
    explicit PyAEException(const std::string& message)
        : std::runtime_error(message)
    {}
};

} // namespace PyAE

// =============================================================
// エラーチェックマクロ
// =============================================================

// エラーをチェックしてログ出力
#define PYAE_CHECK_ERR(err, context) \
    do { \
        A_Err _err = (err); \
        if (_err != A_Err_NONE) { \
            PYAE_LOG_ERROR("AE", std::string(context) + ": " + PyAE::AEErrorToString(_err)); \
        } \
    } while(0)

// エラーをチェックしてリターン（errにセット）
#define PYAE_ERR_CHECK(condition) \
    do { \
        if (err == A_Err_NONE) { \
            err = (condition); \
            if (err != A_Err_NONE) { \
                PYAE_LOG_ERROR("AE", std::string(__FUNCTION__) + ": " + PyAE::AEErrorToString(err)); \
            } \
        } \
    } while(0)

// エラーなら即座にリターン
#define PYAE_RETURN_IF_ERR(condition) \
    do { \
        A_Err _err = (condition); \
        if (_err != A_Err_NONE) { \
            PYAE_LOG_ERROR("AE", std::string(__FUNCTION__) + ": " + PyAE::AEErrorToString(_err)); \
            return _err; \
        } \
    } while(0)

// エラーなら例外をスロー
#define PYAE_THROW_IF_ERR(condition, context) \
    do { \
        A_Err _err = (condition); \
        if (_err != A_Err_NONE) { \
            PYAE_LOG_ERROR("AE", std::string(context) + ": " + PyAE::AEErrorToString(_err)); \
            throw PyAE::AEException(_err, context); \
        } \
    } while(0)

// nullptrチェック
#define PYAE_CHECK_NULL(ptr, context) \
    do { \
        if ((ptr) == nullptr) { \
            PYAE_LOG_ERROR("AE", std::string(context) + ": null pointer"); \
            throw PyAE::PyAEException(std::string(context) + ": null pointer"); \
        } \
    } while(0)

// スイートポインタのnullチェック
#define PYAE_CHECK_SUITE(suite, name) \
    do { \
        if ((suite) == nullptr) { \
            PYAE_LOG_ERROR("AE", std::string("Suite not available: ") + name); \
            throw PyAE::PyAEException(std::string("Suite not available: ") + name); \
        } \
    } while(0)
