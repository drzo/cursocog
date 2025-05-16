#!/usr/bin/env python3
"""
OpenCog Component Health Monitor

This script monitors the health of OpenCog components by collecting and analyzing 
metrics about their performance, stability, and resource usage.

Usage:
  component-health-monitor.py [options]

Options:
  -h --help                 Show this help message and exit
  --component=COMPONENT     Monitor specific component(s) only
  --interval=SECONDS        Monitoring interval in seconds [default: 60]
  --output=FORMAT           Output format (text, json, csv) [default: text]
  --log=FILE                Log to file instead of stdout
  --web                     Start web dashboard on port 8085
  --port=PORT               Web dashboard port [default: 8085]
  --thresholds=FILE         Path to threshold configuration file
"""

import os
import sys
import json
import time
import logging
import argparse
import subprocess
import threading
import datetime
import socket
import psutil
from pathlib import Path

# Default thresholds for alerts
DEFAULT_THRESHOLDS = {
    "memory_percent": 90,  # Alert if memory usage > 90%
    "cpu_percent": 90,     # Alert if CPU usage > 90% 
    "disk_percent": 90,    # Alert if disk usage > 90%
    "response_time": 5000, # Alert if response time > 5000ms
    "error_count": 5,      # Alert if > 5 errors in monitoring interval
}

# OpenCog components to monitor
COMPONENTS = [
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

class ComponentHealthMonitor:
    """Monitors the health of OpenCog components"""
    
    def __init__(self, components=None, interval=60, output_format="text", 
                 log_file=None, web=False, port=8085, threshold_file=None):
        """Initialize the health monitor"""
        self.components = components if components else COMPONENTS
        self.interval = interval
        self.output_format = output_format
        self.web = web
        self.port = port
        self.running = False
        self.metrics = {}
        self.alerts = []
        
        # Set up logging
        self.logger = logging.getLogger('health-monitor')
        self.logger.setLevel(logging.INFO)
        
        if log_file:
            handler = logging.FileHandler(log_file)
        else:
            handler = logging.StreamHandler()
            
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        
        # Load thresholds
        self.thresholds = DEFAULT_THRESHOLDS.copy()
        if threshold_file:
            try:
                with open(threshold_file, 'r') as f:
                    custom_thresholds = json.load(f)
                    self.thresholds.update(custom_thresholds)
                    self.logger.info(f"Loaded custom thresholds from {threshold_file}")
            except Exception as e:
                self.logger.error(f"Error loading thresholds from {threshold_file}: {e}")
    
    def start(self):
        """Start the monitoring process"""
        self.running = True
        self.logger.info(f"Starting component health monitoring for: {', '.join(self.components)}")
        self.logger.info(f"Monitoring interval: {self.interval} seconds")
        
        # Start web dashboard if enabled
        if self.web:
            self._start_web_dashboard()
        
        try:
            while self.running:
                self._collect_metrics()
                self._analyze_metrics()
                self._output_metrics()
                time.sleep(self.interval)
        except KeyboardInterrupt:
            self.logger.info("Monitoring stopped by user")
        finally:
            self.running = False
    
    def stop(self):
        """Stop the monitoring process"""
        self.running = False
        self.logger.info("Stopping component health monitoring")
    
    def _collect_metrics(self):
        """Collect health metrics for each component"""
        timestamp = datetime.datetime.now().isoformat()
        
        # System-wide metrics
        system_metrics = {
            "timestamp": timestamp,
            "cpu_percent": psutil.cpu_percent(interval=1),
            "memory_percent": psutil.virtual_memory().percent,
            "disk_percent": psutil.disk_usage('/').percent,
            "load_avg": os.getloadavg() if os.name != 'nt' else (0, 0, 0)
        }
        
        self.metrics["system"] = system_metrics
        
        # Component-specific metrics
        for component in self.components:
            # Skip if component not installed
            install_dir = os.path.join("/usr/local/share", component)
            if not os.path.exists(install_dir) and not os.path.exists(f"/usr/local/lib/lib{component}"):
                continue
                
            component_metrics = {
                "timestamp": timestamp,
                "installed": True,
                "processes": self._get_component_processes(component),
                "response_time": self._check_component_responsiveness(component),
                "error_count": self._get_error_count(component),
                "uptime": self._get_uptime(component),
                "status": "unknown"
            }
            
            # Determine component status
            if component_metrics["processes"]:
                if component_metrics["error_count"] > self.thresholds["error_count"]:
                    component_metrics["status"] = "warning"
                elif component_metrics["response_time"] > self.thresholds["response_time"]:
                    component_metrics["status"] = "degraded"
                else:
                    component_metrics["status"] = "healthy"
            else:
                component_metrics["status"] = "not running"
            
            self.metrics[component] = component_metrics
            
        self.logger.debug(f"Collected metrics for {len(self.components)} components")
    
    def _get_component_processes(self, component):
        """Get information about processes for a component"""
        processes = []
        
        # Find processes matching the component name
        for proc in psutil.process_iter(['pid', 'name', 'cmdline', 'cpu_percent', 'memory_percent']):
            try:
                if component in proc.info['name'] or any(component in cmd for cmd in proc.info['cmdline'] if cmd):
                    processes.append({
                        "pid": proc.info['pid'],
                        "name": proc.info['name'],
                        "cpu_percent": proc.info['cpu_percent'],
                        "memory_percent": proc.info['memory_percent']
                    })
            except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
                pass
        
        return processes
    
    def _check_component_responsiveness(self, component):
        """Check the responsiveness of a component"""
        response_time = 0
        
        # For CogServer, check socket connection
        if component == "cogserver":
            try:
                start_time = time.time()
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.settimeout(2)
                s.connect(("localhost", 17001))
                s.close()
                response_time = (time.time() - start_time) * 1000  # Convert to ms
            except:
                response_time = 9999  # Error connecting
        
        return response_time
    
    def _get_error_count(self, component):
        """Get the number of errors for a component"""
        error_count = 0
        
        # Check component log files
        log_paths = [
            f"/var/log/{component}.log",
            f"logs/{component}.log",
            f"build_logs/{component}_build.log"
        ]
        
        for log_path in log_paths:
            if os.path.exists(log_path):
                try:
                    # Count error lines in the last part of the file
                    with open(log_path, 'r') as f:
                        # Read the last 100 lines
                        lines = f.readlines()[-100:] if os.path.getsize(log_path) > 0 else []
                        for line in lines:
                            if "error" in line.lower() or "exception" in line.lower():
                                error_count += 1
                except:
                    pass
        
        return error_count
    
    def _get_uptime(self, component):
        """Get the uptime of a component in seconds"""
        uptime = 0
        
        # Find the oldest process matching the component
        for proc in self._get_component_processes(component):
            try:
                p = psutil.Process(proc["pid"])
                proc_uptime = time.time() - p.create_time()
                if proc_uptime > uptime:
                    uptime = proc_uptime
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                pass
        
        return int(uptime)
    
    def _analyze_metrics(self):
        """Analyze collected metrics and generate alerts"""
        new_alerts = []
        
        # Check system metrics
        system = self.metrics.get("system", {})
        if system.get("cpu_percent", 0) > self.thresholds["cpu_percent"]:
            new_alerts.append(f"System CPU usage is high: {system['cpu_percent']}%")
        
        if system.get("memory_percent", 0) > self.thresholds["memory_percent"]:
            new_alerts.append(f"System memory usage is high: {system['memory_percent']}%")
        
        if system.get("disk_percent", 0) > self.thresholds["disk_percent"]:
            new_alerts.append(f"System disk usage is high: {system['disk_percent']}%")
        
        # Check component metrics
        for component, metrics in self.metrics.items():
            if component == "system":
                continue
                
            if metrics.get("status") == "warning":
                new_alerts.append(f"Component {component} has warnings: {metrics.get('error_count', 0)} errors")
            
            if metrics.get("status") == "degraded":
                new_alerts.append(f"Component {component} performance is degraded: response time {metrics.get('response_time', 0)}ms")
            
            if metrics.get("status") == "not running" and metrics.get("installed", False):
                new_alerts.append(f"Component {component} is installed but not running")
            
            # Check process resource usage
            for proc in metrics.get("processes", []):
                if proc.get("cpu_percent", 0) > self.thresholds["cpu_percent"]:
                    new_alerts.append(f"Process {proc['pid']} of {component} has high CPU usage: {proc['cpu_percent']}%")
                
                if proc.get("memory_percent", 0) > self.thresholds["memory_percent"]:
                    new_alerts.append(f"Process {proc['pid']} of {component} has high memory usage: {proc['memory_percent']}%")
        
        # Log new alerts
        for alert in new_alerts:
            if alert not in self.alerts:
                self.logger.warning(alert)
                
        self.alerts = new_alerts
    
    def _output_metrics(self):
        """Output the collected metrics in the specified format"""
        if self.output_format == "json":
            print(json.dumps(self.metrics, indent=2))
        elif self.output_format == "csv":
            # Output as CSV (simplified)
            if not hasattr(self, 'csv_header_printed'):
                components = list(self.metrics.keys())
                headers = ["timestamp"] + [f"{c}_status" for c in components]
                print(",".join(headers))
                self.csv_header_printed = True
            
            timestamp = self.metrics.get("system", {}).get("timestamp", datetime.datetime.now().isoformat())
            values = [timestamp] + [self.metrics.get(c, {}).get("status", "unknown") for c in self.metrics.keys()]
            print(",".join(str(v) for v in values))
        else:
            # Text output (default)
            print("\n=== OpenCog Component Health Report ===")
            print(f"Timestamp: {self.metrics.get('system', {}).get('timestamp', datetime.datetime.now().isoformat())}")
            print(f"System CPU: {self.metrics.get('system', {}).get('cpu_percent', 'N/A')}%")
            print(f"System Memory: {self.metrics.get('system', {}).get('memory_percent', 'N/A')}%")
            print(f"System Disk: {self.metrics.get('system', {}).get('disk_percent', 'N/A')}%")
            print("\nComponent Status:")
            
            for component, metrics in sorted(self.metrics.items()):
                if component == "system":
                    continue
                
                status = metrics.get("status", "unknown")
                processes = len(metrics.get("processes", []))
                uptime = metrics.get("uptime", 0)
                uptime_str = f"{int(uptime // 86400)}d {int((uptime % 86400) // 3600)}h {int((uptime % 3600) // 60)}m" if uptime > 0 else "not running"
                
                print(f"  {component}: {status.upper()} ({processes} processes, uptime: {uptime_str})")
            
            if self.alerts:
                print("\nAlerts:")
                for alert in self.alerts:
                    print(f"  ! {alert}")
            
            print("\n")
    
    def _start_web_dashboard(self):
        """Start a simple web dashboard for monitoring"""
        try:
            import http.server
            import socketserver
            
            # Create a simple HTML dashboard
            def create_dashboard():
                html = """
                <!DOCTYPE html>
                <html>
                <head>
                    <title>OpenCog Component Health Monitor</title>
                    <meta http-equiv="refresh" content="60">
                    <style>
                        body { font-family: Arial, sans-serif; margin: 20px; }
                        table { border-collapse: collapse; width: 100%; }
                        th, td { padding: 8px; text-align: left; border-bottom: 1px solid #ddd; }
                        tr:hover { background-color: #f5f5f5; }
                        .healthy { color: green; }
                        .warning { color: orange; }
                        .degraded { color: orangered; }
                        .not-running { color: red; }
                        .alert { background-color: #ffe6e6; color: red; padding: 5px; margin: 5px 0; }
                    </style>
                </head>
                <body>
                    <h1>OpenCog Component Health Monitor</h1>
                    <p>Last updated: {timestamp}</p>
                    
                    <h2>System Metrics</h2>
                    <table>
                        <tr>
                            <th>Metric</th>
                            <th>Value</th>
                        </tr>
                        <tr>
                            <td>CPU Usage</td>
                            <td>{system[cpu_percent]}%</td>
                        </tr>
                        <tr>
                            <td>Memory Usage</td>
                            <td>{system[memory_percent]}%</td>
                        </tr>
                        <tr>
                            <td>Disk Usage</td>
                            <td>{system[disk_percent]}%</td>
                        </tr>
                        <tr>
                            <td>Load Average</td>
                            <td>{system[load_avg][0]:.2f}, {system[load_avg][1]:.2f}, {system[load_avg][2]:.2f}</td>
                        </tr>
                    </table>
                    
                    <h2>Component Status</h2>
                    <table>
                        <tr>
                            <th>Component</th>
                            <th>Status</th>
                            <th>Processes</th>
                            <th>Uptime</th>
                            <th>Response Time</th>
                            <th>Error Count</th>
                        </tr>
                        {component_rows}
                    </table>
                    
                    {alerts}
                    
                    <script>
                        // Auto-refresh every 60 seconds
                        setTimeout(function() {{
                            location.reload();
                        }}, 60000);
                    </script>
                </body>
                </html>
                """
                
                # Generate component rows
                component_rows = ""
                for component, metrics in sorted(self.metrics.items()):
                    if component == "system":
                        continue
                    
                    status = metrics.get("status", "unknown")
                    status_class = "healthy" if status == "healthy" else "warning" if status == "warning" else "degraded" if status == "degraded" else "not-running"
                    
                    processes = len(metrics.get("processes", []))
                    uptime = metrics.get("uptime", 0)
                    uptime_str = f"{int(uptime // 86400)}d {int((uptime % 86400) // 3600)}h {int((uptime % 3600) // 60)}m" if uptime > 0 else "not running"
                    response_time = metrics.get("response_time", 0)
                    error_count = metrics.get("error_count", 0)
                    
                    component_rows += f"""
                    <tr>
                        <td>{component}</td>
                        <td class="{status_class}">{status.upper()}</td>
                        <td>{processes}</td>
                        <td>{uptime_str}</td>
                        <td>{response_time}ms</td>
                        <td>{error_count}</td>
                    </tr>
                    """
                
                # Generate alerts section
                alerts_html = ""
                if self.alerts:
                    alerts_html = "<h2>Alerts</h2>"
                    for alert in self.alerts:
                        alerts_html += f'<div class="alert">{alert}</div>'
                
                # Replace placeholders
                return html.format(
                    timestamp=self.metrics.get("system", {}).get("timestamp", datetime.datetime.now().isoformat()),
                    system=self.metrics.get("system", {"cpu_percent": 0, "memory_percent": 0, "disk_percent": 0, "load_avg": (0, 0, 0)}),
                    component_rows=component_rows,
                    alerts=alerts_html
                )
            
            # HTTP request handler
            class MonitorHandler(http.server.SimpleHTTPRequestHandler):
                def do_GET(self):
                    self.send_response(200)
                    self.send_header('Content-type', 'text/html')
                    self.end_headers()
                    self.wfile.write(create_dashboard().encode())
                
                def log_message(self, format, *args):
                    # Suppress logs
                    pass
            
            # Start server in a separate thread
            def run_server():
                with socketserver.TCPServer(("", self.port), MonitorHandler) as httpd:
                    self.logger.info(f"Web dashboard started at http://localhost:{self.port}")
                    while self.running:
                        httpd.handle_request()
            
            threading.Thread(target=run_server, daemon=True).start()
            
        except ImportError:
            self.logger.error("Failed to start web dashboard - missing required modules")
        except Exception as e:
            self.logger.error(f"Failed to start web dashboard: {e}")

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(description="OpenCog Component Health Monitor")
    parser.add_argument("--component", help="Monitor specific component(s) only", nargs="+")
    parser.add_argument("--interval", help="Monitoring interval in seconds", type=int, default=60)
    parser.add_argument("--output", help="Output format (text, json, csv)", default="text")
    parser.add_argument("--log", help="Log to file instead of stdout")
    parser.add_argument("--web", help="Start web dashboard", action="store_true")
    parser.add_argument("--port", help="Web dashboard port", type=int, default=8085)
    parser.add_argument("--thresholds", help="Path to threshold configuration file")
    
    args = parser.parse_args()
    
    monitor = ComponentHealthMonitor(
        components=args.component,
        interval=args.interval,
        output_format=args.output,
        log_file=args.log,
        web=args.web,
        port=args.port,
        threshold_file=args.thresholds
    )
    
    monitor.start()

if __name__ == "__main__":
    main() 