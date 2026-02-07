// ErrorHandling.cpp
// PyAE - Python for After Effects
// エラーハンドリングの実装
//
// 注: エラーハンドリング機能は主にヘッダーでマクロとして実装されています（ErrorHandling.h参照）
// このファイルは追加のユーティリティ関数用です

#include "ErrorHandling.h"
#include <sstream>

namespace PyAE {

// 詳細なAEエラーメッセージを取得
std::string GetDetailedAEErrorMessage(A_Err err) {
    std::ostringstream oss;
    oss << "After Effects Error: " << AEErrorToString(err);
    oss << " (code: " << err << ")";

    // 追加のコンテキスト情報
    switch (err) {
        case A_Err_NONE:
            oss << " - Operation completed successfully";
            break;
        case A_Err_GENERIC:
            oss << " - A generic error occurred";
            break;
        case A_Err_STRUCT:
            oss << " - Invalid structure or data format";
            break;
        case A_Err_PARAMETER:
            oss << " - Invalid parameter passed to function";
            break;
        case A_Err_ALLOC:
            oss << " - Memory allocation failed";
            break;
        case A_Err_MISSING_SUITE:
            oss << " - Required suite is not available";
            break;
        default:
            oss << " - Unknown error";
            break;
    }

    return oss.str();
}

// エラーコードが致命的かどうか判定
bool IsErrorFatal(A_Err err) {
    switch (err) {
        case A_Err_ALLOC:
        case A_Err_MISSING_SUITE:
            return true;
        default:
            return false;
    }
}

// エラーコードがリカバリ可能かどうか判定
bool IsErrorRecoverable(A_Err err) {
    switch (err) {
        case A_Err_NONE:
        case A_Err_GENERIC:
        case A_Err_PARAMETER:
            return true;
        default:
            return false;
    }
}

} // namespace PyAE
