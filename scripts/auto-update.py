#!/usr/bin/env python3
"""
OpenCog Auto-Update System

This script checks for updates to OpenCog components and can automatically
update them if desired.

Usage:
  auto-update.py check         Check for available updates
  auto-update.py update        Download and install available updates
  auto-update.py -h | --help   Show this help message

Options:
  -h --help     Show this screen.
  --silent      Don't display progress information.
  --log=FILE    Log output to FILE instead of stdout.
  --version     Show version.
"""

import os
import sys
import json
import time
import logging
import argparse
import platform
import subprocess
import urllib.request
from datetime import datetime
from pathlib import Path

# Constants
OPENCOG_REPO_URL = "https://github.com/opencog"
CONFIG_FILE = os.path.expanduser(os.path.join("~", ".opencog", "auto-update.json"))
VERSION = "1.0.0"

# Setup logging
logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO)
logger = logging.getLogger('opencog-autoupdate')

class AutoUpdater:
    """OpenCog Auto-Update System"""
    
    def __init__(self, silent=False, log_file=None):
        self.silent = silent
        
        if log_file:
            file_handler = logging.FileHandler(log_file)
            file_handler.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))
            logger.addHandler(file_handler)
        
        # Create config directory if it doesn't exist
        os.makedirs(os.path.dirname(CONFIG_FILE), exist_ok=True)
        
        # Load configuration
        self.config = self._load_config()
        
        # Components to check
        self.components = [
            "cogutil",
            "atomspace",
            "atomspace-storage",
            "atomspace-rocks",
            "atomspace-pgres",
            "cogserver",
            "opencog",
            "learn",
            "sensory"
        ]
        
        # Platform-specific settings
        self.is_windows = platform.system() == "Windows"
        self.script_ext = ".ps1" if self.is_windows else ".sh"
        self.path_sep = "\\" if self.is_windows else "/"
    
    def _load_config(self):
        """Load configuration from file"""
        try:
            if os.path.exists(CONFIG_FILE):
                with open(CONFIG_FILE, 'r') as f:
                    return json.load(f)
            else:
                # Default config
                return {
                    "last_check": None,
                    "components": {},
                    "auto_update": False,
                    "check_frequency": 86400,  # Once per day
                    "install_dir": "/usr/local" if not self.is_windows else "C:\\Program Files\\OpenCog"
                }
        except Exception as e:
            logger.error(f"Error loading configuration: {e}")
            return {}
    
    def _save_config(self):
        """Save configuration to file"""
        try:
            with open(CONFIG_FILE, 'w') as f:
                json.dump(self.config, f, indent=2)
        except Exception as e:
            logger.error(f"Error saving configuration: {e}")
    
    def check_updates(self):
        """Check for available updates to all components"""
        logger.info("Checking for updates...")
        
        updates_available = []
        
        # Record time of check
        self.config["last_check"] = datetime.now().isoformat()
        
        for component in self.components:
            logger.info(f"Checking {component}...")
            
            # Check if component exists in config
            if component not in self.config["components"]:
                self.config["components"][component] = {
                    "last_updated": None,
                    "installed_version": None,
                    "latest_version": None
                }
            
            # Get installed version
            installed_version = self._get_installed_version(component)
            self.config["components"][component]["installed_version"] = installed_version
            
            # Get latest version from repository
            latest_version = self._get_latest_version(component)
            self.config["components"][component]["latest_version"] = latest_version
            
            # Compare versions
            if installed_version and latest_version and installed_version != latest_version:
                logger.info(f"Update available for {component}: {installed_version} -> {latest_version}")
                updates_available.append(component)
            else:
                logger.info(f"{component} is up to date")
        
        # Save updated config
        self._save_config()
        
        return updates_available
    
    def update_components(self, components=None):
        """Update specified components or all if none specified"""
        if not components:
            components = self.check_updates()
        
        if not components:
            logger.info("No updates available.")
            return True
        
        logger.info(f"Updating components: {', '.join(components)}")
        
        success = True
        
        for component in components:
            logger.info(f"Updating {component}...")
            try:
                if self._update_component(component):
                    logger.info(f"Successfully updated {component}")
                    self.config["components"][component]["last_updated"] = datetime.now().isoformat()
                else:
                    logger.error(f"Failed to update {component}")
                    success = False
            except Exception as e:
                logger.error(f"Error updating {component}: {e}")
                success = False
        
        # Save updated config
        self._save_config()
        
        return success
    
    def _get_installed_version(self, component):
        """Get the installed version of a component"""
        try:
            # Check if component is installed
            cmd = []
            
            if self.is_windows:
                # PowerShell command to check component version
                cmd = ["powershell", "-Command", f"if (Test-Path {self.config['install_dir']}\\{component}\\VERSION) {{ Get-Content {self.config['install_dir']}\\{component}\\VERSION }}"]
            else:
                # Bash command to check component version
                cmd = ["bash", "-c", f"[ -f {self.config['install_dir']}/share/{component}/VERSION ] && cat {self.config['install_dir']}/share/{component}/VERSION"]
            
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode == 0 and result.stdout.strip():
                return result.stdout.strip()
            
            return None
        except Exception as e:
            logger.error(f"Error getting installed version for {component}: {e}")
            return None
    
    def _get_latest_version(self, component):
        """Get the latest version available from the repository"""
        try:
            # In a real implementation, this would query the GitHub API
            # For this demo, we'll simulate by returning a newer version
            
            installed_version = self.config["components"][component]["installed_version"]
            
            if not installed_version:
                return "1.0.0"  # Initial version if none installed
            
            # Parse version and increment patch level
            parts = installed_version.split('.')
            if len(parts) == 3:
                major, minor, patch = map(int, parts)
                patch += 1
                return f"{major}.{minor}.{patch}"
            
            return installed_version
        except Exception as e:
            logger.error(f"Error getting latest version for {component}: {e}")
            return None
    
    def _update_component(self, component):
        """Update a specific component"""
        try:
            # In a real implementation, this would run the build scripts
            # For this demo, we'll simulate the update process
            
            cmd = []
            
            if self.is_windows:
                # PowerShell command to update component
                cmd = ["powershell", "-Command", f".\\scripts\\incremental-build.ps1 -Rebuild {component}"]
            else:
                # Bash command to update component
                cmd = ["bash", "-c", f"./scripts/incremental-build.sh --rebuild {component}"]
            
            # Run the update command
            logger.info(f"Running command: {' '.join(cmd)}")
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode == 0:
                # Update was successful
                latest_version = self.config["components"][component]["latest_version"]
                self.config["components"][component]["installed_version"] = latest_version
                return True
            else:
                logger.error(f"Update command failed: {result.stderr}")
                return False
        except Exception as e:
            logger.error(f"Error updating component: {e}")
            return False
    
    def auto_check(self):
        """Check for updates automatically based on configured frequency"""
        if not self.config.get("last_check"):
            logger.info("No previous check found, checking now...")
            return self.check_updates()
        
        # Parse last check time
        last_check = datetime.fromisoformat(self.config["last_check"])
        now = datetime.now()
        
        # Check if it's time to check again
        seconds_since_check = (now - last_check).total_seconds()
        
        if seconds_since_check >= self.config["check_frequency"]:
            logger.info(f"Last check was {seconds_since_check / 3600:.1f} hours ago, checking again...")
            return self.check_updates()
        else:
            logger.info(f"Last check was only {seconds_since_check / 3600:.1f} hours ago, skipping...")
            return []

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(description='OpenCog Auto-Update System')
    parser.add_argument('action', choices=['check', 'update'], help='Action to perform')
    parser.add_argument('--silent', action='store_true', help='Don\'t display progress information')
    parser.add_argument('--log', help='Log file to write to')
    parser.add_argument('--version', action='version', version=f'OpenCog Auto-Update {VERSION}')
    
    args = parser.parse_args()
    
    updater = AutoUpdater(silent=args.silent, log_file=args.log)
    
    if args.action == 'check':
        updates = updater.check_updates()
        if updates:
            print(f"Updates available for: {', '.join(updates)}")
            return 0
        else:
            print("All components are up to date")
            return 0
    elif args.action == 'update':
        if updater.update_components():
            print("All components updated successfully")
            return 0
        else:
            print("Some components failed to update")
            return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main()) 