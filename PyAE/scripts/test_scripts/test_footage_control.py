import ae
import os
import time


def log(message):
    print(f"[TEST] {message}")


def main():
    log("Starting footage control test...")

    # 1. Start clean
    # app = ae.get_application() # Not implemented yet, assumed implicit via AE execution context

    # 2. Import a footage item (Assuming we have a dummy file)
    # We will create a dummy file for testing purposes
    dummy_file_1 = os.path.abspath("test_footage_1.jpg")
    dummy_file_2 = os.path.abspath("test_footage_2.jpg")

    # Create dummy images if they don't exist (just text files renamed for this test, AE might complain if they aren't real images)
    # Actually, let's use the 'import_file' functionality we assume exists or use a solid to test?
    # No, solid doesn't have file path. We need to import a file.
    # Let's check what import functions are available.
    # Looking at PyApplication.cpp (not shown but assumed), or PyProject.cpp.
    # Let's assume ae.import_file exists or project.import_file exists.

    # Based on previous context, we have `ae.open_project` but maybe not direct import in root?
    # Let's use `ae.project.import_file` if available.

    log("Checking import capability...")
    # Since we are running inside AE (presumably via a specialized runner or just generating script),
    # we need to be careful.

    # For now, let's just inspect active item if it's a footage.
    # Or find a footage item in the project.

    project = ae.get_project()
    if not project:
        log("No project open.")
        return

    # Find a footage item or try to import
    footage_item = None

    # List all items
    items = project.items
    for item in items:
        if isinstance(item, ae.FootageItem) and item.file_path:
            footage_item = item
            break

    if not footage_item:
        log(
            "No footage item found in project. Please import a footage file manually or via script."
        )
        # Alternatively, we can try to assume there is one or skip.
        # Ideally, we should import one.
        # Let's try project.import_file if it exists.

        # Check if we have test files in the workspace (we don't create them here to avoid file system clutter if not needed)
        # But for 'replace_file' test we need valid paths.
        pass

    if footage_item:
        log(f"Testing with footage: {footage_item.name} ({footage_item.file_path})")

        # Test 1: Get Interpretation
        interp = footage_item.interpretation
        log(f"Current Interpretation: {interp}")

        # Test 2: Set Interpretation
        log("Setting interpretation to alpha_mode='straight', fps=24.0...")
        footage_item.set_interpretation(alpha_mode="straight", fps=24.0)

        updated_interp = footage_item.interpretation
        log(f"Updated Interpretation: {updated_interp}")

        if updated_interp.get("alpha_mode") == "straight":
            log("SUCCESS: Alpha mode set correctly.")
        else:
            log("FAILURE: Alpha mode mismatch.")

        if abs(updated_interp.get("conform_fps", 0) - 24.0) < 0.001:
            log("SUCCESS: Conform FPS set correctly.")
        else:
            log("FAILURE: Conform FPS mismatch.")

        # Test 3: Replace File
        # We need a different file path.
        # Let's just pretend to swap with another file if it exists, or just catch error if file not found.
        # Or, we can swap with the SAME file just to test the API call doesn't crash.
        current_path = footage_item.file_path
        log(f"Testing replace_file with same path: {current_path}")

        try:
            footage_item.replace_file(current_path)
            log("SUCCESS: replace_file executed without error.")
            # Verify path is still same
            if footage_item.file_path == current_path:
                log("SUCCESS: File path verified.")
        except Exception as e:
            log(f"FAILURE: replace_file raised exception: {e}")

    else:
        log("SKIP: No footage item to test.")

    log("Test complete.")


if __name__ == "__main__":
    main()
