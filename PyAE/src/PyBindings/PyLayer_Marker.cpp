// PyLayer_Marker.cpp
// PyAE - Python for After Effects
// レイヤーマーカー機能実装

#include "PyLayerClasses.h"
#include "PluginState.h"
#include "ScopedHandles.h"
#include "StringUtils.h"

#include "AETypeUtils.h"
#include "PyMarker.h"

namespace PyAE {

void PyLayer::AddMarker(double time, const std::string& comment) {
    if (!m_layerH) {
        throw std::runtime_error("Invalid layer");
    }

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.streamSuite || !suites.markerSuite || !suites.keyframeSuite) {
        throw std::runtime_error("Required suites not available for marker operations");
    }

    // マーカーストリームを取得
    AEGP_StreamRefH markerStreamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(),
        m_layerH,
        AEGP_LayerStream_MARKER,
        &markerStreamH);

    if (err != A_Err_NONE || !markerStreamH) {
        throw std::runtime_error("Failed to get marker stream");
    }

    // Use RAII wrapper for automatic stream cleanup
    ScopedStreamRef scopedStream(suites.streamSuite, markerStreamH);

    // Create new marker
    AEGP_MarkerValP markerP = nullptr;
    err = suites.markerSuite->AEGP_NewMarker(&markerP);
    if (err != A_Err_NONE || !markerP) {
        throw std::runtime_error("Failed to create new marker");
    }

    // Set marker comment
    if (!comment.empty()) {
        std::wstring wcomment = StringUtils::Utf8ToWide(comment);
        err = suites.markerSuite->AEGP_SetMarkerString(
            markerP,
            AEGP_MarkerString_COMMENT,
            reinterpret_cast<const A_u_short*>(wcomment.c_str()),
            static_cast<A_long>(wcomment.length()));

        if (err != A_Err_NONE) {
            suites.markerSuite->AEGP_DisposeMarker(markerP);
            throw std::runtime_error("Failed to set marker comment");
        }
    }

    // Set marker time
    A_Time aeTime = AETypeUtils::SecondsToTime(time);

    // Insert marker as keyframe
    AEGP_KeyframeIndex key_index = 0;
    err = suites.keyframeSuite->AEGP_InsertKeyframe(
        markerStreamH,
        AEGP_LTimeMode_LayerTime,
        &aeTime,
        &key_index);

    if (err != A_Err_NONE) {
        suites.markerSuite->AEGP_DisposeMarker(markerP);
        throw std::runtime_error("Failed to insert keyframe");
    }

    // Set marker value to keyframe
    AEGP_StreamValue2 streamValue = {};
    streamValue.val.markerP = markerP;

    err = suites.keyframeSuite->AEGP_SetKeyframeValue(
        markerStreamH,
        key_index,
        &streamValue);

    if (err != A_Err_NONE) {
        suites.markerSuite->AEGP_DisposeMarker(markerP);
        throw std::runtime_error("Failed to set keyframe value");
    }

    // Stream will be automatically disposed by ScopedStreamRef destructor
}

void PyLayer::AddMarkerObject(double time, std::shared_ptr<PyMarker> marker) {
    if (!m_layerH) throw std::runtime_error("Invalid layer");
    if (!marker || !marker->IsValid()) throw std::runtime_error("Invalid marker object");

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.streamSuite || !suites.keyframeSuite) {
        throw std::runtime_error("Required suites not available");
    }

    AEGP_StreamRefH markerStreamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(), m_layerH, AEGP_LayerStream_MARKER, &markerStreamH);
    
    if (err != A_Err_NONE || !markerStreamH) {
        throw std::runtime_error("Failed to get marker stream");
    }

    try {
        A_Time aeTime = AETypeUtils::SecondsToTime(time);
        AEGP_KeyframeIndex key_index = 0;
        err = suites.keyframeSuite->AEGP_InsertKeyframe(
            markerStreamH, AEGP_LTimeMode_LayerTime, &aeTime, &key_index);
        
        if (err != A_Err_NONE) throw std::runtime_error("Failed to insert keyframe");

        AEGP_StreamValue2 streamValue = {};
        streamValue.val.markerP = marker->GetHandle();
        
        err = suites.keyframeSuite->AEGP_SetKeyframeValue(
            markerStreamH, key_index, &streamValue);
            
        if (err != A_Err_NONE) throw std::runtime_error("Failed to set marker value");

        suites.streamSuite->AEGP_DisposeStream(markerStreamH);
    } catch (...) {
        if (markerStreamH) suites.streamSuite->AEGP_DisposeStream(markerStreamH);
        throw;
    }
}

std::string PyLayer::GetMarkerComment(double time) const {
    if (!m_layerH) return "";

    auto& state = PluginState::Instance();
    const auto& suites = state.GetSuites();

    if (!suites.streamSuite || !suites.markerSuite || !suites.keyframeSuite || !suites.memorySuite) {
        return "";
    }

    // マーカーストリームを取得
    AEGP_StreamRefH markerStreamH = nullptr;
    A_Err err = suites.streamSuite->AEGP_GetNewLayerStream(
        state.GetPluginID(),
        m_layerH,
        AEGP_LayerStream_MARKER,
        &markerStreamH);

    if (err != A_Err_NONE || !markerStreamH) {
        return "";
    }

    std::string result;
    try {
        // 時間を設定
        A_Time aeTime = AETypeUtils::SecondsToTime(time);

        // その時間のストリーム値を取得
        AEGP_StreamValue2 streamValue = {};
        err = suites.streamSuite->AEGP_GetNewStreamValue(
            state.GetPluginID(),
            markerStreamH,
            AEGP_LTimeMode_LayerTime,
            &aeTime,
            TRUE,  // pre_expression
            &streamValue);

        if (err == A_Err_NONE && streamValue.val.markerP) {
            // コメント文字列を取得
            AEGP_MemHandle commentH = nullptr;
            err = suites.markerSuite->AEGP_GetMarkerString(
                state.GetPluginID(),
                streamValue.val.markerP,
                AEGP_MarkerString_COMMENT,
                &commentH);

            if (err == A_Err_NONE && commentH) {
                ScopedMemHandle scopedComment(state.GetPluginID(), suites.memorySuite, commentH);
                ScopedMemLock lock(suites.memorySuite, commentH);
                A_UTF16Char* commentPtr = lock.As<A_UTF16Char>();
                if (commentPtr) {
                    result = StringUtils::Utf16ToUtf8(commentPtr);
                }
            }

            // ストリーム値を解放
            suites.streamSuite->AEGP_DisposeStreamValue(&streamValue);
        }

        suites.streamSuite->AEGP_DisposeStream(markerStreamH);

    } catch (...) {
        if (markerStreamH) {
            suites.streamSuite->AEGP_DisposeStream(markerStreamH);
        }
        throw;  // Re-throw exception after cleanup
    }

    return result;
}

} // namespace PyAE
