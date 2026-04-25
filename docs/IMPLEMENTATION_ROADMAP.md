# TsReader Implementation Roadmap

## Goal: Full MPEG-2 PAT/PMT/CAT Support

Minimal scope to achieve **complete MPEG-2 Program Association Table (PAT), Program Map Table (PMT), and Conditional Access Table (CAT) parsing** as the first major implementation milestone.

---

## Current State Summary

| Table | Status | Implementation | Location |
|-------|--------|---|---|
| **PAT** | ✅ Complete | `parsePat()` - 85 lines, fully functional | [TsReaderFrame.cpp#L275](../src/TsReaderFrame.cpp#L275) |
| **PMT** | ❌ Stub | `parsePmt()` - empty, all code commented (#if 0) | [TsReaderFrame.cpp#L380](../src/TsReaderFrame.cpp#L380) |
| **NIT** | ❌ Stub | `parseNit()` - empty, all code commented (#if 0) | [TsReaderFrame.cpp#L363](../src/TsReaderFrame.cpp#L363) |
| **CAT** | ❌ None | Not implemented, only PID recognized | PID 0x0001 |

---

## Phase 1: Infrastructure (Required for All Tables)

**Objective**: Enable section extraction and data structure support

### Task 1.1: Enable Section Extraction Pipeline

**File**: [src/TsReaderFrame.cpp](../src/TsReaderFrame.cpp)  
**Current Issue**: Section extraction code is disabled with `#if 0`

- [ ] **Uncomment section extraction code** (Lines 425-450)
  - Remove `#if 0` and corresponding `#endif`
  - Code extracts payload from packets and calls `parseSection()`
  - Requires PUSI (Payload Unit Start Indicator) handling

- [ ] **Test payload extraction**
  - Verify section size calculation (bits 1-12 of bytes 1-2)
  - Verify pointer_field extraction (byte 0)
  - Validate with PAT parsing (should work immediately)

**Expected Result**: PAT sections should start parsing automatically from packet stream

---

### Task 1.2: Fix getTsPackets() Overload for Specific PID

**File**: [src/TsFile.cpp](../src/TsFile.cpp)  
**Current Issue**: `getTsPackets(uint16_t pid)` is commented out with `#if 0`

- [ ] **Uncomment getTsPackets(pid) overload** (Line ~285)
  - Function returns vector of packets matching specific PID
  - Used by `parseNit()` and `parsePmt()`

- [ ] **Verify method signature**
  ```cpp
  std::vector<ts_packet_t>* getTsPackets(uint16_t pid);
  ```

- [ ] **Test packet retrieval**
  - Verify TsFile maintains PID-indexed packet map
  - Test retrieving packets for PAT (PID 0), CAT (PID 1)

**Expected Result**: Parser functions can retrieve consecutive packets for a specific table

---

### Task 1.3: Create Data Structures in TsMsg.hpp

**File**: [include/TsMsg.hpp](../include/TsMsg.hpp)  
**Current Issue**: File is completely empty - all parsing uses manual byte extraction

Create structures for consistent data representation:

- [ ] **Program Association Table (PAT) structure**
  ```cpp
  typedef struct {
      uint8_t table_id;               // Always 0x00
      uint16_t section_length;        // 12 bits, max 1024 bytes
      uint16_t transport_stream_id;   // 16 bits
      uint8_t version_number;         // 5 bits
      uint8_t current_next;           // 1 bit
      uint8_t section_number;         // 8 bits
      uint8_t last_section;           // 8 bits
      vector<struct {
          uint16_t program_number;    // 16 bits
          uint16_t pid;               // 13 bits (PMT PID or NIT PID for prog 0)
      }> programs;
      uint32_t crc32;                 // 32 bits
  } ts_pat_t;
  ```

- [ ] **Program Map Table (PMT) structure**
  ```cpp
  typedef struct {
      uint8_t table_id;               // Always 0x02
      uint16_t section_length;        // 12 bits
      uint16_t program_number;        // 16 bits
      uint8_t version_number;         // 5 bits
      uint8_t current_next;           // 1 bit
      uint8_t section_number;         // 8 bits
      uint8_t last_section;           // 8 bits
      uint16_t pcr_pid;               // 13 bits
      uint16_t program_info_length;   // 10 bits
      vector<uint8_t> program_descriptors;  // ISO 639 language, audio type, etc.
      
      vector<struct {
          uint8_t stream_type;        // H.264=0x1B, HEVC=0x24, MP3=0x03, AAC=0x0F
          uint16_t elementary_pid;    // 13 bits
          uint16_t es_info_length;    // 10 bits
          vector<uint8_t> es_descriptors;   // Language, subtitles, etc.
      }> elementary_streams;
      uint32_t crc32;
  } ts_pmt_t;
  ```

- [ ] **Conditional Access Table (CAT) structure**
  ```cpp
  typedef struct {
      uint8_t table_id;               // Always 0x01
      uint16_t section_length;        // 12 bits
      uint8_t version_number;         // 5 bits
      uint8_t current_next;           // 1 bit
      uint8_t section_number;         // 8 bits
      uint8_t last_section;           // 8 bits
      
      vector<struct {
          uint16_t ca_system_id;      // 16 bits (Nagravision=0x4721, etc.)
          uint16_t ca_pid;            // 13 bits
          vector<uint8_t> private_data;
      }> ca_descriptors;
      uint32_t crc32;
  } ts_cat_t;
  ```

- [ ] **Adaptation Field structure** (for future use)
  ```cpp
  typedef struct {
      uint8_t length;
      uint8_t discontinuity_indicator;
      uint8_t random_access_indicator;
      uint8_t elementary_stream_priority_indicator;
      uint8_t pcr_flag;
      uint8_t opcr_flag;
      uint8_t splicing_point_flag;
      uint8_t private_data_flag;
      uint8_t adaptation_extension_flag;
      uint64_t program_clock_reference;  // If pcr_flag
      uint64_t original_program_clock_ref;  // If opcr_flag
      vector<uint8_t> private_data;
  } ts_adaptation_field_t;
  ```

- [ ] **Helper Functions in TsMsg.cpp** (if created)
  ```cpp
  // CRC32 validation
  bool validateCRC32(const uint8_t* data, int length, uint32_t expected_crc);
  
  // Stream type descriptions
  const char* getStreamTypeDescription(uint8_t stream_type);
  
  // CA System ID descriptions
  const char* getCASystemDescription(uint16_t ca_system_id);
  ```

**Expected Result**: All three tables have well-defined structures for consistent parsing

---

### Task 1.4: Route All Table Types in parseSection()

**File**: [src/TsReaderFrame.cpp](../src/TsReaderFrame.cpp)  
**Current Issue**: Only PAT (PID 0x00) is handled, others fall through to default

- [ ] **Add cases for all MPEG-2 system PIDs** (in `parseSection()`)
  ```cpp
  switch (pid) {
      case 0x00:  // PAT
          parsePat(tree_root, sect_ptr, sect_size);
          break;
      case 0x01:  // CAT
          parseCAT(tree_root, sect_ptr, sect_size);
          break;
      case 0x10:  // NIT (system PID)
          parseNit(tree_root, sect_ptr, sect_size);
          break;
      case 0x11:  // SDT/BAT (not implemented yet)
          // parseSDT(tree_root, sect_ptr, sect_size);
          break;
      // PIDs > 0x20 are user-defined (PMT PIDs found in PAT)
      default:
          // Check if this is a PMT PID discovered in PAT
          if (isPmtPid(pid)) {
              parsePmt(tree_root, sect_ptr, sect_size);
          }
          break;
  }
  ```

- [ ] **Add isPmtPid() helper function**
  - Maintains set of PMT PIDs discovered from PAT
  - Called during PAT parsing to track program map locations
  - Used in parseSection() default case to identify PMT tables

**Expected Result**: All standard PIDs route to appropriate parser functions

---

## Phase 2: Program Map Table (PMT) Implementation

**Objective**: Parse elementary streams and program structure  
**Priority**: HIGH - enables multi-program stream analysis  
**Estimated Effort**: 2-3 hours

### Task 2.1: Implement parsePmt() Function

**File**: [src/TsReaderFrame.cpp](../src/TsReaderFrame.cpp)  
**Reference**: `parsePat()` implementation (Lines 275-360)

- [ ] **Create parsePmt() function** (~150 lines, following PAT pattern)
  ```cpp
  void TsReaderFrame::parsePmt(wxTreeItemId& tree_root, 
                               const uint8_t* sect_ptr, 
                               uint16_t sect_size)
  {
      // 1. Extract header fields
      // 2. Extract PCR PID
      // 3. Extract program descriptors
      // 4. Loop through elementary streams
      // 5. For each ES: extract stream type, PID, descriptors
      // 6. Validate CRC32
      // 7. Display in tree
  }
  ```

- [ ] **Parse PMT header** (first 8 bytes after table_id)
  - Table ID (1 byte): 0x02
  - Section syntax: bits 7-6 of byte 1 (should be 0x03)
  - Section length: bits 11-0 of bytes 1-2
  - Program number: bytes 3-4
  - Version: bits 5-1 of byte 5
  - Current/next: bit 0 of byte 5
  - Section number: byte 6
  - Last section: byte 7

- [ ] **Parse PCR PID** (3 bytes at offset 8)
  - PCR PID: bits 12-0 of bytes 8-9
  - Program info length: bits 9-0 of bytes 9-10

- [ ] **Parse program descriptors** (variable length)
  - Extract descriptor tag, length, value
  - Support ISO-639 language descriptor (tag 0x0A)
  - Support audio component descriptor (tag 0x52, DVB-only - skip for now)

- [ ] **Parse elementary streams loop**
  ```cpp
  while (offset < payload_length - 4) {  // -4 for CRC32
      uint8_t stream_type = sect_ptr[offset++];
      uint16_t es_pid = ((sect_ptr[offset] & 0x1F) << 8) | sect_ptr[offset+1];
      offset += 2;
      uint16_t es_info_length = ((sect_ptr[offset] & 0x03) << 8) | sect_ptr[offset+1];
      offset += 2;
      
      // Add tree node with stream type and PID
      // Extract ES descriptors (language, subtitles, etc.)
      // Advance offset by es_info_length
  }
  ```

- [ ] **Add stream type translations**
  ```cpp
  const char* streamTypeNames[] = {
      "Reserved",                    // 0x00
      "MPEG-1 Video",               // 0x01
      "MPEG-2 Video",               // 0x02
      "MPEG-1 Audio",               // 0x03
      "MPEG-2 Audio",               // 0x04
      "Private Section",            // 0x05
      "PES Private Data",           // 0x06
      "ISO/IEC 13522 MHEG",        // 0x07
      "DSM CC",                     // 0x08
      "MPEG-2 AUX",                 // 0x09
      "MPEG-4 AAC Audio",          // 0x0F (some encoders)
      // ... continue to 0x1B (H.264), 0x24 (H.265), etc.
  };
  ```

- [ ] **Extract and validate CRC32**
  - Last 4 bytes of section
  - Compare with calculated CRC (can use existing PAT logic)

- [ ] **Display in tree view**
  - Root: "Program X - PMT (PID 0xYYYY)"
    - PCR PID: 0xZZZZ
    - Version: X.X
    - Program descriptors: [list]
    - Elementary Streams:
      - ES 1: H.264 Video (PID 0x0100) - language: eng
      - ES 2: AC-3 Audio (PID 0x0101) - language: eng
      - etc.

**Expected Result**: Complete PMT parsing with full elementary stream details

---

### Task 2.2: Integration with PAT

**File**: [src/TsReaderFrame.cpp](../src/TsReaderFrame.cpp)  
**Current Issue**: PMT PIDs discovered in PAT not tracked

- [ ] **Modify parsePat()** to track PMT PIDs
  ```cpp
  // When parsing PAT programs:
  for (each program) {
      if (program_number > 0) {  // Skip NIT (program 0)
          m_pmtPids.insert(pmt_pid);  // Store for later
      }
  }
  ```

- [ ] **Add member variable to TsReaderFrame**
  ```cpp
  std::set<uint16_t> m_pmtPids;  // PIDs of all discovered PMT tables
  ```

- [ ] **Update parseSection()** default case
  ```cpp
  default:
      if (m_pmtPids.count(pid) > 0) {
          parsePmt(tree_root, sect_ptr, sect_size);
      }
      break;
  ```

**Expected Result**: PMT tables automatically parsed when discovered

---

## Phase 3: Conditional Access Table (CAT) Implementation

**Objective**: Parse encryption/conditional access information  
**Priority**: MEDIUM - needed for encrypted streams  
**Estimated Effort**: 1-2 hours

### Task 3.1: Implement parseCAT() Function

**File**: [src/TsReaderFrame.cpp](../src/TsReaderFrame.cpp)  
**Reference**: `parsePat()` implementation

- [ ] **Create parseCAT() function** (~80 lines)
  ```cpp
  void TsReaderFrame::parseCAT(wxTreeItemId& tree_root, 
                               const uint8_t* sect_ptr, 
                               uint16_t sect_size)
  {
      // 1. Extract header fields
      // 2. Loop through CA descriptors
      // 3. For each CA: extract system ID, PID, private data
      // 4. Validate CRC32
      // 5. Display in tree
  }
  ```

- [ ] **Parse CAT header** (first 8 bytes)
  - Table ID (1 byte): 0x01
  - Section length: bits 11-0 of bytes 1-2
  - Version: bits 5-1 of byte 5
  - Current/next: bit 0 of byte 5
  - Section number: byte 6
  - Last section: byte 7

- [ ] **Parse CA descriptors loop**
  ```cpp
  while (offset < payload_length - 4) {  // -4 for CRC32
      uint16_t ca_system_id = (sect_ptr[offset] << 8) | sect_ptr[offset+1];
      offset += 2;
      uint16_t ca_pid = ((sect_ptr[offset] & 0x1F) << 8) | sect_ptr[offset+1];
      offset += 2;
      
      // If there's private data, extract it
      // Typically: CA system ID determines private data format
  }
  ```

- [ ] **Add CA system ID translations**
  ```cpp
  struct CASystemInfo {
      uint16_t ca_system_id;
      const char* name;
      const char* country;
  };
  
  const CASystemInfo CA_SYSTEMS[] = {
      {0x0100, "IRDETO", "Various"},
      {0x0600, "Irdeto 2", "Various"},
      {0x0E00, "PowerVu", "USA"},
      {0x1800, "Nagravision", "Switzerland"},
      {0x4700, "Mediaguard", "France"},
      {0x4721, "Nagravision 3", "Switzerland"},
      // ... add more as needed
  };
  ```

- [ ] **Extract and validate CRC32**
  - Last 4 bytes of section

- [ ] **Display in tree view**
  - Root: "CAT (PID 0x0001)"
    - CA System 1: Nagravision 3 (0x4721)
      - CA PID: 0x0200
      - Private Data: [hex dump]
    - CA System 2: IRDETO (0x0100)
      - CA PID: 0x0201
      - etc.

**Expected Result**: Complete CA table parsing with system identification

---

### Task 3.2: Route CAT in parseSection()

**File**: [src/TsReaderFrame.cpp](../src/TsReaderFrame.cpp)

- [ ] **Add case for CAT**
  ```cpp
  case 0x01:  // CAT
      parseCAT(tree_root, sect_ptr, sect_size);
      break;
  ```

**Expected Result**: CAT automatically recognized and parsed

---

## Phase 4: Quality Assurance & Testing

**Objective**: Ensure implementations are robust and correct  
**Estimated Effort**: 2-3 hours

### Task 4.1: Create Test Files

- [ ] **Obtain test Transport Stream files**
  - [ ] Simple single-program stream (1 video, 1 audio)
  - [ ] Multi-program stream (2+ programs)
  - [ ] Encrypted stream with CAT
  - [ ] Stream with alternate audio tracks

- [ ] **Document expected output**
  - For each test file, document expected PAT/PMT/CAT structure
  - Reference: `ffprobe` or similar tool for validation

### Task 4.2: Verify Each Parser

- [ ] **Test PAT parsing** (already works, but verify)
  - Correctly identifies program numbers
  - Correctly identifies PMT PIDs
  - Correctly identifies NIT (program 0)
  - CRC32 validation passes

- [ ] **Test PMT parsing**
  - Correctly identifies PCR PID
  - Correctly identifies all elementary streams
  - Correctly identifies stream types (video/audio/other)
  - Stream type translations match `ffprobe` output
  - CRC32 validation passes

- [ ] **Test CAT parsing**
  - Correctly identifies CA system IDs
  - Correctly identifies CA PIDs
  - Works with various encryption systems

### Task 4.3: Error Handling

- [ ] **Handle malformed sections**
  - Invalid section length (too large/small)
  - CRC32 mismatch (log warning, don't crash)
  - Truncated sections (incomplete stream)
  - Mismatched table IDs

- [ ] **Handle edge cases**
  - Empty elementary streams list (rare but valid)
  - Multiple sections per table (table fragmentation)
  - Version changes during stream (re-parsing same sections)

### Task 4.4: Performance Testing

- [ ] **Test with large files**
  - 100+ MB TS file with many PAT/PMT repetitions
  - Ensure parsing completes in reasonable time
  - Monitor memory usage (should not grow unbounded)

- [ ] **Test with many programs**
  - Stream with 50+ programs
  - Ensure tree view remains responsive
  - UI doesn't freeze during parsing

---

## Phase 5: Documentation & Polish

**Objective**: Document implementation and finalize  
**Estimated Effort**: 1 hour

### Task 5.1: Update Code Comments

- [ ] **Add Doxygen-style comments** to new functions
  ```cpp
  /// Parses Program Map Table (PMT) section
  /// @param tree_root    Tree view node to add results to
  /// @param sect_ptr     Pointer to section data
  /// @param sect_size    Size of section in bytes
  void parsePmt(wxTreeItemId& tree_root, 
                const uint8_t* sect_ptr, 
                uint16_t sect_size);
  ```

- [ ] **Document data structures** in TsMsg.hpp
  - Explain each field
  - Specify bit widths
  - Reference MPEG-2 standard clause

### Task 5.2: Update User Documentation

- [ ] **Update COMPONENTS.md**
  - Document `parsePmt()` and `parseCAT()` implementations
  - Add stream type descriptions

- [ ] **Update USER_GUIDE.md**
  - Explain PMT tree display
  - Explain CAT tree display
  - Add examples of multi-program streams

### Task 5.3: Update ARCHITECTURE.md

- [ ] **Update Standards Support section**
  - Mark PMT and CAT as fully implemented
  - Add NIT to "partially implemented" (if applicable)

---

## Summary Table: All Tasks

| Phase | Task | Priority | Effort | Risk | Status |
|-------|------|----------|--------|------|--------|
| **1** | 1.1: Enable section extraction | CRITICAL | 30 min | Low | ⬜ |
| **1** | 1.2: Fix getTsPackets(pid) | CRITICAL | 30 min | Low | ⬜ |
| **1** | 1.3: Create data structures | CRITICAL | 2 hours | Low | ⬜ |
| **1** | 1.4: Route PMT/CAT in parseSection() | HIGH | 1 hour | Low | ⬜ |
| **2** | 2.1: Implement parsePmt() | HIGH | 3 hours | Medium | ⬜ |
| **2** | 2.2: Integrate with PAT | HIGH | 1 hour | Low | ⬜ |
| **3** | 3.1: Implement parseCAT() | MEDIUM | 2 hours | Medium | ⬜ |
| **3** | 3.2: Route CAT parsing | MEDIUM | 30 min | Low | ⬜ |
| **4** | 4.1: Create test files | HIGH | 1 hour | Low | ⬜ |
| **4** | 4.2: Verify each parser | HIGH | 2 hours | Medium | ⬜ |
| **4** | 4.3: Error handling | MEDIUM | 1 hour | Low | ⬜ |
| **4** | 4.4: Performance testing | LOW | 1 hour | Low | ⬜ |
| **5** | 5.1: Code comments | LOW | 1 hour | None | ⬜ |
| **5** | 5.2: User docs | LOW | 1 hour | None | ⬜ |
| **5** | 5.3: Architecture docs | LOW | 30 min | None | ⬜ |

**Total Estimated Effort**: 18-22 hours  
**Critical Path**: Phase 1 → Phase 2 → Phase 3 → QA

---

## Getting Started

**To begin implementation:**

1. Start with **Phase 1, Task 1.1** - uncommenting section extraction
2. Test PAT immediately to verify pipeline works
3. Create data structures (Task 1.3) while understanding PAT better
4. Implement PMT (Phase 2) for maximum impact
5. Follow with CAT (Phase 3) for completeness
6. Extensive testing (Phase 4)
7. Document and polish (Phase 5)

**Expected Result**: Full MPEG-2 PAT/PMT/CAT support enabling professional stream analysis.

---

## References

- **MPEG-2 Standard**: ISO/IEC 13818-1:2000 (Systems)
  - Section 2.4.4: Program Association Table
  - Section 2.4.3: Program Map Table
  - Section 2.4.1: Conditional Access Table

- **DVB Standard**: ETSI EN 300 468 (Service Information)
  - For future NIT/SDT/EIT/TDT implementation

---

**Document Version**: 1.0  
**Created**: 2026-04-24  
**Status**: Ready for Implementation
