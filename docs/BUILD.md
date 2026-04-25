# TsReader Build Guide

## Quick Start with Docker (Recommended)

The easiest way to build TsReader is using the provided Docker environment, which includes Ubuntu 24.04 with wxWidgets 3.3.2 pre-compiled from source. No local dependencies required except Docker.

Status: validated on 2026-04-24. Image `tsreader-builder:wx332` builds successfully and compiles TsReader.

### Prerequisites
- **Docker** 20.10+
- **Docker Compose** v2 (included in Docker Desktop / `docker compose` plugin)

### Build with Docker Compose

```bash
# 1. Build the Docker image (first time only, takes ~10-15 min)
docker compose -f docker/docker-compose.yml build

# 2. Compile TsReader (output goes to build/)
docker compose -f docker/docker-compose.yml run --rm build

# 3. The binary is available at:
ls build/TsReader
```

### Interactive Shell in Container

```bash
docker compose -f docker/docker-compose.yml run --rm shell
# Inside container:
mkdir -p build && cd build && cmake .. && make -j$(nproc)
```

### Build Image Manually (without Compose)

```bash
docker build -f docker/Dockerfile -t tsreader-builder:wx332 .
docker run --rm -v "$PWD":/workspace tsreader-builder:wx332 \
    sh -c "cd /workspace && mkdir -p build && cd build && cmake .. && make -j\$(nproc)"
```

### Docker Files
- [docker/Dockerfile](../docker/Dockerfile) — Ubuntu 24.04 + wxWidgets 3.3.2 from source
- [docker/docker-compose.yml](../docker/docker-compose.yml) — Compose services (`build`, `shell`)

---

## Native Build (Manual Setup)

### Prerequisites

Before building TsReader natively, ensure you have the following installed:

#### Required
- **CMake** 3.10 or higher
- **C++ Compiler** supporting C++17
  - GCC 7.0+
  - Clang 5.0+
  - MSVC 2017+
- **wxWidgets** 3.0 or higher (development libraries)

#### Optional
- **Git** (for version control)
- **Make** or **Ninja** (build tool)
- **Code::Blocks** (IDE support via .cbp file)

## Installation of Dependencies (Native)

### Ubuntu/Debian
```bash
# Install compiler and build tools
sudo apt-get install build-essential cmake

# Install wxWidgets development libraries
sudo apt-get install libwxgtk3.2-dev
```

Note: package names differ across distro versions. On older Ubuntu releases, `libwxgtk3.0-gtk3-dev` may still be valid.

### Fedora/RHEL
```bash
# Install compiler and build tools
sudo dnf install gcc gcc-c++ cmake

# Install wxWidgets development libraries
sudo dnf install wxGTK3-devel
```

### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake
brew install cmake

# Install wxWidgets
brew install wxwidgets
```

### Windows
1. Download and install CMake from https://cmake.org/download/
2. Download and install Visual Studio Build Tools or MinGW
3. Download and build wxWidgets:
   - https://github.com/wxWidgets/wxWidgets/releases
   - Follow platform-specific build instructions

## Building TsReader (Native)

### Step 1: Clone/Navigate to Repository
```bash
cd /path/to/TsReader
```

### Step 2: Create Build Directory
```bash
mkdir build
cd build
```

### Step 3: Configure with CMake
```bash
# Standard configuration
cmake ..

# Or with specific compiler/options
cmake -DCMAKE_BUILD_TYPE=Release ..
```

#### CMake Configuration Options
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..

# With specific compiler
cmake -DCMAKE_CXX_COMPILER=g++-11 ..

# Specify wxWidgets path if CMake can't find it
cmake -DwxWidgets_CONFIG_EXECUTABLE=/usr/bin/wx-config ..
```

### Step 4: Build
```bash
# Using Make (Unix/Linux/macOS)
make

# Using Ninja
ninja

# Using Visual Studio (Windows)
cmake --build . --config Release

# With multiple threads
make -j$(nproc)
```

### Step 5: Run
```bash
# From build directory
./TsReader

# Or install first (optional)
sudo make install
./TsReader
```

## Build Verification

After successful build, verify the executable:

```bash
# Check if executable exists
ls -la TsReader

# Check if executable is linked correctly
ldd TsReader  # Linux
otool -L TsReader  # macOS
```

Expected output should show wxWidgets libraries linked.

## Compilation Flags

TsReader is compiled with strict compiler flags:

```bash
-Wextra          # Enable extra warnings
-Werror          # Treat warnings as errors
-Wformat=2       # Enable format string checking
```

These flags ensure code quality and catch potential issues early.

## CMakeLists.txt Overview

The build is configured in [CMakeLists.txt](../CMakeLists.txt):

- **Project Name**: TsReader
- **Language**: CXX (C++)
- **Standard**: C++17
- **Type**: Executable application
- **Dependencies**: wxWidgets

### Key Build Targets
```
TsReader          # Main executable
```

## Build Troubleshooting

### wxWidgets Not Found
```bash
# Solution 1: Install wxWidgets development package
sudo apt-get install libwxgtk3.2-dev  # Ubuntu/Debian (24.04+)

# Solution 2: Specify wx-config path
cmake -DwxWidgets_CONFIG_EXECUTABLE=/usr/bin/wx-config ..

# Solution 3: Set wxWidgets_ROOT_DIR
cmake -DwxWidgets_ROOT_DIR=/usr/include/wx-3.0 ..
```

### Build Fails on wxWidgets 3.3+ API Changes

If you use wxWidgets 3.3+, watch for API updates in logging structures.

Example symptom:
- `wxLogRecordInfo` has `timestampMS` instead of `timestamp`

Recommended fix pattern:
- Convert milliseconds to `time_t` before constructing `wxDateTime`

```cpp
wxDateTime(static_cast<time_t>(info.timestampMS / 1000))
```

This repository already includes a compatibility fix in `TsLogFrame::DoLogRecord()`.

### Compiler Version Issues
```bash
# Check C++ compiler version
g++ --version

# Update compiler if needed
sudo apt-get install g++-11  # Ubuntu/Debian

# Specify compiler during build
cmake -DCMAKE_CXX_COMPILER=g++-11 ..
```

### CMake Version Too Old
```bash
# Ubuntu/Debian - Install newer CMake
sudo apt-get install cmake

# Or download latest from https://cmake.org/download/
```

### Build Fails with Compilation Errors
```bash
# Check full error output
make VERBOSE=1

# Or with CMake
cmake --build . --verbose
```

## Clean Build

To perform a clean rebuild:

```bash
# Remove build directory
rm -rf build

# Recreate and rebuild
mkdir build
cd build
cmake ..
make
```

## Code::Blocks IDE Build

If using Code::Blocks IDE:

1. Open `TsReader.cbp` in Code::Blocks
2. Ensure wxWidgets is configured in Settings → Compiler
3. Select Build → Build project
4. Or press Ctrl+F9

## Development Build vs Release Build

### Development (Debug) Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
# Enables:
# - Debug symbols
# - Logging output
# - Slower execution
# - Better debugging with GDB
```

### Production (Release) Build
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make
# Enables:
# - Optimizations (-O3)
# - Smaller executable
# - Faster execution
# - Debug symbols stripped
```

## Running Tests (if applicable)

Check if project includes tests:

```bash
# List available tests
ctest --print-labels

# Run all tests
ctest

# Run with verbose output
ctest -V

# Run specific test
ctest -R test_name
```

## Installation

To install TsReader system-wide:

```bash
cd build
sudo make install

# Usually installs to:
# /usr/local/bin/TsReader
# Or check CMAKE_INSTALL_PREFIX
```

To uninstall:
```bash
sudo make uninstall
```

## Platform-Specific Notes

### Linux/macOS
- Standard CMake/Make workflow applies
- Ensure wxWidgets development libraries installed
- May need to set LD_LIBRARY_PATH if wxWidgets in non-standard location

### Windows
- Use Visual Studio generator or MinGW
- Command paths may differ (use backslashes or forward slashes as appropriate)
- Some paths may require quotes if containing spaces

### macOS Specifics
- wxWidgets may be installed via Homebrew in non-standard location
- Might need to set CMAKE_PREFIX_PATH:
  ```bash
  cmake -DCMAKE_PREFIX_PATH=/usr/local/opt/wxwidgets ..
  ```

## Continuous Integration

For CI/CD pipelines, use:

```bash
#!/bin/bash
set -e  # Exit on error

cd /path/to/TsReader
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
# Add tests if available: ctest
```

## Additional Resources

- wxWidgets Documentation: https://docs.wxwidgets.org/
- CMake Documentation: https://cmake.org/cmake/help/documentation.html
- C++17 Standard Reference: https://en.cppreference.com/w/cpp/17
