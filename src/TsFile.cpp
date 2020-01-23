#include <cstdio>
#include <string>
#include <cassert>
#include <sstream>

#include <wx/wx.h>

#include "TsFile.hpp"

#include "options.dbg"
#ifdef DBG_LVL_TSFILE
  #define DBG_LEVEL   DBG_LVL_TSFILE
#endif // DBG_LVL_TSFILE
#include "TsDbg.hpp"


//#define TS_USE_FILE_SEEK


//#define TS_HEADER_SIZE  4
//#define TS_BUFFER       (16 * 1024)
//#define TS_SYNC_BYTE    0x47
//#define TS_PACKET_SIZE  188


static uint8_t    ts_packet[TS_PACKET_SIZE];


TsFileBase::TsFileBase(std::string fileName)
{
  DBGS(DbgWrite("++%s(fileName: %s)\n", __func__, fileName.c_str());)
  m_fSize = 0;
  m_filename = fileName;

  m_fid.open(m_filename, std::ifstream::in | std::ifstream::binary);
  if (m_fid.is_open())
  {
    getSize();
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

TsFileBase::~TsFileBase()
{
  DBGS(DbgWrite("++%s()\n", __func__);)
  if (m_fid.is_open())
  {
    m_fid.close();
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsFileBase::getSize(void)
{
  DBGS(DbgWrite("++%s()\n", __func__);)
  m_fid.seekg(0, m_fid.end);
  m_fSize = m_fid.tellg();
  DBG1(DbgWrite("[%s] m_fSize: %lu\n", __func__, m_fSize);)
  m_fid.seekg(0, m_fid.beg);
  DBGR(DbgWrite("--%s()\n", __func__);)
}

bool TsFileBase::setSeek(uint64_t offset)
{
  bool result = false;

  DBGS(DbgWrite("++%s(offset: %lu)\n", __func__, offset);)
  if (offset < m_fSize)
  {
    m_fid.seekg(offset, m_fid.beg);
    result = true;
  }
  DBGR(DbgWrite("--%s() - result: %d\n", __func__, result);)
  return result;
}

#ifdef TS_USE_FILE_SEEK
void TsFileBase::findTsSyncByte(void)
{
  uint64_t current_idx = m_fid.tellg();
  uint8_t ts_header[TS_HEADER_SIZE];

  DBGS(DbgWrite("++%s()\n", __func__);)
  while(!m_fid.eof() && ((current_idx + TS_PACKET_SIZE) <= m_fSize))
  {
    DBG1(DbgWrite("[%s] idx: %lu\n", __func__, current_idx);)
    m_fid.read((char*) &ts_header[0], 1);
    if (ts_header[0] == TS_SYNC_BYTE)
    {
      m_fid.read((char*) &ts_header[1], 3);

      parseTsPacket(current_idx, ts_header);

      setSeek(current_idx + TS_PACKET_SIZE);

    }
    current_idx = m_fid.tellg();
  }
  DBG1(DbgWrite("[%s] TS vector size: %lu\n", __func__, m_ts_packets.size());)
  DBGR(DbgWrite("--%s()\n", __func__);)
}
#else
void TsFileBase::findTsSyncByte(void)
{
  uint64_t current_idx = 0;

  DBGS(DbgWrite("++%s()\n", __func__);)
  while(!m_fid.eof() && ((current_idx + TS_PACKET_SIZE) <= m_fSize))
  {
    DBG1(DbgWrite("[%s] idx: %lu\n", __func__, current_idx);)
    m_fid.read((char*) &ts_packet[0], 1);
    if (ts_packet[0] == TS_SYNC_BYTE)
    {
      m_fid.read((char*) &ts_packet[1], TS_PACKET_SIZE - 1);

      parseTsPacket(current_idx, ts_packet, TS_PACKET_SIZE);
      current_idx += TS_PACKET_SIZE;
    }
    else
    {
      current_idx++;
    }
  }
  DBG1(DbgWrite("[%s] TS Packets cnt: %lu\n", __func__, m_ts_packets.size());)
  DBGR(DbgWrite("--%s()\n", __func__);)
}
#endif

void TsFileBase::parseTsPacket(uint64_t offset, uint8_t* packet_ptr, uint32_t packet_size)
{
  uint8_t*  header_ptr = packet_ptr;
  ts_packet_t packet;

  DBGS(DbgWrite("++%s(offset: %ul, header: 0x %02x %02x %02x %02x)\n", __func__,
            offset,
            header_ptr[0], header_ptr[1], header_ptr[2], header_ptr[3]);)

  packet.file_offset  = offset;
  packet.sync_byte    = header_ptr[0];
  packet.tei          = (header_ptr[1] & 0x80) >> 7;
  packet.pusi         = (header_ptr[1] & 0x40) >> 6;
  packet.tsp          = (header_ptr[1] & 0x20) >> 5;
  packet.pid          = ((header_ptr[1] & 0x1f) << 8) | header_ptr[2];
  packet.tsc          = (header_ptr[3] & 0xc0) >> 6;
  packet.afc          = (header_ptr[3] & 0x30) >> 4;
  packet.cc           = (header_ptr[3] & 0x0f);

  //if ((packet.pid >= 0) && (packet.pid <= 31))
  {
    memcpy(packet.raw_tab, packet_ptr, packet_size);
    packet.raw_size = packet_size;
  }

  // list of pids
  m_ts_pids[packet.pid].pid = packet.pid;
  m_ts_pids[packet.pid].count++;
  //if ((packet.pid >= 0) && (packet.pid <= 31))
  {
    m_ts_pids[packet.pid].packets.push_back(packet);
  }

  DBGR(DbgWrite("--%s()\n", __func__);)
}

bool TsFileBase::parse(void)
{
  bool result = true;

  DBGS(DbgWrite("++%s()\n", __func__);)
  findTsSyncByte();
  DBGR(DbgWrite("--%s() - result: %d\n", __func__, result);)
  return result;
}

void TsFileBase::show(void)
{
  DBGS(DbgWrite("++%s()\n", __func__);)
  for (uint32_t i = 0; i < m_ts_packets.size(); i++)
  {
    showTsHeader(i, &m_ts_packets[i]);
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsFileBase::showTsHeader(uint64_t idx, ts_packet_t* header)
{
  DBG3(DbgWrite("Packet id: %ul\n", idx);)
  DBG3(DbgWrite("                         TS file offset: %lu\n", header->file_offset);)
  DBG3(DbgWrite("                           TS Sync Byte: 0x%02x\n", header->sync_byte);)
  DBG3(DbgWrite("        Transport error indicator (TEI): %d\n", header->tei);)
  DBG3(DbgWrite("    Payload unit start indicator (PUSI): %d\n", header->pusi);)
  DBG3(DbgWrite("                     Transport priority: %d\n", header->tsp);)
  DBG3(DbgWrite("                                    PID: 0x%04x [ %d ]\n", header->pid, header->pid);)
  DBG3(DbgWrite("     Transport scrambling control (TSC): 0x%02x\n", header->tsc);)
  DBG3(DbgWrite("               Adaptation field control: 0x%02x\n", header->afc);)
  DBG3(DbgWrite("                     Continuity counter: 0x%02x\n", header->cc);)
  DBG3(DbgWrite("---------------------------------------------------------\n");)
}

void TsFileBase::toLog()
{
  std::ofstream   fid;

  DBGS(DbgWrite("++%s()\n", __func__);)
  fid.open(m_filename + ".log", std::ifstream::out);
  if (fid.is_open())
  {
    for (uint32_t i = 0; i < m_ts_packets.size(); i++)
    {
      ts_packet_t* header = &m_ts_packets[i];

      fid << wxString::Format("Packet id: %lu\n", i);
      fid << wxString::Format("                         TS file offset: %lu\n", header->file_offset);
      fid << wxString::Format("                           TS Sync Byte: 0x%02x\n", header->sync_byte);
      fid << wxString::Format("        Transport error indicator (TEI): %d\n", header->tei);
      fid << wxString::Format("    Payload unit start indicator (PUSI): %d\n", header->pusi);
      fid << wxString::Format("                     Transport priority: %d\n", header->tsp);
      fid << wxString::Format("                                    PID: 0x%04x [ %d ]\n", header->pid, header->pid);
      fid << wxString::Format("     Transport scrambling control (TSC): 0x%02x\n", header->tsc);
      fid << wxString::Format("               Adaptation field control: 0x%02x\n", header->afc);
      fid << wxString::Format("                     Continuity counter: 0x%02x\n", header->cc);
      fid << wxString::Format("---------------------------------------------------------\n");
    }
    fid.close();
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

bool TsFileBase::getTsPackets(std::vector<ts_packet_t>& packets)
{
  bool result = true;

  DBGS(DbgWrite("++%s(VECTOR)\n", __func__);)
  packets = m_ts_packets;
  DBGR(DbgWrite("--%s(VECTOR)\n", __func__);)
  return result;
}

bool TsFileBase::getTsPackets(uint16_t pid, std::vector<ts_packet_t>& packets)
{
  std::map<uint16_t, ts_pid_t>::iterator it;
  bool result = false;

  DBGS(DbgWrite("++%s(pid: 0x%04X [%d])\n", __func__, pid, pid);)
  it = m_ts_pids.find(pid);
  if (it != m_ts_pids.end())
  {
    packets = ((ts_pid_t)(it->second)).packets;
    result = true;
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
  return result;
}

bool TsFileBase::getTsPackets(std::list<ts_packet_t>& packets)
{
  bool result = true;

  DBGS(DbgWrite("++%s(LIST)\n", __func__);)
  packets = m_ts_list;
  DBGR(DbgWrite("--%s(LIST)\n", __func__);)
  return result;
}

bool TsFileBase::getTsPids(std::map<uint16_t, ts_pid_t>& pids)
{
  bool result = true;

  DBGS(DbgWrite("++%s()\n", __func__);)
  pids = m_ts_pids;
  DBGR(DbgWrite("--%s()\n", __func__);)
  return result;
}

