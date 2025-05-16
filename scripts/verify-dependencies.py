#!/usr/bin/env python3
# 
# OpenCog Dependency Verification Tool
# 
# This tool checks for all dependencies required to build and run OpenCog,
# verifies their versions, and reports any issues found.
#

import os
import sys
import platform
import subprocess
import json
import re
import logging
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
logger = logging.getLogger('opencog-verify')

# Define colors for terminal output
class Colors:
    if sys.platform == 'win32':
        GREEN = ''
        RED = ''
        YELLOW = ''
        BLUE = ''
        RESET = ''
    else:
        GREEN = '\033[0;32m'
        RED = '\033[0;31m'
        YELLOW = '\033[0;33m'
        BLUE = '\033[0;34m'
        RESET = '\033[0m'

# Define minimum versions for dependencies
MIN_VERSIONS = {
    'cmake': '3.12.0',
    'boost': '1.46.0',
    'cxxtest': '3.10.1',
    'guile': '2.2.0',
    'python': '3.6.0',
    'cython': '0.29.0',
    'rocksdb': '5.0.0',
    'git': '2.0.0',
    'doxygen': '1.8.0',
}

# Define list of dependencies by platform
DEPENDENCIES = {
    'common': [
        'cmake',
        'boost',
        'cxxtest',
        'git',
        'python',
        'cython',
        'doxygen',
        'guile',
    ],
    'linux': [
        'gcc',
        'g++',
        'libpq',
        'rocksdb',
        'postgresql',
    ],
    'darwin': [  # macOS
        'clang',
        'libpq',
        'rocksdb',
        'postgresql',
    ],
    'win32': [
        'vcpkg',
        'msvc',
    ],
    'optional': [
        'ocaml',
        'haskell',
        'graphviz',
    ]
}

# Installation instructions by platform
INSTALL_INSTRUCTIONS = {
    'linux': {
        'cmake': 'sudo apt install cmake',
        'boost': 'sudo apt install libboost-dev libboost-filesystem-dev libboost-program-options-dev libboost-system-dev libboost-thread-dev libboost-math-dev',
        'cxxtest': 'sudo apt install cxxtest',
        'git': 'sudo apt install git',
        'python': 'sudo apt install python3 python3-dev',
        'cython': 'sudo apt install cython3 or pip3 install cython',
        'doxygen': 'sudo apt install doxygen',
        'guile': 'sudo apt install guile-3.0-dev',
        'gcc': 'sudo apt install build-essential',
        'g++': 'sudo apt install build-essential',
        'libpq': 'sudo apt install libpq-dev',
        'rocksdb': 'sudo apt install librocksdb-dev',
        'postgresql': 'sudo apt install postgresql postgresql-client',
        'ocaml': 'sudo apt install ocaml ocaml-findlib',
        'haskell': 'sudo apt install ghc cabal-install',
        'graphviz': 'sudo apt install graphviz',
    },
    'darwin': {  # macOS
        'cmake': 'brew install cmake',
        'boost': 'brew install boost',
        'cxxtest': 'brew install cxxtest',
        'git': 'brew install git',
        'python': 'brew install python',
        'cython': 'pip3 install cython',
        'doxygen': 'brew install doxygen',
        'guile': 'brew install guile',
        'clang': 'Install Xcode command line tools: xcode-select --install',
        'libpq': 'brew install libpq',
        'rocksdb': 'brew install rocksdb',
        'postgresql': 'brew install postgresql',
        'ocaml': 'brew install ocaml opam',
        'haskell': 'brew install ghc cabal-install',
        'graphviz': 'brew install graphviz',
    },
    'win32': {
        'cmake': 'Download from https://cmake.org/download/ or run install-dependencies.ps1',
        'boost': 'Install via vcpkg: vcpkg install boost:x64-windows',
        'cxxtest': 'Install via vcpkg: vcpkg install cxxtest:x64-windows',
        'git': 'Download from https://git-scm.com/download/win or run install-dependencies.ps1',
        'python': 'Download from https://www.python.org/downloads/ or run install-dependencies.ps1',
        'cython': 'pip install cython',
        'doxygen': 'Install via vcpkg: vcpkg install doxygen:x64-windows',
        'guile': 'Download from http://www.gnu.org/software/guile/',
        'vcpkg': 'Clone from https://github.com/Microsoft/vcpkg.git and bootstrap, or run install-dependencies.ps1',
        'msvc': 'Install Visual Studio with C++ development tools from https://visualstudio.microsoft.com/downloads/',
        'ocaml': 'Install using OPAM from https://ocaml.org/docs/installing-ocaml',
        'haskell': 'Install using GHCup from https://www.haskell.org/ghcup/',
        'graphviz': 'Install via vcpkg: vcpkg install graphviz:x64-windows',
    }
}

class DependencyCheck:
    def __init__(self):
        self.system = platform.system().lower()
        self.platform = sys.platform
        self.issues = []
        self.warnings = []
        self.all_deps = []
        self.missing_deps = []
        self.outdated_deps = []
        self.optional_missing = []

        # Map system to platform
        if self.system == 'linux':
            self.platform_key = 'linux'
        elif self.system == 'darwin':
            self.platform_key = 'darwin'
        elif self.system == 'windows':
            self.platform_key = 'win32'
        else:
            self.platform_key = sys.platform

        # Combine common and platform-specific dependencies
        self.all_deps = DEPENDENCIES['common'] + DEPENDENCIES.get(self.platform_key, [])
    
    def is_binary_in_path(self, binary):
        """Check if a binary is available in the system path."""
        try:
            if self.platform == 'win32':
                result = subprocess.run(f'where {binary}', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            else:
                result = subprocess.run(f'which {binary}', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            return result.returncode == 0
        except:
            return False

    def get_version(self, command, regex, args=None):
        """Get the version of a dependency using the specified command and regex."""
        if args is None:
            args = ['--version']
        try:
            result = subprocess.run([command] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            
            # Some commands output to stderr instead of stdout
            output = result.stdout if result.stdout else result.stderr
            
            match = re.search(regex, output)
            if match:
                return match.group(1)
            return None
        except:
            return None

    def compare_versions(self, current, minimum):
        """Compare two version strings."""
        def normalize(v):
            return [int(x) for x in re.sub(r'(\.0+)*$', '', v).split(".")]
        
        try:
            return normalize(current) >= normalize(minimum)
        except:
            return False

    def check_cmake(self):
        """Check CMake installation and version."""
        if not self.is_binary_in_path('cmake'):
            return False, None
        
        version = self.get_version('cmake', r'cmake version (\d+\.\d+\.\d+)')
        if not version:
            return True, None
        
        return True, version

    def check_boost(self):
        """Check Boost installation and version."""
        if self.platform == 'win32':
            # Check if Boost is installed via vcpkg
            vcpkg_path = os.environ.get('VCPKG_ROOT')
            if not vcpkg_path:
                vcpkg_path = os.path.join(os.path.expanduser('~'), 'vcpkg')
            
            if os.path.exists(vcpkg_path):
                try:
                    result = subprocess.run([os.path.join(vcpkg_path, 'vcpkg'), 'list'], 
                                          stdout=subprocess.PIPE, text=True)
                    if 'boost' in result.stdout:
                        # Extract version from vcpkg output - simplified
                        match = re.search(r'boost[^:]*:x64-windows\s+(\d+\.\d+\.\d+)', result.stdout)
                        if match:
                            return True, match.group(1)
                        return True, "Unknown"
                except:
                    pass
            return False, None
        else:
            # Unix-like systems
            try:
                # Check if boost headers can be found
                result = subprocess.run('find /usr/include /usr/local/include -name "boost" -type d 2>/dev/null', 
                                      shell=True, stdout=subprocess.PIPE, text=True)
                
                if result.stdout.strip():
                    # Try to get version from boost/version.hpp
                    version_header = None
                    
                    for path in result.stdout.strip().split('\n'):
                        version_path = os.path.join(path, 'version.hpp')
                        if os.path.exists(version_path):
                            version_header = version_path
                            break
                    
                    if version_header:
                        with open(version_header, 'r') as f:
                            content = f.read()
                            match = re.search(r'#define\s+BOOST_VERSION\s+(\d+)', content)
                            if match:
                                boost_version = int(match.group(1))
                                # Boost version is encoded as MMMSS where M is major and S is minor version
                                major = boost_version // 100000
                                minor = (boost_version // 100) % 1000
                                patch = boost_version % 100
                                return True, f"{major}.{minor}.{patch}"
                
                # Fallback to check if boost is in pkg-config
                pkg_config = subprocess.run(['pkg-config', '--modversion', 'boost'], 
                                          stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                if pkg_config.returncode == 0:
                    return True, pkg_config.stdout.strip()
                
                # If we can find the include directory but not the version, assume it's there but unknown version
                if result.stdout.strip():
                    return True, "Unknown"
                    
                return False, None
            except:
                return False, None

    def check_git(self):
        """Check Git installation and version."""
        if not self.is_binary_in_path('git'):
            return False, None
            
        version = self.get_version('git', r'git version (\d+\.\d+\.\d+)')
        return True, version

    def check_python(self):
        """Check Python installation and version."""
        python_cmd = 'python3' if self.platform != 'win32' else 'python'
        if not self.is_binary_in_path(python_cmd):
            python_cmd = 'python'
            if not self.is_binary_in_path(python_cmd):
                return False, None
        
        version = self.get_version(python_cmd, r'Python (\d+\.\d+\.\d+)', ['-V'])
        return True, version

    def check_cython(self):
        """Check Cython installation."""
        python_cmd = 'python3' if self.platform != 'win32' else 'python'
        if not self.is_binary_in_path(python_cmd):
            python_cmd = 'python'
            if not self.is_binary_in_path(python_cmd):
                return False, None
        
        try:
            result = subprocess.run([python_cmd, '-c', 'import Cython; print(Cython.__version__)'], 
                                  stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            if result.returncode == 0:
                return True, result.stdout.strip()
            return False, None
        except:
            return False, None

    def check_guile(self):
        """Check Guile installation and version."""
        if not self.is_binary_in_path('guile'):
            return False, None
            
        version = self.get_version('guile', r'guile \(GNU Guile\) (\d+\.\d+\.\d+)')
        return True, version

    def check_cxxtest(self):
        """Check CxxTest installation."""
        # Check for cxxtestgen binary
        if self.is_binary_in_path('cxxtestgen'):
            version = self.get_version('cxxtestgen', r'CxxTest version (\d+\.\d+\.\d+)', ['--version'])
            return True, version
        
        # Check for Python module
        python_cmd = 'python3' if self.platform != 'win32' else 'python'
        try:
            result = subprocess.run([python_cmd, '-c', 'import CxxTest; print(CxxTest.__release__)'], 
                                  stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            if result.returncode == 0:
                return True, result.stdout.strip()
        except:
            pass
            
        # Check for typical installation locations
        cxxtest_paths = [
            '/usr/include/cxxtest',
            '/usr/local/include/cxxtest',
            'C:\\cxxtest'
        ]
        
        for path in cxxtest_paths:
            if os.path.exists(path):
                # Try to get version from Version.h
                version_path = os.path.join(path, 'Version.h')
                if os.path.exists(version_path):
                    try:
                        with open(version_path, 'r') as f:
                            content = f.read()
                            match = re.search(r'#define\s+CXXTEST_VERSION\s+"(\d+\.\d+\.\d+)"', content)
                            if match:
                                return True, match.group(1)
                    except:
                        pass
                return True, "Unknown"  # Found CxxTest, but can't determine version
                
        return False, None

    def check_doxygen(self):
        """Check Doxygen installation and version."""
        if not self.is_binary_in_path('doxygen'):
            return False, None
            
        version = self.get_version('doxygen', r'(\d+\.\d+\.\d+)')
        return True, version

    def check_rocksdb(self):
        """Check RocksDB installation."""
        # This is a simplistic check as RocksDB doesn't have a direct command line tool
        if self.platform == 'win32':
            # Check if installed via vcpkg
            vcpkg_path = os.environ.get('VCPKG_ROOT')
            if not vcpkg_path:
                vcpkg_path = os.path.join(os.path.expanduser('~'), 'vcpkg')
            
            if os.path.exists(vcpkg_path):
                try:
                    result = subprocess.run([os.path.join(vcpkg_path, 'vcpkg'), 'list'], 
                                          stdout=subprocess.PIPE, text=True)
                    if 'rocksdb:x64-windows' in result.stdout:
                        # Extract version - simplified
                        match = re.search(r'rocksdb:x64-windows\s+(\d+\.\d+\.\d+)', result.stdout)
                        if match:
                            return True, match.group(1)
                        return True, "Unknown"
                except:
                    pass
            return False, None
        else:
            # Unix-like systems
            # Check if rocksdb headers can be found
            possible_paths = [
                '/usr/include/rocksdb/version.h',
                '/usr/local/include/rocksdb/version.h'
            ]
            
            for path in possible_paths:
                if os.path.exists(path):
                    try:
                        with open(path, 'r') as f:
                            content = f.read()
                            match = re.search(r'#define\s+ROCKSDB_MAJOR\s+(\d+)', content)
                            major = match.group(1) if match else "0"
                            
                            match = re.search(r'#define\s+ROCKSDB_MINOR\s+(\d+)', content)
                            minor = match.group(1) if match else "0"
                            
                            match = re.search(r'#define\s+ROCKSDB_PATCH\s+(\d+)', content)
                            patch = match.group(1) if match else "0"
                            
                            return True, f"{major}.{minor}.{patch}"
                    except:
                        return True, "Unknown"  # Found header but couldn't parse version
            
            # Check using pkg-config
            try:
                pkg_config = subprocess.run(['pkg-config', '--modversion', 'rocksdb'], 
                                          stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                if pkg_config.returncode == 0:
                    return True, pkg_config.stdout.strip()
            except:
                pass
                
            # Check for shared libraries
            lib_paths = [
                '/usr/lib/librocksdb.so',
                '/usr/local/lib/librocksdb.so',
                '/usr/lib64/librocksdb.so'
            ]
            
            for path in lib_paths:
                if os.path.exists(path):
                    return True, "Unknown"  # Found library but can't determine version
                    
            return False, None

    def check_vcpkg(self):
        """Check vcpkg installation (Windows only)."""
        if self.platform != 'win32':
            return True, None  # Not applicable for non-Windows platforms
            
        # Check for VCPKG_ROOT environment variable
        vcpkg_path = os.environ.get('VCPKG_ROOT')
        if not vcpkg_path:
            vcpkg_path = os.path.join(os.path.expanduser('~'), 'vcpkg')
            
        if os.path.exists(vcpkg_path) and os.path.exists(os.path.join(vcpkg_path, 'vcpkg.exe')):
            # Get vcpkg version
            try:
                result = subprocess.run([os.path.join(vcpkg_path, 'vcpkg'), 'version'], 
                                      stdout=subprocess.PIPE, text=True)
                match = re.search(r'version (\d+\.\d+\.\d+)', result.stdout)
                if match:
                    return True, match.group(1)
                return True, "Unknown"
            except:
                return True, "Unknown"
        
        return False, None

    def check_msvc(self):
        """Check for Visual Studio with C++ tools (Windows only)."""
        if self.platform != 'win32':
            return True, None  # Not applicable for non-Windows platforms
            
        # Check for vswhere.exe
        vswhere_path = os.path.join(os.environ.get('ProgramFiles(x86)', 'C:\\Program Files (x86)'), 
                                  'Microsoft Visual Studio', 'Installer', 'vswhere.exe')
        
        if os.path.exists(vswhere_path):
            try:
                # Check for latest Visual Studio with C++ components
                result = subprocess.run([vswhere_path, '-latest', '-products', '*', 
                                       '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', 
                                       '-property', 'installationVersion'], 
                                      stdout=subprocess.PIPE, text=True)
                if result.stdout.strip():
                    return True, result.stdout.strip()
            except:
                pass
                
        # Alternative check for Visual C++ compiler
        try:
            result = subprocess.run(['cl'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            if 'Microsoft' in result.stderr:
                return True, "Unknown"  # Found cl.exe but can't determine version reliably
        except:
            pass
            
        return False, None

    def check_compiler(self):
        """Check for C/C++ compiler."""
        if self.platform == 'win32':
            return self.check_msvc()
            
        # Check GCC for Linux or Clang for macOS
        compiler_cmd = 'gcc' if self.platform == 'linux' else 'clang'
        if not self.is_binary_in_path(compiler_cmd):
            return False, None
            
        version_regex = r'(\d+\.\d+\.\d+)' if self.platform == 'linux' else r'Apple [^]]*clang-(\d+\.\d+\.\d+)'
        version = self.get_version(compiler_cmd, version_regex)
        return True, version

    def check_ocaml(self):
        """Check OCaml installation (optional)."""
        if not self.is_binary_in_path('ocaml'):
            return False, None
            
        version = self.get_version('ocaml', r'The OCaml toplevel, version (\d+\.\d+\.\d+)')
        return True, version

    def check_haskell(self):
        """Check Haskell installation (optional)."""
        if not self.is_binary_in_path('ghc'):
            return False, None
            
        version = self.get_version('ghc', r'version (\d+\.\d+\.\d+)')
        return True, version

    def check_graphviz(self):
        """Check Graphviz installation (optional)."""
        if not self.is_binary_in_path('dot'):
            return False, None
            
        version = self.get_version('dot', r'dot - graphviz version (\d+\.\d+\.\d+)')
        return True, version

    def check_postgresql(self):
        """Check PostgreSQL installation and version."""
        if not self.is_binary_in_path('psql'):
            return False, None
            
        version = self.get_version('psql', r'psql \(PostgreSQL\) (\d+\.\d+)')
        return True, version

    def check_libpq(self):
        """Check LibPQ installation (PostgreSQL client library)."""
        # Look for libpq header files
        if self.platform == 'win32':
            # Check if installed via vcpkg
            vcpkg_path = os.environ.get('VCPKG_ROOT')
            if not vcpkg_path:
                vcpkg_path = os.path.join(os.path.expanduser('~'), 'vcpkg')
            
            if os.path.exists(vcpkg_path):
                try:
                    result = subprocess.run([os.path.join(vcpkg_path, 'vcpkg'), 'list'], 
                                          stdout=subprocess.PIPE, text=True)
                    if 'libpq:x64-windows' in result.stdout:
                        return True, "Installed via vcpkg"
                except:
                    pass
            return False, None
        else:
            # Unix-like systems
            possible_header_paths = [
                '/usr/include/postgresql/libpq-fe.h',
                '/usr/local/include/postgresql/libpq-fe.h',
                '/usr/include/libpq-fe.h',
                '/usr/local/include/libpq-fe.h'
            ]
            
            for path in possible_header_paths:
                if os.path.exists(path):
                    return True, "Found header files"
                    
            # Check using pkg-config
            try:
                pkg_config = subprocess.run(['pkg-config', '--modversion', 'libpq'], 
                                          stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                if pkg_config.returncode == 0:
                    return True, pkg_config.stdout.strip()
            except:
                pass
                
            # Check for shared libraries
            lib_paths = [
                '/usr/lib/libpq.so',
                '/usr/local/lib/libpq.so',
                '/usr/lib64/libpq.so'
            ]
            
            for path in lib_paths:
                if os.path.exists(path):
                    return True, "Found shared library"
                    
            return False, None

    def check_dependency(self, dep):
        """Check a single dependency."""
        check_method_name = f"check_{dep}"
        if hasattr(self, check_method_name):
            check_method = getattr(self, check_method_name)
            return check_method()
        else:
            # Generic check - just look for the binary in path
            return self.is_binary_in_path(dep), None

    def run_checks(self):
        """Run all dependency checks."""
        logger.info(f"{Colors.BLUE}Running OpenCog dependency checks...{Colors.RESET}")
        
        # Check each required dependency
        for dep in self.all_deps:
            installed, version = self.check_dependency(dep)
            
            if not installed:
                self.missing_deps.append(dep)
                platform_key = self.platform_key if self.platform_key in INSTALL_INSTRUCTIONS else 'linux'
                install_instruction = INSTALL_INSTRUCTIONS.get(platform_key, {}).get(dep, f"Install {dep}")
                logger.warning(f"{Colors.RED}✗ {dep} - Not found{Colors.RESET}")
                logger.info(f"  Install with: {install_instruction}")
                self.issues.append(f"Missing dependency: {dep}")
            elif version and dep in MIN_VERSIONS and not self.compare_versions(version, MIN_VERSIONS[dep]):
                self.outdated_deps.append((dep, version, MIN_VERSIONS[dep]))
                logger.warning(f"{Colors.YELLOW}⚠ {dep} - Version {version} found (minimum required: {MIN_VERSIONS[dep]}){Colors.RESET}")
                self.issues.append(f"Outdated dependency: {dep} (found: {version}, required: {MIN_VERSIONS[dep]})")
            else:
                version_str = f" (version {version})" if version else ""
                logger.info(f"{Colors.GREEN}✓ {dep}{version_str}{Colors.RESET}")
        
        # Check optional dependencies
        logger.info(f"\n{Colors.BLUE}Checking optional dependencies...{Colors.RESET}")
        for dep in DEPENDENCIES['optional']:
            installed, version = self.check_dependency(dep)
            
            if not installed:
                self.optional_missing.append(dep)
                logger.info(f"{Colors.YELLOW}○ {dep} - Not found (optional){Colors.RESET}")
                platform_key = self.platform_key if self.platform_key in INSTALL_INSTRUCTIONS else 'linux'
                install_instruction = INSTALL_INSTRUCTIONS.get(platform_key, {}).get(dep, f"Install {dep}")
                logger.info(f"  Install with: {install_instruction}")
                self.warnings.append(f"Missing optional dependency: {dep}")
            else:
                version_str = f" (version {version})" if version else ""
                logger.info(f"{Colors.GREEN}✓ {dep}{version_str}{Colors.RESET}")
            
    def print_summary(self):
        """Print summary of dependency check results."""
        logger.info(f"\n{Colors.BLUE}Dependency Check Summary{Colors.RESET}")
        
        if not self.issues and not self.warnings:
            logger.info(f"{Colors.GREEN}All dependencies are installed and meet minimum version requirements.{Colors.RESET}")
            logger.info("You can build OpenCog by running ./build.sh or build.ps1.")
            return
            
        if self.missing_deps:
            logger.warning(f"{Colors.RED}Missing dependencies ({len(self.missing_deps)}):{Colors.RESET}")
            for dep in self.missing_deps:
                platform_key = self.platform_key if self.platform_key in INSTALL_INSTRUCTIONS else 'linux'
                install_instruction = INSTALL_INSTRUCTIONS.get(platform_key, {}).get(dep, f"Install {dep}")
                logger.warning(f"  - {dep}: {install_instruction}")
        
        if self.outdated_deps:
            logger.warning(f"{Colors.YELLOW}Outdated dependencies ({len(self.outdated_deps)}):{Colors.RESET}")
            for dep, version, required in self.outdated_deps:
                logger.warning(f"  - {dep}: Found version {version}, but {required} or newer is required")
                
        if self.optional_missing:
            logger.info(f"{Colors.BLUE}Missing optional dependencies ({len(self.optional_missing)}):{Colors.RESET}")
            for dep in self.optional_missing:
                platform_key = self.platform_key if self.platform_key in INSTALL_INSTRUCTIONS else 'linux'
                install_instruction = INSTALL_INSTRUCTIONS.get(platform_key, {}).get(dep, f"Install {dep}")
                logger.info(f"  - {dep}: {install_instruction}")
                
        # Provide platform-specific advice
        if self.platform == 'linux':
            logger.info(f"\n{Colors.BLUE}Quick Fix:{Colors.RESET}")
            logger.info("Run the OpenCog dependency installer script:")
            logger.info("  sudo ./install-dependencies.sh")
        elif self.platform == 'win32':
            logger.info(f"\n{Colors.BLUE}Quick Fix:{Colors.RESET}")
            logger.info("Run the OpenCog dependency installer script (as Administrator):")
            logger.info("  .\\install-dependencies.ps1")
        elif self.platform == 'darwin':
            logger.info(f"\n{Colors.BLUE}Quick Fix:{Colors.RESET}")
            logger.info("Install dependencies using Homebrew:")
            logger.info("  brew install cmake boost cxxtest guile python3 rocksdb postgresql")
            logger.info("  pip3 install cython pytest numpy scipy")

def main():
    """Main function to run the dependency check."""
    print(f"""
=======================================================
 OpenCog Dependency Verification Tool
=======================================================
""")
    
    # Parse command line arguments
    import argparse
    parser = argparse.ArgumentParser(description='OpenCog Dependency Verification Tool')
    parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose output')
    parser.add_argument('--json', action='store_true', help='Output results in JSON format')
    args = parser.parse_args()
    
    # Set logging level based on verbosity
    if args.verbose:
        logger.setLevel(logging.DEBUG)
    
    # Run dependency checks
    checker = DependencyCheck()
    checker.run_checks()
    
    # Output results
    if args.json:
        results = {
            'platform': checker.platform,
            'missing_dependencies': checker.missing_deps,
            'outdated_dependencies': [{'name': dep, 'found': ver, 'required': req} for dep, ver, req in checker.outdated_deps],
            'missing_optional': checker.optional_missing,
            'issues': checker.issues,
            'warnings': checker.warnings,
            'all_checked': checker.all_deps + DEPENDENCIES['optional']
        }
        print(json.dumps(results, indent=2))
    else:
        checker.print_summary()
    
    # Return exit code based on issues found
    if checker.issues:
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(main()) 