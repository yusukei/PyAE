# ae_serialize.py
# PyAE - Python for After Effects
# Serialization module for to_dict() / from_dict() / update_from_dict() API
#
# This module provides high-level serialization API for AE objects.
# It wraps existing export_scene.py and import_scene.py functionality.

import ae
import struct

# Import existing export functions
from export_scene import (
    export_comp as _export_comp,
    export_layer as _export_layer,
    export_property_tree as _export_property_tree,
    export_effect as _export_effect,
    export_param as _export_param,
    export_footage as _export_footage,
    value_to_bdata,
    item_type_to_string,
    layer_type_to_string,
)

# Import existing import functions
from import_scene import (
    bdata_to_value,
    bdata_to_path_vertices,
    SceneImporter,
)


# =============================================================================
# Serialization Context
# =============================================================================

class SerializationContext:
    """
    Context manager for serialization/deserialization operations.
    Handles ID-to-object mapping for reference resolution.
    """

    def __init__(self):
        self.id_to_item = {0: None}  # 0 = root folder (None)
        self.project = None
        self._next_id = 1

    def register(self, json_id, ae_item):
        """Register an AE item with its JSON ID for later reference resolution."""
        self.id_to_item[json_id] = ae_item

    def resolve(self, json_id):
        """Resolve a JSON ID to its corresponding AE item."""
        return self.id_to_item.get(json_id)

    def generate_id(self):
        """Generate a unique ID for new items."""
        id = self._next_id
        self._next_id += 1
        return id


# =============================================================================
# Project Serialization
# =============================================================================

def project_to_dict(project=None):
    """
    Export project to dictionary.

    Args:
        project: Project object (optional, uses current project if None)

    Returns:
        dict: Project data with version and items
    """
    if project is None:
        project = ae.Project.get_current()

    if not project:
        raise RuntimeError("No project open")

    data = {"version": 1, "items": []}

    for item in project.items:
        item_data = {
            "name": item.name,
            "type": item_type_to_string(item.type),
            "id": item.id,
        }

        # Parent folder ID (optional)
        try:
            parent_folder = item.parent_folder
            if parent_folder and hasattr(parent_folder, "id"):
                item_data["parent_folder_id"] = parent_folder.id
        except AttributeError:
            pass

        if item.type == ae.ItemType.Comp:
            item_data["comp_data"] = comp_to_dict(item)
        elif item.type == ae.ItemType.Footage:
            item_data["footage_data"] = _export_footage(item)

        data["items"].append(item_data)

    return data


def project_from_dict(data, context=None):
    """
    Import project from dictionary into current project.

    This creates new items in the current project based on the data.

    Args:
        data: Project dictionary (from project_to_dict())
        context: SerializationContext (optional, created if None)

    Returns:
        SerializationContext: Context with ID mappings
    """
    if context is None:
        context = SerializationContext()

    context.project = ae.Project.get_current()
    if not context.project:
        raise RuntimeError("No project open")

    items = data.get("items", [])

    ae.begin_undo_group("Import from dictionary")

    try:
        # Phase 1: Create folders
        _create_folders_from_dict(items, context)

        # Phase 2: Create compositions
        _create_comps_from_dict(items, context)

        # Phase 3: Create footages
        _create_footages_from_dict(items, context)

        # Phase 4: Restore layers
        _restore_layers_from_dict(items, context)

    finally:
        ae.end_undo_group()
        ae.refresh()

    return context


def project_update_from_dict(project, data, context=None):
    """
    Update existing project from dictionary.

    This updates existing items in the project based on matching IDs or names.

    Args:
        project: Project object to update
        data: Project dictionary
        context: SerializationContext (optional)
    """
    if context is None:
        context = SerializationContext()

    context.project = project

    # Build ID mapping from existing items
    for item in project.items:
        context.register(item.id, item)

    items = data.get("items", [])

    ae.begin_undo_group("Update from dictionary")

    try:
        for item_data in items:
            item_id = item_data.get("id")
            existing_item = context.resolve(item_id)

            if existing_item and item_data.get("type") == "Comp":
                comp_data = item_data.get("comp_data", {})
                comp_update_from_dict(existing_item, comp_data, context)

    finally:
        ae.end_undo_group()
        ae.refresh()


# =============================================================================
# Comp Serialization
# =============================================================================

def comp_to_dict(comp):
    """
    Export composition to dictionary.

    Args:
        comp: Comp object

    Returns:
        dict: Composition data including layers
    """
    return _export_comp(comp)


def comp_from_dict(data, parent_folder=None, context=None):
    """
    Create new composition from dictionary.

    Args:
        data: Comp dictionary (from comp_to_dict())
        parent_folder: Parent folder (optional)
        context: SerializationContext (optional)

    Returns:
        Comp: Newly created composition
    """
    if context is None:
        context = SerializationContext()

    project = context.project or ae.Project.get_current()
    if not project:
        raise RuntimeError("No project open")

    # Create composition
    comp = project.create_comp(
        data.get("name", "New Comp"),
        int(data.get("width", 1920)),
        int(data.get("height", 1080)),
        float(data.get("pixel_aspect", 1.0)),
        float(data.get("duration", 10.0)),
        float(data.get("frame_rate", 30.0)),
        parent_folder
    )

    # Apply settings
    _apply_comp_settings(comp, data)

    # Restore layers
    layers_data = data.get("layers", [])
    if layers_data:
        _restore_comp_layers(comp, layers_data, context)

    return comp


def comp_update_from_dict(comp, data, context=None):
    """
    Update existing composition from dictionary.

    Args:
        comp: Comp object to update
        data: Comp dictionary
        context: SerializationContext (optional)
    """
    if context is None:
        context = SerializationContext()

    # Update settings
    _apply_comp_settings(comp, data)

    # Update layers
    layers_data = data.get("layers", [])
    if layers_data:
        _update_comp_layers(comp, layers_data, context)


def _apply_comp_settings(comp, data):
    """Apply composition settings from dictionary."""
    # Background color
    bg_color = data.get("background_color")
    if bg_color and len(bg_color) >= 3:
        try:
            comp.bg_color = tuple(bg_color[:3])
        except (AttributeError, RuntimeError):
            pass

    # Work area
    work_start = data.get("work_area_start")
    work_duration = data.get("work_area_duration")
    if work_start is not None:
        try:
            comp.work_area_start = float(work_start)
        except (AttributeError, RuntimeError):
            pass
    if work_duration is not None:
        try:
            comp.work_area_duration = float(work_duration)
        except (AttributeError, RuntimeError):
            pass


def _restore_comp_layers(comp, layers_data, context):
    """Restore layers in composition from dictionary data."""
    # Sort by index (descending) - AE adds layers at top
    sorted_layers = sorted(layers_data, key=lambda x: x.get("index", 0), reverse=True)

    created_layers = []
    for layer_data in sorted_layers:
        layer = layer_from_dict(comp, layer_data, context)
        if layer:
            created_layers.append((layer, layer_data))

    # Restore layer hierarchy (parent relationships)
    _restore_layer_hierarchy(comp, layers_data)

    # Restore track mattes
    _restore_track_mattes(comp, layers_data)


def _update_comp_layers(comp, layers_data, context):
    """Update existing layers in composition."""
    for layer_data in layers_data:
        layer_index = layer_data.get("index")
        if layer_index is not None and layer_index < comp.num_layers:
            layer = comp.layer(layer_index)
            if layer:
                layer_update_from_dict(layer, layer_data, context)


# =============================================================================
# Layer Serialization
# =============================================================================

def layer_to_dict(layer, comp=None):
    """
    Export layer to dictionary.

    Args:
        layer: Layer object
        comp: Parent composition (optional)

    Returns:
        dict: Layer data including properties and effects
    """
    return _export_layer(layer, comp)


def layer_from_dict(comp, data, context=None):
    """
    Create new layer in composition from dictionary.

    Args:
        comp: Parent composition
        data: Layer dictionary (from layer_to_dict())
        context: SerializationContext (optional)

    Returns:
        Layer: Newly created layer
    """
    if context is None:
        context = SerializationContext()

    layer_type = data.get("type", "").lower()
    layer_name = data.get("name", "Layer")
    duration = data.get("out_point", comp.duration) - data.get("in_point", 0.0)

    is_null = data.get("is_null", False)

    # Create layer based on type
    layer = None

    if is_null or layer_type == "null":
        layer = comp.add_null(layer_name, duration)
    elif layer_type == "solid":
        width = data.get("width", comp.width)
        height = data.get("height", comp.height)
        color = data.get("solid_color", [0.0, 0.0, 0.0])
        layer = comp.add_solid(layer_name, width, height, color, duration)
    elif layer_type == "text":
        source_text = data.get("source_text", "")
        layer = comp.add_text(source_text)
        layer.name = layer_name
    elif layer_type == "shape":
        layer = comp.add_shape()
        layer.name = layer_name
    elif layer_type == "camera":
        layer = comp.add_camera(layer_name, [comp.width / 2, comp.height / 2])
    elif layer_type == "light":
        layer = comp.add_light(layer_name, [comp.width / 2, comp.height / 2])
    elif layer_type == "adjustment":
        layer = comp.add_solid(layer_name, comp.width, comp.height, (1.0, 1.0, 1.0), duration)
        if layer:
            layer.is_adjustment = True
    elif layer_type == "av":
        # AV layer needs source item
        source_item_id = data.get("source_item_id")
        source_item = context.resolve(source_item_id) if source_item_id else None
        if source_item:
            layer = comp.add_layer(source_item, duration)
        else:
            # Fallback to null layer
            layer = comp.add_null(layer_name, duration)

    if layer:
        # Apply layer settings
        _apply_layer_settings(layer, data)

        # Restore properties
        properties_data = data.get("properties", {})
        if properties_data:
            _restore_layer_properties(layer, properties_data, context)

        # Restore effects
        effects_data = data.get("effects", [])
        if effects_data:
            _restore_layer_effects(layer, effects_data, context)

        # Restore masks
        masks_data = data.get("masks", [])
        if masks_data:
            _restore_layer_masks(layer, masks_data, context)

        # Restore text animators
        text_animators = data.get("text_animators", [])
        if text_animators and layer_type == "text":
            _restore_text_animators(layer, text_animators, context)

    return layer


def layer_update_from_dict(layer, data, context=None):
    """
    Update existing layer from dictionary.

    Args:
        layer: Layer object to update
        data: Layer dictionary
        context: SerializationContext (optional)
    """
    if context is None:
        context = SerializationContext()

    # Update layer settings
    _apply_layer_settings(layer, data)

    # Update properties
    properties_data = data.get("properties", {})
    if properties_data:
        _restore_layer_properties(layer, properties_data, context)

    # Update effects
    effects_data = data.get("effects", [])
    if effects_data:
        _update_layer_effects(layer, effects_data, context)


def _apply_layer_settings(layer, data):
    """Apply layer settings from dictionary."""
    # Basic settings
    if "name" in data:
        layer.name = data["name"]

    if "active" in data:
        layer.enabled = data["active"]

    if "in_point" in data:
        layer.in_point = float(data["in_point"])

    if "out_point" in data:
        layer.out_point = float(data["out_point"])

    if "start_time" in data:
        layer.start_time = float(data["start_time"])

    # Flags
    if data.get("is_3d"):
        layer.is_3d = True

    if data.get("is_adjustment"):
        layer.is_adjustment = True

    # Optional settings
    if "solo" in data:
        try:
            layer.solo = data["solo"]
        except (AttributeError, RuntimeError):
            pass

    if "locked" in data:
        try:
            layer.locked = data["locked"]
        except (AttributeError, RuntimeError):
            pass

    if "shy" in data:
        try:
            layer.shy = data["shy"]
        except (AttributeError, RuntimeError):
            pass

    if "blend_mode" in data:
        try:
            layer.blend_mode = data["blend_mode"]
        except (AttributeError, RuntimeError):
            pass

    if "label" in data:
        try:
            layer.label = data["label"]
        except (AttributeError, RuntimeError):
            pass


def _restore_layer_hierarchy(comp, layers_data):
    """Restore layer parent relationships."""
    layers = []
    for i in range(comp.num_layers):
        layers.append(comp.layer(i))

    for layer_data in layers_data:
        index = layer_data.get("index", 0)
        parent_index = layer_data.get("parent_index")

        if parent_index is not None and index < len(layers) and parent_index < len(layers):
            try:
                layers[index].parent = layers[parent_index]
            except (RuntimeError, AttributeError):
                pass


def _restore_track_mattes(comp, layers_data):
    """Restore track matte settings."""
    for layer_data in layers_data:
        index = layer_data.get("index", 0)
        track_matte = layer_data.get("track_matte")

        if track_matte is not None and index < comp.num_layers:
            try:
                layer = comp.layer(index)
                layer.track_matte = track_matte
            except (RuntimeError, AttributeError):
                pass


# =============================================================================
# Property Serialization
# =============================================================================

def property_to_dict(prop):
    """
    Export property to dictionary.

    Args:
        prop: Property object

    Returns:
        dict: Property data including value, keyframes, expression
    """
    return _export_property_tree(prop)


def property_update_from_dict(prop, data, context=None):
    """
    Update property from dictionary.

    Args:
        prop: Property object to update
        data: Property dictionary (from property_to_dict())
        context: SerializationContext (optional)
    """
    if context is None:
        context = SerializationContext()

    prop_type = data.get("type", "property")

    if prop_type == "group":
        # If this is a group, restore children recursively
        children = data.get("children", {})
        for child_name, child_data in children.items():
            _restore_property_recursive(prop, child_data, context)
    else:
        # Leaf property - restore value directly
        _restore_property_value(prop, data, context)


def _restore_layer_properties(layer, properties_data, context):
    """Restore layer properties from dictionary."""
    root = layer.properties
    if not root:
        return

    for prop_name, prop_data in properties_data.items():
        _restore_property_recursive(root, prop_data, context)


def _restore_property_recursive(parent, prop_data, context):
    """Recursively restore property tree."""
    if not isinstance(prop_data, dict):
        return

    match_name = prop_data.get("match_name")
    if not match_name:
        return

    prop_type = prop_data.get("type", "property")

    # Try to get the property
    prop = None
    try:
        prop = parent.property(match_name)
    except (RuntimeError, AttributeError):
        pass

    if not prop or not prop.valid:
        # Try to add if parent is indexed group
        try:
            if hasattr(parent, "grouping_type") and parent.grouping_type == 2:
                if parent.can_add_stream(match_name):
                    prop = parent.add_stream(match_name)
        except (RuntimeError, AttributeError):
            pass

    if not prop or not prop.valid:
        return

    if prop_type == "group":
        # Restore group enabled state
        if "enabled" in prop_data:
            try:
                prop.set_dynamic_stream_flag(0x01, True, prop_data["enabled"])
            except (RuntimeError, AttributeError):
                pass

        # Restore children
        children = prop_data.get("children", {})
        for child_name, child_data in children.items():
            _restore_property_recursive(prop, child_data, context)
    else:
        # Restore leaf property value
        _restore_property_value(prop, prop_data, context)


def _restore_property_value(prop, prop_data, context):
    """Restore a single property value."""
    match_name = prop_data.get("match_name", "")

    # Expression
    expression = prop_data.get("expression")
    if expression:
        try:
            prop.expression = expression
        except (RuntimeError, AttributeError):
            pass

    # Special handling for Shape Path
    if match_name == "ADBE Vector Shape - Group":
        bdata = prop_data.get("bdata", "")
        if bdata:
            vertices = bdata_to_path_vertices(bdata)
            if vertices:
                try:
                    prop.set_shape_path_vertices(vertices, 0.0)
                except (RuntimeError, AttributeError):
                    pass

        # Keyframes for shape path
        for kf_data in prop_data.get("keyframes", []):
            time = kf_data.get("time", 0.0)
            kf_bdata = kf_data.get("bdata", "")
            if kf_bdata:
                vertices = bdata_to_path_vertices(kf_bdata)
                if vertices:
                    try:
                        prop.set_shape_path_vertices(vertices, time)
                    except (RuntimeError, AttributeError):
                        pass
        return

    # Regular property value
    value = prop_data.get("_value")
    if value is None:
        bdata = prop_data.get("bdata", "")
        if bdata:
            value = bdata_to_value(bdata)

    if value is not None:
        try:
            prop.value = value
        except (RuntimeError, AttributeError):
            pass

    # Keyframes
    keyframes = prop_data.get("keyframes", [])
    if keyframes and prop.can_have_keyframes:
        _restore_keyframes(prop, keyframes)


def _restore_keyframes(prop, keyframes_data):
    """Restore keyframes for a property."""
    for kf_data in keyframes_data:
        time = kf_data.get("time", 0.0)
        bdata = kf_data.get("bdata", "")

        value = bdata_to_value(bdata)
        if value is None:
            continue

        try:
            kf_index = prop.add_keyframe(time, value)

            # Interpolation
            in_interp = kf_data.get("in_interpolation")
            out_interp = kf_data.get("out_interpolation")
            if in_interp is not None and out_interp is not None:
                prop.set_keyframe_interpolation(kf_index, in_interp, out_interp)
        except (RuntimeError, AttributeError):
            pass


# =============================================================================
# Effect Serialization
# =============================================================================

def effect_to_dict(effect):
    """
    Export effect to dictionary.

    Args:
        effect: Effect object

    Returns:
        dict: Effect data including parameters
    """
    return _export_effect(effect)


def effect_from_dict(layer, data, context=None):
    """
    Add effect to layer from dictionary.

    Args:
        layer: Layer to add effect to
        data: Effect dictionary (from effect_to_dict())
        context: SerializationContext (optional)

    Returns:
        Effect: Newly created effect
    """
    if context is None:
        context = SerializationContext()

    match_name = data.get("match_name", "")
    if not match_name:
        return None

    try:
        effect = layer.add_effect(match_name)
    except (RuntimeError, AttributeError):
        return None

    if effect:
        # Set custom name
        stream_name = data.get("stream_name")
        if stream_name:
            try:
                effect.set_name(stream_name)
            except (RuntimeError, AttributeError):
                pass

        # Set enabled state
        if "enabled" in data:
            try:
                effect.enabled = data["enabled"]
            except (RuntimeError, AttributeError):
                pass

        # Restore parameters
        params_data = data.get("params", [])
        _restore_effect_params(effect, params_data, context)

    return effect


def effect_update_from_dict(effect, data, context=None):
    """
    Update effect from dictionary.

    Args:
        effect: Effect object to update
        data: Effect dictionary
        context: SerializationContext (optional)
    """
    if context is None:
        context = SerializationContext()

    # Update enabled state
    if "enabled" in data:
        try:
            effect.enabled = data["enabled"]
        except (RuntimeError, AttributeError):
            pass

    # Update parameters
    params_data = data.get("params", [])
    _restore_effect_params(effect, params_data, context)


def _restore_layer_effects(layer, effects_data, context):
    """Restore all effects for a layer."""
    for effect_data in effects_data:
        effect_from_dict(layer, effect_data, context)


def _update_layer_effects(layer, effects_data, context):
    """Update existing effects for a layer."""
    for i, effect_data in enumerate(effects_data):
        if i < layer.num_effects:
            effect = layer.effect(i)
            if effect:
                effect_update_from_dict(effect, effect_data, context)


def _restore_effect_params(effect, params_data, context):
    """Restore effect parameters."""
    for param_data in params_data:
        param_index = param_data.get("index")
        if param_index is None:
            continue

        try:
            param = effect.property_by_index(param_index)
            if param and param.valid:
                bdata = param_data.get("bdata", "")
                if bdata:
                    value = bdata_to_value(bdata)
                    if value is not None:
                        param.value = value

                # Keyframes
                keyframes = param_data.get("keyframes", [])
                if keyframes and param.can_have_keyframes:
                    _restore_keyframes(param, keyframes)
        except (RuntimeError, AttributeError):
            pass


# =============================================================================
# Footage Serialization
# =============================================================================

def footage_to_dict(footage):
    """
    Export footage to dictionary.

    Args:
        footage: Footage object

    Returns:
        dict: Footage data including type, dimensions, and settings

    Example::

        footage = project.items[0]  # Get footage from project
        data = footage_to_dict(footage)
        # data = {
        #     "name": "my_video.mp4",
        #     "footage_type": "File",
        #     "width": 1920,
        #     "height": 1080,
        #     ...
        # }
    """
    # Use existing export function for base data
    data = _export_footage(footage)

    # Add item-level properties if in project
    if hasattr(footage, 'is_in_project') and footage.is_in_project:
        # Name
        try:
            data["name"] = footage.name
        except (AttributeError, RuntimeError):
            pass

        # Comment
        try:
            comment = footage.comment
            if comment:
                data["comment"] = comment
        except (AttributeError, RuntimeError):
            pass

        # Label
        try:
            data["label"] = footage.label
        except (AttributeError, RuntimeError):
            pass

        # Selected
        try:
            data["selected"] = footage.selected
        except (AttributeError, RuntimeError):
            pass

        # Frame rate
        try:
            data["frame_rate"] = footage.frame_rate
        except (AttributeError, RuntimeError):
            pass

        # Solid color (for Solid type)
        if data.get("footage_type") == "Solid":
            try:
                solid_color = footage.get_solid_color()
                if solid_color:
                    data["solid_color"] = list(solid_color)
            except (AttributeError, RuntimeError):
                pass

        # Interpretation
        try:
            interp = footage.interpretation
            if interp:
                data["interpretation"] = interp
        except (AttributeError, RuntimeError):
            pass

        # Item ID
        try:
            data["item_id"] = footage.item_id
        except (AttributeError, RuntimeError):
            pass

    return data


def footage_from_dict(data, parent_folder=None, context=None):
    """
    Create new footage from dictionary.

    Args:
        data: Footage dictionary (from footage_to_dict())
        parent_folder: Parent folder (optional)
        context: SerializationContext (optional)

    Returns:
        Footage: Newly created footage in project

    Example::

        data = {
            "footage_type": "Solid",
            "name": "Red Solid",
            "width": 1920,
            "height": 1080,
            "solid_color": [1.0, 0.0, 0.0, 1.0]
        }
        footage = footage_from_dict(data)
    """
    import os

    if context is None:
        context = SerializationContext()

    project = context.project or ae.Project.get_current()
    if not project:
        raise RuntimeError("No project open")

    footage_type = data.get("footage_type", "")
    name = data.get("name", "Footage")
    width = int(data.get("width", 1920))
    height = int(data.get("height", 1080))
    duration = float(data.get("duration", 1.0))

    footage = None

    # Create footage based on type
    if footage_type == "Solid":
        # Create solid footage
        solid_color = data.get("solid_color", [1.0, 1.0, 1.0, 1.0])
        if len(solid_color) < 4:
            solid_color = list(solid_color) + [1.0] * (4 - len(solid_color))
        footage = ae.Footage.solid(name, width, height, tuple(solid_color[:4]))

    elif footage_type == "Placeholder":
        # Create placeholder footage
        file_path = data.get("file_path")
        if file_path:
            footage = ae.Footage.placeholder_with_path(file_path, width, height, duration)
        else:
            footage = ae.Footage.placeholder(name, width, height, duration)

    elif footage_type == "File":
        # Create from file
        file_path = data.get("file_path", "")
        if file_path and os.path.exists(file_path):
            try:
                footage = ae.Footage.from_file(file_path)
            except (RuntimeError, AttributeError):
                # Fallback to placeholder if file cannot be loaded
                footage = ae.Footage.placeholder_with_path(file_path, width, height, duration)
        elif file_path:
            # File doesn't exist, create placeholder with path
            footage = ae.Footage.placeholder_with_path(file_path, width, height, duration)

    elif footage_type == "Missing":
        # Create placeholder for missing footage
        file_path = data.get("file_path", "")
        if file_path:
            footage = ae.Footage.placeholder_with_path(file_path, width, height, duration)
        else:
            footage = ae.Footage.placeholder(name, width, height, duration)

    # Add to project
    if footage:
        # Determine folder handle
        folder_handle = 0
        if parent_folder:
            if hasattr(parent_folder, '_handle'):
                folder_handle = parent_folder._handle
            elif hasattr(parent_folder, 'id'):
                folder_handle = parent_folder.id
            elif isinstance(parent_folder, int):
                folder_handle = parent_folder

        try:
            footage.add_to_project(folder_handle)
        except (RuntimeError, AttributeError):
            pass

        # Apply additional settings after adding to project
        _apply_footage_settings(footage, data)

    return footage


def footage_update_from_dict(footage, data, context=None):
    """
    Update existing footage from dictionary.

    Note: Some properties like footage_type cannot be changed after creation.
    This function updates modifiable properties only.

    Args:
        footage: Footage object to update
        data: Footage dictionary
        context: SerializationContext (optional)

    Example::

        footage = project.items[0]
        footage_update_from_dict(footage, {
            "name": "New Name",
            "comment": "Updated comment",
            "solid_color": [0.5, 0.5, 0.5, 1.0]
        })
    """
    if context is None:
        context = SerializationContext()

    # Check if footage is in project
    if hasattr(footage, 'is_in_project') and not footage.is_in_project:
        raise RuntimeError("Footage must be in project to update")

    _apply_footage_settings(footage, data)


def _apply_footage_settings(footage, data):
    """Apply footage settings from dictionary."""
    # Name
    if "name" in data:
        try:
            footage.name = data["name"]
        except (AttributeError, RuntimeError):
            pass

    # Comment
    if "comment" in data:
        try:
            footage.comment = data["comment"]
        except (AttributeError, RuntimeError):
            pass

    # Label
    if "label" in data:
        try:
            footage.label = data["label"]
        except (AttributeError, RuntimeError):
            pass

    # Selected
    if "selected" in data:
        try:
            footage.selected = data["selected"]
        except (AttributeError, RuntimeError):
            pass

    # Solid color (for Solid type only)
    if "solid_color" in data:
        solid_color = data["solid_color"]
        if len(solid_color) >= 4:
            try:
                footage.set_solid_color(tuple(solid_color[:4]))
            except (AttributeError, RuntimeError):
                pass

    # Solid dimensions
    if "width" in data and "height" in data:
        footage_type = data.get("footage_type")
        if footage_type == "Solid":
            try:
                footage.set_solid_dimensions(int(data["width"]), int(data["height"]))
            except (AttributeError, RuntimeError):
                pass

    # Interpretation
    if "interpretation" in data:
        interp = data["interpretation"]
        alpha_mode = interp.get("alpha_mode", "")
        conform_fps = interp.get("conform_fps", 0.0)
        try:
            footage.set_interpretation(alpha_mode, conform_fps)
        except (AttributeError, RuntimeError):
            pass


# =============================================================================
# Mask Serialization
# =============================================================================

def _restore_layer_masks(layer, masks_data, context):
    """Restore masks for a layer."""
    root = layer.properties
    if not root:
        return

    try:
        masks_group = root.property("ADBE Mask Parade")
        if not masks_group:
            return

        num_existing = masks_group.num_properties

        for i, mask_data in enumerate(masks_data):
            if i < num_existing:
                mask = masks_group.property(i)
                if mask:
                    properties = mask_data.get("properties", {})
                    for prop_name, prop_data in properties.items():
                        _restore_property_recursive(mask, prop_data, context)
    except (RuntimeError, AttributeError):
        pass


# =============================================================================
# Text Animator Serialization
# =============================================================================

def _restore_text_animators(layer, animators_data, context):
    """Restore text animators for a text layer."""
    root = layer.properties
    if not root:
        return

    try:
        text_props = root.property("ADBE Text Properties")
        if not text_props:
            return

        animators_group = text_props.property("ADBE Text Animators")
        if not animators_group:
            return

        for animator_data in animators_data:
            animator = animators_group.add_stream("ADBE Text Animator")
            if animator:
                # Set name
                name = animator_data.get("name")
                if name:
                    animator.set_name(name)

                # Restore selectors
                selectors_data = animator_data.get("selectors", [])
                _restore_animator_selectors(animator, selectors_data, context)

                # Restore properties
                properties = animator_data.get("properties", {})
                _restore_animator_properties(animator, properties, context)
    except (RuntimeError, AttributeError):
        pass


def _restore_animator_selectors(animator, selectors_data, context):
    """Restore text animator selectors."""
    try:
        selectors_group = animator.property("ADBE Text Selectors")
        if not selectors_group:
            return

        for selector_data in selectors_data:
            match_name = selector_data.get("match_name", "ADBE Text Selector")
            selector = selectors_group.add_stream(match_name)
            if selector:
                properties = selector_data.get("properties", {})
                for prop_name, prop_data in properties.items():
                    _restore_property_recursive(selector, prop_data, context)
    except (RuntimeError, AttributeError):
        pass


def _restore_animator_properties(animator, properties_data, context):
    """Restore text animator properties."""
    try:
        anim_props = animator.property("ADBE Text Animator Properties")
        if not anim_props:
            return

        for prop_name, prop_data in properties_data.items():
            _restore_property_recursive(anim_props, prop_data, context)
    except (RuntimeError, AttributeError):
        pass


# =============================================================================
# Helper Functions for from_dict
# =============================================================================

def _create_folders_from_dict(items, context):
    """Create folders from dictionary items."""
    folder_items = [i for i in items if i.get("type") == "Folder"]

    remaining = folder_items[:]
    max_iterations = len(folder_items) + 1
    iteration = 0

    while remaining and iteration < max_iterations:
        created = []
        for item in remaining:
            item_id = item.get("id", -1)
            parent_id = item.get("parent_folder_id", 0)

            if parent_id not in context.id_to_item:
                continue

            parent_folder = context.resolve(parent_id)
            try:
                folder = context.project.create_folder(item.get("name", "Folder"), parent_folder)
                context.register(item_id, folder)
                created.append(item)
            except (RuntimeError, AttributeError):
                pass

        remaining = [i for i in remaining if i not in created]
        iteration += 1


def _create_comps_from_dict(items, context):
    """Create compositions from dictionary items."""
    comp_items = [i for i in items if i.get("type") == "Comp"]

    for item in comp_items:
        item_id = item.get("id")
        parent_id = item.get("parent_folder_id", 0)
        comp_data = item.get("comp_data", {})

        parent_folder = context.resolve(parent_id)

        # Add name to comp_data if not present
        if "name" not in comp_data:
            comp_data["name"] = item.get("name", "Comp")

        try:
            # Create comp without restoring layers yet
            comp = context.project.create_comp(
                comp_data.get("name", "Comp"),
                int(comp_data.get("width", 1920)),
                int(comp_data.get("height", 1080)),
                float(comp_data.get("pixel_aspect", 1.0)),
                float(comp_data.get("duration", 10.0)),
                float(comp_data.get("frame_rate", 30.0)),
                parent_folder
            )
            _apply_comp_settings(comp, comp_data)
            context.register(item_id, comp)
        except (RuntimeError, AttributeError):
            pass


def _create_footages_from_dict(items, context):
    """Create footages from dictionary items."""
    import os

    footage_items = [i for i in items if i.get("type") == "Footage"]

    for item in footage_items:
        item_id = item.get("id")
        parent_id = item.get("parent_folder_id", 0)
        footage_data = item.get("footage_data", {})
        footage_type = footage_data.get("footage_type", "")

        parent_folder = context.resolve(parent_id)

        if footage_type == "File":
            file_path = footage_data.get("file_path", "")
            if file_path and os.path.exists(file_path):
                try:
                    footage = context.project.import_file(file_path, parent_folder)
                    context.register(item_id, footage)
                except (RuntimeError, AttributeError):
                    pass


def _restore_layers_from_dict(items, context):
    """Restore layers for all compositions."""
    comp_items = [i for i in items if i.get("type") == "Comp"]

    for item in comp_items:
        item_id = item.get("id")
        comp = context.resolve(item_id)
        if not comp:
            continue

        comp_data = item.get("comp_data", {})
        layers_data = comp_data.get("layers", [])

        if layers_data:
            _restore_comp_layers(comp, layers_data, context)
