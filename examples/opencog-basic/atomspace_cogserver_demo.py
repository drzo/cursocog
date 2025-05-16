#!/usr/bin/env python3
"""
OpenCog Basic AtomSpace and CogServer Demo
This script demonstrates how AtomSpace integrates with CogServer for network-accessible 
reasoning and storage capabilities.
"""

import os
import sys
import time
import socket
import threading
import random

def mock_cogserver():
    """Mock implementation of a CogServer to demonstrate the concept"""
    print("\nStarting mock CogServer on port 17001...")
    
    # Create a simple TCP server to simulate the CogServer
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
        server_socket.bind(('localhost', 17001))
        server_socket.listen(5)
        print("CogServer listening on localhost:17001")
        
        # Keep track of connected clients
        clients = []
        
        # Handle client connections
        def handle_client(client_socket, addr):
            print(f"Client connected from {addr}")
            client_socket.send(b"OpenCog CogServer v1.0 (mock)\n> ")
            
            # Basic command parser
            while True:
                try:
                    data = client_socket.recv(1024).decode('utf-8').strip()
                    if not data:
                        break
                    
                    # Process commands
                    if data == "help":
                        response = """Available commands:
  help                - Show this help
  shutdown            - Shutdown the server
  atoms               - List atoms in the AtomSpace
  add <type> <name>   - Add a new atom
  load <filename>     - Load atoms from file
  ls                  - List atoms (shorthand for 'atoms')
  exit                - Close connection
"""
                    elif data == "shutdown":
                        response = "Shutting down server..."
                        client_socket.send((response + "\n> ").encode('utf-8'))
                        # Signal shutdown
                        print("Shutdown command received")
                        os._exit(0)  # This is just for our mock server
                        
                    elif data in ["atoms", "ls"]:
                        response = """AtomSpace contents:
(ConceptNode "human" (stv 0.900000 0.800000))
(ConceptNode "woman" (stv 0.900000 0.900000))
(ConceptNode "man" (stv 0.900000 0.900000))
(ConceptNode "mammal" (stv 0.900000 0.900000))
(ConceptNode "animal" (stv 0.900000 0.900000))
(ConceptNode "Alice" (stv 1.000000 1.000000))
(ConceptNode "Bob" (stv 1.000000 1.000000))
(InheritanceLink (stv 1.000000 1.000000)
  (ConceptNode "woman")
  (ConceptNode "human")
)
(InheritanceLink (stv 1.000000 1.000000)
  (ConceptNode "man")
  (ConceptNode "human")
)
(InheritanceLink (stv 1.000000 1.000000)
  (ConceptNode "human")
  (ConceptNode "mammal")
)
(InheritanceLink (stv 1.000000 1.000000)
  (ConceptNode "mammal")
  (ConceptNode "animal")
)"""
                    elif data.startswith("add "):
                        parts = data.split(" ", 2)
                        if len(parts) == 3:
                            atom_type, atom_name = parts[1], parts[2]
                            response = f"Added ({atom_type} \"{atom_name}\")"
                        else:
                            response = "Error: Invalid syntax. Use 'add <type> <name>'"
                            
                    elif data.startswith("load "):
                        filename = data.split(" ", 1)[1]
                        response = f"Loaded atoms from {filename}"
                        
                    elif data == "exit":
                        response = "Goodbye!"
                        client_socket.send((response + "\n").encode('utf-8'))
                        break
                        
                    else:
                        response = f"Unknown command: {data}"
                    
                    # Send response back to client
                    client_socket.send((response + "\n> ").encode('utf-8'))
                    
                except Exception as e:
                    print(f"Error handling client: {e}")
                    break
            
            # Clean up
            print(f"Client disconnected from {addr}")
            client_socket.close()
            if client_socket in clients:
                clients.remove(client_socket)
        
        # Accept connections in a loop
        while True:
            client_socket, addr = server_socket.accept()
            clients.append(client_socket)
            
            # Handle each client in a separate thread
            client_thread = threading.Thread(target=handle_client, args=(client_socket, addr))
            client_thread.daemon = True
            client_thread.start()
            
    except Exception as e:
        print(f"Server error: {e}")
    finally:
        server_socket.close()

def telnet_client():
    """Simple telnet-like client to connect to the CogServer"""
    print("\nConnecting to CogServer on localhost:17001...")
    time.sleep(2)  # Give the server time to start
    
    # Create a socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        # Connect to the server
        client_socket.connect(('localhost', 17001))
        
        # Receive welcome message
        response = client_socket.recv(4096).decode('utf-8')
        print(response, end='')
        
        # Send commands
        commands = [
            "help",
            "atoms",
            "add ConceptNode cat",
            "add InheritanceLink (ConceptNode cat) (ConceptNode animal)",
            "ls",
            "exit"
        ]
        
        for cmd in commands:
            print(f"\nSending: {cmd}")
            client_socket.send((cmd + "\n").encode('utf-8'))
            
            # Wait for response
            time.sleep(0.5)
            response = client_socket.recv(4096).decode('utf-8')
            print(response, end='')
            
    except Exception as e:
        print(f"Client error: {e}")
    finally:
        client_socket.close()

def main():
    """Main function to run the demo"""
    print("OpenCog Basic AtomSpace and CogServer Demo")
    print("==========================================")
    
    # Start the mock CogServer in a separate thread
    server_thread = threading.Thread(target=mock_cogserver)
    server_thread.daemon = True
    server_thread.start()
    
    # Run the client
    telnet_client()
    
    # Keep the script running to maintain the server
    print("\nPress Ctrl+C to exit...")
    try:
        while server_thread.is_alive():
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nExiting AtomSpace CogServer demo")

if __name__ == "__main__":
    main() 