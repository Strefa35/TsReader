# TsReader Documentation

Welcome to the TsReader documentation! This directory contains comprehensive guides for understanding, building, using, and developing the TsReader application.

## ⚠️ Important Note on Standards Support

TsReader is a **MPEG-2 Transport Stream packet viewer and analyzer** with full support for PAT (Program Association Table) parsing. While infrastructure exists for other TS table types (NIT, PMT), they are not fully implemented. Support for ATSC, SCTE, ISDB, and other standards is not available.

See [Standards Supported](ARCHITECTURE.md#standards-support) section for details.

## Quick Navigation

### 📖 For Everyone
- **[OVERVIEW.md](OVERVIEW.md)** - Start here! Project overview, features, and purpose
- **[USER_GUIDE.md](USER_GUIDE.md)** - How to use TsReader (opening files, viewing data, logging)

### 🏗️ For Developers
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - System design, data flow, and design patterns
- **[COMPONENTS.md](COMPONENTS.md)** - Detailed description of each component
- **[DEVELOPMENT.md](DEVELOPMENT.md)** - Development setup, coding standards, extending features
- **[BUILD.md](BUILD.md)** - Build instructions for all platforms

## Documentation Structure

### OVERVIEW.md
**Purpose**: High-level project understanding

Contains:
- What TsReader is and why it exists
- Key features overview
- Technology stack
- Project structure
- Getting started references

**Read this if**: You want to understand what TsReader does

### ARCHITECTURE.md
**Purpose**: Understanding the system design

Contains:
- High-level architecture diagram
- Component layers (presentation, application, business logic, utilities)
- Data flow diagrams
- Design patterns used
- Threading model
- Packet structure explanation
- Extension points

**Read this if**: You want to understand how TsReader works internally

### COMPONENTS.md
**Purpose**: Detailed component documentation

Contains:
- Detailed description of each component:
  - TsReaderApp (application entry point)
  - TsReaderFrame (main GUI window)
  - TsFile (core parser)
  - TsLogFrame (debug logging)
  - TsThread (worker thread)
  - TsMsg (data structures)
  - TsDbg (debug system)
- Key structures and methods
- Responsibilities and relationships

**Read this if**: You need detailed information about a specific component

### BUILD.md
**Purpose**: Building and installation instructions

Contains:
- Docker-first build workflow (Ubuntu 24.04 + wxWidgets 3.3.2)
- Docker Compose workflow for reproducible builds
- Automatic fallback to `build-user/` when `build/` is not writable
- Prerequisites and dependencies
- Installation instructions for each platform
- Build steps (configure, compile, run)
- CMake options
- Troubleshooting
- Platform-specific notes
- CI/CD setup

**Read this if**: You want to build TsReader from source

### USER_GUIDE.md
**Purpose**: How to use TsReader

Contains:
- Running the application
- Opening files and viewing results
- Menu options and interface elements
- Debug logging usage
- Tips and tricks
- Troubleshooting
- Transport stream structure explanation
- Performance considerations
- Keyboard shortcuts

**Read this if**: You want to use TsReader to analyze files

### DEVELOPMENT.md
**Purpose**: Development and extension guide

Contains:
- Development environment setup
- Code structure and standards
- Naming conventions and coding style
- Adding new features (parsers, UI windows)
- Testing strategies
- Common development tasks
- Debugging techniques
- Version control guidelines
- Contributing guidelines

**Read this if**: You want to develop or extend TsReader

## File Types in Project

### Header Files (.hpp)
Located in `include/` directory. Define interfaces and classes.

### Implementation Files (.cpp)
Located in `src/` directory. Contain implementations.

### Documentation Files (.md)
Located in `docs/` directory (this folder). Markdown formatted guides.

### Build Files
- `CMakeLists.txt` - CMake build configuration
- `TsReader.cbp` - Code::Blocks project file
- `options.dbg` - Debug configuration

### Container Build Files
- `docker/Dockerfile` - Ubuntu 24.04 + wxWidgets 3.3.2 environment
- `docker/docker-compose.yml` - Reproducible build and shell workflows

## Quick Start Paths

### "I want to use TsReader"
1. Read [OVERVIEW.md](OVERVIEW.md) to understand what it does
2. Read [BUILD.md](BUILD.md) to compile it (Docker recommended)
3. Read [USER_GUIDE.md](USER_GUIDE.md) to learn how to use it

### "I want to understand the code"
1. Read [OVERVIEW.md](OVERVIEW.md) for context
2. Read [ARCHITECTURE.md](ARCHITECTURE.md) to understand design
3. Read [COMPONENTS.md](COMPONENTS.md) for component details
4. Browse the source code files in `src/` and `include/`

### "I want to develop/extend TsReader"
1. Read [OVERVIEW.md](OVERVIEW.md) for context
2. Read [BUILD.md](BUILD.md) to set up development environment
3. Read [ARCHITECTURE.md](ARCHITECTURE.md) to understand design
4. Read [DEVELOPMENT.md](DEVELOPMENT.md) for coding standards and guidelines
5. Read [COMPONENTS.md](COMPONENTS.md) for specific component details

### "I want to fix a bug"
1. Run [BUILD.md](BUILD.md) debug build instructions
2. Use [DEVELOPMENT.md](DEVELOPMENT.md) debugging section
3. Reference [COMPONENTS.md](COMPONENTS.md) for affected component

### "I want to contribute to development"
1. Read [IMPLEMENTATION_ROADMAP.md](IMPLEMENTATION_ROADMAP.md) for current priorities
2. Check the detailed checklist for PMT/CAT implementation
3. Follow [DEVELOPMENT.md](DEVELOPMENT.md) coding standards

## Implementation Status & Roadmap

- **[IMPLEMENTATION_ROADMAP.md](IMPLEMENTATION_ROADMAP.md)** - Detailed checklist for PMT/CAT support (Phase 1 infrastructure, Phase 2-3 implementations, Phase 4 testing)

## Key Concepts

### Transport Stream (TS)
Binary container format for MPEG-2 video/audio. Used in digital television, Blu-ray, and video files.

### Packet
Fixed-size (188 bytes) unit containing a sync byte, header information, and payload.

### PID (Packet ID)
13-bit identifier distinguishing different data streams within a transport stream.

### PAT (Program Association Table)
Table that lists available programs and their associated PMT locations.

### PMT (Program Map Table)
Table describing a program's elementary streams (video, audio, subtitles).

### NIT (Network Information Table)
Table containing network-level information.

## Related Resources

### External
- wxWidgets Documentation: https://docs.wxwidgets.org/
- MPEG-2 TS Standard: ISO/IEC 13818-1
- CMake Documentation: https://cmake.org/

### Internal
- Project README: [../README.md](../README.md)
- Project License: [../LICENSE](../LICENSE)

## Document Versions

| Document | Last Updated | Version |
|----------|-------------|---------|
| OVERVIEW.md | 2026-04-24 | 1.0 |
| ARCHITECTURE.md | 2026-04-24 | 1.0 |
| COMPONENTS.md | 2026-04-24 | 1.0 |
| BUILD.md | 2026-04-24 | 1.2 |
| USER_GUIDE.md | 2026-04-24 | 1.0 |
| DEVELOPMENT.md | 2026-04-24 | 1.0 |
| IMPLEMENTATION_ROADMAP.md | 2026-04-24 | 1.0 |

## Getting Help

### Found an Issue in Documentation?
- Check if newer version exists
- Report specific section and suggested improvement
- Include example of confusion or error

### Still Have Questions?
1. Check [BUILD.md](BUILD.md) troubleshooting section
2. Check [USER_GUIDE.md](USER_GUIDE.md) troubleshooting section
3. Review [DEVELOPMENT.md](DEVELOPMENT.md) debugging section
4. Examine source code files referenced in documentation

## Contributing to Documentation

When adding documentation:
- Keep it clear and concise
- Use markdown formatting properly
- Include code examples where helpful
- Update this index with new documents
- Update version date when modifying

## Navigation Map

```
docs/
├── README.md (this file)
│   └── Quick links to all documentation
├── OVERVIEW.md
│   └── Explains what TsReader is
├── ARCHITECTURE.md
│   └── Explains how TsReader works
├── COMPONENTS.md
│   └── Explains each component in detail
├── BUILD.md
│   └── How to build TsReader
├── USER_GUIDE.md
│   └── How to use TsReader
├── IMPLEMENTATION_ROADMAP.md
│   └── Planned PMT/CAT implementation steps
└── DEVELOPMENT.md
    └── How to develop TsReader
```

---

**Last Updated**: April 24, 2026
**Documentation Version**: 1.1
**Project**: TsReader - Transport Stream Analysis Tool
