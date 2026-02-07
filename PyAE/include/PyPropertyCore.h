// PyPropertyCore.h
// PyAE - Python for After Effects
// プロパティクラス宣言

#pragma once

#include <pybind11/pybind11.h>
#include <memory>
#include <string>
#include <vector>

#include "AE_GeneralPlug.h"
#include "PyPropertyTypes.h"
#include "PyStreamUtils.h"

namespace py = pybind11;

namespace PyAE {

// =============================================================
// PyProperty クラス
// After Effects のストリーム（プロパティ）をラップ
// =============================================================
class PyProperty {
public:
    // ==========================================
    // コンストラクタ / デストラクタ
    // ==========================================
    PyProperty();
    explicit PyProperty(AEGP_StreamRefH streamH, bool ownsHandle = true);
    PyProperty(AEGP_StreamRefH streamH, bool ownsHandle,
               StreamUtils::UnknownTypePolicy policy, int effectParamIndex);
    ~PyProperty();

    // コピー禁止、ムーブのみ
    PyProperty(const PyProperty&) = delete;
    PyProperty& operator=(const PyProperty&) = delete;
    PyProperty(PyProperty&& other) noexcept;
    PyProperty& operator=(PyProperty&& other) noexcept;

    // ==========================================
    // 基本情報
    // ==========================================

    /// 有効なストリームハンドルを持つか
    bool IsValid() const;

    /// プロパティ名を取得
    std::string GetName() const;

    /// プロパティ名を設定 (AEGP_SetStreamName)
    /// IndexedGroup の子要素の名前を変更する際に使用
    void SetName(const std::string& name);

    /// マッチ名を取得（スクリプト互換の識別子）
    std::string GetMatchName() const;

    /// 階層深度を取得
    int GetDepth() const;

    // ==========================================
    // 型情報
    // ==========================================

    /// SDK準拠のストリーム値タイプを取得
    StreamValueType GetStreamValueType() const;

    /// SDK準拠の構造タイプを取得
    StreamGroupingType GetGroupingType() const;

    /// 後方互換用の簡略化タイプを取得
    PropertyType GetType() const;

    /// グループかどうか
    bool IsGroup() const;

    /// 葉プロパティかどうか
    bool IsLeaf() const;

    // ==========================================
    // 能力判定（SDK API直接使用）
    // ==========================================

    /// 時間変化可能か（SDK AEGP_CanVaryOverTime直接呼び出し）
    bool CanVaryOverTime() const;

    /// キーフレームを持てるか
    bool CanHaveKeyframes() const;

    /// エクスプレッションを持てるか
    bool CanHaveExpression() const;

    /// 値を直接設定できるか（プリミティブ型のみ）
    bool CanSetValue() const;

    /// プリミティブ型か
    bool IsPrimitive() const;

    /// エフェクトパラメータかどうか
    bool IsEffectParam() const;

    /// エフェクトパラメータインデックスを取得（0ベース、-1 = not an effect param）
    int GetEffectParamIndex() const;

    // ==========================================
    // 値操作
    // ==========================================

    /// 値を取得
    /// @param time コンポジション時間（秒）
    /// @return 値（グループやNO_DATAの場合はNone）
    py::object GetValue(double time = 0.0) const;

    /// 値を設定（プリミティブ型のみ）
    /// @throws std::runtime_error プリミティブ型でない場合
    void SetValue(py::object value);

    /// 指定時間の値を取得
    py::object GetValueAtTime(double time) const;

    /// 指定時間に値を設定（キーフレーム作成/更新）
    void SetValueAtTime(double time, py::object value);

    /// 生のバイナリデータを取得（bdata形式）
    /// @param time コンポジション時間（秒）
    /// @return バイナリデータ（py::bytes）、取得できない場合は空のbytes
    py::bytes GetRawBytes(double time = 0.0) const;

    /// TextDocument の値（テキスト文字列）を設定
    /// @param time コンポジション時間（秒）
    /// @param text 設定するテキスト（UTF-8）
    /// @throws std::runtime_error TextDocumentタイプでない場合
    void SetTextDocumentValue(double time, const std::string& text);

    /// Shape Path の頂点データを設定（ADBE Vector Shape - Group用）
    /// @param vertices 頂点配列（各要素は{"x", "y", "tan_in_x", "tan_in_y", "tan_out_x", "tan_out_y"}のdict）
    /// @param time コンポジション時間（秒）
    /// @throws std::runtime_error Maskタイプでない場合
    void SetShapePathVertices(const py::list& vertices, double time = 0.0);

    // ==========================================
    // メタデータ取得
    // ==========================================

    /// ストリームメタデータを取得（XML tdb4相当）
    /// @return dict with keys: flags, min_value, max_value, grouping_type
    py::dict GetStreamMetadata() const;

    // ==========================================
    // キーフレーム情報
    // ==========================================

    /// キーフレーム数を取得
    int GetNumKeyframes() const;

    /// キーフレームを持つか
    bool IsTimeVarying() const;

    // ==========================================
    // キーフレーム操作
    // ==========================================

    /// キーフレームを追加
    /// @return 追加されたキーフレームのインデックス
    int AddKeyframe(double time, py::object value);

    /// キーフレームを削除
    void RemoveKeyframe(int index);

    /// 全キーフレームを削除
    void RemoveAllKeyframes();

    /// キーフレームの時間を取得
    double GetKeyframeTime(int index) const;

    /// キーフレームの値を取得
    py::object GetKeyframeValue(int index) const;

    /// キーフレームの値を設定
    void SetKeyframeValue(int index, py::object value);

    /// キーフレームの補間タイプを取得
    std::pair<std::string, std::string> GetKeyframeInterpolation(int index) const;

    /// キーフレームの補間タイプを設定
    void SetKeyframeInterpolation(int index, const std::string& inType, const std::string& outType);

    /// 全キーフレームを取得
    py::list GetKeyframes() const;

    // ==========================================
    // エクスプレッション操作
    // ==========================================

    /// エクスプレッションを持つか（安全版）
    bool HasExpression() const;

    /// エクスプレッションを取得
    /// @throws std::runtime_error CanHaveExpression()がfalseの場合
    std::string GetExpression() const;

    /// エクスプレッションを設定
    /// @throws std::runtime_error CanHaveExpression()がfalseの場合
    void SetExpression(const std::string& expression);

    /// エクスプレッションの有効/無効を切り替え
    /// @throws std::runtime_error CanHaveExpression()がfalseの場合
    void EnableExpression(bool enable);

    // ==========================================
    // 子プロパティアクセス
    // ==========================================

    /// 子プロパティ数を取得
    int GetNumProperties() const;

    /// インデックスまたはマッチ名で子プロパティを取得
    std::shared_ptr<PyProperty> GetProperty(py::object key) const;

    /// 全子プロパティを取得
    std::vector<std::shared_ptr<PyProperty>> GetChildren() const;

    // ==========================================
    // 動的プロパティ追加（INDEXED_GROUP用）
    // ==========================================

    /// 新しいストリームを追加可能か確認
    bool CanAddStream(const std::string& match_name) const;

    /// 新しいストリームを追加（INDEXED_GROUP のみ）
    std::shared_ptr<PyProperty> AddStream(const std::string& match_name);

    // ==========================================
    // Dynamic Stream フラグ操作
    // ==========================================

    /// Dynamic Stream フラグを取得（レイヤースタイルの有効/無効など）
    /// @return フラグ値（AEGP_DynStreamFlags）
    /// @throws std::runtime_error DynamicStreamSuiteが利用できない場合
    int GetDynamicStreamFlags() const;

    /// Dynamic Stream フラグを設定（レイヤースタイルの有効/無効など）
    /// @param flag 設定するフラグ（AEGP_DynStreamFlag_ACTIVE_EYEBALL = 0x01など）
    /// @param undoable Undo可能にするか
    /// @param set_flag フラグを設定（true）またはクリア（false）
    /// @throws std::runtime_error DynamicStreamSuiteが利用できない場合
    void SetDynamicStreamFlag(int flag, bool undoable, bool set_flag);

    // ==========================================
    // 内部アクセス
    // ==========================================

    /// 内部ハンドルを取得
    AEGP_StreamRefH GetHandle() const { return m_streamH; }

private:
    // ==========================================
    // 内部ヘルパー
    // ==========================================

    /// SDK AEGP_StreamType を取得
    AEGP_StreamType GetRawStreamType() const;

    /// SDK AEGP_StreamGroupingType を取得
    AEGP_StreamGroupingType GetRawGroupingType() const;

    /// Python値 → ストリーム値変換
    AEGP_StreamValue2 PythonToStreamValue(py::object value) const;

    /// ストリーム値 → Python値変換
    py::object StreamValueToPython(const AEGP_StreamValue2& value) const;

    /// TextDocument の値（テキスト文字列）を取得
    py::object GetTextDocumentValue(double time) const;

    // ==========================================
    // メンバ変数
    // ==========================================
    AEGP_StreamRefH m_streamH;
    bool m_ownsHandle;
    StreamUtils::UnknownTypePolicy m_unknownTypePolicy = StreamUtils::UnknownTypePolicy::ThrowException;
    bool m_isEffectParam = false;
    int m_effectParamIndex = -1;
};

// =============================================================
// Pythonバインディング初期化関数
// =============================================================
void init_property(py::module_& m);

} // namespace PyAE
