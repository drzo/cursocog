#!/usr/bin/env python3
"""
OpenCog Integration Test Framework

This script provides a framework for running integration tests across multiple OpenCog components.
It discovers, runs, and reports on tests that span component boundaries.

Usage:
  integration-test.py [options]

Options:
  -h --help               Show this help message and exit
  --test-dir=DIR          Directory containing test files [default: tests/integration]
  --test=TEST             Run specific test case(s)
  --components=LIST       Comma-separated list of components to test
  --output=FORMAT         Output format (text, xml, json) [default: text]
  --log=FILE              Log to file instead of stdout
  --timeout=SECONDS       Timeout for each test [default: 300]
  --parallel=COUNT        Number of tests to run in parallel [default: 1]
  --no-teardown           Do not run teardown even on test failure
"""

import os
import sys
import json
import time
import logging
import argparse
import unittest
import importlib
import subprocess
import multiprocessing
import traceback
import xml.etree.ElementTree as ET
from pathlib import Path
from datetime import datetime
from concurrent.futures import ProcessPoolExecutor

# Set up logging
logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO)
logger = logging.getLogger('integration-test')

# OpenCog components to include in tests by default
DEFAULT_COMPONENTS = [
    "cogutil",
    "atomspace",
    "atomspace-storage",
    "atomspace-rocks",
    "atomspace-pgres",
    "cogserver",
    "learn",
    "opencog",
    "sensory",
    "evidence"
]

class TestResult:
    """Stores the result of a test run"""
    def __init__(self, name, status, duration, message=None, traceback=None):
        self.name = name
        self.status = status  # 'success', 'failure', 'error', 'skipped'
        self.duration = duration  # in seconds
        self.message = message
        self.traceback = traceback
        self.timestamp = datetime.now().isoformat()

class IntegrationTest:
    """Base class for integration tests"""
    def __init__(self, name, components, description=None):
        self.name = name
        self.components = components if isinstance(components, list) else [components]
        self.description = description or f"Integration test for {', '.join(components)}"
        self.logger = logging.getLogger(f"test.{name}")
    
    def setup(self):
        """Set up the test environment. Override in subclasses."""
        self.logger.info(f"Setting up test {self.name}")
        return True
    
    def run(self):
        """Run the test. Override in subclasses."""
        self.logger.info(f"Running test {self.name}")
        return True
    
    def teardown(self):
        """Clean up after the test. Override in subclasses."""
        self.logger.info(f"Tearing down test {self.name}")
        return True
    
    def execute(self):
        """Execute the full test (setup, run, teardown)"""
        start_time = time.time()
        success = False
        error_message = None
        error_traceback = None
        status = "success"
        
        try:
            if not self.setup():
                status = "failure"
                error_message = "Setup failed"
                return TestResult(self.name, status, time.time() - start_time, error_message, None)
            
            success = self.run()
            
            if not success:
                status = "failure"
                error_message = "Test assertions failed"
            
        except Exception as e:
            status = "error"
            error_message = str(e)
            error_traceback = traceback.format_exc()
            self.logger.error(f"Error in test {self.name}: {e}")
            self.logger.error(error_traceback)
        
        try:
            self.teardown()
        except Exception as e:
            self.logger.error(f"Error in teardown for test {self.name}: {e}")
            if status == "success":
                status = "error"
                error_message = f"Teardown failed: {str(e)}"
                error_traceback = traceback.format_exc()
        
        duration = time.time() - start_time
        return TestResult(self.name, status, duration, error_message, error_traceback)

class AtomSpaceIntegrationTest(IntegrationTest):
    """Base class for AtomSpace integration tests"""
    def __init__(self, name, components=None, description=None):
        components = components or ["atomspace"]
        if "atomspace" not in components:
            components.append("atomspace")
        super().__init__(name, components, description)
        self.atomspace = None
    
    def setup(self):
        """Set up AtomSpace for testing"""
        try:
            # Import AtomSpace and create a new instance
            from opencog.atomspace import AtomSpace
            self.atomspace = AtomSpace()
            return True
        except ImportError:
            self.logger.error("Failed to import AtomSpace. Is it installed?")
            return False
        except Exception as e:
            self.logger.error(f"Error setting up AtomSpace: {e}")
            return False
    
    def teardown(self):
        """Clean up AtomSpace"""
        # AtomSpace will be garbage collected
        self.atomspace = None
        return True

class AtomSpaceStorageTest(AtomSpaceIntegrationTest):
    """Test integration between AtomSpace and storage backends"""
    def __init__(self, name="atomspace_storage_test", storage_type="rocks", components=None):
        self.storage_type = storage_type
        components = components or ["atomspace", f"atomspace-{storage_type}"]
        super().__init__(name, components)
        self.storage = None
    
    def setup(self):
        """Set up AtomSpace and storage"""
        if not super().setup():
            return False
        
        try:
            if self.storage_type == "rocks":
                # Import RocksStorageNode and set up storage
                from opencog.atomspace import RocksStorageNode
                self.storage = RocksStorageNode("rocks:///tmp/opencog_test_rocks")
                self.storage.open()
                self.atomspace.add_node(self.storage)
            elif self.storage_type == "pgres":
                # Import PostgresStorageNode and set up storage
                from opencog.atomspace import PostgresStorageNode
                self.storage = PostgresStorageNode("postgres://opencog_test:password@localhost/opencog_test")
                self.storage.open()
                self.atomspace.add_node(self.storage)
            else:
                self.logger.error(f"Unsupported storage type: {self.storage_type}")
                return False
            
            return True
        except ImportError:
            self.logger.error(f"Failed to import storage backend for {self.storage_type}. Is it installed?")
            return False
        except Exception as e:
            self.logger.error(f"Error setting up storage: {e}")
            return False
    
    def run(self):
        """Run storage tests"""
        try:
            # Create some test atoms
            from opencog.atomspace import ConceptNode, Link, types
            
            # Create test atoms
            a = ConceptNode("test_a", atomspace=self.atomspace)
            b = ConceptNode("test_b", atomspace=self.atomspace)
            l = Link(types.Link, [a, b], atomspace=self.atomspace)
            
            # Store atoms
            self.storage.store_atom(a)
            self.storage.store_atom(b)
            self.storage.store_atom(l)
            
            # Clear atomspace
            self.atomspace.clear()
            
            # Load atoms
            a2 = ConceptNode("test_a", atomspace=self.atomspace)
            self.storage.load_atom(a2)
            
            # Verify that the link was loaded (with implicit loading of b)
            links = list(self.atomspace.get_incoming(a2))
            if not links:
                self.logger.error("Failed to load link")
                return False
            
            # Check that the loaded link has the correct structure
            loaded_link = links[0]
            outgoing = loaded_link.get_out()
            if len(outgoing) != 2:
                self.logger.error(f"Link has wrong number of outgoing atoms: {len(outgoing)}")
                return False
            
            if outgoing[0].name != "test_a" or outgoing[1].name != "test_b":
                self.logger.error("Link has wrong outgoing atoms")
                return False
            
            self.logger.info("AtomSpace storage test passed")
            return True
        except Exception as e:
            self.logger.error(f"Error in storage test: {e}")
            return False
    
    def teardown(self):
        """Clean up storage"""
        try:
            if self.storage:
                self.storage.close()
                
                # Remove temporary database files if using RocksDB
                if self.storage_type == "rocks":
                    import shutil
                    try:
                        shutil.rmtree("/tmp/opencog_test_rocks")
                    except:
                        pass
                
                # Drop test database if using Postgres
                if self.storage_type == "pgres":
                    try:
                        import psycopg2
                        conn = psycopg2.connect("dbname='postgres' user='opencog_test' password='password' host='localhost'")
                        conn.autocommit = True
                        cur = conn.cursor()
                        cur.execute("DROP DATABASE IF EXISTS opencog_test")
                        cur.close()
                        conn.close()
                    except:
                        pass
            
            return super().teardown()
        except Exception as e:
            self.logger.error(f"Error in teardown: {e}")
            return False

class CogServerIntegrationTest(IntegrationTest):
    """Test integration with CogServer"""
    def __init__(self, name="cogserver_integration_test", components=None):
        components = components or ["cogserver", "atomspace"]
        super().__init__(name, components)
        self.server_process = None
        self.server_output = None
    
    def setup(self):
        """Start CogServer for testing"""
        try:
            # Start CogServer in a subprocess
            self.server_output = open("/tmp/cogserver_test.log", "w")
            self.server_process = subprocess.Popen(
                ["cogserver", "-c", "/tmp/cogserver_test.conf"],
                stdout=self.server_output,
                stderr=self.server_output
            )
            
            # Wait for server to start
            time.sleep(5)
            
            # Check if server is running
            if self.server_process.poll() is not None:
                self.logger.error("CogServer failed to start")
                return False
            
            return True
        except Exception as e:
            self.logger.error(f"Error setting up CogServer: {e}")
            return False
    
    def run(self):
        """Run CogServer tests"""
        try:
            # Test connection to CogServer
            import socket
            
            # Connect to CogServer
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(5)
            s.connect(("localhost", 17001))
            
            # Send a command and check response
            s.sendall(b"help\n")
            response = s.recv(4096).decode('utf-8')
            s.close()
            
            if "Available commands" not in response:
                self.logger.error("Unexpected response from CogServer")
                return False
            
            self.logger.info("CogServer connection test passed")
            return True
        except Exception as e:
            self.logger.error(f"Error in CogServer test: {e}")
            return False
    
    def teardown(self):
        """Stop CogServer"""
        try:
            # Stop the server
            if self.server_process:
                self.server_process.terminate()
                self.server_process.wait(timeout=10)
                if self.server_process.poll() is None:
                    self.server_process.kill()
            
            # Close output file
            if self.server_output:
                self.server_output.close()
            
            # Clean up temporary files
            for file in ["/tmp/cogserver_test.log", "/tmp/cogserver_test.conf"]:
                try:
                    os.remove(file)
                except:
                    pass
            
            return True
        except Exception as e:
            self.logger.error(f"Error in teardown: {e}")
            return False

def discover_tests(test_dir, components=None):
    """Discover integration tests in the given directory"""
    tests = []
    
    if not os.path.exists(test_dir):
        logger.error(f"Test directory not found: {test_dir}")
        return tests
    
    # Filter components if specified
    if components:
        component_filter = components.split(',')
    else:
        component_filter = None
    
    # Add built-in tests
    tests.append(AtomSpaceStorageTest("atomspace_rocks_test", "rocks"))
    if not component_filter or "atomspace-pgres" in component_filter:
        tests.append(AtomSpaceStorageTest("atomspace_pgres_test", "pgres"))
    if not component_filter or "cogserver" in component_filter:
        tests.append(CogServerIntegrationTest())
    
    # Discover test files
    for root, _, files in os.walk(test_dir):
        for file in files:
            if file.startswith("test_") and file.endswith(".py"):
                try:
                    # Import the test module
                    module_path = os.path.join(root, file)
                    module_name = os.path.splitext(file)[0]
                    spec = importlib.util.spec_from_file_location(module_name, module_path)
                    module = importlib.util.module_from_spec(spec)
                    spec.loader.exec_module(module)
                    
                    # Find test classes
                    for name in dir(module):
                        obj = getattr(module, name)
                        if (isinstance(obj, type) and 
                            issubclass(obj, IntegrationTest) and 
                            obj != IntegrationTest and
                            obj != AtomSpaceIntegrationTest):
                            
                            # Create an instance and check if it should be included
                            instance = obj()
                            if component_filter:
                                # Skip if none of the test's components are in the filter
                                if not any(c in component_filter for c in instance.components):
                                    continue
                            
                            tests.append(instance)
                except Exception as e:
                    logger.error(f"Error loading tests from {file}: {e}")
    
    return tests

def format_duration(seconds):
    """Format a duration in seconds to a human-readable string"""
    if seconds < 60:
        return f"{seconds:.2f}s"
    elif seconds < 3600:
        minutes = int(seconds / 60)
        seconds = seconds % 60
        return f"{minutes}m {seconds:.2f}s"
    else:
        hours = int(seconds / 3600)
        minutes = int((seconds % 3600) / 60)
        seconds = seconds % 60
        return f"{hours}h {minutes}m {seconds:.2f}s"

def print_results(results, format="text"):
    """Print test results in the specified format"""
    if format == "json":
        # JSON output
        json_results = []
        for result in results:
            json_results.append({
                "name": result.name,
                "status": result.status,
                "duration": result.duration,
                "message": result.message,
                "traceback": result.traceback,
                "timestamp": result.timestamp
            })
        print(json.dumps(json_results, indent=2))
    elif format == "xml":
        # JUnit XML output
        root = ET.Element("testsuites")
        suite = ET.SubElement(root, "testsuite", name="integration-tests")
        
        for result in results:
            case = ET.SubElement(suite, "testcase", 
                                name=result.name,
                                time=str(result.duration))
            
            if result.status == "failure":
                failure = ET.SubElement(case, "failure", message=result.message or "Test failed")
                if result.traceback:
                    failure.text = result.traceback
            elif result.status == "error":
                error = ET.SubElement(case, "error", message=result.message or "Test error")
                if result.traceback:
                    error.text = result.traceback
            elif result.status == "skipped":
                ET.SubElement(case, "skipped", message=result.message or "Test skipped")
        
        xml_str = ET.tostring(root, encoding="unicode")
        print(xml_str)
    else:
        # Text output (default)
        successful = [r for r in results if r.status == "success"]
        failed = [r for r in results if r.status == "failure"]
        errors = [r for r in results if r.status == "error"]
        skipped = [r for r in results if r.status == "skipped"]
        
        total_duration = sum(r.duration for r in results)
        
        print("\n=== OpenCog Integration Test Results ===")
        print(f"Ran {len(results)} tests in {format_duration(total_duration)}")
        print(f"  Successful: {len(successful)}")
        print(f"  Failed: {len(failed)}")
        print(f"  Errors: {len(errors)}")
        print(f"  Skipped: {len(skipped)}")
        
        if failed:
            print("\nFailed tests:")
            for result in failed:
                print(f"  - {result.name} ({format_duration(result.duration)})")
                if result.message:
                    print(f"    {result.message}")
        
        if errors:
            print("\nErrors:")
            for result in errors:
                print(f"  - {result.name} ({format_duration(result.duration)})")
                if result.message:
                    print(f"    {result.message}")
                if result.traceback:
                    print(f"    {result.traceback.splitlines()[0]}")
        
        if successful and len(successful) == len(results):
            print("\nAll tests passed successfully!")

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(description="OpenCog Integration Test Framework")
    parser.add_argument("--test-dir", help="Directory containing test files", default="tests/integration")
    parser.add_argument("--test", help="Run specific test case(s)", nargs="+")
    parser.add_argument("--components", help="Comma-separated list of components to test")
    parser.add_argument("--output", help="Output format (text, xml, json)", default="text")
    parser.add_argument("--log", help="Log to file instead of stdout")
    parser.add_argument("--timeout", help="Timeout for each test in seconds", type=int, default=300)
    parser.add_argument("--parallel", help="Number of tests to run in parallel", type=int, default=1)
    parser.add_argument("--no-teardown", help="Do not run teardown even on test failure", action="store_true")
    
    args = parser.parse_args()
    
    # Configure logging
    if args.log:
        handler = logging.FileHandler(args.log)
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        logger.addHandler(handler)
        logger.setLevel(logging.INFO)
    
    # Discover tests
    tests = discover_tests(args.test_dir, args.components)
    
    # Filter tests if specified
    if args.test:
        tests = [t for t in tests if t.name in args.test]
    
    if not tests:
        logger.error("No tests found")
        return 1
    
    logger.info(f"Found {len(tests)} tests to run")
    
    # Run tests
    results = []
    
    if args.parallel > 1:
        # Run tests in parallel
        def run_test(test):
            result = test.execute()
            logger.info(f"Test {test.name}: {result.status} ({format_duration(result.duration)})")
            return result
        
        with ProcessPoolExecutor(max_workers=args.parallel) as executor:
            futures = [executor.submit(run_test, test) for test in tests]
            for future in futures:
                try:
                    result = future.result(timeout=args.timeout)
                    results.append(result)
                except Exception as e:
                    logger.error(f"Error running test: {e}")
    else:
        # Run tests sequentially
        for test in tests:
            try:
                result = test.execute()
                logger.info(f"Test {test.name}: {result.status} ({format_duration(result.duration)})")
                results.append(result)
            except Exception as e:
                logger.error(f"Error running test: {e}")
    
    # Print results
    print_results(results, args.output)
    
    # Return non-zero exit code if any tests failed
    return 0 if all(r.status == "success" for r in results) else 1

if __name__ == "__main__":
    sys.exit(main()) 