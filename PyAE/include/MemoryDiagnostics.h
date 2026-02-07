// MemoryDiagnostics.h
// PyAE - Python for After Effects
// メモリ診断ユーティリティ

#pragma once

#include "AE_GeneralPlug.h"
#include <string>

namespace PyAE {

/**
 * @brief メモリ診断ユーティリティ
 *
 * AEGP_MemorySuite1を使用してメモリ使用状況を監視し、
 * メモリリークの検出を支援する。
 */
class MemoryDiagnostics {
public:
    /**
     * @brief メモリ統計情報
     */
    struct MemStats {
        A_long handleCount;  // 確保されているハンドル数
        A_long totalSize;    // 確保されているメモリサイズ（バイト）

        MemStats() : handleCount(0), totalSize(0) {}
        MemStats(A_long count, A_long size) : handleCount(count), totalSize(size) {}

        // 差分を計算
        MemStats operator-(const MemStats& other) const {
            return MemStats(handleCount - other.handleCount, totalSize - other.totalSize);
        }

        // メモリが増加したか
        bool HasIncreased() const {
            return handleCount > 0 || totalSize > 0;
        }
    };

    /**
     * @brief プラグインのメモリ使用状況を取得
     * @return メモリ統計情報
     */
    static MemStats GetMemoryStats();

    /**
     * @brief メモリレポートを有効化/無効化
     * @param enable true=有効, false=無効
     *
     * 有効にすると、After Effectsがメモリ確保/解放を詳細にレポートする。
     * デバッグ時のみ使用すること（パフォーマンスに影響）。
     */
    static void EnableMemoryReporting(bool enable);

    /**
     * @brief メモリ統計をログ出力
     * @param context ログのコンテキスト（例: "Before test", "After test"）
     */
    static void LogMemoryStats(const std::string& context);

    /**
     * @brief メモリリークをチェック
     * @param before テスト前のメモリ統計
     * @param after テスト後のメモリ統計
     * @param toleranceHandles 許容するハンドル数の差（デフォルト: 0）
     * @param toleranceSize 許容するサイズの差（バイト、デフォルト: 0）
     * @return メモリリークが検出された場合true
     */
    static bool CheckMemoryLeak(
        const MemStats& before,
        const MemStats& after,
        A_long toleranceHandles = 0,
        A_long toleranceSize = 0);

    /**
     * @brief メモリリークの詳細をログ出力
     * @param before テスト前のメモリ統計
     * @param after テスト後のメモリ統計
     * @param context ログのコンテキスト
     */
    static void LogMemoryLeakDetails(
        const MemStats& before,
        const MemStats& after,
        const std::string& context);

    /**
     * @brief メモリハンドルのサイズを取得
     * @param memH メモリハンドル
     * @return メモリサイズ（バイト）、失敗時は0
     */
    static AEGP_MemSize GetMemHandleSize(AEGP_MemHandle memH);
};

} // namespace PyAE
