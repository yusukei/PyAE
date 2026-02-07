#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

namespace PyAE {
namespace Validation {

/**
 * @brief 値が指定された範囲内にあることを検証
 * @tparam T 値の型
 * @param value 検証する値
 * @param min 最小値（この値を含む）
 * @param max 最大値（この値を含む）
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::invalid_argument 値が範囲外の場合
 */
template<typename T>
void RequireRange(T value, T min, T max, const char* name) {
    if (value < min || value > max) {
        std::ostringstream oss;
        oss << name << " must be between " << min << " and " << max
            << ", got " << value;
        throw std::invalid_argument(oss.str());
    }
}

/**
 * @brief 値が正の値であることを検証（ゼロを含まない）
 * @tparam T 値の型
 * @param value 検証する値
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::invalid_argument 値が0以下の場合
 */
template<typename T>
void RequirePositive(T value, const char* name) {
    if (value <= 0) {
        std::ostringstream oss;
        oss << name << " must be positive, got " << value;
        throw std::invalid_argument(oss.str());
    }
}

/**
 * @brief 値が非負（0以上）であることを検証
 * @tparam T 値の型
 * @param value 検証する値
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::invalid_argument 値が負の場合
 */
template<typename T>
void RequireNonNegative(T value, const char* name) {
    if (value < 0) {
        std::ostringstream oss;
        oss << name << " must be non-negative, got " << value;
        throw std::invalid_argument(oss.str());
    }
}

/**
 * @brief 値がゼロでないことを検証
 * @tparam T 値の型
 * @param value 検証する値
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::invalid_argument 値が0の場合
 */
template<typename T>
void RequireNonZero(T value, const char* name) {
    if (value == 0) {
        std::ostringstream oss;
        oss << name << " cannot be zero";
        throw std::invalid_argument(oss.str());
    }
}

/**
 * @brief 文字列が空でないことを検証
 * @param value 検証する文字列
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::invalid_argument 文字列が空の場合
 */
inline void RequireNonEmpty(const std::string& value, const char* name) {
    if (value.empty()) {
        throw std::invalid_argument(std::string(name) + " cannot be empty");
    }
}

/**
 * @brief ワイド文字列が空でないことを検証
 * @param value 検証するワイド文字列
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::invalid_argument 文字列が空の場合
 */
inline void RequireNonEmpty(const std::wstring& value, const char* name) {
    if (value.empty()) {
        throw std::invalid_argument(std::string(name) + " cannot be empty");
    }
}

/**
 * @brief ポインタ値（uintptr_t）がNULLでないことを検証
 * @param ptr 検証するポインタ値
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::invalid_argument ポインタがNULLの場合
 */
inline void RequireNonNull(uintptr_t ptr, const char* name) {
    if (ptr == 0) {
        throw std::invalid_argument(std::string(name) + " cannot be null");
    }
}

/**
 * @brief 文字列の長さが制限内であることを検証
 * @param value 検証する文字列
 * @param maxLength 最大長
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::invalid_argument 文字列が長すぎる場合
 */
inline void RequireMaxLength(const std::string& value, size_t maxLength, const char* name) {
    if (value.size() > maxLength) {
        std::ostringstream oss;
        oss << name << " exceeds maximum length (" << maxLength
            << " characters), got " << value.size();
        throw std::invalid_argument(oss.str());
    }
}

/**
 * @brief 色成分（0.0-1.0）の範囲を検証
 * @param value 検証する色成分値
 * @param component 成分名（"Red", "Green", "Blue", "Alpha"）
 * @throws std::invalid_argument 値が0.0-1.0の範囲外の場合
 */
inline void RequireColorRange(double value, const char* component) {
    RequireRange(value, 0.0, 1.0, component);
}

/**
 * @brief インデックスが有効な範囲内にあることを検証（0ベース）
 * @param index 検証するインデックス
 * @param count コレクションの要素数
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::out_of_range インデックスが範囲外の場合
 */
inline void RequireValidIndex(int index, int count, const char* name) {
    if (index < 0 || index >= count) {
        std::ostringstream oss;
        oss << name << " index out of range: " << index
            << " (valid range: 0-" << (count - 1) << ")";
        throw std::out_of_range(oss.str());
    }
}

/**
 * @brief 1ベースのインデックスが有効な範囲内にあることを検証
 * @param index 検証するインデックス（1から開始）
 * @param count コレクションの要素数
 * @param name パラメータ名（エラーメッセージ用）
 * @throws std::out_of_range インデックスが範囲外の場合
 */
inline void RequireValidIndex1Based(int index, int count, const char* name) {
    if (index < 1 || index > count) {
        std::ostringstream oss;
        oss << name << " index out of range: " << index
            << " (valid range: 1-" << count << ")";
        throw std::out_of_range(oss.str());
    }
}

/**
 * @brief After Effects のコンポジション寸法制限を検証
 * @param width 幅
 * @param height 高さ
 * @throws std::invalid_argument 寸法が制限外の場合
 */
inline void RequireValidCompDimensions(int width, int height) {
    constexpr int MIN_DIM = 4;
    constexpr int MAX_DIM = 30000;

    if (width < MIN_DIM || height < MIN_DIM) {
        std::ostringstream oss;
        oss << "Dimensions must be at least " << MIN_DIM << "x" << MIN_DIM
            << ", got " << width << "x" << height;
        throw std::invalid_argument(oss.str());
    }

    if (width > MAX_DIM || height > MAX_DIM) {
        std::ostringstream oss;
        oss << "Dimensions must not exceed " << MAX_DIM << "x" << MAX_DIM
            << ", got " << width << "x" << height;
        throw std::invalid_argument(oss.str());
    }
}

/**
 * @brief After Effects のデュレーション制限を検証
 * @param seconds デュレーション（秒）
 * @throws std::invalid_argument デュレーションが制限外の場合
 */
inline void RequireValidDuration(double seconds) {
    RequirePositive(seconds, "Duration");

    // AE max is typically 10800 seconds = 3 hours
    constexpr double MAX_DURATION = 10800.0;
    if (seconds > MAX_DURATION) {
        std::ostringstream oss;
        oss << "Duration exceeds maximum allowed (" << MAX_DURATION
            << " seconds), got " << seconds;
        throw std::invalid_argument(oss.str());
    }
}

/**
 * @brief 不透明度（0-100）の範囲を検証
 * @param opacity 不透明度
 * @throws std::invalid_argument 不透明度が0-100の範囲外の場合
 */
inline void RequireValidOpacity(double opacity) {
    RequireRange(opacity, 0.0, 100.0, "Opacity");
}

/**
 * @brief アスペクト比が有効な値であることを検証
 * @param aspect アスペクト比
 * @throws std::invalid_argument アスペクト比が不正な場合
 */
inline void RequireValidAspectRatio(double aspect) {
    RequirePositive(aspect, "Aspect ratio");

    // Typical range for pixel aspect ratio: 0.1 to 10.0
    constexpr double MIN_ASPECT = 0.1;
    constexpr double MAX_ASPECT = 10.0;
    if (aspect < MIN_ASPECT || aspect > MAX_ASPECT) {
        std::ostringstream oss;
        oss << "Aspect ratio out of practical range (" << MIN_ASPECT
            << "-" << MAX_ASPECT << "), got " << aspect;
        throw std::invalid_argument(oss.str());
    }
}

} // namespace Validation
} // namespace PyAE
