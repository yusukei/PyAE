# PowerShell script to run auto test with environment variables
$env:PYAE_AUTO_TEST = "1"
$env:PYAE_TEST_MODULE = "test_runner"
$env:PYAE_TEST_FUNCTION = "run_all_tests"
$env:PYAE_TEST_OUTPUT = "C:\temp\test_result.json"
$env:PYAE_TEST_EXIT = "1"
$env:PYAE_TEST_DELAY = "2"

Write-Host "Environment variables set:"
Write-Host "PYAE_AUTO_TEST=$env:PYAE_AUTO_TEST"
Write-Host "PYAE_TEST_MODULE=$env:PYAE_TEST_MODULE"
Write-Host "PYAE_TEST_FUNCTION=$env:PYAE_TEST_FUNCTION"
Write-Host "PYAE_TEST_OUTPUT=$env:PYAE_TEST_OUTPUT"
Write-Host "Starting After Effects..."

& "C:\Program Files\Adobe\Adobe After Effects 2025\Support Files\AfterFX.exe"
