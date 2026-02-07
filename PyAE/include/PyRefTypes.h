// PyRefTypes.h
// PyAE - Python for After Effects
// 共通参照型の定義（循環依存回避用）
// 複数のバインディングファイルで使用される参照クラスを定義

#pragma once

#include <pybind11/pybind11.h>
#include "AE_GeneralPlug.h"

namespace PyAE {

// =============================================================
// アイテム参照クラス
// - 循環依存を回避するための軽量参照
// - AEGP_ItemH をラップ
// =============================================================
class PyItemRef {
public:
    PyItemRef() : m_itemH(nullptr) {}
    explicit PyItemRef(AEGP_ItemH itemH) : m_itemH(itemH) {}

    AEGP_ItemH GetHandle() const { return m_itemH; }
    bool IsValid() const { return m_itemH != nullptr; }

private:
    AEGP_ItemH m_itemH;
};

// =============================================================
// フォルダ参照クラス
// - フォルダアイテムへの軽量参照
// =============================================================
class PyFolderRef {
public:
    PyFolderRef() : m_itemH(nullptr) {}
    explicit PyFolderRef(AEGP_ItemH itemH) : m_itemH(itemH) {}

    AEGP_ItemH GetHandle() const { return m_itemH; }
    bool IsValid() const { return m_itemH != nullptr; }

private:
    AEGP_ItemH m_itemH;
};

// =============================================================
// コンポジション参照クラス
// - コンポジションへの軽量参照
// =============================================================
class PyCompRef {
public:
    PyCompRef() : m_compH(nullptr) {}
    explicit PyCompRef(AEGP_CompH compH) : m_compH(compH) {}

    AEGP_CompH GetHandle() const { return m_compH; }
    bool IsValid() const { return m_compH != nullptr; }

private:
    AEGP_CompH m_compH;
};

// =============================================================
// レイヤー参照クラス
// - レイヤーへの軽量参照
// =============================================================
// PyLayerRef removed to avoid conflict with PyCompClasses.h
// class PyLayerRef { ... };

// =============================================================
// ヘルパー関数: AEGP_ItemH を実オブジェクトに変換
// - PyItem.cpp で定義
// - PyProject.cpp など他のファイルから呼び出し可能
// =============================================================
namespace py = pybind11;
py::object ResolveItemHandle(AEGP_ItemH itemH);

} // namespace PyAE
