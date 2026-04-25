# TsReader Development Guide

## For Developers

This guide covers extending and maintaining the TsReader project.

---

## Development Environment Setup

### Prerequisites
- See [BUILD.md](BUILD.md) for compiler and build tool requirements
- IDE recommended: Code::Blocks, Visual Studio, or CLion
- Git for version control

### IDE Setup

#### Code::Blocks
1. Open `TsReader.cbp`
2. Settings → Compiler → Select compiler (GCC/Clang/MSVC)
3. Ensure wxWidgets is configured in compiler settings
4. Build → Build project (F9)

#### Visual Studio
1. Install CMake support extension
2. Open folder containing TsReader
3. CMake will configure automatically
4. Build → Build All

#### CLion
1. File → Open → Select TsReader directory
2. CLion loads CMakeLists.txt automatically
3. Build → Build Project (Ctrl+F9)

---

## Code Structure

### Directory Layout
```
TsReader/
├── CMakeLists.txt          # Build configuration
├── include/                # Header files (.hpp)
│   ├── TsReaderApp.hpp     # Application class
│   ├── TsReaderFrame.hpp   # Main GUI window
│   ├── TsFile.hpp          # File parsing engine
│   ├── TsLogFrame.hpp      # Logger window
│   ├── TsMsg.hpp           # Data structures
│   ├── TsThread.hpp        # Threading
│   └── TsDbg.hpp           # Debug macros
├── src/                    # Implementation files (.cpp)
│   ├── TsReaderApp.cpp
│   ├── TsReaderFrame.cpp
│   ├── TsFile.cpp
│   ├── TsLogFrame.cpp
│   ├── TsMsg.cpp
│   ├── TsThread.cpp
│   └── TsDbg.cpp
├── docs/                   # Documentation
└── options.dbg             # Debug configuration
```

---

## Coding Standards

### C++ Standards
- **Standard**: C++17
- **Compiler Flags**: `-Wextra -Werror -Wformat=2`
- Treat all warnings as errors

### Naming Conventions

#### Classes
- Use PascalCase: `TsReaderFrame`, `TsFileParser`
- Prefix with class-type indicator: `TsFrame`, `TsThread`

#### Methods
- Use camelCase: `parseFile()`, `onOpenFile()`
- Event handlers: `On*` prefix (wxWidgets convention)
- Boolean getters: `is*()` or `has*()`

#### Variables
- Local variables: `camelCase`
- Member variables: `m_` prefix + camelCase (e.g., `m_filePath`)
- Constants: `UPPER_CASE` or `kCamelCase`
- Static variables: `s_` prefix

#### Files
- Class files: `ClassName.hpp`, `ClassName.cpp`
- No space in names; use PascalCase

### Code Style

#### Formatting
```cpp
// Braces on same line
if (condition) {
    // code
} else {
    // code
}

// Function formatting
void MyClass::myMethod() {
    // implementation
}

// Indentation: 4 spaces or 1 tab (be consistent)
```

#### Comments
```cpp
// Single-line comments for brief explanations
/* Multi-line comments for
   longer explanations spanning
   multiple lines */

/// Doxygen-style comments for documentation
/// @param file The file path to parse
/// @return Status code: 0 on success, -1 on failure
```

#### Header Guards
```cpp
#ifndef TS_READER_FRAME_HPP
#define TS_READER_FRAME_HPP

// Content...

#endif // TS_READER_FRAME_HPP
```

### wxWidgets Conventions

- Event handlers: `On` prefix + action (e.g., `OnOpenFile`)
- Event table declarations
- Use `wxASSERT()` for precondition checks
- Resource management: `wxSafeDelete()` for cleanup

---

## Adding New Features

### Adding a New Parser Section (e.g., SDT)

1. **Define Structure in TsMsg.hpp**:
```cpp
struct sdt_section_t {
    uint8_t table_id;
    uint16_t transport_stream_id;
    vector<service_t> services;
    // Add fields as needed
};
```

2. **Implement Parser in TsFile.cpp**:
```cpp
void TsFile::parseSDT(const uint8_t* payload) {
    // Extract fields from payload
    // Parse service descriptors
    // Store results
    DBG_INFO("SDT parsed successfully");
}
```

3. **Update parseSection() in TsFile.cpp**:
```cpp
case PID_SDT:  // PID 17
    parseSDT(payload);
    break;
```

4. **Update Display in TsReaderFrame.cpp**:
```cpp
// Add tree view nodes for SDT data
wxTreeItemId sdtNode = m_tree->AppendItem(root, "PID 17 (SDT)");
// Add service information...
```

### Adding a New UI Window

1. **Create New Frame Class**:
```cpp
// NewWindow.hpp
class TsNewWindow : public wxFrame {
public:
    TsNewWindow(wxWindow* parent);
    ~TsNewWindow();
private:
    // UI elements
    wxTextCtrl* m_textCtrl;
};
```

2. **Implement in NewWindow.cpp**:
```cpp
TsNewWindow::TsNewWindow(wxWindow* parent) 
    : wxFrame(parent, wxID_ANY, "New Window") {
    // Initialize UI
}
```

3. **Integrate with TsReaderFrame**:
```cpp
// In TsReaderFrame.hpp
TsNewWindow* m_newWindow;

// In TsReaderFrame.cpp
void TsReaderFrame::OnShowNewWindow() {
    if (!m_newWindow) {
        m_newWindow = new TsNewWindow(this);
    }
    m_newWindow->Show();
}
```

### Adding Debug Output

Use TsDbg macros:

```cpp
// In any component
#include "TsDbg.hpp"

// Error level (always logged)
DBG_ERROR("Critical error: " + errorMsg);

// Warning level
DBG_WARN("Sync byte mismatch at offset " + to_string(offset));

// Info level
DBG_INFO("File parsing started");

// Debug level
DBG_DEBUG("Packet details: PID=" + to_string(pid) + ", size=" + to_string(size));
```

---

## Testing

### Building for Testing

```bash
mkdir build-test
cd build-test
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Manual Testing Checklist

- [ ] File opens successfully
- [ ] Progress bar updates during parsing
- [ ] Tree view populates correctly
- [ ] Log window shows messages
- [ ] No memory leaks (valgrind check)
- [ ] Handles corrupted files gracefully
- [ ] UI remains responsive
- [ ] Application closes cleanly

### Testing with Valgrind (Linux)

```bash
valgrind --leak-check=full --show-leak-kinds=all ./TsReader
```

### Testing with Different File Sizes

Test with:
- Small file (< 1 MB)
- Medium file (10-100 MB)
- Large file (> 100 MB)
- Empty or corrupted file

---

## Common Development Tasks

### Modifying Parsing Logic

Files to modify: `TsFile.hpp` and `TsFile.cpp`

```cpp
// Add new parsing method
void TsFile::parseNewSection(const uint8_t* data, int length) {
    // Parse implementation
    DBG_INFO("New section parsed");
}
```

### Modifying UI Elements

Files to modify: `TsReaderFrame.hpp` and `TsReaderFrame.cpp`

```cpp
// Add new tree node
wxTreeItemId item = m_tree->AppendItem(parent, "New Item");
m_tree->SetItemData(item, new wxTreeItemData(...));
```

### Adding Menu Items

In `TsReaderFrame::TsReaderFrame()`:

```cpp
// Create menu item
wxMenuItem* newItem = fileMenu->Append(wxID_ANY, "New Option");
// Bind event
Bind(wxEVT_MENU, &TsReaderFrame::OnNewOption, this, newItem->GetId());
```

### Adding Configuration Options

Edit `options.dbg`:
```
DEBUG_LEVEL=INFO
DEBUG_OUTPUT=console
DEBUG_THREADS=yes
```

Then read in initialization:
```cpp
// In TsReaderApp::OnInit()
readDebugOptions("options.dbg");
```

---

## Performance Optimization

### Profiling
```bash
# Linux - using gprof
g++ -pg -O2 *.cpp -o TsReader
./TsReader
gprof TsReader gmon.out
```

### Memory Optimization
- Avoid unnecessary copies: use references and move semantics
- Clear containers after processing
- Use `std::unique_ptr` for automatic cleanup

### Threading Optimization
- Keep worker thread work minimal
- Batch UI updates
- Use appropriate synchronization primitives

---

## Debugging

### GDB Commands
```bash
# Build with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with GDB
gdb ./TsReader

# Common GDB commands
(gdb) run                    # Start program
(gdb) break TsFile.cpp:100   # Set breakpoint
(gdb) continue               # Continue execution
(gdb) print variable_name    # Print variable
(gdb) backtrace              # Show call stack
(gdb) next                   # Next line
(gdb) step                   # Step into
```

### Logging Strategy
Enable different debug levels:
```cpp
// In TsDbg.hpp
#define DEBUG_LEVEL_TRACE
#define DEBUG_LEVEL_DEBUG
#define DEBUG_LEVEL_INFO
```

---

## Building Documentation

Generate Doxygen documentation:

```bash
# Install Doxygen
sudo apt-get install doxygen graphviz

# Generate documentation
doxygen Doxyfile  # If Doxyfile exists
# Or create one and configure for source directory
```

---

## Version Control

### Commit Message Format
```
[COMPONENT] Brief description

Detailed explanation of changes:
- What was changed
- Why it was changed
- Any relevant notes

Fixes #issue_number (if applicable)
```

Example:
```
[TsFile] Improve PAT parsing performance

- Cache PAT results to avoid re-parsing
- Reduce memory allocation in loop
- Performance improvement: 20% faster on large files

Fixes #42
```

### Branch Naming
- Feature: `feature/description`
- Bugfix: `fix/description`
- Docs: `docs/description`

---

## Troubleshooting Development Issues

### Build Fails with Undefined References
```bash
# Solution: Ensure all .cpp files listed in CMakeLists.txt
# Check CMakeLists.txt add_executable() command
```

### wxWidgets Compiler Errors
```bash
# Solution: Rebuild wxWidgets with same compiler
# Or update wxWidgets package
```

### Thread Crashes
```cpp
// Solution: Ensure thread-safe operations
// Use wxMutex for shared data
wxMutex m_mutex;
{
    wxMutexLocker lock(m_mutex);
    // Critical section
}
```

### Memory Leaks
```bash
# Build with debug symbols and use valgrind
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
valgrind --leak-check=full ./TsReader
```

---

## Contributing Guidelines

1. **Fork** the repository
2. **Create** a feature branch
3. **Make** changes with clean commits
4. **Test** thoroughly (manual + automated)
5. **Document** changes in code comments
6. **Submit** pull request with description

## Code Review Checklist

- [ ] Follows naming conventions
- [ ] No compiler warnings
- [ ] Includes debug logging
- [ ] Memory-safe (no leaks)
- [ ] Thread-safe if applicable
- [ ] Handles error cases
- [ ] Updates documentation
- [ ] Tested on target platform
