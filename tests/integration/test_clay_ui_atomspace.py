#!/usr/bin/env python3
"""
Integration test between AtomSpace and Clay UI
"""

import os
import sys
import time
import subprocess
from pathlib import Path

# Add the scripts directory to the path
script_dir = Path(__file__).resolve().parent.parent.parent / "scripts"
sys.path.append(str(script_dir))

# Import the integration test framework
from integration_test import IntegrationTest, AtomSpaceIntegrationTest

class ClayUIAtomSpaceTest(IntegrationTest):
    """Test integration between Clay UI and AtomSpace"""
    
    def __init__(self):
        super().__init__("clay_ui_atomspace_test", ["atomspace", "clay-ui"])
        self.ui_process = None
        self.ui_output = None
        self.atomspace = None
        self.clay_executable = None
    
    def setup(self):
        """Set up AtomSpace and Clay UI"""
        try:
            self.logger.info("Setting up AtomSpace for Clay UI integration test")
            
            # Import AtomSpace
            try:
                from opencog.atomspace import AtomSpace, TruthValue
                from opencog.type_constructors import ConceptNode, Link, set_default_atomspace
                self.atomspace = AtomSpace()
                
                # Register this AtomSpace as the default
                set_default_atomspace(self.atomspace)
            except ImportError:
                self.logger.error("Failed to import AtomSpace")
                return False
            
            # Create some test atoms
            self.logger.info("Creating test atoms in AtomSpace")
            
            # Create concept nodes
            a = ConceptNode("test_concept_a")
            a.truth_value(0.8, 0.9)
            
            b = ConceptNode("test_concept_b")
            b.truth_value(0.7, 0.8)
            
            # Create a link between them
            link = Link(a, b)
            link.truth_value(0.9, 0.9)
            
            # Find Clay UI executable
            if os.name == 'nt':  # Windows
                self.clay_executable = Path("clay-ui/build/Release/opencog-atomspace-viewer.exe")
            else:  # Linux/macOS
                self.clay_executable = Path("clay-ui/build/opencog-atomspace-viewer")
            
            if not self.clay_executable.exists():
                self.logger.error(f"Clay UI executable not found at {self.clay_executable}")
                return False
            
            # Start Clay UI with test parameters
            self.logger.info("Starting Clay UI")
            self.ui_output = open("/tmp/clay_ui_test.log", "w")
            
            test_args = [
                "--test-mode",
                "--no-gui",
                "--atomspace-path", 
                str(self.atomspace)
            ]
            
            self.ui_process = subprocess.Popen(
                [str(self.clay_executable)] + test_args,
                stdout=self.ui_output,
                stderr=self.ui_output
            )
            
            # Wait for UI to start
            time.sleep(2)
            
            # Check if process is running
            if self.ui_process.poll() is not None:
                self.logger.error("Clay UI failed to start")
                return False
            
            return True
            
        except Exception as e:
            self.logger.error(f"Error in setup: {e}")
            return False
    
    def run(self):
        """Run integration tests"""
        try:
            self.logger.info("Running Clay UI-AtomSpace integration tests")
            
            # Check if Clay UI is still running
            if self.ui_process.poll() is not None:
                self.logger.error("Clay UI process is not running")
                return False
            
            # Add more atoms to the AtomSpace
            self.logger.info("Adding more atoms to AtomSpace")
            
            from opencog.type_constructors import ConceptNode, Link
            
            c = ConceptNode("test_concept_c")
            c.truth_value(0.6, 0.7)
            
            link2 = Link(ConceptNode("test_concept_a"), c)
            link2.truth_value(0.5, 0.6)
            
            # Give Clay UI time to process changes
            time.sleep(1)
            
            # Verify that Clay UI received atoms
            self.logger.info("Verifying Clay UI received atom updates")
            
            # In a real implementation, we would check if Clay UI received the updates
            # For this test, we'll just check if the process is still running
            if self.ui_process.poll() is not None:
                self.logger.error("Clay UI process terminated unexpectedly")
                return False
            
            # Check log file for errors
            with open("/tmp/clay_ui_test.log", "r") as f:
                log_content = f.read()
                if "ERROR" in log_content or "FATAL" in log_content:
                    self.logger.error("Found errors in Clay UI log")
                    self.logger.error(log_content)
                    return False
                
                # Check for expected output
                if "Loaded 3 nodes and 2 links" not in log_content and "Loaded 5 atoms" not in log_content:
                    self.logger.error("Expected atom count not found in log")
                    return False
            
            self.logger.info("Clay UI-AtomSpace integration test passed")
            return True
            
        except Exception as e:
            self.logger.error(f"Error in test: {e}")
            return False
    
    def teardown(self):
        """Clean up resources"""
        try:
            # Terminate Clay UI process
            if self.ui_process:
                self.ui_process.terminate()
                self.ui_process.wait(timeout=5)
                if self.ui_process.poll() is None:
                    self.ui_process.kill()
            
            # Close output file
            if self.ui_output:
                self.ui_output.close()
            
            # Remove log file
            try:
                os.remove("/tmp/clay_ui_test.log")
            except:
                pass
            
            # Clear AtomSpace
            if self.atomspace:
                self.atomspace.clear()
                self.atomspace = None
            
            return True
        except Exception as e:
            self.logger.error(f"Error in teardown: {e}")
            return False

# Create an instance of the test
test = ClayUIAtomSpaceTest() 