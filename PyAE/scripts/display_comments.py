import ae.sdk


def main():
    try:
        plugin_id = ae.sdk.AEGP_GetPluginID()
    except Exception as e:
        print(f"Failed to get plugin ID: {e}")
        return

    # Get active item
    itemH = ae.sdk.AEGP_GetActiveItem()
    if not itemH:
        print("No active item.")
        return

    # Check if item is Comp
    try:
        compH = ae.sdk.AEGP_GetCompFromItem(itemH)
    except Exception:
        print("Active item is not a composition.")
        return

    print("--- Timeline Comments ---")

    # Iterate layers
    try:
        num_layers = ae.sdk.AEGP_GetCompNumLayers(compH)
    except AttributeError:
        print("AEGP_GetCompNumLayers not available. Please rebuild plugin.")
        return

    for i in range(num_layers):
        try:
            layerH = ae.sdk.AEGP_GetCompLayerByIndex(compH, i)
            layer_name = ae.sdk.AEGP_GetLayerName(plugin_id, layerH)
        except Exception as e:
            print(f"Error accessing layer {i}: {e}")
            continue

        # Get marker stream
        streamH = 0
        try:
            streamH = ae.sdk.AEGP_GetNewLayerStream(
                plugin_id, layerH, ae.sdk.AEGP_LayerStream_MARKER
            )
        except Exception:
            # Layer might not support markers or other error
            continue

        try:
            num_kfs = ae.sdk.AEGP_GetStreamNumKfs(streamH)

            if num_kfs > 0:
                print(f"Layer: {layer_name} ({num_kfs} markers)")
                for k in range(num_kfs):
                    time = ae.sdk.AEGP_GetKeyframeTime(
                        streamH, k, ae.sdk.AEGP_LTimeMode_LayerTime
                    )
                    markerP = ae.sdk.AEGP_GetKeyframeValue_Marker(streamH, k)

                    comment = ae.sdk.AEGP_GetMarkerString(
                        plugin_id, markerP, ae.sdk.AEGP_MarkerString_COMMENT
                    )

                    print(f"  [{time:.2f}s]: {comment}")
        except Exception as e:
            print(f"Error processing markers for layer {layer_name}: {e}")
        finally:
            if streamH:
                ae.sdk.AEGP_DisposeStream(streamH)


if __name__ == "__main__":
    main()
