// StringUtils.h
// PyAE - Python for After Effects
// 文字列変換ユーティリティ

#pragma once

#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace PyAE {
namespace StringUtils {

// UTF-8 → UTF-16 (Windows wchar_t) 変換
inline std::wstring Utf8ToWide(const std::string& utf8) {
    if (utf8.empty()) return L"";

#ifdef _WIN32
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (size == 0) return L"";

    std::wstring result(size - 1, L'\0');  // null終端分を除く
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &result[0], size);
    return result;
#else
    // 非Windows環境用の簡易実装（ASCIIのみ）
    std::wstring result;
    result.reserve(utf8.size());
    for (char c : utf8) {
        result.push_back(static_cast<wchar_t>(static_cast<unsigned char>(c)));
    }
    return result;
#endif
}

// UTF-16 → UTF-8 変換
inline std::string WideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return "";

#ifdef _WIN32
    int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size == 0) return "";

    std::string result(size - 1, '\0');  // null終端分を除く
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &result[0], size, nullptr, nullptr);
    return result;
#else
    // 非Windows環境用の簡易実装（ASCIIのみ）
    std::string result;
    result.reserve(wide.size());
    for (wchar_t wc : wide) {
        if (wc < 128) {
            result.push_back(static_cast<char>(wc));
        } else {
            result.push_back('?');
        }
    }
    return result;
#endif
}

// A_UTF16Char* → std::string (UTF-8) 変換
inline std::string Utf16ToUtf8(const A_UTF16Char* utf16) {
    if (!utf16) return "";

    // A_UTF16Charはwchar_tと同じサイズ（Windows）
    return WideToUtf8(std::wstring(reinterpret_cast<const wchar_t*>(utf16)));
}

// ローカルエンコーディング (A_char*, Shift-JIS等) → UTF-8 変換
// AE SDK の A_char* を返す関数用
inline std::string LocalToUtf8(const char* local) {
    if (!local || local[0] == '\0') return "";

#ifdef _WIN32
    // まずローカルエンコーディング → UTF-16 に変換
    int wideSize = MultiByteToWideChar(CP_ACP, 0, local, -1, nullptr, 0);
    if (wideSize == 0) return "";

    std::wstring wide(wideSize - 1, L'\0');
    MultiByteToWideChar(CP_ACP, 0, local, -1, &wide[0], wideSize);

    // UTF-16 → UTF-8 に変換
    return WideToUtf8(wide);
#else
    // 非Windows環境では直接返す（UTF-8想定）
    return std::string(local);
#endif
}

// std::string (UTF-8) → A_UTF16Char用のstd::wstring 変換
// 注意: 返されたwstringのc_str()をreinterpret_castしてA_UTF16Char*として使用
inline std::wstring Utf8ToUtf16(const std::string& utf8) {
    return Utf8ToWide(utf8);
}

} // namespace StringUtils
} // namespace PyAE
