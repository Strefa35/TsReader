/*
 * Copyright (c) 2019, 4Embedded.Systems all rights reserved.
  */

#include <cstdio>
#include <cstring>
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


#define TS_HEADER_SIZE      (4)
//#define TS_PACKET_SIZE      (188)
//#define TS_BUFFER           (TS_PACKET_SIZE + 48) // + 48 bytes of safe area

#define TS_SYNC_BYTE        0x47

#define TS_AFC_PAYLOAD      (0x1)
#define TS_AFC_ADAPTATION   (0x2)


TsFileBase::TsFileBase(std::string fileName) :
      m_packet_size(TS_PACKET_SIZE),
      m_ts_packet(nullptr)
{
  DBGS(DbgWrite("++%s(fileName: %s)\n", __func__, fileName.c_str());)

  m_fSize = 0;
  m_filename = fileName;

  m_fid.open(m_filename, std::ifstream::in | std::ifstream::binary);
  if (m_fid.is_open())
  {
    if (getSize())
    {
      m_packet_size = 0;
      if (m_packet_size || findTsPacketSize())
      {
        m_ts_packet = new uint8_t [TS_BUFFER];
        if (m_ts_packet)
        {
          std::memset(m_ts_packet, 0x00, TS_BUFFER);
        }
      }
    }
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

TsFileBase::~TsFileBase()
{
  DBGS(DbgWrite("++%s()\n", __func__);)
  if (m_ts_packet)
  {
    delete[] m_ts_packet;
  }
  if (m_fid.is_open())
  {
    m_fid.close();
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

uint64_t TsFileBase::getSize(void)
{
  DBGS(DbgWrite("++%s()\n", __func__);)
  m_fid.seekg(0, m_fid.end);
  m_fSize = (uint64_t) m_fid.tellg();
  DBG1(DbgWrite("[%s] m_fSize: %u\n", __func__, m_fSize);)
  m_fid.seekg(0, m_fid.beg);
  DBGR(DbgWrite("--%s()\n", __func__);)
  return m_fSize;
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

bool TsFileBase::findTsPacketSize(void)
{
  bool      ts_sync_found = false;
  bool      ts_error = false;
  uint64_t  first_packet = 0;
  uint64_t  current_idx = 0;
  uint64_t  packet_cnt = 0;
  uint16_t  packet_size = TS_PACKET_SIZE;
  uint16_t  last_packet_size = 0;
  uint8_t   one_byte;

  DBGS(DbgWrite("++%s()\n", __func__);)
  setSeek(0);
  while(!m_fid.eof() && ((current_idx + TS_PACKET_SIZE) <= m_fSize))
  {
    DBG1(DbgWrite("\tcurrent_idx: %d\n", current_idx);)
    setSeek(current_idx);
    m_fid.read((char*) &one_byte, 1);
    if (one_byte == TS_SYNC_BYTE)
    {
      DBG1(DbgWrite("\tTS SYNC BYTE - %u\n", current_idx);)
      packet_cnt++;
      if (ts_sync_found)
      {
        if (last_packet_size && (packet_size != (current_idx - first_packet)))
        {
          last_packet_size = current_idx - first_packet;
          ts_error = true;
          DBGW(DbgWrite("Packet size is different: %d <--> %d\n", packet_size, last_packet_size);)
          break; // We have to STOP analysing due to different packet's size in the stream
        }
        last_packet_size = current_idx - first_packet;
        if (last_packet_size != packet_size)
        {
          packet_size = last_packet_size;
        }
        m_ts_size[packet_size]++;
        DBG1(DbgWrite("\tpacket_size: %d\n", packet_size);)
      }
      else
      {
        ts_sync_found = true;
      }
      first_packet = current_idx;
      current_idx += packet_size;
      if (current_idx >= m_fSize)
      {
        m_ts_size[m_fSize - first_packet]++;
      }
    }
    else
    {
      current_idx++;
    }
  }

  if (!ts_error)
  {
    m_packet_size = packet_size;
  }
  else
  {
    m_packet_size = 0;
  }
  DBGR(DbgWrite("--%s() - packet_size: %d, last_packet_size: %d\n", __func__, packet_size, last_packet_size);)
  return !ts_error;
}

void TsFileBase::readTsStream(void)
{
  uint8_t*  packet_ptr = m_ts_packet;
  uint16_t  packet_size = m_packet_size;
  uint64_t  current_idx = 0;

  DBGS(DbgWrite("++%s()\n", __func__);)
  setSeek(0);
  while(!m_fid.eof() && ((current_idx + packet_size) <= m_fSize))
  {
    DBG1(DbgWrite("[%s] idx: %lu\n", __func__, current_idx);)
    setSeek(current_idx);
    m_fid.read((char*) &packet_ptr[0], 1);
    if (packet_ptr[0] == TS_SYNC_BYTE)
    {
      m_fid.read((char*) &packet_ptr[1], TS_HEADER_SIZE - 1);
      m_fid.read((char*) &packet_ptr[TS_HEADER_SIZE], packet_size - TS_HEADER_SIZE);
      parseTsHeader(current_idx, packet_ptr, packet_size);
      current_idx += packet_size;
    }
    else
    {
      current_idx++;
    }
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
}

void TsFileBase::parseTsHeader(uint64_t offset, uint8_t* header_ptr, uint32_t header_size)
{
  ts_packet_t packet;

  DBGS(DbgWrite("++%s(offset: %lu, header: 0x %02x %02x %02x %02x)\n", __func__,
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

  packet.raw_size = header_size;
  memcpy(packet.raw_tab, header_ptr, header_size);

  // list of pids
  m_ts_pids[packet.pid].pid = packet.pid;
  m_ts_pids[packet.pid].count++;
  m_ts_pids[packet.pid].packets.push_back(packet);

  DBGR(DbgWrite("--%s()\n", __func__);)
}

bool TsFileBase::parse(void)
{
  bool result = true;

  DBGS(DbgWrite("++%s()\n", __func__);)
  readTsStream();
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

bool TsFileBase::getTsPackets(std::vector<ts_packet_t>** packets)
{
  bool result = true;

  DBGS(DbgWrite("++%s(VECTOR)\n", __func__);)
  *packets = &m_ts_packets;
  DBGR(DbgWrite("--%s(VECTOR)\n", __func__);)
  return result;
}
#if 0
bool TsFileBase::getTsPackets(uint16_t pid, std::vector<ts_packet_t>** packets)
{
  std::map<uint16_t, ts_pid_t>::iterator it;
  bool result = false;

  DBGS(DbgWrite("++%s(pid: 0x%04X [%d])\n", __func__, pid, pid);)
  it = m_ts_pids.find(pid);
  if (it != m_ts_pids.end())
  {
    *packets = &(((ts_pid_t)(it->second)).packets);
    result = true;
  }
  DBGR(DbgWrite("--%s()\n", __func__);)
  return result;
}
#endif
bool TsFileBase::getTsPids(std::map<uint16_t, ts_pid_t>** pids)
{
  bool result = true;

  DBGS(DbgWrite("++%s()\n", __func__);)
  *pids = &m_ts_pids;
  DBGR(DbgWrite("--%s()\n", __func__);)
  return result;
}

