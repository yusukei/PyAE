"""
Windows Process Management Utilities for PyAE Testing

This module provides robust process management utilities for
running tests in After Effects with proper timeout, monitoring,
and cleanup capabilities.
"""

import subprocess
import time
import os
import sys
import json
import signal
from datetime import datetime
from pathlib import Path
from typing import Optional, Dict, List, Tuple, Any
from dataclasses import dataclass, asdict
from enum import Enum


class ProcessStatus(Enum):
    """Process execution status."""
    SUCCESS = "success"
    TIMEOUT = "timeout"
    CRASHED = "crashed"
    NOT_FOUND = "not_found"
    ERROR = "error"


@dataclass
class ProcessResult:
    """Result of a process execution."""
    status: ProcessStatus
    return_code: Optional[int]
    stdout: str
    stderr: str
    duration_seconds: float
    start_time: str
    end_time: str

    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary for JSON serialization."""
        result = asdict(self)
        result['status'] = self.status.value
        return result


class WindowsProcessManager:
    """Manages Windows processes with monitoring and cleanup."""

    def __init__(self):
        self.running_processes: List[subprocess.Popen] = []

    def find_process_by_name(self, name: str) -> List[Dict[str, Any]]:
        """
        Find processes by name using tasklist.

        Args:
            name: Process name (e.g., "AfterFX.exe")

        Returns:
            List of process information dictionaries
        """
        try:
            result = subprocess.run(
                ["tasklist", "/fi", f"imagename eq {name}", "/fo", "csv", "/nh"],
                capture_output=True,
                text=True,
                timeout=10
            )

            processes = []
            for line in result.stdout.strip().split('\n'):
                if line and not line.startswith('INFO:'):
                    parts = line.strip('"').split('","')
                    if len(parts) >= 2:
                        processes.append({
                            'name': parts[0],
                            'pid': int(parts[1]) if parts[1].isdigit() else 0,
                            'session': parts[2] if len(parts) > 2 else '',
                            'session_num': parts[3] if len(parts) > 3 else '',
                            'memory': parts[4] if len(parts) > 4 else ''
                        })
            return processes
        except Exception as e:
            print(f"Error finding process: {e}")
            return []

    def is_process_running(self, name: str) -> bool:
        """Check if a process with given name is running."""
        processes = self.find_process_by_name(name)
        return len(processes) > 0

    def kill_process_by_name(self, name: str, force: bool = False) -> bool:
        """
        Kill a process by name.

        Args:
            name: Process name
            force: If True, use /F flag for forceful termination

        Returns:
            True if successful
        """
        try:
            cmd = ["taskkill", "/im", name]
            if force:
                cmd.append("/f")

            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=30
            )
            return result.returncode == 0
        except Exception as e:
            print(f"Error killing process: {e}")
            return False

    def kill_process_by_pid(self, pid: int, force: bool = False) -> bool:
        """Kill a process by PID."""
        try:
            cmd = ["taskkill", "/pid", str(pid)]
            if force:
                cmd.append("/f")

            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=30
            )
            return result.returncode == 0
        except Exception as e:
            print(f"Error killing process by PID: {e}")
            return False

    def run_with_timeout(
        self,
        cmd: List[str],
        timeout_seconds: int = 300,
        poll_interval: float = 1.0,
        on_progress: Optional[callable] = None
    ) -> ProcessResult:
        """
        Run a command with timeout and progress monitoring.

        Args:
            cmd: Command and arguments as list
            timeout_seconds: Maximum execution time
            poll_interval: How often to check process status
            on_progress: Callback function(elapsed_seconds) for progress

        Returns:
            ProcessResult with execution details
        """
        start_time = datetime.now()
        start_time_str = start_time.isoformat()

        # Check if executable exists
        if not Path(cmd[0]).exists():
            return ProcessResult(
                status=ProcessStatus.NOT_FOUND,
                return_code=None,
                stdout="",
                stderr=f"Executable not found: {cmd[0]}",
                duration_seconds=0,
                start_time=start_time_str,
                end_time=start_time_str
            )

        try:
            # Start process
            process = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                creationflags=subprocess.CREATE_NO_WINDOW if sys.platform == 'win32' else 0
            )
            self.running_processes.append(process)

            # Monitor with timeout
            elapsed = 0
            while elapsed < timeout_seconds:
                # Check if process has finished
                return_code = process.poll()
                if return_code is not None:
                    # Process finished
                    stdout, stderr = process.communicate()
                    end_time = datetime.now()

                    self.running_processes.remove(process)

                    return ProcessResult(
                        status=ProcessStatus.SUCCESS if return_code == 0 else ProcessStatus.ERROR,
                        return_code=return_code,
                        stdout=stdout.decode('utf-8', errors='replace') if stdout else "",
                        stderr=stderr.decode('utf-8', errors='replace') if stderr else "",
                        duration_seconds=(end_time - start_time).total_seconds(),
                        start_time=start_time_str,
                        end_time=end_time.isoformat()
                    )

                # Progress callback
                if on_progress:
                    on_progress(elapsed)

                time.sleep(poll_interval)
                elapsed += poll_interval

            # Timeout reached - terminate process
            process.terminate()
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()

            if process in self.running_processes:
                self.running_processes.remove(process)

            end_time = datetime.now()
            return ProcessResult(
                status=ProcessStatus.TIMEOUT,
                return_code=None,
                stdout="",
                stderr=f"Process timed out after {timeout_seconds} seconds",
                duration_seconds=(end_time - start_time).total_seconds(),
                start_time=start_time_str,
                end_time=end_time.isoformat()
            )

        except Exception as e:
            end_time = datetime.now()
            return ProcessResult(
                status=ProcessStatus.ERROR,
                return_code=None,
                stdout="",
                stderr=str(e),
                duration_seconds=(end_time - start_time).total_seconds(),
                start_time=start_time_str,
                end_time=end_time.isoformat()
            )

    def cleanup_all(self):
        """Terminate all tracked running processes."""
        for process in self.running_processes[:]:
            try:
                process.terminate()
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    process.kill()
                    process.wait()
            except Exception:
                pass
            finally:
                if process in self.running_processes:
                    self.running_processes.remove(process)


class AfterEffectsRunner:
    """Specialized runner for After Effects script execution."""

    # Default paths for After Effects installations
    AE_PATHS = [
        r"C:\Program Files\Adobe\Adobe After Effects 2025\Support Files\AfterFX.exe",
        r"C:\Program Files\Adobe\Adobe After Effects 2024\Support Files\AfterFX.exe",
        r"C:\Program Files\Adobe\Adobe After Effects 2023\Support Files\AfterFX.exe",
    ]

    def __init__(self, ae_path: Optional[str] = None):
        """
        Initialize After Effects runner.

        Args:
            ae_path: Path to AfterFX.exe. If None, auto-detect.
        """
        self.process_manager = WindowsProcessManager()
        self.ae_path = ae_path or self._find_ae_executable()

    def _find_ae_executable(self) -> Optional[str]:
        """Find After Effects executable."""
        for path in self.AE_PATHS:
            if Path(path).exists():
                return path
        return None

    def is_ae_installed(self) -> bool:
        """Check if After Effects is installed."""
        return self.ae_path is not None and Path(self.ae_path).exists()

    def is_ae_running(self) -> bool:
        """Check if After Effects is currently running."""
        return self.process_manager.is_process_running("AfterFX.exe")

    def kill_ae(self, force: bool = False) -> bool:
        """Kill all After Effects processes."""
        return self.process_manager.kill_process_by_name("AfterFX.exe", force=force)

    def run_script(
        self,
        script_path: str,
        timeout_seconds: int = 300,
        close_after: bool = True,
        on_progress: Optional[callable] = None
    ) -> ProcessResult:
        """
        Run an ExtendScript (.jsx) file in After Effects.

        Args:
            script_path: Path to .jsx script
            timeout_seconds: Maximum execution time
            close_after: Whether AE should close after script execution
            on_progress: Progress callback

        Returns:
            ProcessResult with execution details
        """
        if not self.is_ae_installed():
            return ProcessResult(
                status=ProcessStatus.NOT_FOUND,
                return_code=None,
                stdout="",
                stderr="After Effects not installed or not found",
                duration_seconds=0,
                start_time=datetime.now().isoformat(),
                end_time=datetime.now().isoformat()
            )

        if not Path(script_path).exists():
            return ProcessResult(
                status=ProcessStatus.NOT_FOUND,
                return_code=None,
                stdout="",
                stderr=f"Script not found: {script_path}",
                duration_seconds=0,
                start_time=datetime.now().isoformat(),
                end_time=datetime.now().isoformat()
            )

        # Build command
        # -r: Run script and continue
        # -s: Run script and exit (not always reliable)
        cmd = [self.ae_path, "-r", script_path]

        return self.process_manager.run_with_timeout(
            cmd,
            timeout_seconds=timeout_seconds,
            on_progress=on_progress
        )

    def ensure_closed(self, timeout_seconds: int = 30) -> bool:
        """
        Ensure After Effects is closed, waiting if necessary.

        Args:
            timeout_seconds: How long to wait before force-killing

        Returns:
            True if AE is not running
        """
        if not self.is_ae_running():
            return True

        # Try graceful termination first
        self.kill_ae(force=False)

        # Wait for process to end
        elapsed = 0
        while elapsed < timeout_seconds:
            if not self.is_ae_running():
                return True
            time.sleep(1)
            elapsed += 1

        # Force kill if still running
        self.kill_ae(force=True)
        time.sleep(2)

        return not self.is_ae_running()


@dataclass
class TestRunResult:
    """Result of a complete test run."""
    overall_status: str
    total_tests: int
    passed: int
    failed: int
    skipped: int
    duration_seconds: float
    start_time: str
    end_time: str
    ae_process_result: Optional[Dict[str, Any]]
    log_file: Optional[str]
    json_file: Optional[str]
    errors: List[str]

    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return asdict(self)


class PyAETestRunner:
    """
    Comprehensive test runner for PyAE tests.

    Handles:
    - Process management
    - Timeout enforcement
    - Result collection
    - Cleanup
    """

    def __init__(
        self,
        ae_path: Optional[str] = None,
        plugin_dir: Optional[str] = None,
        results_dir: Optional[str] = None
    ):
        """
        Initialize PyAE test runner.

        Args:
            ae_path: Path to AfterFX.exe
            plugin_dir: Path to PyAE plugin directory
            results_dir: Directory for test results
        """
        self.ae_runner = AfterEffectsRunner(ae_path)

        self.plugin_dir = Path(plugin_dir or
            r"C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\PyAE")

        self.results_dir = Path(results_dir or
            Path.home() / "Documents" / "PyAE_TestResults")
        self.results_dir.mkdir(parents=True, exist_ok=True)

    def deploy_tests(self, source_dir: Path) -> bool:
        """
        Deploy test files to plugin directory.

        Args:
            source_dir: Source directory containing tests

        Returns:
            True if successful
        """
        import shutil

        try:
            # Deploy scripts
            scripts_src = source_dir / "scripts"
            scripts_dst = self.plugin_dir / "scripts"

            if scripts_src.exists():
                scripts_dst.mkdir(parents=True, exist_ok=True)
                for f in scripts_src.glob("*.jsx"):
                    shutil.copy2(f, scripts_dst)
                for f in scripts_src.glob("*.py"):
                    shutil.copy2(f, scripts_dst)

            # Deploy tests
            tests_src = source_dir / "tests"
            tests_dst = self.plugin_dir / "tests"

            if tests_src.exists():
                if tests_dst.exists():
                    shutil.rmtree(tests_dst)
                shutil.copytree(tests_src, tests_dst)

            return True
        except Exception as e:
            print(f"Error deploying tests: {e}")
            return False

    def run_tests(
        self,
        script_name: str = "run_tests.jsx",
        timeout_seconds: int = 600,
        cleanup_ae: bool = True
    ) -> TestRunResult:
        """
        Run the full test suite.

        Args:
            script_name: Name of the test runner script
            timeout_seconds: Maximum time for entire test run
            cleanup_ae: Whether to ensure AE is closed after tests

        Returns:
            TestRunResult with complete execution details
        """
        start_time = datetime.now()
        errors = []

        # Generate timestamped result files
        timestamp = start_time.strftime("%Y%m%d_%H%M%S")
        log_file = self.results_dir / f"test_results_{timestamp}.txt"
        json_file = self.results_dir / f"test_results_{timestamp}.json"

        # Check prerequisites
        if not self.ae_runner.is_ae_installed():
            errors.append("After Effects is not installed")
            return self._create_error_result(start_time, errors)

        if not (self.plugin_dir / "PyAEBridge.dll").exists():
            errors.append("PyAEBridge.dll not found - please build the plugin first")
            return self._create_error_result(start_time, errors)

        script_path = self.plugin_dir / "scripts" / script_name
        if not script_path.exists():
            errors.append(f"Test script not found: {script_path}")
            return self._create_error_result(start_time, errors)

        # Progress callback
        def on_progress(elapsed):
            print(f"\r  Running... {elapsed:.0f}s elapsed", end="", flush=True)

        # Run tests
        print(f"Starting test run at {start_time.isoformat()}")
        print(f"Script: {script_path}")
        print(f"Timeout: {timeout_seconds}s")

        result = self.ae_runner.run_script(
            str(script_path),
            timeout_seconds=timeout_seconds,
            on_progress=on_progress
        )
        print()  # New line after progress

        # Cleanup
        if cleanup_ae:
            print("Ensuring After Effects is closed...")
            self.ae_runner.ensure_closed(timeout_seconds=30)

        # Collect results
        end_time = datetime.now()

        # Try to find and parse result files from plugin directory
        plugin_results_json = self.plugin_dir / "scripts" / "test_results.json"
        plugin_results_txt = self.plugin_dir / "scripts" / "test_results.txt"

        total_tests = 0
        passed = 0
        failed = 0
        skipped = 0

        if plugin_results_json.exists():
            try:
                with open(plugin_results_json, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                    summary = data.get('summary', {})
                    total_tests = summary.get('total', 0)
                    passed = summary.get('passed', 0)
                    failed = summary.get('failed', 0)
                    skipped = summary.get('skipped', 0)

                # Copy to results directory
                import shutil
                shutil.copy2(plugin_results_json, json_file)
            except Exception as e:
                errors.append(f"Error reading results: {e}")

        if plugin_results_txt.exists():
            try:
                import shutil
                shutil.copy2(plugin_results_txt, log_file)
            except Exception as e:
                errors.append(f"Error copying log: {e}")

        # Determine overall status
        if result.status == ProcessStatus.TIMEOUT:
            overall_status = "TIMEOUT"
            errors.append(f"Test run timed out after {timeout_seconds}s")
        elif result.status == ProcessStatus.SUCCESS and failed == 0:
            overall_status = "PASSED"
        elif failed > 0:
            overall_status = "FAILED"
        else:
            overall_status = "ERROR"
            if result.stderr:
                errors.append(result.stderr)

        return TestRunResult(
            overall_status=overall_status,
            total_tests=total_tests,
            passed=passed,
            failed=failed,
            skipped=skipped,
            duration_seconds=(end_time - start_time).total_seconds(),
            start_time=start_time.isoformat(),
            end_time=end_time.isoformat(),
            ae_process_result=result.to_dict(),
            log_file=str(log_file) if log_file.exists() else None,
            json_file=str(json_file) if json_file.exists() else None,
            errors=errors
        )

    def _create_error_result(
        self,
        start_time: datetime,
        errors: List[str]
    ) -> TestRunResult:
        """Create an error result when tests cannot be run."""
        return TestRunResult(
            overall_status="ERROR",
            total_tests=0,
            passed=0,
            failed=0,
            skipped=0,
            duration_seconds=0,
            start_time=start_time.isoformat(),
            end_time=datetime.now().isoformat(),
            ae_process_result=None,
            log_file=None,
            json_file=None,
            errors=errors
        )


def main():
    """Command-line interface for the test runner."""
    import argparse

    parser = argparse.ArgumentParser(description='PyAE Test Runner')
    parser.add_argument('--deploy', action='store_true', help='Deploy tests only')
    parser.add_argument('--run', action='store_true', help='Run tests')
    parser.add_argument('--timeout', type=int, default=600, help='Timeout in seconds')
    parser.add_argument('--script', default='run_tests.jsx', help='Test script to run')
    parser.add_argument('--kill-ae', action='store_true', help='Kill After Effects')
    parser.add_argument('--status', action='store_true', help='Check AE status')

    args = parser.parse_args()

    runner = PyAETestRunner()

    if args.kill_ae:
        print("Killing After Effects processes...")
        success = runner.ae_runner.ensure_closed(timeout_seconds=30)
        print("Done" if success else "Failed to kill AE")
        return

    if args.status:
        print(f"After Effects installed: {runner.ae_runner.is_ae_installed()}")
        print(f"After Effects running: {runner.ae_runner.is_ae_running()}")
        print(f"Plugin directory: {runner.plugin_dir}")
        print(f"Results directory: {runner.results_dir}")
        return

    if args.deploy:
        source = Path(__file__).parent.parent
        print(f"Deploying from {source}...")
        success = runner.deploy_tests(source)
        print("Deployed successfully" if success else "Deployment failed")
        return

    if args.run:
        result = runner.run_tests(
            script_name=args.script,
            timeout_seconds=args.timeout
        )

        print("\n" + "=" * 50)
        print("TEST RUN COMPLETE")
        print("=" * 50)
        print(f"Status: {result.overall_status}")
        print(f"Total: {result.total_tests}")
        print(f"Passed: {result.passed}")
        print(f"Failed: {result.failed}")
        print(f"Duration: {result.duration_seconds:.1f}s")

        if result.errors:
            print("\nErrors:")
            for err in result.errors:
                print(f"  - {err}")

        if result.log_file:
            print(f"\nLog: {result.log_file}")
        if result.json_file:
            print(f"JSON: {result.json_file}")

        return 0 if result.overall_status == "PASSED" else 1

    # Default: show help
    parser.print_help()


if __name__ == "__main__":
    sys.exit(main() or 0)
