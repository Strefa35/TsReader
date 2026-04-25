# TsReader

A desktop application for reading, parsing, and analyzing **MPEG-2 Transport Stream** files with focus on packet-level inspection and Program Association Table (PAT) parsing.

## Features

- **Transport Stream File Analysis**: Open and inspect `.ts`, `.m2ts`, and other TS container files
- **Packet Visualization**: Display TS packets in a hierarchical tree view organized by PID (Packet ID)
- **Program Association Table (PAT)**: Full parsing support for program mapping
- **Multi-threaded Processing**: Asynchronous file parsing keeps UI responsive
- **Debug Logging**: Real-time debug output window with configurable logging
- **Progress Tracking**: Visual progress indication during file parsing

## Supported Standards

| Standard | Status | Details |
|----------|--------|---------|
| **MPEG-2** (ISO/IEC 13818-1) | ✅ Full | Core Transport Stream structure, 188-byte packets |
| **Program Association Table (PAT)** | ✅ Full | PID 0, program mapping |
| **DVB** (ETSI EN 300 468) | ⚠️ Partial | Infrastructure for NIT, PMT (parser stubs) |
| **ATSC** (USA) | ❌ None | Not implemented |
| **SCTE-35** (Splice Information) | ❌ None | Not implemented |
| **ISDB** (Japan) | ❌ None | Not implemented |

**Legend**: ✅ Full support | ⚠️ Partial/Incomplete | ❌ Not implemented

## Quick Start

### Requirements
- Docker 20.10+ (recommended)
- Docker Compose v2 (`docker compose`)

### Building (Recommended: Docker)

```bash
# 0. Create local env for user/group mapping inside container
printf "LOCAL_UID=%s\nLOCAL_GID=%s\n" "$(id -u)" "$(id -g)" > .env.local

# 1. Build image
docker compose --env-file .env.local -f docker/docker-compose.yml build

# 2. Compile TsReader
docker compose --env-file .env.local -f docker/docker-compose.yml run --rm build

# 3. Binary location
# Default: build/TsReader
# Fallback (if build/ is not writable): build-user/TsReader
ls build/TsReader || ls build-user/TsReader
```

This environment uses Ubuntu 24.04 and wxWidgets 3.3.2 built from source.

If you previously ran container builds as root, `build/` may become non-writable for your user. Current compose config automatically falls back to `build-user/` in that case.

### Building (Native)

```bash
mkdir build
cd build
cmake ..
make
./TsReader
```

For detailed build instructions, see [BUILD.md](docs/BUILD.md).

## Documentation

Complete documentation is available in the [docs/](docs/) directory:

### 📖 For Users
- **[USER_GUIDE.md](docs/USER_GUIDE.md)** - How to use TsReader, menu options, troubleshooting
- **[OVERVIEW.md](docs/OVERVIEW.md)** - Project overview and feature summary

### 🏗️ For Developers
- **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** - System design, data flow, design patterns, standards support
- **[COMPONENTS.md](docs/COMPONENTS.md)** - Detailed component descriptions and responsibilities
- **[DEVELOPMENT.md](docs/DEVELOPMENT.md)** - Development setup, coding standards, extending features
- **[BUILD.md](docs/BUILD.md)** - Build instructions for all platforms (Linux, macOS, Windows)

### 🚀 Implementation & Roadmap
- **[IMPLEMENTATION_ROADMAP.md](docs/IMPLEMENTATION_ROADMAP.md)** - Step-by-step checklist for PMT/CAT implementation (18-22 hours work)

### 📚 Documentation Index
- **[docs/README.md](docs/README.md)** - Documentation index and navigation

## Project Structure

```
TsReader/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── LICENSE                     # Project license
├── docker/                     # Reproducible build environment
│   ├── Dockerfile             # Ubuntu 24.04 + wxWidgets 3.3.2
│   └── docker-compose.yml     # Build and shell services
├── docs/                       # Complete documentation
│   ├── README.md              # Documentation index
│   ├── OVERVIEW.md            # Project overview
│   ├── ARCHITECTURE.md        # System architecture
│   ├── COMPONENTS.md          # Component documentation
│   ├── BUILD.md               # Build instructions
│   ├── USER_GUIDE.md          # User manual
│   └── DEVELOPMENT.md         # Developer guide
├── include/                    # Header files
│   ├── TsReaderApp.hpp        # Application entry point
│   ├── TsReaderFrame.hpp      # Main GUI window
│   ├── TsFile.hpp             # Core parser engine
│   ├── TsLogFrame.hpp         # Debug logging window
│   ├── TsMsg.hpp              # Data structures
│   ├── TsThread.hpp           # Threading support
│   └── TsDbg.hpp              # Debug utilities
└── src/                        # Implementation files
    ├── TsReaderApp.cpp
    ├── TsReaderFrame.cpp
    ├── TsFile.cpp
    ├── TsLogFrame.cpp
    ├── TsMsg.cpp
    ├── TsThread.cpp
    └── TsDbg.cpp
```

## Technology Stack

- **Language**: C++17
- **GUI Framework**: wxWidgets 3.0+ (validated build with wxWidgets 3.3.2)
- **Build System**: CMake 3.10+
- **Standards**: MPEG-2 (ISO/IEC 13818-1)
- **Compilation Flags**: `-Wextra -Werror -Wformat=2`

## What is a Transport Stream?

A Transport Stream (TS) is a standard format defined in MPEG-2 (ISO/IEC 13818-1) for multiplexing video, audio, and data. Used in:
- Digital terrestrial/cable/satellite television
- Blu-ray Disc content
- MPEG-2 video files (.ts, .m2ts, .m2t, .mts extensions)
- Some streaming protocols

Each TS consists of fixed-size packets (188 bytes each) identified by a Packet ID (PID). The Program Association Table (PAT) acts as an index describing what content is available in the stream.

## Architecture Overview

TsReader follows a **Model-View** architecture with:

- **Presentation Layer**: Main window (TsReaderFrame) and debug log window (TsLogFrame)
- **Application Layer**: wxWidgets application management (TsReaderApp)
- **Business Logic Layer**: Core parsing engine (TsFile) and data structures (TsMsg)
- **Utilities**: Threading support (TsThread) and debug system (TsDbg)

For detailed architecture information, see [ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Usage

### Opening a File

1. Launch TsReader
2. Select **File → Open** (or Ctrl+O)
3. Choose a Transport Stream file
4. Parsing begins automatically
5. Results appear in the tree view

### Viewing Results

Packets are organized by PID:
- **PID 0**: Program Association Table (main index)
- **PID 1**: Conditional Access Table (encryption info)
- **PID 16**: Network Information Table
- **User PIDs** (32-8190): Video, audio, data streams

For more details, see [USER_GUIDE.md](docs/USER_GUIDE.md).

## Development

To contribute or extend TsReader:

1. See [DEVELOPMENT.md](docs/DEVELOPMENT.md) for setup and coding standards
2. Review [ARCHITECTURE.md](docs/ARCHITECTURE.md) for design patterns
3. Check [COMPONENTS.md](docs/COMPONENTS.md) for component details

### Building for Development

```bash
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

This enables debug symbols and debug output for development.

## Implementation Status

**Current Focus**: MPEG-2 Transport Stream packet analysis with complete PAT support

- ✅ Fully Implemented
  - MPEG-2 TS packet structure parsing
  - Program Association Table (PAT) parsing
  - Packet grouping by PID
  - Tree-based visualization
  - Debug logging system

- ⚠️ Partially Implemented
  - Network Information Table (NIT) - parser stub
  - Program Map Table (PMT) - parser stub
  - Infrastructure for DVB tables

- ❌ Not Implemented
  - ATSC (USA digital television)
  - SCTE-35 (splice information)
  - ISDB (Japanese television)
  - Full DVB table support

See [ARCHITECTURE.md](docs/ARCHITECTURE.md#standards-support) for detailed standards support information.

## License

See [LICENSE](LICENSE) file for project license information.

## Getting Help

- **Usage Questions**: See [USER_GUIDE.md](docs/USER_GUIDE.md)
- **Build Issues**: See [BUILD.md](docs/BUILD.md) troubleshooting section
- **Development**: See [DEVELOPMENT.md](docs/DEVELOPMENT.md)
- **Architecture Questions**: See [ARCHITECTURE.md](docs/ARCHITECTURE.md)
- **Component Details**: See [COMPONENTS.md](docs/COMPONENTS.md)

## Credits

TsReader is a C++/wxWidgets application for analyzing MPEG-2 Transport Streams.
