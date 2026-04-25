# TsReader - Project Overview

## What is TsReader?

TsReader is a desktop application designed to read, parse, and analyze **Transport Stream (TS)** files, which are binary containers used in the MPEG-2 standard for video and audio transmission. The application provides a graphical user interface built with **wxWidgets** to visualize the structure of TS packets in a hierarchical tree view.

## Purpose

The primary goal of TsReader is to enable developers, testers, and engineers to:
- Open and inspect MPEG-2 Transport Stream files
- Visualize the packet structure with detailed information
- Debug and analyze packet IDs (PIDs) and their associated data
- Parse and display Transport Stream sections (focus on Program Association Table)
- Monitor parsing progress with real-time feedback
- View detailed debugging logs in a dedicated window

## Key Features

- **File Parsing**: Reads Transport Stream files and extracts individual 188-byte packets
- **Tree Visualization**: Displays parsed data in a hierarchical tree structure grouped by PID
- **Section Parsing**: 
  - **PAT** (Program Association Table) - fully implemented, maps programs to their PMT PIDs
  - Infrastructure for NIT and PMT (parser stubs, not fully implemented)
- **Progress Tracking**: Visual progress bar during file parsing
- **Debug Logging**: Separate logging window with configurable debug levels
- **Multi-threaded Processing**: Asynchronous file parsing to keep UI responsive

## Technology Stack

- **Language**: C++17
- **GUI Framework**: wxWidgets
- **Build System**: CMake
- **Application Type**: Desktop Executable
- **Compilation Standards**: `-Wextra -Werror -Wformat=2`

## Project Structure

```
TsReader/
├── CMakeLists.txt          # Build configuration
├── README.md               # Project readme
├── LICENSE                 # License information
├── TsReader.cbp            # Code::Blocks project file
├── options.dbg             # Debug options
├── docs/                   # Documentation (this directory)
├── include/                # Header files
│   ├── TsReaderApp.hpp     # Main application class
│   ├── TsReaderFrame.hpp   # Main window frame
│   ├── TsFile.hpp          # Core file parsing logic
│   ├── TsLogFrame.hpp      # Debug logging window
│   ├── TsMsg.hpp           # Message/packet structures
│   ├── TsThread.hpp        # Threading implementation
│   └── TsDbg.hpp           # Debug macros and utilities
└── src/                    # Implementation files
    ├── TsReaderApp.cpp
    ├── TsReaderFrame.cpp
    ├── TsFile.cpp
    ├── TsLogFrame.cpp
    ├── TsMsg.cpp
    ├── TsThread.cpp
    └── TsDbg.cpp
```

## Getting Started

- See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed system design
- See [COMPONENTS.md](COMPONENTS.md) for component descriptions
- See [BUILD.md](BUILD.md) for build and compilation instructions

## Development Notes

This project is built with a focus on:
- **Clean code**: Strict compiler warnings treated as errors
- **Separation of concerns**: Clear division between UI, parsing logic, and utilities
- **Responsive UI**: Asynchronous processing keeps the interface responsive during parsing
- **Comprehensive logging**: Debug capabilities for troubleshooting
