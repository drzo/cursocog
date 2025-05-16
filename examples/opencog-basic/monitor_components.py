#!/usr/bin/env python3
"""
OpenCog Component Health Monitor
This script demonstrates the use of the Clay UI component health monitor to check the status of OpenCog components
"""

import sys
import time
import subprocess
import os
import json
from datetime import datetime

def check_opencog_installation():
    """Check if OpenCog components are installed"""
    opencog_components = [
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
    
    installed_components = []
    missing_components = []
    
    print(f"Checking OpenCog components status at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("="*70)
    
    for component in opencog_components:
        # Check for installed libraries
        lib_found = False
        lib_paths = [
            f"/usr/local/lib/lib{component}.so",
            f"/usr/local/lib/lib{component}.dylib",
            f"C:\\Program Files\\OpenCog\\lib\\{component}.dll",
            os.path.join(os.getcwd(), "..", component, "build", "lib", f"lib{component}.so")
        ]
        
        for path in lib_paths:
            if os.path.exists(path):
                lib_found = True
                break
        
        # Check for installed include headers
        include_found = False
        include_paths = [
            f"/usr/local/include/{component}",
            f"C:\\Program Files\\OpenCog\\include\\{component}",
            os.path.join(os.getcwd(), "..", component, "include")
        ]
        
        for path in include_paths:
            if os.path.exists(path):
                include_found = True
                break
        
        # Determine if component is installed
        if lib_found or include_found:
            status = "Installed"
            if lib_found and include_found:
                status_details = "Both library and headers found"
            elif lib_found:
                status_details = "Library found, headers missing"
            else:
                status_details = "Headers found, library missing"
            
            installed_components.append((component, status, status_details))
        else:
            status = "Not Found"
            status_details = "Neither library nor headers found"
            missing_components.append((component, status, status_details))
    
    # Display results in table format
    print(f"{'Component':<20} {'Status':<15} {'Details':<35}")
    print("-"*70)
    
    for component, status, details in installed_components:
        print(f"{component:<20} {status:<15} {details:<35}")
    
    for component, status, details in missing_components:
        print(f"{component:<20} {status:<15} {details:<35}")
    
    print("\nSummary:")
    print(f"Found {len(installed_components)} of {len(opencog_components)} OpenCog components")
    
    return installed_components, missing_components

def check_process_health():
    """Check if any OpenCog processes are running"""
    opencog_processes = [
        "cogserver",
        "opencog-atomspace-viewer"
    ]
    
    running_processes = []
    
    print("\nChecking OpenCog processes...")
    print("-"*70)
    
    if sys.platform == 'win32':
        # Windows - use WMIC
        try:
            for proc_name in opencog_processes:
                result = subprocess.run(['wmic', 'process', 'where', f"name like '%{proc_name}%'", 'get', 'processid,name,commandline'], 
                                        capture_output=True, text=True)
                if proc_name in result.stdout:
                    pid = result.stdout.strip().split('\n')[-1].strip()
                    if pid and pid.isdigit():
                        running_processes.append((proc_name, pid))
        except Exception as e:
            print(f"Error checking Windows processes: {e}")
    else:
        # Linux/Mac - use ps
        try:
            for proc_name in opencog_processes:
                result = subprocess.run(['ps', '-ef'], capture_output=True, text=True)
                for line in result.stdout.split('\n'):
                    if proc_name in line and 'grep' not in line:
                        parts = line.split()
                        if len(parts) > 1:
                            pid = parts[1]
                            running_processes.append((proc_name, pid))
        except Exception as e:
            print(f"Error checking Linux/Mac processes: {e}")
    
    if running_processes:
        print(f"{'Process':<25} {'PID':<10}")
        print("-"*40)
        for proc_name, pid in running_processes:
            print(f"{proc_name:<25} {pid:<10}")
    else:
        print("No OpenCog processes currently running")
    
    return running_processes

def launch_component_health_monitor():
    """Launch the Clay UI component health monitor"""
    print("\nLaunching Component Health Monitor...")
    
    # Simulate how to run the real component health monitor
    monitor_script = "../scripts/component-health-monitor.py"
    
    if os.path.exists(monitor_script):
        print("Found component health monitor script. Launch command:")
        print(f"python {monitor_script} --web --port 8085")
        print("\nThis would start the web-based component health monitor on http://localhost:8085")
        
        # Uncomment to actually launch the monitor (if it exists)
        # subprocess.Popen(['python', monitor_script, '--web', '--port', '8085'])
    else:
        print(f"Component health monitor script not found at '{monitor_script}'")
        print("To use the health monitor:")
        print("1. Install the full OpenCog system with Clay UI")
        print("2. Run: python ../scripts/component-health-monitor.py --web --port 8085")
        print("3. Open a browser to http://localhost:8085")

def show_dashboard_usage():
    """Show instructions for using the Clay UI dashboard for monitoring"""
    print("\nUsing the Clay UI Dashboard for Monitoring:")
    print("-"*70)
    print("The Clay UI Dashboard provides real-time monitoring for OpenCog components:")
    print("1. System metrics (CPU, Memory, Disk usage)")
    print("2. Component status (healthy, warning, error, not running)")
    print("3. Process details for each component")
    print("4. Response times and error counts")
    print("5. Alerts for components with issues")
    print("\nDashboard Features:")
    print("- Auto-refresh every 60 seconds")
    print("- Color-coded status indicators")
    print("- Status history tracking")
    print("- Component restart capabilities")
    print("- One-click access to component logs")

def main():
    """Main function to monitor OpenCog components"""
    print("OpenCog Component Health Monitor Demo")
    print("=====================================")
    
    # Check OpenCog installation
    installed, missing = check_opencog_installation()
    
    # Check running processes
    running_processes = check_process_health()
    
    # Show how to launch the component health monitor
    launch_component_health_monitor()
    
    # Show dashboard usage instructions
    show_dashboard_usage()
    
    # Print conclusion
    print("\nConclusion:")
    if len(installed) >= 5:
        print("OpenCog Basic appears to be properly installed")
        if running_processes:
            print("Some OpenCog components are currently running")
        else:
            print("No OpenCog components are currently running - start them to use the system")
    else:
        print("OpenCog Basic installation appears incomplete")
        print("Follow the installation instructions in the README.md file")

if __name__ == "__main__":
    main() 