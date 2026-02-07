"""
ae.sdk - Low-level Access to After Effects SDK Suites
"""

# Constants
AEGP_LayerStream_MARKER: int
AEGP_MarkerString_NONE: int
AEGP_MarkerString_COMMENT: int
AEGP_MarkerString_CHAPTER: int
AEGP_MarkerString_URL: int
AEGP_MarkerString_FRAME_TARGET: int
AEGP_MarkerString_CUE_POINT_NAME: int

AEGP_LTimeMode_LayerTime: int
AEGP_LTimeMode_CompTime: int

# Layer Quality Constants
AEGP_LayerQual_NONE: int
AEGP_LayerQual_WIREFRAME: int
AEGP_LayerQual_DRAFT: int
AEGP_LayerQual_BEST: int

# Layer Sampling Quality Constants
AEGP_LayerSamplingQual_BILINEAR: int
AEGP_LayerSamplingQual_BICUBIC: int

# Layer Label Constants
AEGP_Label_NONE: int
AEGP_Label_NO_LABEL: int
AEGP_Label_1: int
AEGP_Label_2: int
AEGP_Label_3: int
AEGP_Label_4: int
AEGP_Label_5: int
AEGP_Label_6: int
AEGP_Label_7: int
AEGP_Label_8: int
AEGP_Label_9: int
AEGP_Label_10: int
AEGP_Label_11: int
AEGP_Label_12: int
AEGP_Label_13: int
AEGP_Label_14: int
AEGP_Label_15: int
AEGP_Label_16: int

# Transfer Mode (Blend Mode) Constants
PF_Xfer_NONE: int
PF_Xfer_COPY: int
PF_Xfer_BEHIND: int
PF_Xfer_IN_FRONT: int
PF_Xfer_DISSOLVE: int
PF_Xfer_ADD: int
PF_Xfer_MULTIPLY: int
PF_Xfer_SCREEN: int
PF_Xfer_OVERLAY: int
PF_Xfer_SOFT_LIGHT: int
PF_Xfer_HARD_LIGHT: int
PF_Xfer_DARKEN: int
PF_Xfer_LIGHTEN: int
PF_Xfer_DIFFERENCE: int
PF_Xfer_HUE: int
PF_Xfer_SATURATION: int
PF_Xfer_COLOR: int
PF_Xfer_LUMINOSITY: int
PF_Xfer_MULTIPLY_ALPHA: int
PF_Xfer_MULTIPLY_ALPHA_LUMA: int
PF_Xfer_MULTIPLY_NOT_ALPHA: int
PF_Xfer_MULTIPLY_NOT_ALPHA_LUMA: int
PF_Xfer_ADDITIVE_PREMUL: int
PF_Xfer_ALPHA_ADD: int
PF_Xfer_COLOR_DODGE: int
PF_Xfer_COLOR_BURN: int
PF_Xfer_EXCLUSION: int
PF_Xfer_DIFFERENCE2: int
PF_Xfer_COLOR_DODGE2: int
PF_Xfer_COLOR_BURN2: int
PF_Xfer_LINEAR_DODGE: int
PF_Xfer_LINEAR_BURN: int
PF_Xfer_LINEAR_LIGHT: int
PF_Xfer_VIVID_LIGHT: int
PF_Xfer_PIN_LIGHT: int
PF_Xfer_HARD_MIX: int
PF_Xfer_LIGHTER_COLOR: int
PF_Xfer_DARKER_COLOR: int
PF_Xfer_SUBTRACT: int
PF_Xfer_DIVIDE: int

# Matte Mode Constants
AEGP_MatteMode_STRAIGHT: int
AEGP_MatteMode_PREMUL_BLACK: int
AEGP_MatteMode_PREMUL_BG_COLOR: int

# Channel Order Constants
AEGP_ChannelOrder_ARGB: int
AEGP_ChannelOrder_BGRA: int

# Item Quality Constants
AEGP_ItemQuality_DRAFT: int
AEGP_ItemQuality_BEST: int

# Field Render Constants
PF_Field_FRAME: int
PF_Field_UPPER: int
PF_Field_LOWER: int

# RenderQueueMonitor Finished Status Constants
AEGP_RQM_FinishedStatus_UNKNOWN: int
AEGP_RQM_FinishedStatus_SUCCEEDED: int
AEGP_RQM_FinishedStatus_ABORTED: int
AEGP_RQM_FinishedStatus_ERRED: int

# Helper Functions
def AEGP_GetPluginID() -> int:
    """Get the plugin ID (required for many SDK calls)"""
    ...

# AEGP_StreamSuite
def AEGP_GetNewLayerStream(plugin_id: int, layerH: int, stream_type: int) -> int:
    """
    Get a new stream reference for the specified layer stream.
    Returns: streamH (int)
    """
    ...

def AEGP_DisposeStream(streamH: int) -> None:
    """Dispose of a stream reference."""
    ...

# AEGP_MarkerSuite3
def AEGP_NewMarker() -> int:
    """
    Creates a new marker. Must be disposed with AEGP_DisposeMarker.
    Returns: markerP (int)
    """
    ...

def AEGP_DisposeMarker(markerP: int) -> None:
    """Dispose of a marker."""
    ...

def AEGP_DuplicateMarker(markerP: int) -> int:
    """
    Duplicates a marker. Must be disposed with AEGP_DisposeMarker.

    Args:
        markerP: Source marker handle

    Returns:
        new_markerP: New marker handle (duplicated)
    """
    ...

def AEGP_GetMarkerFlag(markerP: int, flagType: int) -> bool:
    """
    Gets a marker flag value.

    Args:
        markerP: Marker handle
        flagType: Flag type (AEGP_MarkerFlag_NAVIGATION=0, AEGP_MarkerFlag_PROTECT_REGION=1)

    Returns:
        Flag value (True/False)
    """
    ...

def AEGP_SetMarkerFlag(markerP: int, flagType: int, valueB: bool) -> None:
    """
    Sets a marker flag value.

    Args:
        markerP: Marker handle
        flagType: Flag type (AEGP_MarkerFlag_NAVIGATION=0, AEGP_MarkerFlag_PROTECT_REGION=1)
        valueB: Flag value to set
    """
    ...

def AEGP_GetMarkerString(plugin_id: int, markerP: int, strType: int) -> str:
    """
    Gets a marker string value.

    Args:
        plugin_id: Plugin ID
        markerP: Marker handle
        strType: String type (AEGP_MarkerString_COMMENT=0, CHAPTER=1, URL=2, etc.)

    Returns:
        String value
    """
    ...

def AEGP_SetMarkerString(markerP: int, strType: int, str: str) -> None:
    """
    Set a string value on a marker.

    Args:
        markerP: Marker handle
        strType: String type (AEGP_MarkerString_COMMENT=0, CHAPTER=1, URL=2, etc.)
        str: String value to set
    """
    ...

def AEGP_GetMarkerDuration(markerP: int) -> dict:
    """
    Gets marker duration as A_Time.

    Args:
        markerP: Marker handle

    Returns:
        dict with 'value' (int) and 'scale' (int) representing A_Time
    """
    ...

def AEGP_SetMarkerDuration(markerP: int, durationT: dict) -> None:
    """
    Sets marker duration from A_Time dict.

    Args:
        markerP: Marker handle
        durationT: dict with 'value' (int) and 'scale' (int) representing A_Time
    """
    ...

def AEGP_GetMarkerLabel(markerP: int) -> int:
    """
    Gets marker label color index.

    Args:
        markerP: Marker handle

    Returns:
        Label color index (0-16, where 0=None, 1-16=various colors)
    """
    ...

def AEGP_SetMarkerLabel(markerP: int, value: int) -> None:
    """
    Sets marker label color index.

    Args:
        markerP: Marker handle
        value: Label color index (0-16)
    """
    ...

def AEGP_CountCuePointParams(markerP: int) -> int:
    """
    Returns the number of cue point parameters.

    Args:
        markerP: Marker handle

    Returns:
        Number of cue point parameters
    """
    ...

def AEGP_GetIndCuePointParam(plugin_id: int, markerP: int, param_indexL: int) -> dict:
    """
    Gets a cue point parameter at index.

    Args:
        plugin_id: Plugin ID
        markerP: Marker handle
        param_indexL: Parameter index (0 to count-1)

    Returns:
        dict with 'key' (str) and 'value' (str)
    """
    ...

def AEGP_SetIndCuePointParam(markerP: int, param_indexL: int, key: str, value: str) -> None:
    """
    Sets a cue point parameter at index.

    Args:
        markerP: Marker handle
        param_indexL: Parameter index (0 to count-1)
        key: Parameter key
        value: Parameter value
    """
    ...

def AEGP_InsertCuePointParam(markerP: int, param_indexL: int) -> None:
    """
    Inserts a new cue point parameter slot at index.
    Must follow with AEGP_SetIndCuePointParam to set the actual data.

    Args:
        markerP: Marker handle
        param_indexL: Index to insert (0 to count)
    """
    ...

def AEGP_DeleteIndCuePointParam(markerP: int, param_indexL: int) -> None:
    """
    Deletes the cue point parameter at index.

    Args:
        markerP: Marker handle
        param_indexL: Parameter index (0 to count-1)
    """
    ...

# AEGP_KeyframeSuite
def AEGP_InsertKeyframe(streamH: int, time_mode: int, time: float) -> int:
    """
    Insert a keyframe at the specified time.
    Returns: keyIndex (int)
    """
    ...

def AEGP_SetKeyframeValue_Marker(streamH: int, keyIndex: int, markerP: int) -> None:
    """
    Set a marker value for a keyframe.
    """
    ...

# AEGP_ProjSuite
def AEGP_GetNumProjects() -> int:
    """Get the number of open projects."""
    ...

def AEGP_GetProjectByIndex(index: int) -> int:
    """Get project handle by index."""
    ...

def AEGP_GetProjectName(projH: int) -> str:
    """Get project name."""
    ...

def AEGP_GetProjectPath(plugin_id: int, projH: int) -> str:
    """Get the file path of a project (UTF-16 encoded)."""
    ...

def AEGP_GetProjectRootFolder(projH: int) -> int:
    """Get the root folder item of a project. Returns: itemH (int)"""
    ...

def AEGP_SaveProjectToPath(projH: int, path: str) -> None:
    """Save project to the specified path."""
    ...

def AEGP_GetProjectTimeDisplay(projH: int) -> dict:
    """
    Get the time display settings of a project.
    Returns dict with keys:
    - display_mode: int
    - footage_display_mode: int
    - display_dropframe: bool
    - use_feet_frames: bool
    - timebase: int
    - frames_per_foot: int
    - frames_display_mode: int
    """
    ...

def AEGP_SetProjectTimeDisplay(projH: int, time_display: dict) -> None:
    """
    Set the time display settings of a project (UNDOABLE).
    time_display dict should contain the same keys as returned by AEGP_GetProjectTimeDisplay.
    """
    ...

def AEGP_ProjectIsDirty(projH: int) -> bool:
    """Check if a project has been modified since last save."""
    ...

def AEGP_SaveProjectAs(projH: int, path: str) -> None:
    """Save project with a new path."""
    ...

def AEGP_NewProject() -> int:
    """Create a new project. Returns: projH (int)"""
    ...

def AEGP_OpenProjectFromPath(path: str) -> int:
    """
    Open a project from file path.
    WARNING: Will close any open projects!
    Returns: projH (int)
    """
    ...

def AEGP_GetProjectBitDepth(projH: int) -> int:
    """Get the bit depth of a project (8, 16, or 32)."""
    ...

def AEGP_SetProjectBitDepth(projH: int, bit_depth: int) -> None:
    """Set the bit depth of a project (UNDOABLE)."""
    ...

# AEGP_ItemSuite
def AEGP_GetActiveItem() -> int:
    """Get active item handle (or 0 if none)."""
    ...

def AEGP_GetItemType(itemH: int) -> int:
    """Get item type."""
    ...

def AEGP_GetItemName(plugin_id: int, itemH: int) -> str:
    """Get item name."""
    ...

def AEGP_GetItemDuration(itemH: int) -> float:
    """Get item duration in seconds."""
    ...

def AEGP_GetItemFlags(itemH: int) -> int:
    """Get item flags."""
    ...

def AEGP_GetItemDimensions(itemH: int) -> tuple[int, int]:
    """Get item dimensions (width, height)."""
    ...

def AEGP_GetFirstProjItem(projectH: int) -> int:
    """Get the first project item.

    Returns:
        int: Item handle
    """
    ...

def AEGP_GetNextProjItem(projectH: int, itemH: int) -> int:
    """Get the next project item. Returns 0 after last item.

    Args:
        projectH: Project handle
        itemH: Current item handle

    Returns:
        int: Next item handle (0 after last item)
    """
    ...

def AEGP_GetItemID(itemH: int) -> int:
    """Get item ID (unique identifier).

    Args:
        itemH: Item handle

    Returns:
        int: Item ID
    """
    ...

def AEGP_DeleteItem(itemH: int) -> None:
    """Delete item (UNDOABLE). Removes item from all comps.

    Args:
        itemH: Item handle
    """
    ...

def AEGP_SetItemName(itemH: int, name: str) -> None:
    """Set item name (UNDOABLE).

    Args:
        itemH: Item handle
        name: New item name
    """
    ...

def AEGP_SelectItem(itemH: int, select: bool, deselect_others: bool) -> None:
    """Select or deselect item.

    Args:
        itemH: Item handle
        select: True to select, False to deselect
        deselect_others: If True, deselect all other items
    """
    ...

def AEGP_IsItemSelected(itemH: int) -> bool:
    """Check if item is selected.

    Args:
        itemH: Item handle

    Returns:
        bool: True if selected
    """
    ...

def AEGP_GetItemParentFolder(itemH: int) -> int:
    """Get parent folder of item. Returns 0 if item is at root.

    Args:
        itemH: Item handle

    Returns:
        int: Parent folder handle (0 if at root)
    """
    ...

def AEGP_SetItemParentFolder(itemH: int, parent_folder_itemH: int) -> None:
    """Set parent folder of item (UNDOABLE).

    Args:
        itemH: Item handle
        parent_folder_itemH: Parent folder handle
    """
    ...

def AEGP_GetItemPixelAspectRatio(itemH: int) -> tuple[int, int]:
    """Get pixel aspect ratio as (numerator, denominator) tuple.

    Args:
        itemH: Item handle

    Returns:
        tuple[int, int]: (numerator, denominator)
    """
    ...

def AEGP_GetItemComment(plugin_id: int, itemH: int) -> str:
    """Get item comment.

    Args:
        plugin_id: Plugin ID
        itemH: Item handle

    Returns:
        str: Item comment
    """
    ...

def AEGP_SetItemComment(itemH: int, comment: str) -> None:
    """Set item comment (UNDOABLE).

    Args:
        itemH: Item handle
        comment: Comment text
    """
    ...

def AEGP_GetItemLabel(itemH: int) -> int:
    """Get item label color.

    Args:
        itemH: Item handle

    Returns:
        int: Label ID
    """
    ...

def AEGP_SetItemLabel(itemH: int, label: int) -> None:
    """Set item label color (UNDOABLE).

    Args:
        itemH: Item handle
        label: Label ID
    """
    ...

def AEGP_GetItemCurrentTime(itemH: int) -> float:
    """Get item current time (not updated while rendering).

    Args:
        itemH: Item handle

    Returns:
        float: Current time in seconds
    """
    ...

def AEGP_SetItemCurrentTime(itemH: int, time_seconds: float) -> None:
    """Set item current time (UNDOABLE).

    Args:
        itemH: Item handle
        time_seconds: Time in seconds
    """
    ...

def AEGP_SetItemUseProxy(itemH: int, use_proxy: bool) -> None:
    """Set whether to use proxy (UNDOABLE). Error if item has no proxy.

    Args:
        itemH: Item handle
        use_proxy: True to use proxy, False otherwise
    """
    ...

def AEGP_CreateNewFolder(name: str, parent_folderH: int) -> int:
    """Create new folder. Pass 0 for parent to create at root.

    Args:
        name: Folder name
        parent_folderH: Parent folder handle (0 for root)

    Returns:
        int: New folder handle
    """
    ...

def AEGP_GetTypeName(item_type: int) -> str:
    """Get type name string from item type constant.

    Args:
        item_type: Item type constant

    Returns:
        str: Type name
    """
    ...

def AEGP_GetItemMRUView(itemH: int) -> int:
    """Get most recently used view for item. Returns 0 if no view available.

    Args:
        itemH: Item handle

    Returns:
        int: Item view pointer (0 if no view available)
    """
    ...

# AEGP_CompSuite
def AEGP_GetCompFromItem(itemH: int) -> int:
    """Get comp handle from item handle."""
    ...

def AEGP_GetItemFromComp(compH: int) -> int:
    """Get item handle from comp handle."""
    ...

def AEGP_GetCompFlags(compH: int) -> int:
    """Get comp flags."""
    ...

def AEGP_GetCompBGColor(compH: int) -> tuple[float, float, float]:
    """Get comp background color (r, g, b) as 0.0-1.0 floats."""
    ...

def AEGP_SetCompBGColor(compH: int, r: float, g: float, b: float) -> None:
    """Set comp background color (r, g, b) as 0.0-1.0 floats."""
    ...

def AEGP_GetCompFramerate(compH: int) -> float:
    """Get comp framerate (fps)."""
    ...

def AEGP_GetCompDownSampleFactor(compH: int) -> tuple[int, int]:
    """Get comp downsample factor (x, y)."""
    ...

def AEGP_SetCompDownSampleFactor(compH: int, x: int, y: int) -> None:
    """Set comp downsample factor."""
    ...

def AEGP_SetCompFrameRate(compH: int, fps: float) -> None:
    """Set comp framerate (fps)."""
    ...

def AEGP_SetCompFrameDuration(compH: int, duration: float) -> None:
    """Set comp frame duration (1/framerate) in seconds."""
    ...

def AEGP_SetCompPixelAspectRatio(compH: int, num: int, den: int) -> None:
    """Set comp pixel aspect ratio."""
    ...

def AEGP_SetCompDimensions(compH: int, width: int, height: int) -> None:
    """Set comp dimensions."""
    ...

def AEGP_DuplicateComp(compH: int) -> int:
    """Duplicate composition. Returns new compH."""
    ...

def AEGP_GetCompFrameDuration(compH: int) -> float:
    """Get frame duration in seconds."""
    ...

def AEGP_GetMostRecentlyUsedComp() -> int:
    """Get most recently used comp handle (or 0)."""
    ...

# Display Settings
def AEGP_GetShowLayerNameOrSourceName(compH: int) -> bool:
    """Get whether to show layer names (True) or source names (False)."""
    ...

def AEGP_SetShowLayerNameOrSourceName(compH: int, show_layer_names: bool) -> None:
    """Set whether to show layer names (True) or source names (False)."""
    ...

def AEGP_GetShowBlendModes(compH: int) -> bool:
    """Get whether to show blend modes in timeline."""
    ...

def AEGP_SetShowBlendModes(compH: int, show_blend_modes: bool) -> None:
    """Set whether to show blend modes in timeline."""
    ...

def AEGP_GetCompDisplayStartTime(compH: int) -> float:
    """Get display start time in seconds."""
    ...

def AEGP_SetCompDisplayStartTime(compH: int, seconds: float) -> None:
    """Set display start time in seconds."""
    ...

def AEGP_GetCompDisplayDropFrame(compH: int) -> bool:
    """Get drop frame display state."""
    ...

def AEGP_SetCompDisplayDropFrame(compH: int, drop_frame: bool) -> None:
    """Set drop frame display state."""
    ...

# Motion Blur Settings
def AEGP_GetCompShutterAnglePhase(compH: int) -> tuple[float, float]:
    """Get shutter angle and phase. Returns (angle, phase)."""
    ...

def AEGP_GetCompShutterFrameRange(compH: int, comp_time: float) -> tuple[float, float]:
    """Get shutter frame range. Returns (start, duration)."""
    ...

def AEGP_GetCompSuggestedMotionBlurSamples(compH: int) -> int:
    """Get suggested motion blur samples."""
    ...

def AEGP_SetCompSuggestedMotionBlurSamples(compH: int, samples: int) -> None:
    """Set suggested motion blur samples."""
    ...

def AEGP_GetCompMotionBlurAdaptiveSampleLimit(compH: int) -> int:
    """Get motion blur adaptive sample limit."""
    ...

def AEGP_SetCompMotionBlurAdaptiveSampleLimit(compH: int, samples: int) -> None:
    """Set motion blur adaptive sample limit."""
    ...

# Layer Creation Functions
def AEGP_CreateSolidInComp(
    compH: int, name: str, width: int, height: int, red: float, green: float, blue: float, duration: float
) -> int:
    """Create solid layer in comp. Returns layerH."""
    ...

def AEGP_CreateCameraInComp(compH: int, name: str) -> int:
    """Create camera layer in comp. Returns layerH."""
    ...

def AEGP_CreateLightInComp(compH: int, name: str, light_type: int) -> int:
    """Create light layer in comp. Returns layerH."""
    ...

def AEGP_CreateNullInComp(compH: int, name: str, duration: float) -> int:
    """Create null layer in comp. Returns layerH."""
    ...

def AEGP_CreateComp(
    parentFolderH: int, name: str, width: int, height: int,
    pixel_aspect_num: int, pixel_aspect_den: int, duration: float, framerate: float
) -> int:
    """Create new composition. Returns compH."""
    ...

def AEGP_CreateTextLayerInComp(compH: int, select_new_layer: bool, horizontal: bool) -> int:
    """Create text layer in comp. Returns layerH."""
    ...

def AEGP_CreateBoxTextLayerInComp(
    compH: int, select_new_layer: bool, box_width: float, box_height: float, horizontal: bool
) -> int:
    """Create box text layer in comp. Returns layerH."""
    ...

def AEGP_CreateVectorLayerInComp(compH: int) -> int:
    """Create vector (shape) layer in comp. Returns layerH."""
    ...

# Work Area Functions
def AEGP_GetCompWorkAreaStart(compH: int) -> float:
    """Get work area start time in seconds."""
    ...

def AEGP_GetCompWorkAreaDuration(compH: int) -> float:
    """Get work area duration in seconds."""
    ...

def AEGP_SetCompWorkAreaStartAndDuration(compH: int, start: float, duration: float) -> None:
    """Set work area start and duration in seconds."""
    ...

# Selection/Collection Functions
def AEGP_GetNewCollectionFromCompSelection(compH: int) -> int:
    """Get collection from comp selection. Returns collectionH."""
    ...

def AEGP_SetSelection(compH: int, collectionH: int) -> None:
    """Set layer selection from collection."""
    ...

def AEGP_ReorderCompSelection(compH: int, index: int) -> None:
    """Reorder comp selection."""
    ...

# Marker Functions
def AEGP_GetNewCompMarkerStream(compH: int) -> int:
    """Get comp marker stream. Returns streamH."""
    ...

def AEGP_SetCompDuration(compH: int, duration: float) -> None:
    """Set comp duration in seconds."""
    ...

# AEGP_LayerSuite
def AEGP_GetCompNumLayers(compH: int) -> int:
    """Get number of layers in comp."""
    ...

def AEGP_GetCompLayerByIndex(compH: int, index: int) -> int:
    """Get layer handle by index."""
    ...

def AEGP_GetLayerIndex(layerH: int) -> int:
    """Get layer index."""
    ...

def AEGP_GetLayerSourceItem(layerH: int) -> int:
    """Get layer source item handle (or 0)."""
    ...

def AEGP_GetLayerName(plugin_id: int, layerH: int) -> str:
    """Get layer name."""
    ...

def AEGP_GetLayerParent(layerH: int) -> int:
    """Get parent layer handle (or 0)."""
    ...

def AEGP_SetLayerParent(layerH: int, parentLayerH: int) -> None:
    """Set parent layer."""
    ...

def AEGP_GetLayerFlags(layerH: int) -> int:
    """Get layer flags."""
    ...

def AEGP_SetLayerFlag(layerH: int, flag: int, value: int) -> None:
    """Set layer flag."""
    ...

def AEGP_ReorderLayer(layerH: int, index: int) -> None:
    """Reorder layer to index."""
    ...

# AEGP_KeyframeSuite (Additional)
def AEGP_GetStreamNumKfs(streamH: int) -> int:
    """
    Get number of keyframes in stream.
    Note: AE 2025 renamed to AEGP_GetStreamNumKFs (capital 'KFs')
    """
    ...

def AEGP_GetKeyframeTime(streamH: int, keyIndex: int, time_mode: int) -> float:
    """Get keyframe time in seconds."""
    ...

def AEGP_GetKeyframeValue_Marker(plugin_id: int, streamH: int, keyIndex: int) -> int:
    """Get keyframe marker value handle."""
    ...

# AEGP_CommandSuite
def AEGP_GetUniqueCommand() -> int:
    """Get a unique command ID."""
    ...

def AEGP_InsertMenuCommand(command: int, name: str, menu_id: int, after_item: int) -> None:
    """Insert a menu command into the specified menu.

    Args:
        command: Unique command ID (from AEGP_GetUniqueCommand)
        name: Menu item display name
        menu_id: Which menu to add to (AEGP_Menu_FILE=2, AEGP_Menu_EDIT=3, etc.)
        after_item: Insertion position:
            -2 (AEGP_MENU_INSERT_SORTED): Auto-sort alphabetically
            -1 (AEGP_MENU_INSERT_AT_BOTTOM): Add at end
             0 (AEGP_MENU_INSERT_AT_TOP): Add at beginning
            >=1: Insert after item at index
    """
    ...

def AEGP_RemoveMenuCommand(command: int) -> None:
    """Remove a menu command."""
    ...

def AEGP_SetMenuCommandName(command: int, name: str) -> None:
    """Set the name of a menu command.

    Args:
        command: Command ID
        name: New display name
    """
    ...

def AEGP_EnableCommand(command: int) -> None:
    """Enable a command."""
    ...

def AEGP_DisableCommand(command: int) -> None:
    """Disable a command."""
    ...

def AEGP_CheckMarkMenuCommand(command: int, check: bool) -> None:
    """Set or clear checkmark on a menu command.

    Args:
        command: Command ID
        check: True to show checkmark, False to hide
    """
    ...

def AEGP_DoCommand(command: int) -> None:
    """Execute a command."""
    ...

# AEGP_UtilitySuite6
def AEGP_StartUndoGroup(undo_name: str) -> None:
    """Start an undo group. Must be balanced with AEGP_EndUndoGroup()."""
    ...

def AEGP_EndUndoGroup() -> None:
    """End an undo group."""
    ...

def AEGP_ExecuteScript(script: str) -> str:
    """
    Execute ExtendScript code and return the result.

    Args:
        script: ExtendScript code to execute

    Returns:
        Result string from script execution

    Raises:
        RuntimeError: If script execution fails
    """
    ...

def AEGP_GetPluginID() -> int:
    """Get the plugin ID required for many SDK calls."""
    ...

# Report/Dialog Functions
def AEGP_ReportInfo(plugin_id: int, info_string: str) -> None:
    """
    Display an information dialog with the plugin name and message.

    Args:
        plugin_id: Plugin ID
        info_string: Message to display
    """
    ...

def AEGP_ReportInfoUnicode(plugin_id: int, info_string: str) -> None:
    """
    Display an information dialog with Unicode string support.

    Args:
        plugin_id: Plugin ID
        info_string: Message to display (UTF-8 encoded)
    """
    ...

# Version Functions
def AEGP_GetDriverPluginInitFuncVersion() -> Tuple[int, int]:
    """
    Get the driver plugin initialization function version.

    Returns:
        Tuple of (major_version, minor_version)
    """
    ...

def AEGP_GetDriverImplementationVersion() -> Tuple[int, int]:
    """
    Get the driver implementation version.

    Returns:
        Tuple of (major_version, minor_version)
    """
    ...

# Error Handling Functions
def AEGP_StartQuietErrors() -> int:
    """
    Start suppressing error dialogs.

    Returns:
        Error state token to pass to AEGP_EndQuietErrors()
    """
    ...

def AEGP_EndQuietErrors(report_quieted_errors: bool, err_state: int) -> None:
    """
    End error suppression and optionally report the last quieted error.

    Args:
        report_quieted_errors: If True, report the last error that was suppressed
        err_state: Error state token from AEGP_StartQuietErrors()
    """
    ...

def AEGP_GetLastErrorMessage() -> Tuple[str, int]:
    """
    Get the last error message and error number.

    Returns:
        Tuple of (error_message, error_number)
    """
    ...

# Registration and Platform Functions
def AEGP_RegisterWithAEGP(global_refcon: int, plugin_name: str) -> int:
    """
    Register a plugin with After Effects.

    Args:
        global_refcon: Global reference constant
        plugin_name: Name of the plugin (max AEGP_MAX_PLUGIN_NAME_SIZE)

    Returns:
        Plugin ID to use in other AEGP calls
    """
    ...

def AEGP_GetMainHWND() -> int:
    """
    Get the main window handle.

    Returns:
        Window handle (HWND on Windows) as uintptr_t
    """
    ...

def AEGP_ShowHideAllFloaters(include_tool_pal: bool) -> None:
    """
    Toggle visibility of all floating palettes.

    Args:
        include_tool_pal: If True, include tool palettes
    """
    ...

# Paint Palette Functions
def AEGP_PaintPalGetForeColor() -> Dict[str, float]:
    """
    Get the foreground color from the paint palette.

    Returns:
        Dictionary with keys: red, green, blue, alpha (all floats 0.0-1.0)
    """
    ...

def AEGP_PaintPalGetBackColor() -> Dict[str, float]:
    """
    Get the background color from the paint palette.

    Returns:
        Dictionary with keys: red, green, blue, alpha (all floats 0.0-1.0)
    """
    ...

def AEGP_PaintPalSetForeColor(color: Dict[str, float]) -> None:
    """
    Set the foreground color in the paint palette.

    Args:
        color: Dictionary with keys: red, green, blue, alpha (all floats 0.0-1.0)
    """
    ...

def AEGP_PaintPalSetBackColor(color: Dict[str, float]) -> None:
    """
    Set the background color in the paint palette.

    Args:
        color: Dictionary with keys: red, green, blue, alpha (all floats 0.0-1.0)
    """
    ...

# Character Palette Functions
def AEGP_CharPalGetFillColor() -> Tuple[bool, Dict[str, float]]:
    """
    Get the text fill color.

    Returns:
        Tuple of (is_defined, color_dict)
        - is_defined: True if fill color is defined
        - color_dict: Dictionary with keys: red, green, blue, alpha (only valid if is_defined is True)
    """
    ...

def AEGP_CharPalGetStrokeColor() -> Tuple[bool, Dict[str, float]]:
    """
    Get the text stroke color.

    Returns:
        Tuple of (is_defined, color_dict)
        - is_defined: True if stroke color is defined
        - color_dict: Dictionary with keys: red, green, blue, alpha (only valid if is_defined is True)
    """
    ...

def AEGP_CharPalSetFillColor(color: Dict[str, float]) -> None:
    """
    Set the text fill color.

    Args:
        color: Dictionary with keys: red, green, blue, alpha (all floats 0.0-1.0)
    """
    ...

def AEGP_CharPalSetStrokeColor(color: Dict[str, float]) -> None:
    """
    Set the text stroke color.

    Args:
        color: Dictionary with keys: red, green, blue, alpha (all floats 0.0-1.0)
    """
    ...

def AEGP_CharPalIsFillColorUIFrontmost() -> bool:
    """
    Check if fill color UI is frontmost.

    Returns:
        True if fill color is frontmost, False if stroke color is frontmost
    """
    ...

# Conversion Functions
def AEGP_ConvertFpLongToHSFRatio(number: float) -> Tuple[int, int]:
    """
    Convert floating point number to HSF ratio.

    Args:
        number: Floating point number

    Returns:
        Tuple of (numerator, denominator)
    """
    ...

def AEGP_ConvertHSFRatioToFpLong(numerator: int, denominator: int) -> float:
    """
    Convert HSF ratio to floating point number.

    Args:
        numerator: Ratio numerator
        denominator: Ratio denominator

    Returns:
        Floating point number
    """
    ...

# Miscellaneous Utility Functions
def AEGP_CauseIdleRoutinesToBeCalled() -> None:
    """
    Request that idle routines be called.

    This routine is thread-safe and asynchronous. It will return before
    the idle handler is called.
    """
    ...

def AEGP_GetSuppressInteractiveUI() -> bool:
    """
    Check if interactive UI is suppressed (headless/service mode).

    Returns:
        True if running in non-interactive mode (no UI)
    """
    ...

def AEGP_WriteToOSConsole(text: str) -> None:
    """
    Write text to the OS console.

    Console is guaranteed to be available when UI is suppressed.
    Use AEGP_ReportInfo() for general messaging as it adapts to the mode.

    Args:
        text: Text to write to console
    """
    ...

def AEGP_WriteToDebugLog(subsystem: str, event_type: str, info: str) -> None:
    """
    Write an entry to the debug log.

    Writes to debug log or command line if launched with -debug flag.

    Args:
        subsystem: Subsystem identifier
        event_type: Event type identifier
        info: Information to log
    """
    ...

def AEGP_IsScriptingAvailable() -> bool:
    """
    Check if scripting functionality is available.

    Returns:
        True if scripting is available
    """
    ...

def AEGP_HostIsActivated() -> bool:
    """
    Check if After Effects is activated (licensed).

    Returns:
        True if After Effects is activated
    """
    ...

def AEGP_GetPluginPlatformRef(plugin_id: int) -> int:
    """
    Get the plugin platform reference.

    Args:
        plugin_id: Plugin ID

    Returns:
        Platform reference as uintptr_t
        (CFBundleRef on Mac for mach-o plugins, NULL for CFM/Windows)
    """
    ...

def AEGP_UpdateFontList() -> None:
    """
    Rescan the system font list.

    Returns quickly if the font list hasn't changed.
    """
    ...

def AEGP_GetPluginPaths(plugin_id: int, path_type: int) -> str:
    """
    Get a path associated with the plugin.

    Args:
        plugin_id: Plugin ID
        path_type: AEGP_GetPathTypes enum value
            - AEGP_GetPathTypes_PLUGIN (0): Plugin file path
            - AEGP_GetPathTypes_USER_PLUGIN (1): User plugin directory
            - AEGP_GetPathTypes_ALLUSER_PLUGIN (2): All users plugin directory
            - AEGP_GetPathTypes_APP (3): Application directory

    Returns:
        Path as UTF-8 string
    """
    ...

# AEGP_StreamSuite (Extended)
def AEGP_GetNewStreamValue(
    plugin_id: int, streamH: int, time_mode: int, time: float, pre_expression: bool
) -> int:
    """
    Get stream value at time. Returns pointer (uintptr_t) to heap-allocated AEGP_StreamValue2.
    Must be disposed with AEGP_DisposeStreamValue.
    """
    ...

def AEGP_DisposeStreamValue(valueP: int) -> None:
    """Dispose stream value pointer and free memory."""
    ...

def AEGP_SetStreamValue(plugin_id: int, streamH: int, valueP: int) -> None:
    """Set stream value (only valid when no keyframes)."""
    ...

def AEGP_GetStreamName(streamH: int, force_english: bool) -> str:
    """Get stream display name."""
    ...

def AEGP_GetStreamType(streamH: int) -> int:
    """Get stream type enum (AEGP_StreamType)."""
    ...

def AEGP_GetStreamProperties(streamH: int) -> dict:
    """
    Get stream properties.
    Returns dict with keys: 'flags' (int), 'min' (float), 'max' (float)
    """
    ...

# Stream Type Constants
AEGP_StreamType_NO_DATA: int
AEGP_StreamType_TwoD: int
AEGP_StreamType_TwoD_SPATIAL: int
AEGP_StreamType_ThreeD: int
AEGP_StreamType_ThreeD_SPATIAL: int
AEGP_StreamType_COLOR: int
AEGP_StreamType_MARKER: int
AEGP_StreamType_LAYER_ID: int
AEGP_StreamType_MASK_ID: int
AEGP_StreamType_MASK: int
AEGP_StreamType_TEXT_DOCUMENT: int

# Stream Grouping Type Constants (DynamicStreamSuite)
AEGP_StreamGroupingType_NONE: int
AEGP_StreamGroupingType_LEAF: int
AEGP_StreamGroupingType_NAMED_GROUP: int
AEGP_StreamGroupingType_INDEXED_GROUP: int

# DynamicStreamSuite4 - Property Tree Traversal
def AEGP_GetNewStreamRefForLayer(layerH: int) -> int:
    """
    Get root stream reference for layer's property tree.
    Essential for recursive property serialization.

    Args:
        layerH: Layer handle

    Returns:
        Stream handle (must be disposed with AEGP_DisposeStream)

    Example:
    ```python
    def serialize_layer_properties(layer):
        root_stream = ae.sdk.AEGP_GetNewStreamRefForLayer(layer._handle)
        properties = traverse_property_tree(root_stream)
        ae.sdk.AEGP_DisposeStream(root_stream)
        return properties
    ```
    """
    ...

def AEGP_GetNewStreamRefForMask(maskH: int) -> int:
    """
    Get root stream reference for mask's property tree.

    Args:
        maskH: Mask handle

    Returns:
        Stream handle (must be disposed with AEGP_DisposeStream)
    """
    ...

def AEGP_GetNewParentStreamRef(streamH: int) -> int:
    """
    Get parent stream reference, navigating up the property tree.

    Args:
        streamH: Child stream handle

    Returns:
        Parent stream handle (must be disposed with AEGP_DisposeStream)

    Example:
    ```python
    # Navigate from Position property to Transform group
    pos_stream = layer.position._streamH
    transform_stream = ae.sdk.AEGP_GetNewParentStreamRef(pos_stream)
    ae.sdk.AEGP_DisposeStream(transform_stream)
    ```
    """
    ...

def AEGP_GetStreamGroupingType(streamH: int) -> int:
    """
    Determine if stream is a leaf property, named group, or indexed group.
    Critical for property tree traversal logic.

    Args:
        streamH: Stream handle

    Returns:
        AEGP_StreamGroupingType enum value:
        - AEGP_StreamGroupingType_LEAF: Single property (e.g., Opacity)
        - AEGP_StreamGroupingType_NAMED_GROUP: Named properties (e.g., Transform)
        - AEGP_StreamGroupingType_INDEXED_GROUP: Array-like (e.g., Masks, Effects)

    Example:
    ```python
    group_type = ae.sdk.AEGP_GetStreamGroupingType(stream_h)
    if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
        # Serialize leaf value
        value = ae.sdk.AEGP_GetNewStreamValue(...)
    elif group_type == ae.sdk.AEGP_StreamGroupingType_NAMED_GROUP:
        # Recurse into children
        num_children = ae.sdk.AEGP_GetNumStreamsInGroup(stream_h)
    ```
    """
    ...

def AEGP_GetNumStreamsInGroup(streamH: int) -> int:
    """
    Get number of child streams in a group.
    Returns error for leaf streams.

    Args:
        streamH: Group stream handle

    Returns:
        Number of child streams

    Note:
        Only valid for NAMED_GROUP and INDEXED_GROUP types.
        Will throw for LEAF type.
    """
    ...

def AEGP_GetNewStreamRefByIndex(parentStreamH: int, index: int) -> int:
    """
    Get child stream by index.
    Legal for NAMED_GROUP and INDEXED_GROUP, not for LEAF.

    Args:
        parentStreamH: Parent group stream handle
        index: Child index (0-based)

    Returns:
        Child stream handle (must be disposed with AEGP_DisposeStream)

    Example - Recursive property tree traversal:
    ```python
    def traverse_property_tree(stream_h, depth=0):
        group_type = ae.sdk.AEGP_GetStreamGroupingType(stream_h)

        if group_type == ae.sdk.AEGP_StreamGroupingType_LEAF:
            # Leaf node - get value
            name = ae.sdk.AEGP_GetStreamName(stream_h, False)
            value = ae.sdk.AEGP_GetNewStreamValue(...)
            return {name: value}
        else:
            # Group node - recurse
            num_children = ae.sdk.AEGP_GetNumStreamsInGroup(stream_h)
            children = {}
            for i in range(num_children):
                child_h = ae.sdk.AEGP_GetNewStreamRefByIndex(stream_h, i)
                child_data = traverse_property_tree(child_h, depth + 1)
                children.update(child_data)
                ae.sdk.AEGP_DisposeStream(child_h)
            return children
    ```
    """
    ...

# Stream Value Helpers - Type-Specific Getters/Setters
def AEGP_GetStreamValue_Double(valueP: int) -> float:
    """Extract 1D value from stream value pointer."""
    ...

def AEGP_SetStreamValue_Double(valueP: int, val: float) -> None:
    """Set 1D value in stream value pointer."""
    ...

def AEGP_GetStreamValue_Color(valueP: int) -> tuple[float, float, float, float]:
    """Extract RGBA color (0.0-1.0) from stream value pointer."""
    ...

def AEGP_SetStreamValue_Color(valueP: int, r: float, g: float, b: float, a: float) -> None:
    """Set RGBA color in stream value pointer."""
    ...

def AEGP_GetStreamValue_Point(valueP: int) -> tuple[float, float]:
    """Extract 2D point (x, y) from stream value pointer."""
    ...

def AEGP_SetStreamValue_Point(valueP: int, x: float, y: float) -> None:
    """Set 2D point in stream value pointer."""
    ...

def AEGP_GetStreamValue_ThreeD(valueP: int) -> tuple[float, float, float]:
    """Extract 3D point (x, y, z) from stream value pointer."""
    ...

def AEGP_SetStreamValue_ThreeD(valueP: int, x: float, y: float, z: float) -> None:
    """Set 3D point in stream value pointer."""
    ...

# AEGP_KeyframeSuite (Extended)
def AEGP_DeleteKeyframe(streamH: int, keyIndex: int) -> None:
    """Delete a keyframe."""
    ...

def AEGP_GetKeyframeFlags(streamH: int, keyIndex: int) -> int:
    """Get keyframe flags."""
    ...

def AEGP_SetKeyframeFlag(streamH: int, keyIndex: int, flag: int, value: bool) -> None:
    """Set a keyframe flag."""
    ...

def AEGP_GetNewKeyframeValue(plugin_id: int, streamH: int, keyIndex: int) -> int:
    """
    Get keyframe value. Returns pointer (uintptr_t) to heap-allocated AEGP_StreamValue2.
    Must be disposed with AEGP_DisposeStreamValue.
    """
    ...

def AEGP_SetKeyframeValue(streamH: int, keyIndex: int, valueP: int) -> None:
    """Set keyframe value using stream value pointer."""
    ...

# Keyframe Flags (AE 2025: AUTO_BEZIER → AUTOBEZIER)
AEGP_KeyframeFlag_NONE: int
AEGP_KeyframeFlag_TEMPORAL_CONTINUOUS: int
AEGP_KeyframeFlag_TEMPORAL_AUTOBEZIER: int  # AE 2025: renamed from AUTO_BEZIER
AEGP_KeyframeFlag_SPATIAL_CONTINUOUS: int
AEGP_KeyframeFlag_SPATIAL_AUTOBEZIER: int   # AE 2025: renamed from AUTO_BEZIER
AEGP_KeyframeFlag_ROVING: int

# AEGP_FootageSuite
def AEGP_GetMainFootageFromItem(itemH: int) -> int:
    """Get main footage handle from item. Returns FootageH."""
    ...

def AEGP_GetProxyFootageFromItem(itemH: int) -> int:
    """Get proxy footage handle from item. Returns FootageH. Raises error if no proxy."""
    ...

def AEGP_GetFootageNumFiles(footageH: int) -> tuple[int, int]:
    """Get number of files and files per frame. Returns (num_files, files_per_frame)."""
    ...

def AEGP_GetFootagePath(
    plugin_id: int, footageH: int, frame_num: int, file_index: int
) -> str:
    """Get footage file path for specified frame and file index."""
    ...

def AEGP_GetFootageSignature(footageH: int) -> int:
    """Get footage signature (NONE, MISSING, or SOLID)."""
    ...

def AEGP_NewFootage(plugin_id: int, path: str, interp_style: int) -> int:
    """Create new footage from file path. Returns FootageH."""
    ...

def AEGP_AddFootageToProject(footageH: int, folderH: int) -> int:
    """Add footage to project in specified folder. Returns ItemH."""
    ...

def AEGP_ReplaceItemMainFootage(footageH: int, itemH: int) -> None:
    """Replace item's main footage."""
    ...

def AEGP_SetItemProxyFootage(footageH: int, itemH: int) -> None:
    """Set item's proxy footage."""
    ...

def AEGP_DisposeFootage(footageH: int) -> None:
    """Dispose footage handle (only if not added to project)."""
    ...

def AEGP_NewSolidFootage(name: str, width: int, height: int, color_rgba: tuple) -> int:
    """Create new solid footage. color_rgba is (r,g,b,[a]). Returns FootageH."""
    ...

# Footage Signature Constants
AEGP_FootageSignature_NONE: int
AEGP_FootageSignature_MISSING: int
AEGP_FootageSignature_SOLID: int

# Interpretation Style Constants
AEGP_InterpretationStyle_NO_DIALOG_GUESS: int
AEGP_InterpretationStyle_DIALOG_OK: int
AEGP_InterpretationStyle_NO_DIALOG_NO_GUESS: int

# AEGP_EffectSuite
def AEGP_GetLayerNumEffects(layerH: int) -> int:
    """Get number of effects applied to layer."""
    ...

def AEGP_GetLayerEffectByIndex(plugin_id: int, layerH: int, effect_index: int) -> int:
    """Get effect handle by index. Returns EffectRefH."""
    ...

def AEGP_GetInstalledKeyFromLayerEffect(effectH: int) -> int:
    """Get installed effect key from effect handle. Returns key as int."""
    ...

def AEGP_GetEffectFlags(effectH: int) -> int:
    """Get effect flags."""
    ...

def AEGP_SetEffectFlags(effectH: int, mask: int, flags: int) -> None:
    """Set effect flags with mask."""
    ...

def AEGP_ReorderEffect(effectH: int, effect_index: int) -> None:
    """Change effect order to specified index."""
    ...

def AEGP_DisposeEffect(effectH: int) -> None:
    """Dispose effect handle."""
    ...

def AEGP_ApplyEffect(plugin_id: int, layerH: int, installed_key: int) -> int:
    """Apply effect to layer using installed key. Returns EffectRefH."""
    ...

def AEGP_DeleteLayerEffect(effectH: int) -> None:
    """Delete effect from layer."""
    ...

def AEGP_GetNumInstalledEffects() -> int:
    """Get total number of installed effects."""
    ...

def AEGP_GetNextInstalledEffect(installed_key: int) -> int:
    """Get next installed effect key. Pass AEGP_InstalledEffectKey_NONE to start."""
    ...

def AEGP_GetEffectName(installed_key: int) -> str:
    """Get effect name."""
    ...

def AEGP_GetEffectMatchName(installed_key: int) -> str:
    """Get effect match name (internal name)."""
    ...

def AEGP_GetEffectCategory(installed_key: int) -> str:
    """Get effect category."""
    ...

def AEGP_DuplicateEffect(effectH: int) -> int:
    """Duplicate effect handle. Returns new EffectRefH."""
    ...

# Effect Masks (AE 13.0+)
def AEGP_NumEffectMask(effectH: int) -> int:
    """Get number of masks assigned to an effect."""
    ...

def AEGP_GetEffectMaskID(effectH: int, mask_index: int) -> int:
    """Get mask ID at specified index."""
    ...

def AEGP_AddEffectMask(effectH: int, id_val: int) -> int:
    """Add a mask to the effect. Returns stream handle."""
    ...

def AEGP_RemoveEffectMask(effectH: int, id_val: int) -> None:
    """Remove a mask from the effect."""
    ...

def AEGP_SetEffectMask(effectH: int, mask_index: int, id_val: int) -> int:
    """Set mask at specified index. Returns stream handle."""
    ...

def AEGP_GetIsInternalEffect(installed_key: int) -> bool:
    """Check if effect is an internal (built-in) effect."""
    ...

def AEGP_GetEffectParamUnionByIndex(plugin_id: int, effectH: int, param_index: int) -> dict:
    """
    Get effect parameter definition union.

    Args:
        plugin_id: Plugin ID
        effectH: Effect handle
        param_index: Parameter index (0-based, 0 is input layer)

    Returns:
        Dictionary with:
        - param_type: PF_ParamType integer value
        - param_type_name: Human-readable type name
        - warning: Reminder not to use values from this union

    NOTE: DO NOT USE THE VALUES FROM THIS UNION!
    This returns the parameter definition structure, not actual values.
    Use AEGP_GetNewEffectStreamByIndex + AEGP_GetNewStreamValue for actual parameter values.

    FUTURE USE - Effect Serialization:
    This function is useful for building effect serialization/deserialization systems.
    Primary use cases:

    1. Detect PF_Param_ARBITRARY_DATA - Custom plugin data requiring special handling
       Example: Plugin-specific settings, internal state, custom structures

    2. Identify parameter structure - Find GROUP_START/END, BUTTON, NO_DATA parameters
       to understand the effect's UI organization

    3. Build serialization strategies - Determine which parameters need custom serializers
       vs. standard value serialization via StreamSuite

    Example - Building a serialization strategy:
    ```python
    def get_serialization_strategy(effect):
        strategies = {
            'simple_params': [],      # Standard value serialization
            'arbitrary_data': [],     # Custom serialization required
            'skip': []               # No data to serialize (BUTTON, NO_DATA)
        }

        plugin_id = ae.get_plugin_id()
        effect_h = effect._handle

        for i in range(effect.num_properties):
            info = ae.sdk.AEGP_GetEffectParamUnionByIndex(plugin_id, effect_h, i)
            param_type = info["param_type"]

            if param_type == ae.sdk.PF_Param_ARBITRARY_DATA:
                strategies['arbitrary_data'].append(i)
            elif param_type in [ae.sdk.PF_Param_BUTTON, ae.sdk.PF_Param_NO_DATA]:
                strategies['skip'].append(i)
            else:
                strategies['simple_params'].append(i)

        return strategies

    # Use the strategy for serialization
    strategy = get_serialization_strategy(effect)
    for idx in strategy['simple_params']:
        stream = ae.sdk.AEGP_GetNewEffectStreamByIndex(plugin_id, effect_h, idx)
        value = ae.sdk.AEGP_GetNewStreamValue(plugin_id, stream, time)
        # Serialize value...
    ```
    """
    ...

def AEGP_EffectCallGeneric(plugin_id: int, effectH: int, time: float | None = None, effect_cmd: int = ...) -> None:
    """
    Generic effect function call.

    Args:
        plugin_id: Plugin ID
        effectH: Effect handle
        time: Time in seconds (layer time base), or None
        effect_cmd: PF_Cmd command (default: PF_Cmd_COMPLETELY_GENERAL)

    Note:
        Use PF_Cmd_COMPLETELY_GENERAL for general effect calls.
        For other commands, consult AE SDK documentation for required effect_extraPV parameters.

    FUTURE USE - Effect Deserialization:
    This function is critical for notifying effects after restoring parameters from serialized data.
    Primary use cases:

    1. Reinitialize effect internal state after parameter restoration
       Commands: PF_Cmd_PARAMS_SETUP, PF_Cmd_SEQUENCE_SETUP

    2. Trigger parameter-dependent updates
       Commands: PF_Cmd_USER_CHANGED_PARAM, PF_Cmd_UPDATE_PARAMS_UI

    3. Force effect recalculation at specific time points
       Commands: PF_Cmd_RENDER

    Example - Effect deserialization workflow:
    ```python
    def restore_effect_from_data(layer, effect_data):
        # 1. Create effect instance
        effect = layer.add_effect(effect_data["match_name"])

        # 2. Restore all parameter values
        for param_idx, param_value in effect_data["params"].items():
            param = effect.property_by_index(param_idx)
            param.set_value(param_value)

        # 3. Notify effect of restored state
        plugin_id = ae.get_plugin_id()
        effect_h = effect._handle

        # Reinitialize parameter setup
        ae.sdk.AEGP_EffectCallGeneric(
            plugin_id, effect_h,
            effect_cmd=ae.sdk.PF_Cmd_PARAMS_SETUP
        )

        # Update UI to reflect restored parameters
        ae.sdk.AEGP_EffectCallGeneric(
            plugin_id, effect_h,
            effect_cmd=ae.sdk.PF_Cmd_UPDATE_PARAMS_UI
        )

        return effect
    ```

    Common commands for deserialization:
    - PF_Cmd_PARAMS_SETUP: Reinitialize parameter definitions
    - PF_Cmd_SEQUENCE_SETUP: Initialize sequence-specific data
    - PF_Cmd_USER_CHANGED_PARAM: Notify of parameter changes
    - PF_Cmd_UPDATE_PARAMS_UI: Force UI refresh
    - PF_Cmd_RENDER: Trigger rendering at specific time
    """
    ...

# Effect Flags Constants
AEGP_EffectFlags_NONE: int
AEGP_EffectFlags_ACTIVE: int
AEGP_EffectFlags_AUDIO_ONLY: int
AEGP_EffectFlags_AUDIO_TOO: int
AEGP_EffectFlags_MISSING: int
AEGP_InstalledEffectKey_NONE: int

# PF_ParamType Constants
PF_Param_LAYER: int
PF_Param_SLIDER: int
PF_Param_FIX_SLIDER: int
PF_Param_ANGLE: int
PF_Param_CHECKBOX: int
PF_Param_COLOR: int
PF_Param_POINT: int
PF_Param_POPUP: int
PF_Param_NO_DATA: int
PF_Param_FLOAT_SLIDER: int
PF_Param_ARBITRARY_DATA: int
PF_Param_PATH: int
PF_Param_GROUP_START: int
PF_Param_GROUP_END: int
PF_Param_BUTTON: int
PF_Param_POINT_3D: int

# PF_Cmd Constants (for AEGP_EffectCallGeneric)
PF_Cmd_COMPLETELY_GENERAL: int
PF_Cmd_ABOUT: int
PF_Cmd_GLOBAL_SETUP: int
PF_Cmd_GLOBAL_SETDOWN: int
PF_Cmd_PARAMS_SETUP: int
PF_Cmd_SEQUENCE_SETUP: int
PF_Cmd_RENDER: int
PF_Cmd_USER_CHANGED_PARAM: int
PF_Cmd_UPDATE_PARAMS_UI: int

# AEGP_MaskSuite
def AEGP_GetLayerNumMasks(layerH: int) -> int:
    """Get number of masks on layer."""
    ...

def AEGP_GetLayerMaskByIndex(layerH: int, mask_index: int) -> int:
    """Get mask handle by index. Returns MaskRefH. Must dispose with AEGP_DisposeMask."""
    ...

def AEGP_DisposeMask(maskH: int) -> None:
    """Dispose mask handle."""
    ...

def AEGP_GetMaskInvert(maskH: int) -> bool:
    """Get mask invert state."""
    ...

def AEGP_SetMaskInvert(maskH: int, invert: bool) -> None:
    """Set mask invert state."""
    ...

def AEGP_GetMaskMode(maskH: int) -> int:
    """Get mask mode (NONE, ADD, SUBTRACT, etc.)."""
    ...

def AEGP_SetMaskMode(maskH: int, mode: int) -> None:
    """Set mask mode."""
    ...

def AEGP_GetMaskMotionBlurState(maskH: int) -> int:
    """Get mask motion blur state."""
    ...

def AEGP_SetMaskMotionBlurState(maskH: int, blur_state: int) -> None:
    """Set mask motion blur state."""
    ...

def AEGP_GetMaskFeatherFalloff(maskH: int) -> int:
    """Get mask feather falloff type (SMOOTH or LINEAR)."""
    ...

def AEGP_SetMaskFeatherFalloff(maskH: int, falloff: int) -> None:
    """Set mask feather falloff type."""
    ...

def AEGP_GetMaskID(maskH: int) -> int:
    """Get unique mask ID."""
    ...

def AEGP_CreateNewMask(layerH: int) -> tuple[int, int]:
    """
    Create a new mask on a layer.

    Returns:
        tuple[int, int]: (maskH, mask_index) - Mask handle and 0-based index
    """
    ...

def AEGP_DeleteMaskFromLayer(maskH: int) -> None:
    """Delete mask from layer. Note: still need to call AEGP_DisposeMask afterward."""
    ...

def AEGP_DuplicateMask(maskH: int) -> int:
    """
    Duplicate a mask.

    Returns:
        int: Handle to the duplicated mask
    """
    ...

def AEGP_GetMaskColor(maskH: int) -> tuple[float, float, float]:
    """
    Get mask color (RGB, 0.0-1.0 range).

    Returns:
        tuple[float, float, float]: (red, green, blue)
    """
    ...

def AEGP_SetMaskColor(maskH: int, red: float, green: float, blue: float) -> None:
    """Set mask color (RGB, 0.0-1.0 range)."""
    ...

def AEGP_GetMaskLockState(maskH: int) -> bool:
    """Get mask lock state."""
    ...

def AEGP_SetMaskLockState(maskH: int, lock: bool) -> None:
    """Set mask lock state."""
    ...

def AEGP_GetMaskIsRotoBezier(maskH: int) -> bool:
    """Check if mask is RotoBezier."""
    ...

def AEGP_SetMaskIsRotoBezier(maskH: int, is_roto_bezier: bool) -> None:
    """Set mask as RotoBezier."""
    ...

# Mask Mode Constants
PF_MaskMode_NONE: int
PF_MaskMode_ADD: int
PF_MaskMode_SUBTRACT: int
PF_MaskMode_INTERSECT: int
PF_MaskMode_LIGHTEN: int
PF_MaskMode_DARKEN: int
PF_MaskMode_DIFFERENCE: int

# Mask Motion Blur Constants
AEGP_MaskMBlur_SAME_AS_LAYER: int
AEGP_MaskMBlur_OFF: int
AEGP_MaskMBlur_ON: int

# Mask Feather Falloff Constants
AEGP_MaskFeatherFalloff_SMOOTH: int
AEGP_MaskFeatherFalloff_LINEAR: int

# AEGP_RenderQueueSuite
def AEGP_AddCompToRenderQueue(compH: int, path: str) -> int:
    """Add composition to render queue with default options. Returns RQItemRefH."""
    ...

def AEGP_SetRenderQueueState(state: int) -> None:
    """Set render queue state (STOPPED, PAUSED, or RENDERING)."""
    ...

def AEGP_GetRenderQueueState() -> int:
    """Get current render queue state."""
    ...

# Render Queue State Constants
AEGP_RenderQueueState_STOPPED: int
AEGP_RenderQueueState_PAUSED: int
AEGP_RenderQueueState_RENDERING: int

# AEGP_RQItemSuite
def AEGP_GetNumRQItems() -> int:
    """Get number of render queue items."""
    ...

def AEGP_GetRQItemByIndex(index: int) -> int:
    """Get render queue item handle by index. Returns RQItemRefH."""
    ...

def AEGP_GetNumOutputModulesForRQItem(rq_itemH: int) -> int:
    """Get number of output modules for an RQ item."""
    ...

def AEGP_GetRenderState(rq_itemH: int) -> int:
    """Get render state of an item."""
    ...

def AEGP_SetRenderState(rq_itemH: int, status: int) -> None:
    """Set render state of an item."""
    ...

def AEGP_GetNextRQItem(current_rq_itemH: int) -> int:
    """
    Get next RQ item. Pass 0 for current_rq_itemH to get first item.

    Args:
        current_rq_itemH: Current RQ item handle (or 0 to get first item)

    Returns:
        int: Next RQ item handle (or 0 if no more items)
    """
    ...

def AEGP_GetStartedTime(rq_itemH: int) -> float:
    """
    Get render started time in seconds.

    Returns:
        float: Started time in seconds (0.0 if not started)
    """
    ...

def AEGP_GetElapsedTime(rq_itemH: int) -> float:
    """
    Get elapsed render time in seconds.

    Returns:
        float: Elapsed time in seconds (0.0 if not rendered)
    """
    ...

def AEGP_GetLogType(rq_itemH: int) -> int:
    """
    Get log type for RQ item.

    Returns:
        int: Log type value
    """
    ...

def AEGP_SetLogType(rq_itemH: int, logtype: int) -> None:
    """
    Set log type for RQ item.

    Args:
        rq_itemH: Render queue item handle
        logtype: Log type value
    """
    ...

def AEGP_RemoveOutputModule(rq_itemH: int, outmodH: int) -> None:
    """
    Remove output module from RQ item.

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle to remove
    """
    ...

def AEGP_GetComment(rq_itemH: int) -> str:
    """
    Get comment for RQ item.

    Returns:
        str: Comment string
    """
    ...

def AEGP_SetComment(rq_itemH: int, comment: str) -> None:
    """
    Set comment for RQ item.

    Args:
        rq_itemH: Render queue item handle
        comment: Comment string to set
    """
    ...

def AEGP_DeleteRQItem(rq_itemH: int) -> None:
    """Delete render queue item."""
    ...

def AEGP_GetCompFromRQItem(rq_itemH: int) -> int:
    """Get composition handle from RQ item."""
    ...

# AEGP_OutputModuleSuite
#
# IMPORTANT: Index conventions
# ----------------------------
# AEGP_GetOutputModuleByIndex uses 0-based indexing (exception in AEGP SDK).
# Most other AEGP functions use 1-based indexing.
#
# IMPORTANT: Handle pairing rules
# -------------------------------
# rq_itemH and outmodH must be used as consistent pairs:
#   - (rq_itemH, outmodH): Both actual handles from same RQ item ✅
#   - (0, 0): Queuebert pattern (first RQ item, first output module) ✅
#   - (0, outmodH) or (rq_itemH, 0): Mixed patterns ❌ Error
#
# NOTE: outmodH = 0 is a VALID handle (first output module), not null!

def AEGP_GetOutputModuleByIndex(rq_itemH: int, index: int) -> int:
    """
    Get output module handle by index.

    IMPORTANT: Uses 0-based indexing (exception in AEGP SDK).

    Args:
        rq_itemH: Render queue item handle
        index: 0-based index of output module

    Returns:
        int: Output module handle (may be 0, which is valid!)
    """
    ...

def AEGP_SetOutputFilePath(rq_itemH: int, outmodH: int, path: str) -> None:
    """
    Set output file path.

    IMPORTANT: rq_itemH and outmodH must be consistent pairs.
    Use (rq_itemH, outmodH) or (0, 0), never mix them.

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle (0 is valid for first module)
        path: Output file path
    """
    ...

def AEGP_GetOutputFilePath(rq_itemH: int, outmodH: int) -> str:
    """
    Get output file path.

    IMPORTANT: rq_itemH and outmodH must be consistent pairs.
    Use (rq_itemH, outmodH) or (0, 0), never mix them.

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle (0 is valid for first module)

    Returns:
        str: Output file path
    """
    ...

def AEGP_AddDefaultOutputModule(rq_itemH: int) -> int:
    """
    Add default output module to RQ item.

    Args:
        rq_itemH: Render queue item handle

    Returns:
        int: Output module handle (may be 0, which is valid!)
    """
    ...

def AEGP_GetEmbedOptions(rq_itemH: int, outmodH: int) -> int:
    """
    Get embedding options for output module.

    Returns:
        int: Embedding type
            -1: NONE
            0: NOTHING
            1: LINK
            2: LINK_AND_COPY
    """
    ...

def AEGP_SetEmbedOptions(rq_itemH: int, outmodH: int, embed_options: int) -> None:
    """
    Set embedding options for output module.

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle
        embed_options: Embedding type (-1=NONE, 0=NOTHING, 1=LINK, 2=LINK_AND_COPY)
    """
    ...

def AEGP_GetPostRenderAction(rq_itemH: int, outmodH: int) -> int:
    """
    Get post-render action for output module.

    Returns:
        int: Post-render action
            -1: NONE
            0: IMPORT
            1: IMPORT_AND_REPLACE_USAGE
            2: SET_PROXY
    """
    ...

def AEGP_SetPostRenderAction(rq_itemH: int, outmodH: int, post_render_action: int) -> None:
    """
    Set post-render action for output module.

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle
        post_render_action: Post-render action (-1=NONE, 0=IMPORT, 1=IMPORT_AND_REPLACE_USAGE, 2=SET_PROXY)
    """
    ...

def AEGP_GetEnabledOutputs(rq_itemH: int, outmodH: int) -> int:
    """
    Get enabled output types (bitfield).

    Returns:
        int: Bitfield of enabled outputs
            0: NONE
            1: VIDEO
            2: AUDIO
            3: VIDEO + AUDIO
    """
    ...

def AEGP_SetEnabledOutputs(rq_itemH: int, outmodH: int, enabled_types: int) -> None:
    """
    Set enabled output types (bitfield).

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle
        enabled_types: Bitfield (0=NONE, 1=VIDEO, 2=AUDIO, 3=VIDEO+AUDIO)
    """
    ...

def AEGP_GetOutputChannels(rq_itemH: int, outmodH: int) -> int:
    """
    Get output video channels.

    Returns:
        int: Video channels
            -1: NONE
            0: RGB
            1: RGBA
            2: ALPHA
    """
    ...

def AEGP_SetOutputChannels(rq_itemH: int, outmodH: int, output_channels: int) -> None:
    """
    Set output video channels.

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle
        output_channels: Video channels (-1=NONE, 0=RGB, 1=RGBA, 2=ALPHA)
    """
    ...

def AEGP_GetStretchInfo(rq_itemH: int, outmodH: int) -> dict:
    """
    Get stretch info for output module.

    Returns:
        dict with:
            'is_enabled': bool - Whether stretch is enabled
            'stretch_quality': int - Stretch quality (-1=NONE, 0=LOW, 1=HIGH)
            'locked': bool - Whether stretch is locked
    """
    ...

def AEGP_SetStretchInfo(rq_itemH: int, outmodH: int, is_enabled: bool, stretch_quality: int) -> None:
    """
    Set stretch info for output module.

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle
        is_enabled: Whether to enable stretch
        stretch_quality: Stretch quality (-1=NONE, 0=LOW, 1=HIGH)
    """
    ...

def AEGP_GetCropInfo(rq_itemH: int, outmodH: int) -> dict:
    """
    Get crop info for output module.

    Returns:
        dict with:
            'is_enabled': bool - Whether crop is enabled
            'left': int - Left crop coordinate
            'top': int - Top crop coordinate
            'right': int - Right crop coordinate
            'bottom': int - Bottom crop coordinate
    """
    ...

def AEGP_SetCropInfo(rq_itemH: int, outmodH: int, is_enabled: bool, crop_rect: dict) -> None:
    """
    Set crop info for output module.

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle
        is_enabled: Whether to enable crop
        crop_rect: dict with 'left', 'top', 'right', 'bottom' (int)
    """
    ...

def AEGP_GetSoundFormatInfo(rq_itemH: int, outmodH: int) -> dict:
    """
    Get sound format info for output module.

    Returns:
        dict with:
            'sample_rate': float - Sample rate (e.g., 48000.0)
            'encoding': int - Sound encoding (3=UNSIGNED_PCM, 4=SIGNED_PCM, 5=FLOAT)
            'bytes_per_sample': int - Bytes per sample (1, 2, or 4)
            'num_channels': int - Number of channels (1=mono, 2=stereo)
            'audio_enabled': bool - Whether audio is enabled
    """
    ...

def AEGP_SetSoundFormatInfo(rq_itemH: int, outmodH: int, sound_format: dict, audio_enabled: bool) -> None:
    """
    Set sound format info for output module.

    Args:
        rq_itemH: Render queue item handle
        outmodH: Output module handle
        sound_format: dict with 'sample_rate', 'encoding', 'bytes_per_sample', 'num_channels'
        audio_enabled: Whether to enable audio
    """
    ...

def AEGP_GetExtraOutputModuleInfo(rq_itemH: int, outmodH: int) -> dict:
    """
    Get extra output module info (format, info strings, sequence flags).

    Returns:
        dict with:
            'format': str - Format string
            'info': str - Additional info string
            'is_sequence': bool - Whether output is a sequence
            'multi_frame': bool - Whether output supports multiple frames
    """
    ...

# Render Item Status Constants
AEGP_RenderItemStatus_NEEDS_OUTPUT: int
AEGP_RenderItemStatus_UNQUEUED: int
AEGP_RenderItemStatus_QUEUED: int
AEGP_RenderItemStatus_RENDERING: int
AEGP_RenderItemStatus_USER_STOPPED: int
AEGP_RenderItemStatus_ERR_STOPPED: int
AEGP_RenderItemStatus_DONE: int

# AEGP_CollectionSuite
def AEGP_NewCollection(plugin_id: int) -> int:
    """Create new empty collection. Returns Collection2H."""
    ...

def AEGP_DisposeCollection(collectionH: int) -> None:
    """Dispose collection."""
    ...

def AEGP_GetCollectionNumItems(collectionH: int) -> int:
    """Get number of items in collection."""
    ...

def AEGP_GetCollectionItemByIndex(collectionH: int, index: int) -> dict:
    """Get collection item by index. Returns dict with 'type' and handle key."""
    ...

def AEGP_CollectionPushBack(collectionH: int, item: dict) -> None:
    """Add item to collection. Item dict must have 'type' and corresponding handle key."""
    ...

def AEGP_CollectionErase(collectionH: int, index_first: int, index_last: int) -> None:
    """Erase items from collection. Range is exclusive [first, last)."""
    ...

# Collection Item Type Constants
AEGP_CollectionItemType_NONE: int
AEGP_CollectionItemType_LAYER: int
AEGP_CollectionItemType_MASK: int
AEGP_CollectionItemType_EFFECT: int
AEGP_CollectionItemType_STREAM: int
AEGP_CollectionItemType_MASK_VERTEX: int
AEGP_CollectionItemType_KEYFRAME: int

# Text Document & Layer Suite
def AEGP_GetNewText(text_documentH: int) -> str:
    """
    Get text contents from TextDocumentH.
    Returns UTF-8 string.
    """
    ...

def AEGP_SetText(text_documentH: int, text: str) -> None:
    """
    Set text contents to TextDocumentH.
    """
    ...

def AEGP_GetNewTextOutlines(layerH: int) -> int:
    """
    Get new TextOutlinesH from layer (at time 0).
    Returns outlinesH. Must be disposed with AEGP_DisposeTextOutlines.
    """
    ...

def AEGP_DisposeTextOutlines(outlinesH: int) -> None:
    """
    Dispose TextOutlinesH.
    """
    ...

def AEGP_GetNumTextOutlines(outlinesH: int) -> int:
    """
    Get number of text outlines.
    """
    ...

def AEGP_GetIndexedTextOutline(outlinesH: int, path_index: int) -> int:
    """
    Get indexed text outline path.

    Returns:
        int: PF_PathOutlinePtr handle (do not dispose - owned by outlinesH)
    """
    ...

def AEGP_GetStreamValue_TextDocumentH(valueP: int) -> int:
    """
    Extract text_documentH from AEGP_StreamValue2 pointer.
    """
    ...

# Mask Outline Suite
def AEGP_GetStreamValue_MaskOutlineH(valueP: int) -> int:
    """
    Extract mask_outlineH (AEGP_MaskOutlineValH) from AEGP_StreamValue2 pointer.
    """
    ...

def AEGP_IsMaskOutlineOpen(mask_outlineH: int) -> bool:
    """
    Check if mask outline is open.
    """
    ...

def AEGP_SetMaskOutlineOpen(mask_outlineH: int, open: bool) -> None:
    """
    Set mask outline open state.
    """
    ...

def AEGP_GetMaskOutlineNumSegments(mask_outlineH: int) -> int:
    """
    Get number of segments in mask outline.
    """
    ...

def AEGP_GetMaskOutlineVertexInfo(mask_outlineH: int, index: int) -> dict:
    """
    Get vertex info at index.
    Returns dict with keys: x, y, tan_in_x, tan_in_y, tan_out_x, tan_out_y
    """
    ...

def AEGP_SetMaskOutlineVertexInfo(mask_outlineH: int, index: int, vertex: dict) -> None:
    """
    Set vertex info at index.
    vertex dict can contain partial keys.
    """
    ...

def AEGP_CreateVertex(mask_outlineH: int, index: int) -> None:
    """
    Create new vertex at index.
    """
    ...

def AEGP_DeleteVertex(mask_outlineH: int, index: int) -> None:
    """
    Delete vertex at index.
    """
    ...

# -----------------------------------------------------------------------
# MaskOutlineSuite3 - Feather Functions
# -----------------------------------------------------------------------

def AEGP_GetMaskOutlineNumFeathers(mask_outlineH: int) -> int:
    """
    Get number of feathers in mask outline.

    Args:
        mask_outlineH: Mask outline handle

    Returns:
        Number of feathers
    """
    ...

def AEGP_GetMaskOutlineFeatherInfo(mask_outlineH: int, which_feather: int) -> dict:
    """
    Get feather info at index.

    Args:
        mask_outlineH: Mask outline handle
        which_feather: Feather index

    Returns:
        dict with:
            'segment': Mask segment where feather is located (int)
            'segment_sF': Feather location on segment, 0-1 (float)
            'radiusF': Feather radius (float, negative if type == INNER)
            'ui_corner_angleF': Angle of UI handle on corners, 0-1 (float)
            'tensionF': Tension of boundary at feather pt, 0-1 (float)
            'interp': Interpolation type (int: 0=NORMAL, 1=HOLD_CW)
            'type': Feather type (int: 0=OUTER, 1=INNER)
    """
    ...

def AEGP_SetMaskOutlineFeatherInfo(mask_outlineH: int, which_feather: int, feather: dict) -> None:
    """
    Set feather info at index.

    Args:
        mask_outlineH: Mask outline handle
        which_feather: Feather index (must already exist, use Create to add new)
        feather: dict with same keys as returned by AEGP_GetMaskOutlineFeatherInfo
    """
    ...

def AEGP_CreateMaskOutlineFeather(mask_outlineH: int, feather: dict) -> int:
    """
    Create new feather.

    Args:
        mask_outlineH: Mask outline handle
        feather: dict with feather parameters

    Returns:
        Index of new feather
    """
    ...

def AEGP_DeleteMaskOutlineFeather(mask_outlineH: int, index: int) -> None:
    """
    Delete feather at index.

    Args:
        mask_outlineH: Mask outline handle
        index: Feather index to delete
    """
    ...

# -----------------------------------------------------------------------
# AEGP_WorldSuite3 - Frame Buffer Management
# -----------------------------------------------------------------------

# World Type Constants
AEGP_WorldType_NONE: int
AEGP_WorldType_8: int
AEGP_WorldType_16: int
AEGP_WorldType_32: int

# Quality Constants (for FastBlur)
PF_Quality_LO: int
PF_Quality_HI: int

# Mode Flags (for FastBlur alpha handling)
PF_MF_Alpha_PREMUL: int
PF_MF_Alpha_STRAIGHT: int

def AEGP_NewWorld(plugin_id: int, world_type: int, width: int, height: int) -> int:
    """
    Create a new world (frame buffer) with specified type and dimensions.

    Args:
        plugin_id: Plugin ID
        world_type: AEGP_WorldType constant (NONE=0, 8=1, 16=2, 32=3)
        width: Width in pixels
        height: Height in pixels

    Returns:
        int: World handle (AEGP_WorldH)

    Note:
        Must be disposed with AEGP_DisposeWorld when no longer needed.
    """
    ...

def AEGP_DisposeWorld(worldH: int) -> None:
    """
    Dispose a world (free resources).

    Args:
        worldH: World handle
    """
    ...

def AEGP_GetWorldType(worldH: int) -> int:
    """
    Get the pixel type of a world.

    Args:
        worldH: World handle

    Returns:
        int: AEGP_WorldType constant
    """
    ...

def AEGP_GetWorldSize(worldH: int) -> tuple[int, int]:
    """
    Get the dimensions of a world.

    Args:
        worldH: World handle

    Returns:
        tuple[int, int]: (width, height)
    """
    ...

def AEGP_GetWorldRowBytes(worldH: int) -> int:
    """
    Get the bytes per row (stride) of a world.

    Args:
        worldH: World handle

    Returns:
        int: Number of bytes per row
    """
    ...

def AEGP_GetWorldBaseAddr8(worldH: int) -> int:
    """
    Get the base address of an 8-bit world's pixel data.

    Args:
        worldH: World handle (must be AEGP_WorldType_8)

    Returns:
        int: Pointer to pixel data (uintptr_t)

    Raises:
        RuntimeError: If world is not AEGP_WorldType_8
    """
    ...

def AEGP_GetWorldBaseAddr16(worldH: int) -> int:
    """
    Get the base address of a 16-bit world's pixel data.

    Args:
        worldH: World handle (must be AEGP_WorldType_16)

    Returns:
        int: Pointer to pixel data (uintptr_t)

    Raises:
        RuntimeError: If world is not AEGP_WorldType_16
    """
    ...

def AEGP_GetWorldBaseAddr32(worldH: int) -> int:
    """
    Get the base address of a 32-bit float world's pixel data.

    Args:
        worldH: World handle (must be AEGP_WorldType_32)

    Returns:
        int: Pointer to pixel data (uintptr_t)

    Raises:
        RuntimeError: If world is not AEGP_WorldType_32
    """
    ...

def AEGP_FillOutPFEffectWorld(worldH: int) -> dict:
    """
    Fill out a PF_EffectWorld structure from an AEGP_WorldH.

    Useful for using PF (Pixel Format) routines with an AEGP World.

    Args:
        worldH: World handle

    Returns:
        dict with:
            'width': int - Width in pixels
            'height': int - Height in pixels
            'rowbytes': int - Bytes per row
            'world_flags': int - World flags
            'data': int - Pointer to pixel data (uintptr_t)
    """
    ...

def AEGP_FastBlur(radius: float, mode_flags: int, quality: int, worldH: int) -> None:
    """
    Apply fast blur to a world.

    Only works on user-allocated worlds, not on checked-out frames (which are read-only).

    Args:
        radius: Blur radius in pixels
        mode_flags: PF_MF_Alpha_PREMUL (0) or PF_MF_Alpha_STRAIGHT (1)
        quality: PF_Quality_LO (0) or PF_Quality_HI (1)
        worldH: World handle (must be user-allocated, not a checked-out frame)

    Raises:
        RuntimeError: If world is read-only (checked-out frame)
    """
    ...

def AEGP_NewPlatformWorld(plugin_id: int, world_type: int, width: int, height: int) -> int:
    """
    Create a platform-specific world.

    Args:
        plugin_id: Plugin ID
        world_type: AEGP_WorldType constant
        width: Width in pixels
        height: Height in pixels

    Returns:
        int: Platform world handle (AEGP_PlatformWorldH)

    Note:
        Must be disposed with AEGP_DisposePlatformWorld when no longer needed.
    """
    ...

def AEGP_DisposePlatformWorld(platformWorldH: int) -> None:
    """
    Dispose a platform-specific world.

    Args:
        platformWorldH: Platform world handle
    """
    ...

def AEGP_NewReferenceFromPlatformWorld(plugin_id: int, platformWorldH: int) -> int:
    """
    Create an AEGP_WorldH reference from a platform-specific world.

    This allows using platform-specific worlds with standard AEGP World functions.

    Args:
        plugin_id: Plugin ID
        platformWorldH: Platform world handle

    Returns:
        int: World handle (AEGP_WorldH)

    Note:
        The returned world handle must be disposed with AEGP_DisposeWorld.
        Dispose the world reference before disposing the platform world.
    """
    ...

# =========================================================================
# AEGP_RegisterSuite5 - Plugin Registration and Hooks
# =========================================================================
#
# **Important**: Most RegisterSuite functions are NOT available from Python.
# These functions require C callback function pointers and must be called
# during plugin initialization (EntryPointFunc).
#
# PyAE is already an initialized plugin, so most registration functions
# cannot be called at runtime from Python.
#
# **Available function**:
# - AEGP_RegisterPresetLocalizationString: Register localized preset names
#
# **Unavailable functions** (require C callbacks):
# - AEGP_RegisterCommandHook
# - AEGP_RegisterUpdateMenuHook
# - AEGP_RegisterDeathHook
# - AEGP_RegisterVersionHook
# - AEGP_RegisterAboutStringHook
# - AEGP_RegisterAboutHook
# - AEGP_RegisterArtisan
# - AEGP_RegisterIO
# - AEGP_RegisterIdleHook
# - AEGP_RegisterTracker
# - AEGP_RegisterInteractiveArtisan
# =========================================================================

def AEGP_RegisterCommandHook() -> None:
    """
    Register a command hook.

    **Not available from Python**: This function requires a C callback
    function pointer and must be called during plugin initialization.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterUpdateMenuHook() -> None:
    """
    Register a menu update hook.

    **Not available from Python**: This function requires a C callback
    function pointer and must be called during plugin initialization.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterDeathHook() -> None:
    """
    Register a plugin termination hook.

    **Not available from Python**: This function requires a C callback
    function pointer and must be called during plugin initialization.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterVersionHook() -> None:
    """
    Register a version hook.

    **Not available from Python**: This function requires a C callback
    function pointer and must be called during plugin initialization.

    **Note**: This hook is not called in AE 5.0 and later.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterAboutStringHook() -> None:
    """
    Register an About string hook.

    **Not available from Python**: This function requires a C callback
    function pointer and must be called during plugin initialization.

    **Note**: This hook is not called in AE 5.0 and later.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterAboutHook() -> None:
    """
    Register an About dialog hook.

    **Not available from Python**: This function requires a C callback
    function pointer and must be called during plugin initialization.

    **Note**: This hook is not called in AE 5.0 and later.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterArtisan() -> None:
    """
    Register an Artisan plugin.

    **Not available from Python**: This function requires C callback
    function pointers (PR_ArtisanEntryPoints) and must be called
    during plugin initialization.

    Artisan plugins are for custom 3D rendering and cannot be
    implemented in Python.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterIO() -> None:
    """
    Register an I/O plugin.

    **Not available from Python**: This function requires C callback
    function pointers (AEIO_FunctionBlock4) and must be called
    during plugin initialization.

    I/O plugins are for file format import/export and cannot be
    implemented in Python.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterIdleHook() -> None:
    """
    Register an idle hook.

    **Not available from Python**: This function requires a C callback
    function pointer and must be called during plugin initialization.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterTracker() -> None:
    """
    Register a tracker plugin.

    **Not available from Python**: This function requires C callback
    function pointers (PT_TrackerEntryPoints) and must be called
    during plugin initialization.

    Tracker plugins cannot be implemented in Python.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterInteractiveArtisan() -> None:
    """
    Register an Interactive Artisan plugin.

    **Not available from Python**: This function requires C callback
    function pointers (PR_ArtisanEntryPoints) and must be called
    during plugin initialization.

    Interactive Artisan plugins are for custom 3D rendering with UI
    and cannot be implemented in Python.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RegisterPresetLocalizationString(english_name: str, localized_name: str) -> None:
    """
    Register a localized string for preset property names.

    When presets are loaded, any property name starting with a tab character ('\\t')
    followed by the registered English name will be replaced with the localized name.

    Args:
        english_name: English property name (without tab character)
        localized_name: Localized name

    Example:
        >>> import ae.sdk as sdk
        >>> # Replace "\\tOpacity" in presets with "不透明度"
        >>> sdk.AEGP_RegisterPresetLocalizationString("Opacity", "不透明度")
        >>> sdk.AEGP_RegisterPresetLocalizationString("Position", "位置")
        >>> sdk.AEGP_RegisterPresetLocalizationString("Scale", "スケール")

    Note:
        - In English environments, the tab character is simply removed
        - This function can be called multiple times
        - The registered strings apply to preset loading and Expression references

    Raises:
        ValueError: If either argument is an empty string
        RuntimeError: If Register Suite is not available or registration fails
    """
    ...

# ================================================================
# AEGP_RenderAsyncManagerSuite1
# ================================================================

def AEGP_CheckoutOrRender_ItemFrame_AsyncManager(
    async_managerP: int,
    purpose_id: int,
    roH: int
) -> int:
    """
    非同期レンダリングマネージャを使用してアイテムフレームをチェックアウトまたはレンダリング

    AsyncManagerは複数の非同期レンダリングリクエストを自動的に管理します。
    同じpurpose_idで新しいリクエストが発行されると、古いリクエストは自動的にキャンセルされます。

    Args:
        async_managerP: 非同期マネージャのポインタ（PF_AsyncManagerP）
        purpose_id: リクエストを識別するための一意ID（同じ用途のリクエストをキャンセルするヒントとして使用）
        roH: レンダリングするアイテムフレームの記述（AEGP_RenderOptionsH）

    Returns:
        レンダリングされたフレームのレシート（AEGP_FrameReceiptH）
        成功した場合でもピクセルデータがない場合があります（worldなし）

    Raises:
        RuntimeError: レンダリングリクエストが失敗した場合
        ValueError: 引数が無効な場合

    Note:
        - このAPIは主にPF_Event_DRAWでのカスタムUI用（PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER）
        - purpose_idは、古いリクエストを自動キャンセルするための識別子です
        - フレームレシートはAEGP_FrameSuite経由でアクセスする必要があります

    Version:
        Introduced in After Effects 13.5
    """
    ...

def AEGP_CheckoutOrRender_LayerFrame_AsyncManager(
    async_managerP: int,
    purpose_id: int,
    lroH: int
) -> int:
    """
    非同期レンダリングマネージャを使用してレイヤーフレームをチェックアウトまたはレンダリング

    AsyncManagerは複数の非同期レンダリングリクエストを自動的に管理します。
    同じpurpose_idで新しいリクエストが発行されると、古いリクエストは自動的にキャンセルされます。

    Args:
        async_managerP: 非同期マネージャのポインタ（PF_AsyncManagerP）
        purpose_id: リクエストを識別するための一意ID（同じ用途のリクエストをキャンセルするヒントとして使用）
        lroH: レンダリングするレイヤーフレームの記述（AEGP_LayerRenderOptionsH）

    Returns:
        レンダリングされたフレームのレシート（AEGP_FrameReceiptH）
        成功した場合でもピクセルデータがない場合があります（worldなし）

    Raises:
        RuntimeError: レンダリングリクエストが失敗した場合
        ValueError: 引数が無効な場合

    Note:
        - このAPIは主にPF_Event_DRAWでのカスタムUI用（PF_OutFlags2_CUSTOM_UI_ASYNC_MANAGER）
        - purpose_idは、古いリクエストを自動キャンセルするための識別子です
        - フレームレシートはAEGP_FrameSuite経由でアクセスする必要があります

    Version:
        Introduced in After Effects 13.5
    """
    ...

# ================================================================
# AEGP_RenderSuite5
# ================================================================

def AEGP_RenderAndCheckoutFrame(optionsH: int) -> int:
    """
    Render and checkout a composition frame synchronously.

    This function renders a composition frame based on the provided render options
    and returns a receipt handle that can be used to access the rendered frame data.

    Args:
        optionsH: Render options handle (AEGP_RenderOptionsH) created with
                  AEGP_RenderOptions_NewFromItem

    Returns:
        int: Frame receipt handle (AEGP_FrameReceiptH) - use AEGP_GetReceiptWorld
             to access the rendered pixel data

    Raises:
        RuntimeError: If rendering fails
        ValueError: If optionsH is invalid

    Note:
        - Must call AEGP_CheckinFrame when done with the receipt
        - The receipt contains the rendered world (frame buffer)
        - This is a synchronous operation and may take time

    Example:
        >>> import ae.sdk as sdk
        >>> options = sdk.AEGP_RenderOptions_NewFromItem(item_h)
        >>> sdk.AEGP_RenderOptions_SetTime(options, 1.0)  # 1 second
        >>> receipt = sdk.AEGP_RenderAndCheckoutFrame(options)
        >>> world = sdk.AEGP_GetReceiptWorld(receipt)
        >>> # ... use world ...
        >>> sdk.AEGP_CheckinFrame(receipt)
        >>> sdk.AEGP_RenderOptions_Dispose(options)
    """
    ...

def AEGP_RenderAndCheckoutLayerFrame(optionsH: int) -> int:
    """
    Render and checkout a layer frame synchronously.

    This function renders a layer frame based on the provided layer render options
    and returns a receipt handle that can be used to access the rendered frame data.

    Args:
        optionsH: Layer render options handle (AEGP_LayerRenderOptionsH) created with
                  AEGP_NewFromLayer or related functions

    Returns:
        int: Frame receipt handle (AEGP_FrameReceiptH) - use AEGP_GetReceiptWorld
             to access the rendered pixel data

    Raises:
        RuntimeError: If rendering fails
        ValueError: If optionsH is invalid

    Note:
        - Must call AEGP_CheckinFrame when done with the receipt
        - The receipt contains the rendered world (frame buffer)
        - This is a synchronous operation and may take time
        - Use this for rendering individual layers, not entire compositions

    Example:
        >>> import ae.sdk as sdk
        >>> options = sdk.AEGP_NewFromLayer(layer_h)
        >>> sdk.AEGP_SetLayerRenderOptionsTime(options, 1.0)
        >>> receipt = sdk.AEGP_RenderAndCheckoutLayerFrame(options)
        >>> world = sdk.AEGP_GetReceiptWorld(receipt)
        >>> # ... use world ...
        >>> sdk.AEGP_CheckinFrame(receipt)
        >>> sdk.AEGP_DisposeLayerRenderOptions(options)
    """
    ...

def AEGP_RenderAndCheckoutLayerFrame_Async() -> None:
    """
    Asynchronous layer frame rendering (NOT IMPLEMENTED - stub function).

    **Not available from Python**: This function requires C callback function
    pointers for asynchronous completion notification and cannot be implemented
    in Python. Use AEGP_RenderAndCheckoutLayerFrame for synchronous rendering
    or AEGP_CheckoutOrRender_LayerFrame_AsyncManager for async manager-based rendering.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_CancelAsyncRequest(asyncRequestId: int) -> None:
    """
    Cancel an asynchronous render request.

    This function cancels a previously submitted asynchronous render request
    identified by the request ID.

    Args:
        asyncRequestId: Async request ID returned by async render functions

    Raises:
        RuntimeError: If cancellation fails or request ID is invalid
        ValueError: If asyncRequestId is invalid

    Note:
        - Only applicable for async render requests
        - May not cancel immediately if rendering is already in progress
        - Safe to call even if request has already completed
    """
    ...

def AEGP_CheckinFrame(receiptH: int) -> None:
    """
    Check in (release) a frame receipt after use.

    This function releases the frame receipt and its associated resources.
    Must be called for every receipt obtained from render functions.

    Args:
        receiptH: Frame receipt handle (AEGP_FrameReceiptH) obtained from
                  AEGP_RenderAndCheckoutFrame or AEGP_RenderAndCheckoutLayerFrame

    Raises:
        RuntimeError: If checkin fails
        ValueError: If receiptH is invalid

    Note:
        - Must be called after you're done using the receipt
        - The receipt and its world become invalid after checkin
        - Failure to checkin causes memory leaks

    Example:
        >>> receipt = sdk.AEGP_RenderAndCheckoutFrame(options)
        >>> try:
        ...     world = sdk.AEGP_GetReceiptWorld(receipt)
        ...     # ... use world ...
        ... finally:
        ...     sdk.AEGP_CheckinFrame(receipt)  # Always checkin
    """
    ...

def AEGP_GetReceiptWorld(receiptH: int) -> int:
    """
    Get the world (frame buffer) from a frame receipt.

    This function retrieves the rendered world handle from a frame receipt,
    allowing access to the pixel data.

    Args:
        receiptH: Frame receipt handle (AEGP_FrameReceiptH)

    Returns:
        int: World handle (AEGP_WorldH) - use WorldSuite functions to access pixels

    Raises:
        RuntimeError: If world retrieval fails
        ValueError: If receiptH is invalid

    Note:
        - The world is valid only while the receipt is checked out
        - Do not dispose the world - it's owned by the receipt
        - Use AEGP_GetWorldPixelData8/16/32 to access pixel data

    Example:
        >>> receipt = sdk.AEGP_RenderAndCheckoutFrame(options)
        >>> world = sdk.AEGP_GetReceiptWorld(receipt)
        >>> width, height = sdk.AEGP_GetWorldSize(world)
        >>> pixels = sdk.AEGP_GetWorldPixelData8(world)
        >>> sdk.AEGP_CheckinFrame(receipt)
    """
    ...

def AEGP_GetRenderedRegion(receiptH: int) -> dict:
    """
    Get the actually rendered region from a frame receipt.

    This function returns the region that was actually rendered, which may be
    smaller than the requested region due to optimizations.

    Args:
        receiptH: Frame receipt handle (AEGP_FrameReceiptH)

    Returns:
        dict: Rendered region with keys:
            - 'left' (int): Left coordinate
            - 'top' (int): Top coordinate
            - 'right' (int): Right coordinate
            - 'bottom' (int): Bottom coordinate

    Raises:
        RuntimeError: If region retrieval fails
        ValueError: If receiptH is invalid

    Note:
        - The rendered region may be smaller than the region of interest
        - Use this to determine what portion of the frame is valid
        - Coordinates are in pixel space
    """
    ...

def AEGP_IsRenderedFrameSufficient(rendered_optionsH: int, proposed_optionsH: int) -> bool:
    """
    Check if a rendered frame is sufficient for proposed render options.

    This function determines whether a previously rendered frame (with rendered_optionsH)
    can be reused for a new render request (with proposed_optionsH) without re-rendering.

    Args:
        rendered_optionsH: Render options of the previously rendered frame
        proposed_optionsH: Render options being proposed for a new render

    Returns:
        bool: True if the rendered frame can be reused, False if re-rendering is needed

    Raises:
        RuntimeError: If comparison fails
        ValueError: If either options handle is invalid

    Note:
        - Used for render caching optimization
        - Considers resolution, time, region of interest, and other settings
        - Returns False if any setting requires higher quality or different content
    """
    ...

def AEGP_RenderNewItemSoundData(itemH: int, start_time: float, duration: float, sound_format: dict) -> int:
    """
    Render audio data from an item.

    This function renders audio data from a composition or footage item
    for a specified time range.

    Args:
        itemH: Item handle (composition or footage with audio)
        start_time: Start time in seconds
        duration: Duration to render in seconds
        sound_format: Audio format specification dict with keys:
            - 'sample_rate' (float): Sample rate in Hz (e.g., 44100.0)
            - 'num_channels' (int): Number of channels (1=mono, 2=stereo)
            - 'bytes_per_sample' (int): Bytes per sample (1, 2, or 4)

    Returns:
        int: Sound data handle (AEGP_SoundDataH) - use SoundDataSuite to access samples

    Raises:
        RuntimeError: If rendering fails or item has no audio
        ValueError: If arguments are invalid

    Note:
        - Must dispose sound data handle when done
        - Use SoundDataSuite functions to access audio samples
        - Audio is always rendered in the specified format
    """
    ...

def AEGP_GetCurrentTimestamp() -> tuple[int, int, int, int]:
    """
    Get the current timestamp for change detection.

    This function returns a timestamp that can be used with
    AEGP_HasItemChangedSinceTimestamp to detect changes to items.

    Returns:
        tuple: Timestamp as (A_u_long, A_u_long, A_u_long, A_u_long)
               representing AEGP_TimeStamp structure

    Note:
        - Use this before rendering to get a baseline timestamp
        - Compare with later timestamps to detect changes
        - Timestamps are opaque values - only useful for comparison

    Example:
        >>> timestamp = sdk.AEGP_GetCurrentTimestamp()
        >>> # ... later ...
        >>> changed = sdk.AEGP_HasItemChangedSinceTimestamp(item_h, 0.0, 5.0, timestamp)
        >>> if changed:
        ...     print("Item has changed, need to re-render")
    """
    ...

def AEGP_HasItemChangedSinceTimestamp(itemH: int, start_time: float, duration: float, timestamp: tuple) -> bool:
    """
    Check if an item has changed since a given timestamp.

    This function determines whether an item's rendering would be different
    from when the timestamp was obtained.

    Args:
        itemH: Item handle (composition or footage)
        start_time: Start time of the range to check in seconds
        duration: Duration of the range to check in seconds
        timestamp: Timestamp tuple from AEGP_GetCurrentTimestamp

    Returns:
        bool: True if the item has changed and needs re-rendering, False otherwise

    Raises:
        RuntimeError: If check fails
        ValueError: If arguments are invalid

    Note:
        - Detects changes to layer properties, effects, footage, etc.
        - Useful for render caching and avoiding unnecessary re-renders
        - Checks changes within the specified time range only

    Example:
        >>> timestamp = sdk.AEGP_GetCurrentTimestamp()
        >>> # ... cache render result ...
        >>> # ... later, before using cache ...
        >>> if sdk.AEGP_HasItemChangedSinceTimestamp(item_h, 0.0, 5.0, timestamp):
        ...     # Re-render needed
        ...     pass
    """
    ...

def AEGP_IsItemWorthwhileToRender(roH: int, timestamp: tuple) -> bool:
    """
    Check if an item is worthwhile to render at the specified options.

    This function determines whether rendering would produce meaningful output
    or if the item is effectively invisible/empty at the specified settings.

    Args:
        roH: Render options handle (AEGP_RenderOptionsH)
        timestamp: Timestamp tuple from AEGP_GetCurrentTimestamp

    Returns:
        bool: True if rendering is worthwhile, False if the result would be empty

    Raises:
        RuntimeError: If check fails
        ValueError: If arguments are invalid

    Note:
        - Useful for optimizing render queues
        - Returns False if all layers are invisible, composition is empty, etc.
        - Considers layer visibility, opacity, and other factors
    """
    ...

def AEGP_CheckinRenderedFrame() -> None:
    """
    Check in rendered frame (NOT IMPLEMENTED - stub function).

    **Not available from Python**: This function uses an internal frame
    checkout system that is not exposed to Python. Use AEGP_CheckinFrame instead.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_GetReceiptGuid(receiptH: int) -> str:
    """
    Get the unique GUID of a frame receipt.

    This function returns a globally unique identifier for the frame receipt,
    which can be used for tracking and debugging.

    Args:
        receiptH: Frame receipt handle (AEGP_FrameReceiptH)

    Returns:
        str: GUID string in standard format (e.g., "550e8400-e29b-41d4-a716-446655440000")

    Raises:
        RuntimeError: If GUID retrieval fails
        ValueError: If receiptH is invalid

    Note:
        - Each receipt has a unique GUID
        - Useful for debugging and logging
        - GUID remains constant for the receipt's lifetime
    """
    ...

# ================================================================
# AEGP_RenderOptionsSuite4
# ================================================================

def AEGP_RenderOptions_NewFromItem(itemH: int) -> int:
    """
    Create new render options from an item (composition or footage).

    This function creates a render options handle initialized with default
    settings for the specified item.

    Args:
        itemH: Item handle (typically a composition)

    Returns:
        int: Render options handle (AEGP_RenderOptionsH)

    Raises:
        RuntimeError: If creation fails
        ValueError: If itemH is invalid

    Note:
        - Must call AEGP_RenderOptions_Dispose when done
        - Options are initialized with default settings (full resolution, current time, etc.)
        - Use other RenderOptions functions to configure settings

    Example:
        >>> options = sdk.AEGP_RenderOptions_NewFromItem(comp_h)
        >>> sdk.AEGP_RenderOptions_SetTime(options, 1.0)
        >>> sdk.AEGP_RenderOptions_SetDownsampleFactor(options, 2, 2)
        >>> receipt = sdk.AEGP_RenderAndCheckoutFrame(options)
        >>> # ... use receipt ...
        >>> sdk.AEGP_CheckinFrame(receipt)
        >>> sdk.AEGP_RenderOptions_Dispose(options)
    """
    ...

def AEGP_RenderOptions_Duplicate(optionsH: int) -> int:
    """
    Duplicate render options.

    This function creates a copy of existing render options with all settings.

    Args:
        optionsH: Source render options handle

    Returns:
        int: New render options handle (AEGP_RenderOptionsH) with copied settings

    Raises:
        RuntimeError: If duplication fails
        ValueError: If optionsH is invalid

    Note:
        - Must call AEGP_RenderOptions_Dispose on the duplicated options
        - All settings are copied from the source
        - Useful for creating variations of render settings
    """
    ...

def AEGP_RenderOptions_Dispose(optionsH: int) -> None:
    """
    Dispose render options and free resources.

    Args:
        optionsH: Render options handle to dispose

    Raises:
        RuntimeError: If disposal fails
        ValueError: If optionsH is invalid

    Note:
        - Must be called for every render options handle created
        - The handle becomes invalid after disposal
        - Failure to dispose causes memory leaks
    """
    ...

def AEGP_RenderOptions_SetTime(optionsH: int, time_seconds: float) -> None:
    """
    Set the render time for render options.

    Args:
        optionsH: Render options handle
        time_seconds: Time to render in seconds

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Time is in composition time (seconds)
        - Affects which frame is rendered
        - Default is usually the current time indicator position

    Example:
        >>> options = sdk.AEGP_RenderOptions_NewFromItem(comp_h)
        >>> sdk.AEGP_RenderOptions_SetTime(options, 2.5)  # Render at 2.5 seconds
    """
    ...

def AEGP_RenderOptions_GetTime(optionsH: int) -> float:
    """
    Get the render time from render options.

    Args:
        optionsH: Render options handle

    Returns:
        float: Render time in seconds

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid

    Note:
        - Time is in composition time (seconds)
        - Returns the time that will be rendered
    """
    ...

def AEGP_RenderOptions_SetTimeStep(optionsH: int, time_step_seconds: float) -> None:
    """
    Set the time step (shutter interval) for motion blur calculations.

    Args:
        optionsH: Render options handle
        time_step_seconds: Time step in seconds

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Affects motion blur quality and calculation
        - Smaller values = more samples = better quality but slower
        - Only relevant when motion blur is enabled
    """
    ...

def AEGP_RenderOptions_GetTimeStep(optionsH: int) -> float:
    """
    Get the time step from render options.

    Args:
        optionsH: Render options handle

    Returns:
        float: Time step in seconds

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid

    Note:
        - Returns the time step used for motion blur calculations
    """
    ...

def AEGP_RenderOptions_SetFieldRender(optionsH: int, field_render: int) -> None:
    """
    Set field rendering mode (for interlaced video).

    Args:
        optionsH: Render options handle
        field_render: Field render mode constant:
            - PF_Field_FRAME: Progressive (no fields)
            - PF_Field_UPPER: Upper field first
            - PF_Field_LOWER: Lower field first

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Used for interlaced video output
        - Most modern video uses PF_Field_FRAME (progressive)
    """
    ...

def AEGP_RenderOptions_GetFieldRender(optionsH: int) -> int:
    """
    Get field rendering mode from render options.

    Args:
        optionsH: Render options handle

    Returns:
        int: Field render mode constant (PF_Field_FRAME, PF_Field_UPPER, or PF_Field_LOWER)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid
    """
    ...

def AEGP_RenderOptions_SetWorldType(optionsH: int, type: int) -> None:
    """
    Set the output world type (bit depth).

    Args:
        optionsH: Render options handle
        type: World type constant:
            - AEGP_WorldType_8: 8 bits per channel
            - AEGP_WorldType_16: 16 bits per channel
            - AEGP_WorldType_32: 32 bits per channel (float)

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Determines the bit depth of the rendered output
        - Higher bit depth = better quality but more memory
        - AEGP_WorldType_32 is floating point (HDR capable)

    Example:
        >>> options = sdk.AEGP_RenderOptions_NewFromItem(comp_h)
        >>> sdk.AEGP_RenderOptions_SetWorldType(options, sdk.AEGP_WorldType_16)
    """
    ...

def AEGP_RenderOptions_GetWorldType(optionsH: int) -> int:
    """
    Get the output world type from render options.

    Args:
        optionsH: Render options handle

    Returns:
        int: World type constant (AEGP_WorldType_8, 16, or 32)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid
    """
    ...

def AEGP_RenderOptions_SetDownsampleFactor(optionsH: int, x: int, y: int) -> None:
    """
    Set the downsample factor (resolution divisor).

    Args:
        optionsH: Render options handle
        x: Horizontal downsample factor (1 = full resolution, 2 = half, 4 = quarter, etc.)
        y: Vertical downsample factor

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid (factors must be positive)

    Note:
        - Factors must be power of 2 values (1, 2, 4, 8, etc.)
        - Higher factors = faster rendering but lower quality
        - Use for preview or draft quality renders

    Example:
        >>> options = sdk.AEGP_RenderOptions_NewFromItem(comp_h)
        >>> sdk.AEGP_RenderOptions_SetDownsampleFactor(options, 2, 2)  # Half resolution
    """
    ...

def AEGP_RenderOptions_GetDownsampleFactor(optionsH: int) -> tuple[int, int]:
    """
    Get the downsample factor from render options.

    Args:
        optionsH: Render options handle

    Returns:
        tuple: (x_factor, y_factor) downsample factors

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid

    Note:
        - Returns the horizontal and vertical downsample factors
        - (1, 1) means full resolution
    """
    ...

def AEGP_RenderOptions_SetRegionOfInterest(optionsH: int, roi: dict) -> None:
    """
    Set the region of interest (render area).

    Args:
        optionsH: Render options handle
        roi: Region of interest dict with keys:
            - 'left' (int): Left coordinate in pixels
            - 'top' (int): Top coordinate
            - 'right' (int): Right coordinate (exclusive)
            - 'bottom' (int): Bottom coordinate (exclusive)

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Only the specified region is rendered
        - Coordinates are in composition pixel space (before downsampling)
        - Use to optimize rendering of partial frames
        - Empty region renders the entire frame

    Example:
        >>> roi = {'left': 100, 'top': 100, 'right': 300, 'bottom': 300}
        >>> sdk.AEGP_RenderOptions_SetRegionOfInterest(options, roi)
    """
    ...

def AEGP_RenderOptions_GetRegionOfInterest(optionsH: int) -> dict:
    """
    Get the region of interest from render options.

    Args:
        optionsH: Render options handle

    Returns:
        dict: Region dict with 'left', 'top', 'right', 'bottom' keys

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid

    Note:
        - Returns the current region of interest
        - Empty/full region is represented by composition bounds
    """
    ...

def AEGP_RenderOptions_SetMatteMode(optionsH: int, mode: int) -> None:
    """
    Set the alpha channel interpretation mode.

    Args:
        optionsH: Render options handle
        mode: Matte mode constant:
            - AEGP_MatteMode_STRAIGHT: Straight (unmatted) alpha
            - AEGP_MatteMode_PREMUL_BLACK: Premultiplied with black
            - AEGP_MatteMode_PREMUL_BG_COLOR: Premultiplied with background color

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Controls how alpha channel is handled in the output
        - Most compositing uses premultiplied alpha (PREMUL_BLACK)
        - Straight alpha is used for some file formats
    """
    ...

def AEGP_RenderOptions_GetMatteMode(optionsH: int) -> int:
    """
    Get the alpha channel interpretation mode.

    Args:
        optionsH: Render options handle

    Returns:
        int: Matte mode constant (AEGP_MatteMode_STRAIGHT, PREMUL_BLACK, or PREMUL_BG_COLOR)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid
    """
    ...

def AEGP_RenderOptions_SetChannelOrder(optionsH: int, order: int) -> None:
    """
    Set the channel order for pixel data.

    Args:
        optionsH: Render options handle
        order: Channel order constant:
            - AEGP_ChannelOrder_ARGB: Alpha, Red, Green, Blue
            - AEGP_ChannelOrder_BGRA: Blue, Green, Red, Alpha

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Controls the byte order of pixels in the rendered world
        - Different platforms and APIs prefer different orders
        - Most common is BGRA on Windows, ARGB on Mac
    """
    ...

def AEGP_RenderOptions_GetChannelOrder(optionsH: int) -> int:
    """
    Get the channel order from render options.

    Args:
        optionsH: Render options handle

    Returns:
        int: Channel order constant (AEGP_ChannelOrder_ARGB or BGRA)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid
    """
    ...

def AEGP_RenderOptions_GetRenderGuideLayers(optionsH: int) -> bool:
    """
    Check if guide layers are included in rendering.

    Args:
        optionsH: Render options handle

    Returns:
        bool: True if guide layers are rendered, False otherwise

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid

    Note:
        - Guide layers are normally excluded from rendering
        - Used for layout/reference purposes only
    """
    ...

def AEGP_RenderOptions_SetRenderGuideLayers(optionsH: int, render_them: bool) -> None:
    """
    Set whether guide layers should be included in rendering.

    Args:
        optionsH: Render options handle
        render_them: True to render guide layers, False to exclude them (default)

    Raises:
        RuntimeError: If setting fails
        ValueError: If optionsH is invalid

    Note:
        - Guide layers are typically excluded from final renders
        - Set to True for preview/reference purposes
    """
    ...

def AEGP_RenderOptions_GetRenderQuality(optionsH: int) -> int:
    """
    Get the render quality setting.

    Args:
        optionsH: Render options handle

    Returns:
        int: Quality constant:
            - AEGP_ItemQuality_DRAFT: Draft quality (faster)
            - AEGP_ItemQuality_BEST: Best quality (slower)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid

    Note:
        - Affects the quality of rendering (antialiasing, subpixel positioning, etc.)
        - Draft is faster but lower quality, Best is slower but highest quality
    """
    ...

def AEGP_RenderOptions_SetRenderQuality(optionsH: int, quality: int) -> None:
    """
    Set the render quality setting.

    Args:
        optionsH: Render options handle
        quality: Quality constant:
            - AEGP_ItemQuality_DRAFT: Draft quality (faster)
            - AEGP_ItemQuality_BEST: Best quality (slower)

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Controls the overall render quality
        - Draft quality is suitable for previews
        - Best quality should be used for final output
    """
    ...

# ================================================================
# AEGP_LayerRenderOptionsSuite2
# ================================================================

def AEGP_NewFromLayer(layerH: int) -> int:
    """
    Create layer render options from a layer.

    This function creates a layer render options handle initialized for
    rendering the specified layer.

    Args:
        layerH: Layer handle

    Returns:
        int: Layer render options handle (AEGP_LayerRenderOptionsH)

    Raises:
        RuntimeError: If creation fails
        ValueError: If layerH is invalid

    Note:
        - Must call AEGP_DisposeLayerRenderOptions when done
        - Options are initialized with default settings
        - Renders the layer as it appears in the composition

    Example:
        >>> options = sdk.AEGP_NewFromLayer(layer_h)
        >>> sdk.AEGP_SetLayerRenderOptionsTime(options, 1.0)
        >>> receipt = sdk.AEGP_RenderAndCheckoutLayerFrame(options)
        >>> # ... use receipt ...
        >>> sdk.AEGP_CheckinFrame(receipt)
        >>> sdk.AEGP_DisposeLayerRenderOptions(options)
    """
    ...

def AEGP_NewFromUpstreamOfEffect(effectH: int) -> int:
    """
    Create layer render options for the layer upstream (before) an effect.

    This function creates render options that render the layer as it appears
    before the specified effect is applied, useful for effect plugins.

    Args:
        effectH: Effect handle (AEGP_EffectRefH)

    Returns:
        int: Layer render options handle (AEGP_LayerRenderOptionsH)

    Raises:
        RuntimeError: If creation fails
        ValueError: If effectH is invalid

    Note:
        - Must call AEGP_DisposeLayerRenderOptions when done
        - Renders the layer without the specified effect and effects after it
        - Useful for effect development and debugging

    Example:
        >>> # Render layer before "Blur" effect
        >>> options = sdk.AEGP_NewFromUpstreamOfEffect(blur_effect_h)
        >>> receipt = sdk.AEGP_RenderAndCheckoutLayerFrame(options)
    """
    ...

def AEGP_NewFromDownstreamOfEffect(effectH: int) -> int:
    """
    Create layer render options for the layer downstream (after) an effect.

    This function creates render options that render the layer as it appears
    after the specified effect is applied.

    Args:
        effectH: Effect handle (AEGP_EffectRefH)

    Returns:
        int: Layer render options handle (AEGP_LayerRenderOptionsH)

    Raises:
        RuntimeError: If creation fails
        ValueError: If effectH is invalid

    Note:
        - Must call AEGP_DisposeLayerRenderOptions when done
        - Renders the layer with the specified effect and all effects before it
        - Effects after the specified effect are not applied

    Example:
        >>> # Render layer up to and including "Blur" effect
        >>> options = sdk.AEGP_NewFromDownstreamOfEffect(blur_effect_h)
        >>> receipt = sdk.AEGP_RenderAndCheckoutLayerFrame(options)
    """
    ...

def AEGP_DuplicateLayerRenderOptions(optionsH: int) -> int:
    """
    Duplicate layer render options.

    This function creates a copy of existing layer render options with all settings.

    Args:
        optionsH: Source layer render options handle

    Returns:
        int: New layer render options handle (AEGP_LayerRenderOptionsH)

    Raises:
        RuntimeError: If duplication fails
        ValueError: If optionsH is invalid

    Note:
        - Must call AEGP_DisposeLayerRenderOptions on the duplicated options
        - All settings are copied from the source
    """
    ...

def AEGP_DisposeLayerRenderOptions(optionsH: int) -> None:
    """
    Dispose layer render options and free resources.

    Args:
        optionsH: Layer render options handle to dispose

    Raises:
        RuntimeError: If disposal fails
        ValueError: If optionsH is invalid

    Note:
        - Must be called for every layer render options handle created
        - The handle becomes invalid after disposal
        - Failure to dispose causes memory leaks
    """
    ...

def AEGP_SetLayerRenderOptionsTime(optionsH: int, time_seconds: float) -> None:
    """
    Set the render time for layer render options.

    Args:
        optionsH: Layer render options handle
        time_seconds: Time to render in seconds (composition time)

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Time is in composition time (seconds)
        - Affects which frame of the layer is rendered

    Example:
        >>> options = sdk.AEGP_NewFromLayer(layer_h)
        >>> sdk.AEGP_SetLayerRenderOptionsTime(options, 2.5)
    """
    ...

def AEGP_GetLayerRenderOptionsTime(optionsH: int) -> float:
    """
    Get the render time from layer render options.

    Args:
        optionsH: Layer render options handle

    Returns:
        float: Render time in seconds

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid
    """
    ...

def AEGP_SetLayerRenderOptionsTimeStep(optionsH: int, time_step_seconds: float) -> None:
    """
    Set the time step for motion blur calculations.

    Args:
        optionsH: Layer render options handle
        time_step_seconds: Time step in seconds

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Affects motion blur quality
        - Only relevant when motion blur is enabled on the layer
    """
    ...

def AEGP_GetLayerRenderOptionsTimeStep(optionsH: int) -> float:
    """
    Get the time step from layer render options.

    Args:
        optionsH: Layer render options handle

    Returns:
        float: Time step in seconds

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid
    """
    ...

def AEGP_SetLayerRenderOptionsWorldType(optionsH: int, type: int) -> None:
    """
    Set the output world type (bit depth) for layer rendering.

    Args:
        optionsH: Layer render options handle
        type: World type constant:
            - AEGP_WorldType_8: 8 bits per channel
            - AEGP_WorldType_16: 16 bits per channel
            - AEGP_WorldType_32: 32 bits per channel (float)

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Determines the bit depth of the rendered layer output
        - Higher bit depth = better quality but more memory
    """
    ...

def AEGP_GetLayerRenderOptionsWorldType(optionsH: int) -> int:
    """
    Get the output world type from layer render options.

    Args:
        optionsH: Layer render options handle

    Returns:
        int: World type constant (AEGP_WorldType_8, 16, or 32)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid
    """
    ...

def AEGP_SetLayerRenderOptionsDownsampleFactor(optionsH: int, x: int, y: int) -> None:
    """
    Set the downsample factor for layer rendering.

    Args:
        optionsH: Layer render options handle
        x: Horizontal downsample factor (1 = full, 2 = half, etc.)
        y: Vertical downsample factor

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid (factors must be positive, power of 2)

    Note:
        - Higher factors = faster rendering but lower quality
        - Useful for preview renders

    Example:
        >>> options = sdk.AEGP_NewFromLayer(layer_h)
        >>> sdk.AEGP_SetLayerRenderOptionsDownsampleFactor(options, 2, 2)
    """
    ...

def AEGP_GetLayerRenderOptionsDownsampleFactor(optionsH: int) -> tuple[int, int]:
    """
    Get the downsample factor from layer render options.

    Args:
        optionsH: Layer render options handle

    Returns:
        tuple: (x_factor, y_factor) downsample factors

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid
    """
    ...

def AEGP_SetLayerRenderOptionsMatteMode(optionsH: int, mode: int) -> None:
    """
    Set the alpha channel interpretation mode for layer rendering.

    Args:
        optionsH: Layer render options handle
        mode: Matte mode constant:
            - AEGP_MatteMode_STRAIGHT: Straight alpha
            - AEGP_MatteMode_PREMUL_BLACK: Premultiplied with black
            - AEGP_MatteMode_PREMUL_BG_COLOR: Premultiplied with background color

    Raises:
        RuntimeError: If setting fails
        ValueError: If arguments are invalid

    Note:
        - Controls how alpha channel is handled in the layer output
    """
    ...

def AEGP_GetLayerRenderOptionsMatteMode(optionsH: int) -> int:
    """
    Get the alpha channel interpretation mode from layer render options.

    Args:
        optionsH: Layer render options handle

    Returns:
        int: Matte mode constant (AEGP_MatteMode_STRAIGHT, PREMUL_BLACK, or PREMUL_BG_COLOR)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If optionsH is invalid
    """
    ...

# ================================================================
# AEGP_RenderQueueMonitorSuite1
# ================================================================

def AEGP_RQM_RegisterListener() -> None:
    """
    Register a render queue monitor listener (NOT IMPLEMENTED - stub function).

    **Not available from Python**: This function requires C callback function
    pointers for render queue events and cannot be implemented in Python.
    Use render queue polling or external monitoring instead.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RQM_DeregisterListener() -> None:
    """
    Deregister a render queue monitor listener (NOT IMPLEMENTED - stub function).

    **Not available from Python**: This function is paired with RegisterListener
    which is not available in Python.

    Raises:
        RuntimeError: Always raises an exception
    """
    ...

def AEGP_RQM_GetProjectName(session_id: int) -> str:
    """
    Get the project name for a render queue monitoring session.

    Args:
        session_id: Render queue monitoring session ID

    Returns:
        str: Project name

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If session_id is invalid

    Note:
        - Returns the name of the project being rendered
        - Session ID is provided by render queue monitoring events
    """
    ...

def AEGP_RQM_GetAppVersion(session_id: int) -> str:
    """
    Get the After Effects application version for a render queue session.

    Args:
        session_id: Render queue monitoring session ID

    Returns:
        str: Application version string (e.g., "25.0.0")

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If session_id is invalid

    Note:
        - Returns the version of After Effects performing the render
    """
    ...

def AEGP_RQM_GetNumJobItems(session_id: int) -> int:
    """
    Get the number of job items in a render queue session.

    Args:
        session_id: Render queue monitoring session ID

    Returns:
        int: Number of job items in the render queue

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If session_id is invalid

    Note:
        - Each composition in the render queue is a job item
        - Use this to iterate through all job items
    """
    ...

def AEGP_RQM_GetJobItemID(session_id: int, job_item_index: int) -> int:
    """
    Get the job item ID at the specified index.

    Args:
        session_id: Render queue monitoring session ID
        job_item_index: Zero-based job item index (0 to num_items-1)

    Returns:
        int: Job item ID

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid or index is out of range

    Note:
        - Job item IDs are used with other RQM functions
        - Index is zero-based
    """
    ...

def AEGP_RQM_GetNumJobItemRenderSettings(session_id: int, item_id: int) -> int:
    """
    Get the number of render settings for a job item.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID from AEGP_RQM_GetJobItemID

    Returns:
        int: Number of render settings

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid

    Note:
        - Render settings are key-value pairs describing render configuration
        - Use this to iterate through all settings
    """
    ...

def AEGP_RQM_GetJobItemRenderSetting(session_id: int, item_id: int, setting_index: int) -> tuple[str, str]:
    """
    Get a render setting key-value pair for a job item.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        setting_index: Zero-based setting index (0 to num_settings-1)

    Returns:
        tuple: (key, value) pair as (str, str)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid or index is out of range

    Note:
        - Returns render settings like quality, resolution, time span, etc.
        - Both key and value are strings

    Example:
        >>> num_settings = sdk.AEGP_RQM_GetNumJobItemRenderSettings(session_id, item_id)
        >>> for i in range(num_settings):
        ...     key, value = sdk.AEGP_RQM_GetJobItemRenderSetting(session_id, item_id, i)
        ...     print(f"{key}: {value}")
    """
    ...

def AEGP_RQM_GetNumJobItemOutputModules(session_id: int, item_id: int) -> int:
    """
    Get the number of output modules for a job item.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID

    Returns:
        int: Number of output modules

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid

    Note:
        - Each job item can have multiple output modules (different formats/destinations)
        - Use this to iterate through all output modules
    """
    ...

def AEGP_RQM_GetNumJobItemOutputModuleSettings(session_id: int, item_id: int, output_module_index: int) -> int:
    """
    Get the number of settings for an output module.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        output_module_index: Zero-based output module index

    Returns:
        int: Number of output module settings

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid

    Note:
        - Output module settings describe format, codec, path, etc.
        - Use this to iterate through all settings
    """
    ...

def AEGP_RQM_GetJobItemOutputModuleSetting(session_id: int, item_id: int, output_module_index: int, setting_index: int) -> tuple[str, str]:
    """
    Get an output module setting key-value pair.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        output_module_index: Zero-based output module index
        setting_index: Zero-based setting index

    Returns:
        tuple: (key, value) pair as (str, str)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid or indices are out of range

    Note:
        - Returns settings like format, codec, output path, etc.
        - Both key and value are strings

    Example:
        >>> num_modules = sdk.AEGP_RQM_GetNumJobItemOutputModules(session_id, item_id)
        >>> for om_idx in range(num_modules):
        ...     num_settings = sdk.AEGP_RQM_GetNumJobItemOutputModuleSettings(
        ...         session_id, item_id, om_idx)
        ...     for set_idx in range(num_settings):
        ...         key, val = sdk.AEGP_RQM_GetJobItemOutputModuleSetting(
        ...             session_id, item_id, om_idx, set_idx)
        ...         print(f"{key}: {val}")
    """
    ...

def AEGP_RQM_GetNumJobItemOutputModuleWarnings(session_id: int, item_id: int, output_module_index: int) -> int:
    """
    Get the number of warnings for an output module.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        output_module_index: Zero-based output module index

    Returns:
        int: Number of warnings

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid

    Note:
        - Warnings indicate potential issues with output settings
        - Use this to iterate through all warnings
    """
    ...

def AEGP_RQM_GetJobItemOutputModuleWarning(session_id: int, item_id: int, output_module_index: int, warning_index: int) -> str:
    """
    Get a warning message for an output module.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        output_module_index: Zero-based output module index
        warning_index: Zero-based warning index

    Returns:
        str: Warning message

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid or indices are out of range

    Note:
        - Warnings indicate potential issues like codec incompatibility,
          unsupported features, etc.

    Example:
        >>> num_warnings = sdk.AEGP_RQM_GetNumJobItemOutputModuleWarnings(
        ...     session_id, item_id, 0)
        >>> for i in range(num_warnings):
        ...     warning = sdk.AEGP_RQM_GetJobItemOutputModuleWarning(
        ...         session_id, item_id, 0, i)
        ...     print(f"Warning: {warning}")
    """
    ...

def AEGP_RQM_GetNumJobItemFrameProperties(session_id: int, item_id: int, frame_id: int) -> int:
    """
    Get the number of properties for a rendered frame.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        frame_id: Frame ID (frame number)

    Returns:
        int: Number of frame properties

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid

    Note:
        - Frame properties describe the rendered frame (size, duration, etc.)
        - Use this to iterate through all properties
    """
    ...

def AEGP_RQM_GetJobItemFrameProperty(session_id: int, item_id: int, frame_id: int, property_index: int) -> tuple[str, str]:
    """
    Get a frame property key-value pair.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        frame_id: Frame ID (frame number)
        property_index: Zero-based property index

    Returns:
        tuple: (key, value) pair as (str, str)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid or index is out of range

    Note:
        - Returns properties like frame size, render time, etc.
        - Both key and value are strings
    """
    ...

def AEGP_RQM_GetNumJobItemOutputModuleProperties(session_id: int, item_id: int, output_module_index: int) -> int:
    """
    Get the number of properties for an output module.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        output_module_index: Zero-based output module index

    Returns:
        int: Number of output module properties

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid

    Note:
        - Properties describe output module characteristics
        - Use this to iterate through all properties
    """
    ...

def AEGP_RQM_GetJobItemOutputModuleProperty(session_id: int, item_id: int, output_module_index: int, property_index: int) -> tuple[str, str]:
    """
    Get an output module property key-value pair.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        output_module_index: Zero-based output module index
        property_index: Zero-based property index

    Returns:
        tuple: (key, value) pair as (str, str)

    Raises:
        RuntimeError: If retrieval fails
        ValueError: If arguments are invalid or indices are out of range

    Note:
        - Returns properties describing the output module
        - Both key and value are strings
    """
    ...

def AEGP_RQM_GetJobItemFrameThumbnail(session_id: int, item_id: int, frame_id: int, width: int, height: int) -> tuple[int, int, bytes]:
    """
    Get a thumbnail image for a rendered frame.

    This function retrieves a thumbnail preview of a rendered frame
    at the specified dimensions.

    Args:
        session_id: Render queue monitoring session ID
        item_id: Job item ID
        frame_id: Frame ID (frame number)
        width: Desired thumbnail width in pixels
        height: Desired thumbnail height in pixels

    Returns:
        tuple: (actual_width, actual_height, pixel_data)
            - actual_width: Actual thumbnail width (may differ from requested)
            - actual_height: Actual thumbnail height (may differ from requested)
            - pixel_data: Raw pixel data as bytes (format depends on context)

    Raises:
        RuntimeError: If thumbnail retrieval fails
        ValueError: If arguments are invalid

    Note:
        - Thumbnail maintains aspect ratio, so actual size may differ from requested
        - Pixel data format is typically BGRA or ARGB (32 bits per pixel)
        - Useful for monitoring render progress visually

    Example:
        >>> width, height, pixels = sdk.AEGP_RQM_GetJobItemFrameThumbnail(
        ...     session_id, item_id, 0, 320, 240)
        >>> # Save or display thumbnail
        >>> print(f"Thumbnail size: {width}x{height}")
    """
    ...


# ================================================================
# AEGP_MemorySuite1
# ================================================================

# Memory allocation flags
AEGP_MemFlag_NONE: int  # No special flags (0)
AEGP_MemFlag_CLEAR: int  # Clear memory to zero on allocation (1)
AEGP_MemFlag_QUIET: int  # Suppress memory reporting for this allocation (2)


def AEGP_NewMemHandle(what: str, size: int, flags: int = 0) -> int:
    """
    Allocate a new memory handle.

    **WARNING**: Low-level memory operation. You must call AEGP_FreeMemHandle
    to release the memory. For high-level diagnostics, use ae.get_memory_stats().

    Args:
        what: Description string for debugging
        size: Size in bytes to allocate (must be positive)
        flags: Memory flags (AEGP_MemFlag_NONE, AEGP_MemFlag_CLEAR, AEGP_MemFlag_QUIET)

    Returns:
        int: Memory handle (as uintptr_t)

    Raises:
        ValueError: If size <= 0
        RuntimeError: If Memory Suite is not available or allocation fails

    Example:
        >>> mem_h = sdk.AEGP_NewMemHandle("MyData", 1024, sdk.AEGP_MemFlag_CLEAR)
        >>> # Use memory...
        >>> sdk.AEGP_FreeMemHandle(mem_h)  # Always free when done!
    """
    ...


def AEGP_FreeMemHandle(memH: int) -> None:
    """
    Free a memory handle allocated by AEGP_NewMemHandle.

    Args:
        memH: Memory handle to free

    Raises:
        ValueError: If memH is null (0)
        RuntimeError: If Memory Suite is not available or freeing fails

    Note:
        - Call this for every handle allocated with AEGP_NewMemHandle
        - Do not use the handle after freeing
    """
    ...


def AEGP_LockMemHandle(memH: int) -> int:
    """
    Lock a memory handle and return pointer to the data.

    Locks are nestable - each lock must be matched with an unlock.

    Args:
        memH: Memory handle to lock

    Returns:
        int: Pointer to locked memory (as uintptr_t)

    Raises:
        ValueError: If memH is null (0)
        RuntimeError: If Memory Suite is not available or locking fails

    Note:
        - Always call AEGP_UnlockMemHandle when done
        - Multiple locks on the same handle return the same pointer
        - Memory may be moved when all locks are released
    """
    ...


def AEGP_UnlockMemHandle(memH: int) -> None:
    """
    Unlock a memory handle.

    Must be called once for each call to AEGP_LockMemHandle.

    Args:
        memH: Memory handle to unlock

    Raises:
        ValueError: If memH is null (0)
        RuntimeError: If Memory Suite is not available or unlocking fails
    """
    ...


def AEGP_GetMemHandleSize(memH: int) -> int:
    """
    Get the size of a memory handle in bytes.

    Args:
        memH: Memory handle

    Returns:
        int: Size in bytes

    Raises:
        ValueError: If memH is null (0)
        RuntimeError: If Memory Suite is not available or operation fails
    """
    ...


def AEGP_ResizeMemHandle(what: str, new_size: int, memH: int) -> None:
    """
    Resize a memory handle.

    **WARNING**: The underlying memory may be reallocated.
    Do not use old pointers after resizing.

    Args:
        what: Description string for debugging
        new_size: New size in bytes (can be 0 to release data but keep handle)
        memH: Memory handle to resize

    Raises:
        ValueError: If memH is null (0) or new_size is negative
        RuntimeError: If Memory Suite is not available or resize fails
    """
    ...


def AEGP_SetMemReportingOn(enable: bool) -> None:
    """
    Enable or disable detailed memory reporting.

    When enabled, After Effects reports all memory allocations and frees.
    Use for debugging only as it impacts performance.

    Args:
        enable: True to enable, False to disable

    Raises:
        RuntimeError: If Memory Suite is not available

    Note:
        - This affects all plugins, not just the current one
        - Disable when not debugging to avoid performance impact
    """
    ...


def AEGP_GetMemStats() -> dict:
    """
    Get memory statistics for this plugin.

    Returns:
        dict: Dictionary with keys:
            - 'handle_count': Number of memory handles allocated
            - 'total_size': Total memory size in bytes

    Raises:
        RuntimeError: If Memory Suite is not available or operation fails

    Note:
        - For high-level memory diagnostics, prefer ae.get_memory_stats()
        - Use for tracking memory leaks during development

    Example:
        >>> stats = sdk.AEGP_GetMemStats()
        >>> print(f"Handles: {stats['handle_count']}, Size: {stats['total_size']} bytes")
    """
    ...


# ============================================================
# PF Adv App Suite (Refresh/Recalculation)
# ============================================================

def PF_SetProjectDirty() -> None:
    """プロジェクトを「変更あり」としてマーク"""
    ...

def PF_SaveProject() -> None:
    """現在のプロジェクトを保存"""
    ...

def PF_SaveBackgroundState() -> None:
    """バックグラウンド状態を保存"""
    ...

def PF_ForceForeground() -> None:
    """After Effectsをフォアグラウンドに移動"""
    ...

def PF_RestoreBackgroundState() -> None:
    """バックグラウンド状態を復元"""
    ...

def PF_RefreshAllWindows() -> None:
    """すべてのウィンドウを強制リフレッシュ（コンプビューア含む）"""
    ...

def PF_InfoDrawText(line1: str, line2: str) -> None:
    """情報パネルにテキストを表示（2行）"""
    ...

def PF_InfoDrawText3(line1: str, line2: str, line3: str) -> None:
    """情報パネルにテキストを表示（3行）"""
    ...

def PF_InfoDrawText3Plus(line1: str, line2_jr: str, line2_jl: str, line3_jr: str, line3_jl: str) -> None:
    """情報パネルにフォーマット付きテキストを表示（右寄せ・左寄せ対応）"""
    ...

def PF_AppendInfoText(text: str) -> None:
    """情報パネルにテキストを追加"""
    ...

def PF_InfoDrawColor(red: int, green: int, blue: int, alpha: int = 255) -> None:
    """情報パネルにカラーを表示"""
    ...


# ============================================================
# PF Adv Item Suite (Active Item Control)
# ============================================================

# PF_Step constants
PF_Step_FORWARD: int
PF_Step_BACKWARD: int

def PF_MoveTimeStepActiveItem(time_dir: int, num_steps: int) -> None:
    """
    アクティブアイテムの時間ステップを移動

    Args:
        time_dir: PF_Step_FORWARD (0) または PF_Step_BACKWARD (1)
        num_steps: 移動するステップ数
    """
    ...

def PF_TouchActiveItem() -> None:
    """アクティブアイテムを「変更済み」としてマークし、再評価をトリガー"""
    ...

def PF_EffectIsActiveOrEnabled(contextH: int) -> bool:
    """エフェクトがアクティブまたは有効かどうかを確認"""
    ...
