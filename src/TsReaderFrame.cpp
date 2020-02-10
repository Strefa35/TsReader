/*
 * Copyright (c) 2019, 4Embedded.Systems all rights reserved.
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/filename.h>

#include "TsFile.hpp"
#include "TsReaderFrame.hpp"

#include "options.dbg"
#ifdef DBG_LVL_TSREADERFRAME
  #define DBG_LEVEL   DBG_LVL_TSREADERFRAME
#endif // DBG_LVL_TSREADERFRAME
#include "TsDbg.hpp"

// ID for the menu commands
enum
{
  ID_File   = 100,
  ID_Open,
  ID_Exit   = wxID_EXIT,

  ID_Tools  = 110,
  ID_Logger,
  ID_Hello,

  ID_Help   = 200,
  ID_About  = wxID_ABOUT,

  ID_LAST
};

#define TS_PAYLOAD_COLUMN   16

#define TS_AFC_PAYLOAD      (0x1)
#define TS_AFC_ADAPTATION   (0x2)


static std::string ts_unknown_pid_name = "-";

wxBEGIN_EVENT_TABLE(TsReaderFrame, wxFrame)
    EVT_MENU(ID_Open,     TsReaderFrame::OnOpen)
    EVT_MENU(ID_Hello,    TsReaderFrame::OnHello)
    EVT_MENU(ID_Exit,     TsReaderFrame::OnExit)
    EVT_MENU(ID_About,    TsReaderFrame::OnAbout)
    EVT_MENU(ID_Logger,   TsReaderFrame::OnLogger)

    EVT_CLOSE(TsReaderFrame::OnClose)
    EVT_WINDOW_CREATE(TsReaderFrame::OnChildCreate)
wxEND_EVENT_TABLE()


TsReaderFrame::TsReaderFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size),
          m_logger(nullptr), m_tsFile(nullptr),
          m_packets(nullptr), m_pids(nullptr)
{
  DBGS(DbgWrite("++%s()\n", __func__);)

  setPidNames();

  createMenuBar();
  createStatusBar();

  m_TreeCtrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE, wxDefaultValidator);
  m_TreeCtrlRoot = m_TreeCtrl->AddRoot(_T("TS stream"));

  wxFont font(wxFontInfo().Family(wxFONTFAMILY_TELETYPE));
  m_TreeCtrl->SetFont(font);

  m_TreeCtrl->ExpandAll();
  m_TreeCtrl->ScrollTo(m_TreeCtrlRoot);

	this->Layout();
	//this->Center();
	this->Centre(wxBOTH);

  DBGR(DbgWrite("--%s()\n", __func__);)
}

TsReaderFrame::~TsReaderFrame()
{
  DBGS(DbgWrite("++%s()\n", __func__);)
  if (m_tsFile)
  {
    DBG1(DbgWrite("[%s] TsFile has been deleted.\n", __func__);)
    delete m_tsFile;
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::createMenuBar()
{
  DBGS(DbgWrite("++%s()\n", __func__);)

  m_menuBar = new wxMenuBar;

  wxMenu *menuFile = new wxMenu;
  menuFile->Append(ID_Open, "&Open TS\tCtrl-O", "Open TS stream");
  menuFile->AppendSeparator();
  menuFile->Append(ID_Exit, "E&xit\tAlt-X");
  m_menuBar->Append(menuFile, "&File");

  wxMenu *menuTools = new wxMenu;
  menuTools->Append(ID_Logger, "&Logger\tCtrl-L", "Start logger window");
  menuTools->AppendSeparator();
  menuTools->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
  m_menuBar->Append(menuTools, "&Tools");

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(ID_About, "&About");
  m_menuBar->Append(menuHelp, "&Help");

  SetMenuBar(m_menuBar);

  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::createStatusBar()
{
  DBGS(DbgWrite("++%s()\n", __func__);)

  wxRect rect;

  wxStatusBar* m_statusBar = CreateStatusBar(3, wxSTB_DEFAULT_STYLE, wxID_ANY);
  m_statusBar->GetFieldRect(2, rect);
  m_progress = new wxGauge(m_statusBar, wxID_ANY, 100, rect.GetPosition(), rect.GetSize(), wxGA_SMOOTH);
  m_progress->SetValue(0);

  SetStatusBar(m_statusBar);

  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::OnExit(wxCommandEvent& event)
{
  DBGS(DbgWrite("++%s()\n", __func__);)
  Close(true);
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::OnAbout(wxCommandEvent& event)
{
  wxMessageBox("TsReader is a tool to reading & parsing TS files",
                "About TsReader", wxOK | wxICON_INFORMATION );
}

void TsReaderFrame::setPidNames(void)
{
  DBGS(DbgWrite("++%s()\n", __func__);)
  setPidName(0x0000, "PAT");
  setPidName(0x0001, "CAT");
  setPidName(0x0002, "TSDT");
  setPidName(0x0010, "NIT");
  setPidName(0x0011, "SDT/BAT");
  setPidName(0x0012, "EIT");
  setPidName(0x0014, "TDT/TOT");
  setPidName(0x1FFF, "Null Packet");
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::setPidName(uint16_t pid, const char* name)
{
  DBGS(DbgWrite("++%s(pid: 0x%04x [%d], name: %s)\n", __func__, pid, pid, name);)
  m_pidName[pid] = name;
  DBGR(DbgWrite("--%s()\n", __func__);)
}

std::string TsReaderFrame::getPidName(uint16_t pid)
{
  std::string name;
  std::map<uint16_t, std::string>::iterator it;

  DBGS(DbgWrite("++%s(pid: 0x%04x [%d])\n", __func__, pid, pid);)
  it = m_pidName.find(pid);
  if (it != m_pidName.end())
  {
    name = m_pidName[pid];
  }
  else
  {
    name = ts_unknown_pid_name;
  }
  DBGR(DbgWrite("--%s() - name: %s\n", __func__, name.c_str());)
  return name;
}

bool TsReaderFrame::preparePacketsTree(wxTreeItemId& tree_root, std::vector<ts_packet_t>& packets)
{
  uint64_t packets_cnt = packets.size();
  uint64_t packets_idx = 0;
  wxTreeItemId root, item_root, item_leaf, header_root, raw_root;

  DBGS(DbgWrite("++%s()\n", __func__);)
  DBGS(DbgWrite("[%s] Packets cnt: %lu\n", __func__, packets_cnt);)

  m_progress->SetRange(packets_cnt);
  root = m_TreeCtrl->AppendItem(tree_root, wxString::Format("Packets [%lu]", packets_cnt));
  for (uint64_t i = 0; i < packets_cnt; i++)
  {
    ts_packet_t* packet = &packets[i];

    m_progress->SetValue(++packets_idx);

    item_root = m_TreeCtrl->AppendItem(root, wxString::Format("{ %lu }", packets_idx));

    header_root = m_TreeCtrl->AppendItem(item_root, wxString::Format("File"));
    item_leaf = m_TreeCtrl->AppendItem(header_root, wxString::Format("offset: %lu", packet->file_offset));

    header_root = m_TreeCtrl->AppendItem(item_root, wxString::Format("Header"));
    item_leaf = m_TreeCtrl->AppendItem(header_root, wxString::Format("TS Sync Byte: 0x%02x", packet->sync_byte));
    item_leaf = m_TreeCtrl->AppendItem(header_root, wxString::Format("Transport error indicator (TEI): %d", packet->tei));
    item_leaf = m_TreeCtrl->AppendItem(header_root, wxString::Format("Payload unit start indicator (PUSI): %d", packet->pusi));
    item_leaf = m_TreeCtrl->AppendItem(header_root, wxString::Format("Transport priority: %d", packet->tsp));
    item_leaf = m_TreeCtrl->AppendItem(header_root, wxString::Format("PID: 0x%04x [ %d ]", packet->pid, packet->pid));
    item_leaf = m_TreeCtrl->AppendItem(header_root, wxString::Format("Transport scrambling control (TSC): 0x%02x", packet->tsc));
    item_leaf = m_TreeCtrl->AppendItem(header_root, wxString::Format("Adaptation field control: 0x%02x [%c %c]", packet->afc,
                                                                    packet->afc & TS_AFC_ADAPTATION ? 'A' : ' ',
                                                                    packet->afc & TS_AFC_PAYLOAD    ? 'P' : ' '
                                                                    ));
    item_leaf = m_TreeCtrl->AppendItem(header_root, wxString::Format("Continuity counter: 0x%02x", packet->cc));
#if 0
    if (packet->afc & TS_AFC_ADAPTATION)
    {
      wxTreeItemId adaptation_root, adaptation_leaf;

      adaptation_root = m_TreeCtrl->AppendItem(item_root, wxString::Format("Adaptation field"));
      adaptation_leaf = m_TreeCtrl->AppendItem(adaptation_root, wxString::Format("Length: %d", packet->adaptation.length));
      adaptation_leaf = m_TreeCtrl->AppendItem(adaptation_root, wxString::Format("F2: 0x%02x", packet->adaptation.f2));
      adaptation_leaf = m_TreeCtrl->AppendItem(adaptation_root, wxString::Format("PCR: %lu", packet->adaptation.pcr));
      adaptation_leaf = m_TreeCtrl->AppendItem(adaptation_root, wxString::Format("OPCR: %lu", packet->adaptation.opcr));
      adaptation_leaf = m_TreeCtrl->AppendItem(adaptation_root, wxString::Format("Splice: %d", packet->adaptation.splice));
      adaptation_leaf = m_TreeCtrl->AppendItem(adaptation_root, wxString::Format("Private Length: %d", packet->adaptation.private_length));
    }
    if (packet->afc & TS_AFC_PAYLOAD)
    {
      wxTreeItemId payload_root, payload_leaf;

      payload_root = m_TreeCtrl->AppendItem(item_root, wxString::Format("Payload data"));
      payload_leaf = m_TreeCtrl->AppendItem(payload_root, wxString::Format("Table id: 0x%02x [%d]", packet->payload.table_id, packet->payload.table_id));
      payload_leaf = m_TreeCtrl->AppendItem(payload_root, wxString::Format("Section size: %d", packet->payload.section_size));

    }
#endif // 0

#ifdef TS_FILE_KEEP_RAW_DATA
    raw_root = m_TreeCtrl->AppendItem(item_root, wxString::Format("RAW data [%d]", packet->raw_size));
    wxString  raw_string = "";
    for (uint8_t raw_idx = 0; raw_idx < packet->raw_size; raw_idx++)
    {
      if ((raw_idx > 0) && ((raw_idx % TS_PAYLOAD_COLUMN) == 0))
      {
        item_leaf = m_TreeCtrl->AppendItem(raw_root, raw_string);
        raw_string = "";
      }
      raw_string += wxString::Format("%02X ", packet->raw_tab[raw_idx]);
    }
    item_leaf = m_TreeCtrl->AppendItem(raw_root, raw_string);
#endif // TS_FILE_KEEP_RAW_DATA
  }
  m_progress->SetValue(0);

  DBGR(DbgWrite("--%s()\n", __func__);)
  return true;
}

void TsReaderFrame::parsePat(wxTreeItemId& tree_root, uint8_t* sect_ptr, uint16_t sect_size)
{
  DBGS(DbgWrite("++%s(sect_ptr: %p, sect_size: %d)\n", __func__, sect_ptr, sect_size);)
  if (sect_ptr && sect_size)
  {
    wxTreeItemId item_root, item_leaf, loop_root;

    uint16_t  sect_idx  = 0;
    uint8_t   pointer_field = 0;

    uint8_t   bytes[2];

    uint8_t   tid;
    uint16_t  section_length;
    uint16_t  tsid;
    uint8_t   version_number;
    uint8_t   section_number;
    uint8_t   last_section_number;
    uint8_t   loop_size;
    uint8_t   loop_cnt;
    uint32_t  crc32;

    pointer_field = sect_ptr[sect_idx++];
    // skip
    sect_idx += pointer_field;

    // section header
    tid = sect_ptr[sect_idx];
    section_length = ((uint8_t) (sect_ptr[sect_idx + 1]) & 0x0F) << 8;
    section_length |= ((uint8_t) (sect_ptr[sect_idx + 2]));
    tsid = ((sect_ptr[sect_idx + 3]) << 8) | (sect_ptr[sect_idx + 4]);
    version_number = (sect_ptr[sect_idx + 5] & 0x3E) >> 1;
    section_number = sect_ptr[sect_idx + 6];
    last_section_number = sect_ptr[sect_idx + 7];

    bytes[0] = sect_ptr[sect_idx + 1];
    bytes[1] = sect_ptr[sect_idx + 5];

    sect_idx += 8;

    loop_size = section_length
                - 5 /* 5 bytes between section length and loop */
                - 4 /* 4 bytes of CRC32*/;
    loop_cnt = loop_size / 4;

    crc32 = sect_ptr[sect_idx + loop_size] << 24;
    crc32 |= sect_ptr[sect_idx + loop_size + 1] << 16;
    crc32 |= sect_ptr[sect_idx + loop_size + 2] << 8;
    crc32 |= sect_ptr[sect_idx + loop_size + 3];

    DBG1(DbgWrite("   TID: 0x%02x [%d]\n", tid, tid);)
    DBG1(DbgWrite("   section_length: %d\n", section_length);)

    item_root = m_TreeCtrl->AppendItem(tree_root, wxString::Format("PAT ver. %d", version_number));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("TID: 0x%02X [%d]", tid, tid));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("section_syntax_indicator: %d", bytes[0] & 0x80 ? 1 : 0));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("'0': %d", bytes[0] & 0x40 ? 1 : 0));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("reserved: %d", (bytes[0] & 0x30) >> 4));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("section_length: %d", section_length));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("transport_stream_id: 0x%04X [%d]", tsid, tsid));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("reserved: 0x%02X", (bytes[1] & 0xC0) >> 6));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("version_number: %d", version_number));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("current_next_indicator: %d", (bytes[1] & 0x01)));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("section_number: %d", section_number));
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("last_section_number: %d", last_section_number));
    loop_root = m_TreeCtrl->AppendItem(item_root, wxString::Format("Loop -> N=%d", loop_cnt));

    for (uint8_t loop_idx = 0; loop_idx < loop_cnt; loop_idx++)
    {
      uint16_t  program_number = ((sect_ptr[sect_idx]) << 8) | (sect_ptr[sect_idx + 1]);
      uint16_t  pid = ((sect_ptr[sect_idx + 2] & 0x1F) << 8) | (sect_ptr[sect_idx + 3]);

      sect_idx += 4;
      if (program_number == 0) // Network PID
      {
        item_leaf = m_TreeCtrl->AppendItem(loop_root, wxString::Format("NIT (PID: 0x%04X [%d])", pid, pid));
        parseNit(item_leaf, pid);
      }
      else // PMT PID
      {
        item_leaf = m_TreeCtrl->AppendItem(loop_root, wxString::Format("PMT Program #%d (PID: 0x%04X [%d])", program_number, pid, pid));
        parsePmt(item_leaf, pid);
      }
    }
    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("CRC32: 0x%08X", crc32));

  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::parseNit(wxTreeItemId& tree_root, uint16_t pid)
{
  std::vector<ts_packet_t> packets;

  DBGS(DbgWrite("++%s(pid: 0x%04X [%d])\n", __func__, pid, pid);)
#if 0
  if (m_tsFile->getTsPackets(pid, &m_packets))
  {
    if (m_packets->size())
    {
      preparePacketsTree(tree_root, &packets);
    }
  }
#endif
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::parsePmt(wxTreeItemId& tree_root, uint16_t pid)
{
#if 0
  std::vector<ts_packet_t> packets;

  DBGS(DbgWrite("++%s(pid: 0x%04X [%d])\n", __func__, pid, pid);)
  if (m_tsFile->getTsPackets(pid, packets))
  {
    if (packets.size())
    {
      preparePacketsTree(tree_root, packets);
    }
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
#endif
}

void TsReaderFrame::parseSection(wxTreeItemId& tree_root, uint16_t pid, uint8_t* sect_ptr, uint16_t sect_size)
{
  DBGS(DbgWrite("++%s(pid: 0x%04x [%d], sect_ptr: %p, sect_size: %d)\n", __func__, pid, pid, sect_ptr, sect_size);)
  if (sect_ptr && sect_size)
  {
    switch (pid)
    {
      case 0x00: // PAT
      {
        parsePat(tree_root, sect_ptr, sect_size);
        break;
      }
      default:
      {
        break;
      }
    }
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::parsePacket(wxTreeItemId& root_tree, ts_packet_t* packet_ptr)
{
  DBGS(DbgWrite("++%s(packet_ptr: %p)\n", __func__, packet_ptr);)
  if (packet_ptr)
  {
    uint32_t packet_idx = 4; // behind the header

    if (packet_ptr->afc & TS_AFC_ADAPTATION)
    {
#if 0
      uint8_t adaptation_field_length = packet_ptr->raw_tab[packet_idx++];

      if (adaptation_field_length)
      {

      }
      packet_idx += adaptation_field_length;
#endif
    }

    if (packet_ptr->afc & TS_AFC_PAYLOAD)
    {
#if 0
      uint8_t*  sect_ptr  = &packet_ptr->raw_tab[packet_idx];
      uint16_t  sect_size = 0;

      sect_size |= ((uint8_t) (packet_ptr->raw_tab[packet_idx + 1]) & 0x0F) << 8;
      sect_size |= ((uint8_t) (packet_ptr->raw_tab[packet_idx + 2]));

      parseSection(root_tree, packet_ptr->pid, sect_ptr, sect_size);
#endif
    }
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::prepareSectionsTree(wxTreeItemId& tree_root, uint16_t pid, std::vector<ts_packet_t>& packets)
{
  uint64_t packets_cnt = packets.size();
  uint64_t packets_idx = 0;
  wxTreeItemId item_root, item_leaf;

  DBGS(DbgWrite("++%s(pid: 0x%04x [%d])\n", __func__, pid, pid);)
  DBGS(DbgWrite("   [%s] Packets cnt: %lu\n", __func__, packets_cnt);)
  m_progress->SetRange(packets_cnt);
  item_root = m_TreeCtrl->AppendItem(tree_root, wxString::Format("Sections [%lu]", packets_cnt));
  for (uint64_t i = 0; i < packets_cnt; i++)
  {
    ts_packet_t* packet = &packets[i];

#ifdef TS_FILE_KEEP_RAW_DATA
    DBG2(DbgWrite("Packet %d, size: %d\n", i, packet->raw_size);)
    DBG3(DbgMemory(packet->raw_tab, packet->raw_size);)
#endif // TS_FILE_KEEP_RAW_DATA

    item_leaf = m_TreeCtrl->AppendItem(item_root, wxString::Format("{ section: %lu }", i + 1));
    parsePacket(item_leaf, packet);

    m_progress->SetValue(++packets_idx);

  }
  m_progress->SetValue(0);
  DBGR(DbgWrite("--%s()\n", __func__);)
}

bool TsReaderFrame::preparePidsTree(std::map<uint16_t, ts_pid_t>& pids)
{
  uint64_t pids_cnt    = pids.size();
  uint64_t pids_idx    = 0;
  wxTreeItemId root, item_root, item_leaf;

  DBGS(DbgWrite("++%s()\n", __func__);)
  DBGS(DbgWrite("[%s] Pids cnt: %lu\n", __func__, pids_cnt);)

  m_progress->SetRange(pids_cnt);
  root = m_TreeCtrl->AppendItem(m_TreeCtrlRoot, wxString::Format("Pids [%lu]", pids_cnt));
  for (std::map<uint16_t,ts_pid_t>::iterator it = pids.begin(); it != pids.end(); it++)
  {
    ts_pid_t pid = it->second;

    m_progress->SetValue(++pids_idx);

    item_root = m_TreeCtrl->AppendItem(root, wxString::Format("PID: 0x%04x [%4d] [%-12s] - %lu", pid.pid, pid.pid, getPidName(pid.pid).c_str(), pid.count));
    if (pid.packets.size())
    {
      preparePacketsTree(item_root, pid.packets);

      if ((pid.pid >= 0) && (pid.pid <= 31))
      {
        prepareSectionsTree(item_root, pid.pid, pid.packets);
      }
    }
  }
  m_progress->SetValue(0);

  m_TreeCtrl->ScrollTo(m_TreeCtrlRoot);

  DBGR(DbgWrite("--%s()\n", __func__);)
  return true;
}

void TsReaderFrame::parseTsFile(wxString& fileName)
{
  std::string name(fileName.c_str());

  DBGS(DbgWrite("++%s(fileName: %s)\n", __func__, fileName.c_str());)
  if (m_tsFile)
  {
    delete m_tsFile;
  }
  m_TreeCtrl->DeleteChildren(m_TreeCtrlRoot);
  m_tsFile = new TsFileBase(name);
  if (m_tsFile)
  {
    if (m_tsFile->parse())
    {
      if (m_tsFile->getTsPackets(&m_packets))
      {
        DBG1(DbgWrite("TS Packets cnt: %d\n", m_packets->size());)
        if (m_packets->size())
        {
          preparePacketsTree(m_TreeCtrlRoot, *m_packets);
        }
      }
      if (m_tsFile->getTsPids(&m_pids))
      {
        DBG1(DbgWrite("   TS Pids cnt: %d\n", m_pids->size());)
        if (m_pids->size())
        {
          preparePidsTree(*m_pids);
        }
      }
    }
    else
    {
      DBGW(DbgWrite("[WARNING][%s:%d] Something went wrong\n", __func__, __LINE__);)
    }
  }
  else
  {
    DBGW(DbgWrite("[WARNING][%s:%d] Something went wrong\n", __func__, __LINE__);)
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::OnOpen(wxCommandEvent& event)
{
  wxString  name = "File: ";

  DBGS(DbgWrite("++%s()\n", __func__);)

  wxFileDialog openFileDialog(this, _("Open TS stream"), "", "*.ts",
                     "TS files (*.ts)|*.mpeg",
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_OK)
  {
    wxString  fileName;

    fileName.append(openFileDialog.GetDirectory());
    fileName.append(wxFileName::GetPathSeparator());
    fileName.append(openFileDialog.GetFilename());

    name.append(openFileDialog.GetFilename());
    SetStatusText(name, 0);

    parseTsFile(fileName);
  }
  else
  {
    name.append("-");
    SetStatusText(name, 0);
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::OnHello(wxCommandEvent& event)
{
  wxLogMessage("TsReader message...");
}

void TsReaderFrame::OnLogger(wxCommandEvent& event)
{
  if (!m_logger)
  {
    m_logger = new TsLogFrame(this, "Logger");
    m_logger->Show(true);
  }
}

void TsReaderFrame::OnChildCreate(wxWindowCreateEvent& event)
{
  wxWindow* w = event.GetWindow();

  DBGS(DbgWrite("++%s()\n", __func__);)
  if (w)
  {
    DBG1(DbgWrite("   [%s] Logger created\n", __func__);)
    w->Connect(wxEVT_DESTROY, wxWindowDestroyEventHandler(TsReaderFrame::OnChildDestroy), NULL, this);
  }
  else
  {
    DBG1(DbgWrite("   [%s] Child created\n", __func__);)
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::OnChildDestroy(wxWindowDestroyEvent& event)
{
  wxWindow* w = event.GetWindow();

  DBGS(DbgWrite("++%s()\n", __func__);)
  if (w == m_logger)
  {
    DBG1(DbgWrite("   [%s] Logger Window\n", __func__);)
    //delete m_logger;
    m_logger = nullptr;
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsReaderFrame::OnClose(wxCloseEvent& event)
{
  DBGS(DbgWrite("++%s()\n", __func__);)
  if (event.GetId() == this->GetId())
  {
    this->Destroy();
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}
