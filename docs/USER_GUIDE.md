# TsReader User Guide

## Getting Started

### Running the Application

After building (see [BUILD.md](BUILD.md)), run TsReader:

```bash
./TsReader
```

The main window will open with an empty tree view, menu bar, and status bar.

## Basic Usage

### Opening a Transport Stream File

1. **Using File Menu**:
   - Click **File** → **Open**
   - Browse to your `.ts` or `.m2ts` file
   - Click **Open**

2. **Using Keyboard Shortcut**:
   - Press `Ctrl+O` (if implemented)

### What Happens When You Open a File

1. A file selection dialog appears
2. Once selected, parsing begins in the background
3. Progress bar shows parsing progress
4. Status bar displays current operation
5. Results appear in the tree view once complete

### Viewing Results

The tree view displays packets organized by **PID (Packet ID)**:

```
Root
├── PID 0 (PAT) - Program Association Table
│   ├── Packet 1
│   ├── Packet 2
│   └── Packet Info...
├── PID 256 (PMT) - Program Map Table
│   ├── Packet 1
│   └── Packet Info...
├── PID 512 (Video Stream)
│   ├── Packet 1
│   ├── Packet 2
│   └── ...
└── Other PIDs...
```

### Expanding/Collapsing Tree Items

- **Double-click** an item to expand/collapse
- **Click ▶/▼** arrow next to item name
- **Right-click** for context menu (if available)

## Menu Options

### File Menu
- **Open**: Open a Transport Stream file
- **Recent Files**: Quick access to recently opened files (if implemented)
- **Exit**: Close application

### View Menu
- **Logger**: Show/hide debug logging window

### Help Menu
- **About**: Display version and application information
- **Help**: Open this user guide (if implemented)

## Interface Elements

### Status Bar
Shows current status of operations:
- "Ready" - Waiting for user action
- "Parsing..." - File is being parsed
- "Parse complete - Found X PIDs" - Parsing finished
- Error messages if something goes wrong

### Progress Bar
Visual indicator of parsing progress:
- Appears during file parsing
- Shows percentage of file processed
- Disappears when parsing completes

### Tree View
Hierarchical display of parsed packet data:
- Each node represents a PID or packet
- Expandable nodes show related information
- Clickable for detailed information (if implemented)

## Debug Logging

### Opening Logger Window

1. Click **View** → **Logger**
2. A separate window opens showing debug messages
3. Messages display in real-time during parsing

### Log Information

Each log line contains:
- **Timestamp**: When the event occurred
- **Thread ID**: Which thread generated the message
- **Level**: Message severity (ERROR, WARN, INFO, DEBUG)
- **Message**: Description of the event

### Log Levels

| Level | Meaning | Color (optional) |
|-------|---------|------------------|
| ERROR | Critical error occurred | Red |
| WARN | Warning condition | Yellow |
| INFO | Informational message | Green |
| DEBUG | Debug-level information | Gray |
| TRACE | Very detailed trace info | Gray |

### Logger Controls

- **Clear**: Remove all messages from log
- **Pause**: Stop receiving new messages
- **Save**: Save log to file
- **Close**: Hide logger window (remains open if minimized)

## Tips and Tricks

### Performance

- **Large Files**: Parsing very large TS files may take time. The progress bar provides feedback.
- **Responsive UI**: The application remains responsive during parsing thanks to background thread.
- **Memory**: Very large files may consume significant memory due to packet storage.

### Common Scenarios

#### Analyzing Video Stream
1. Open `.ts` or `.m2ts` file containing video
2. Look for PID with video codec (often PID 256-300)
3. Expand PID node to see packet details
4. Check PMT (PID 16) for program structure

#### Analyzing Audio Stream
1. Open file containing audio
2. Look for PID with audio codec (MP2, AC3, AAC, etc.)
3. Usually found in PMT-referenced PIDs
4. Compare packet counts across different audio streams

#### Debugging Stream Structure
1. Open file
2. Open Logger from View menu
3. Watch debug output during parsing
4. Identify packet patterns and anomalies
5. Use log information for troubleshooting

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+O | Open file |
| Ctrl+Q | Quit application |
| Ctrl+L | Toggle Logger window |
| F5 | Refresh/Reload (if implemented) |

## Understanding Transport Stream Structure

### Standards Supported by TsReader

TsReader implements support for:

| Standard | Status | Notes |
|----------|--------|-------|
| **MPEG-2** (ISO/IEC 13818-1) | ✅ Full | Core TS packet structure, 188-byte packets |
| **Program Association Table (PAT)** | ✅ Full | PID 0, program mapping |
| **Conditional Access Table (CAT)** | ✅ Recognized | PID 1, identified but not parsed |
| **Network Information Table (NIT)** | ⚠️ Partial | PID 16, parser stub only |
| **Program Map Table (PMT)** | ⚠️ Partial | Parser stub only |
| **DVB** (ETSI EN 300 468) | ⚠️ Partial | Infrastructure for SDT, EIT, TDT recognized by PID |
| **ATSC** (USA Digital TV) | ❌ None | VCT, STT, RRT not supported |
| **SCTE** (Splice Info) | ❌ None | SCTE-35 not supported |
| **ISDB** (Japanese TV) | ❌ None | Not supported |

**✅ Full**: Completely implemented and functional  
**⚠️ Partial**: Infrastructure present but parsing incomplete  
**❌ None**: Not implemented

### PID Assignments

Standard PID allocations:

| PID | Purpose | Description |
|-----|---------|-------------|
| 0 | PAT | Program Association Table - lists available programs |
| 1 | CAT | Conditional Access Table - encryption info |
| 16 | NIT | Network Information Table - network details |
| 17-31 | Reserved | SDT, EIT, TDT, TOT, etc. |
| 8191 | Null | Filler packets |
| Others | User | Video, audio, subtitles, teletext |

### Packet Types

**PAT (Program Association Table)**
- PID 0
- Lists all programs in the stream
- Associates program numbers with PMT PIDs

**PMT (Program Map Table)**
- Specified in PAT
- Describes a single program
- Lists elementary streams (video, audio, etc.) with their PIDs

**NIT (Network Information Table)**
- PID 16
- Contains network information
- Usually less critical for basic stream analysis

## Troubleshooting

### File Won't Open

**Problem**: "Failed to open file" error
- **Solution 1**: Check file path and permissions
- **Solution 2**: Ensure file is valid TS format (.ts, .m2ts, .m2t)
- **Solution 3**: File may be corrupted; try different file

### Parsing Hangs

**Problem**: Progress bar stuck or very slow
- **Solution 1**: File may be very large; wait for completion
- **Solution 2**: Close and try again
- **Solution 3**: Check Logger for error messages

### No Data Appears in Tree

**Problem**: File opened but tree view is empty
- **Solution 1**: File may be empty or invalid format
- **Solution 2**: Try opening Logger to see error messages
- **Solution 3**: Check file is readable (permissions)

### Application Crashes

**Problem**: Application unexpectedly closes
- **Solution 1**: Check Logger output before crash
- **Solution 2**: Try with different file
- **Solution 3**: Rebuild application (see [BUILD.md](BUILD.md))
- **Solution 4**: Report issue with crash details

### High Memory Usage

**Problem**: Application uses excessive memory
- **Solution 1**: Close and reopen application
- **Solution 2**: Don't parse extremely large files all at once
- **Solution 3**: Check for resource leaks (contact developers)

## Advanced Features

### Command Line Usage (if implemented)

```bash
# Open specific file on startup
./TsReader filename.ts

# Start with debug logging
./TsReader --debug filename.ts

# Set debug level
./TsReader --debug-level verbose filename.ts
```

### Configuration Files

Debug configuration can be set in `options.dbg`:
- Debug levels
- Output formatting
- Logging verbosity

## Performance Considerations

### File Size
- **Small** (< 10 MB): Opens instantly
- **Medium** (10-100 MB): Takes seconds to minutes
- **Large** (> 100 MB): Takes minutes; may need patience

### Memory Usage
- Each packet (188 bytes) is stored in memory
- Large files with many packets consume more memory
- Typical TS stream: ~4-8 Mb/s = 40-80 MB/minute

### System Requirements

| Aspect | Minimum | Recommended |
|--------|---------|------------|
| RAM | 512 MB | 2 GB+ |
| CPU | 1 GHz | 2 GHz+ |
| Disk | 50 MB free | 500 MB+ |
| OS | Linux, macOS, Windows (with wxWidgets) |

## Getting Help

1. **Check Logger**: Most issues show messages in Logger window
2. **Check BUILD.md**: For build-related issues
3. **Check ARCHITECTURE.md**: To understand internal workings
4. **Review README.md**: General project information

## Reporting Issues

When reporting issues, include:
1. TsReader version (About dialog)
2. Operating system and version
3. File size and format (.ts, .m2ts, etc.)
4. Full error message from Logger
5. Steps to reproduce the problem

## About Transport Streams

### What is a Transport Stream?

A Transport Stream (TS) is a container format for video/audio defined in the MPEG-2 standard. It's used in:
- **Digital Television**: Broadcast, cable, satellite
- **Blu-ray**: Video disc format
- **Video Files**: `.ts` and `.m2ts` files
- **Streaming**: Some protocols use TS containerization

### Why Analyze Transport Streams?

- **Debugging**: Verify stream structure and content
- **Testing**: Validate encoder/muxer output
- **Quality Analysis**: Check for errors or corruption
- **Integration**: Understand stream composition for processing

## Further Reading

- [ARCHITECTURE.md](ARCHITECTURE.md): How TsReader works internally
- [COMPONENTS.md](COMPONENTS.md): Detailed component documentation
- [BUILD.md](BUILD.md): Build and installation instructions
- [OVERVIEW.md](OVERVIEW.md): Project overview

## Keyboard Reference

| Key | Action |
|-----|--------|
| Ctrl+O | Open File |
| Ctrl+Q / Alt+F4 | Exit |
| F1 | Help |
| Esc | Cancel operation |
| Tab | Navigate between elements |
| Enter | Confirm/Expand |
| Arrow Keys | Navigate tree |
