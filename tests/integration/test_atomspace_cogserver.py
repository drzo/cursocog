#!/usr/bin/env python3
"""
Integration tests between AtomSpace and CogServer
"""

import os
import sys
import time
import socket
import subprocess
import tempfile
from pathlib import Path

# Add the scripts directory to the path
script_dir = Path(__file__).resolve().parent.parent.parent / "scripts"
sys.path.append(str(script_dir))

# Import the integration test framework
from integration_test import IntegrationTest, AtomSpaceIntegrationTest

class AtomSpaceCogServerTest(AtomSpaceIntegrationTest):
    """Test integration between AtomSpace and CogServer"""
    
    def __init__(self):
        super().__init__("atomspace_cogserver_test", ["atomspace", "cogserver"])
        self.server_process = None
        self.server_output = None
        self.config_file = None
    
    def setup(self):
        """Set up AtomSpace and CogServer"""
        # First set up AtomSpace
        if not super().setup():
            return False
        
        try:
            # Create a temporary configuration file for CogServer
            with tempfile.NamedTemporaryFile(delete=False, suffix=".conf", mode='w') as f:
                self.config_file = f.name
                f.write("SERVER_PORT = 16001\n")
                f.write("LOG_FILE = /tmp/cogserver_atomspace_test.log\n")
                f.write("LOG_LEVEL = debug\n")
                f.write("ANSI_ENABLED = true\n")
            
            # Start CogServer in a subprocess
            self.server_output = open("/tmp/cogserver_atomspace_test.log", "w")
            self.server_process = subprocess.Popen(
                ["cogserver", "-c", self.config_file],
                stdout=self.server_output,
                stderr=self.server_output
            )
            
            # Wait for server to start
            time.sleep(3)
            
            # Check if server is running
            if self.server_process.poll() is not None:
                self.logger.error("CogServer failed to start")
                return False
                
            # Create client to interact with CogServer
            self.cogserver_client = CogServerClient("localhost", 16001)
            
            return True
        except Exception as e:
            self.logger.error(f"Error setting up CogServer: {e}")
            return False
    
    def run(self):
        """Run integration tests"""
        try:
            # Test 1: Create atom in AtomSpace and verify it exists
            from opencog.atomspace import ConceptNode, Link, types
            
            self.logger.info("Creating test atoms in AtomSpace")
            a = ConceptNode("test_atom_a", atomspace=self.atomspace)
            b = ConceptNode("test_atom_b", atomspace=self.atomspace)
            link = Link(types.Link, [a, b], atomspace=self.atomspace)
            
            # Test 2: Load atoms into CogServer via commands
            self.logger.info("Loading atoms into CogServer")
            
            # Send commands to load atoms
            responses = []
            responses.append(self.cogserver_client.send_command(
                f'py-eval (ConceptNode "test_atom_a" (stv 0.8 0.9))'
            ))
            responses.append(self.cogserver_client.send_command(
                f'py-eval (ConceptNode "test_atom_b" (stv 0.7 0.8))'
            ))
            responses.append(self.cogserver_client.send_command(
                f'py-eval (Link (ConceptNode "test_atom_a") (ConceptNode "test_atom_b"))'
            ))
            
            # Check for errors in responses
            for i, response in enumerate(responses):
                if "ERROR" in response:
                    self.logger.error(f"Error in command {i+1}: {response}")
                    return False
            
            # Test 3: Query atoms in CogServer
            self.logger.info("Querying atoms in CogServer")
            response = self.cogserver_client.send_command('py-eval (cog-get-atoms \'ConceptNode)')
            
            # Verify test atoms exist in the response
            if "test_atom_a" not in response or "test_atom_b" not in response:
                self.logger.error("Test atoms not found in CogServer")
                return False
            
            # Test 4: Modify atom in CogServer and verify change
            self.logger.info("Modifying atom in CogServer")
            modify_response = self.cogserver_client.send_command(
                'py-eval (cog-set-tv! (ConceptNode "test_atom_a") (stv 0.5 0.6))'
            )
            
            # Verify the TV was changed
            tv_response = self.cogserver_client.send_command(
                'py-eval (cog-tv (ConceptNode "test_atom_a"))'
            )
            
            if "0.5" not in tv_response or "0.6" not in tv_response:
                self.logger.error("TruthValue modification failed")
                self.logger.error(f"Response: {tv_response}")
                return False
            
            self.logger.info("All AtomSpace-CogServer integration tests passed!")
            return True
            
        except Exception as e:
            self.logger.error(f"Error in test: {e}")
            return False
    
    def teardown(self):
        """Clean up resources"""
        try:
            # Stop the server
            if self.server_process:
                try:
                    self.cogserver_client.send_command('shutdown')
                except:
                    pass
                    
                try:
                    self.server_process.terminate()
                    self.server_process.wait(timeout=5)
                    if self.server_process.poll() is None:
                        self.server_process.kill()
                except:
                    pass
            
            # Close output file
            if self.server_output:
                self.server_output.close()
            
            # Remove temporary config file
            if self.config_file and os.path.exists(self.config_file):
                os.remove(self.config_file)
                
            # Remove log file
            try:
                os.remove("/tmp/cogserver_atomspace_test.log")
            except:
                pass
            
            # Call parent teardown to clean up AtomSpace
            return super().teardown()
        except Exception as e:
            self.logger.error(f"Error in teardown: {e}")
            return False

class CogServerClient:
    """Simple client for interacting with CogServer"""
    
    def __init__(self, host="localhost", port=17001, timeout=5):
        self.host = host
        self.port = port
        self.timeout = timeout
    
    def send_command(self, command):
        """Send a command to CogServer and return the response"""
        response = ""
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(self.timeout)
            s.connect((self.host, self.port))
            
            # Send command with newline terminator
            if not command.endswith('\n'):
                command += '\n'
            s.sendall(command.encode('utf-8'))
            
            # Read response
            while True:
                data = s.recv(4096)
                if not data:
                    break
                response += data.decode('utf-8')
                
                # Check if response is complete (ends with prompt)
                if response.endswith("opencog> "):
                    response = response[:-10]  # Remove prompt
                    break
            
            s.close()
        except socket.timeout:
            response += " (TIMEOUT)"
        except Exception as e:
            response = f"ERROR: {str(e)}"
        
        return response.strip()

# Create an instance of the test
test = AtomSpaceCogServerTest() 