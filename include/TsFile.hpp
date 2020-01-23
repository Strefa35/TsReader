#ifndef TSFILE_HPP_INCLUDED
#define TSFILE_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>

#include <stdint.h>

#define TS_HEADER_SIZE  4
#define TS_BUFFER       (16 * 1024)
#define TS_SYNC_BYTE    0x47
#define TS_PACKET_SIZE  188

#if 0
typedef struct ts_adaptation_s
{
  uint8_t   length;
  uint8_t   f2;
  uint64_t  pcr;
  uint64_t  opcr;
  uint8_t   splice;
  uint8_t   private_length;
} ts_adaptation_t;

typedef struct ts_payload_s
{
  uint8_t   table_id;
  uint16_t  section_size;
} ts_payload_t;
#endif // 0

typedef struct ts_packet_s
{
  uint64_t  file_offset;

  uint8_t   sync_byte;
  uint8_t   tei;            // Transport error indicator (TEI)
  uint8_t   pusi;           // Payload unit start indicator (PUSI)
  uint8_t   tsp;            // Transport priority
  uint16_t  pid;
  uint8_t   tsc;            // Transport scrambling control (TSC)
  uint8_t   afc;            // Adaptation field control
  uint8_t   cc;             // Continuity counter

#if 0
  ts_adaptation_t adaptation;

  ts_payload_t    payload;
#endif // 0

  uint8_t   raw_size;
  uint8_t   raw_tab[TS_PACKET_SIZE];

} ts_packet_t;

typedef struct ts_pid_s
{
  uint16_t  pid;
  uint64_t  count;

  std::vector<ts_packet_t> packets;

} ts_pid_t;

class TsFileBase
{
  public:
    TsFileBase(std::string fileName);
    ~TsFileBase();

    bool parse(void);
    void show(void);

    void toLog(void);

    bool getTsPackets(std::vector<ts_packet_t>& packets);
    bool getTsPackets(std::list<ts_packet_t>& packets);
    bool getTsPackets(uint16_t pid, std::vector<ts_packet_t>& packets);

    bool getTsPids(std::map<uint16_t, ts_pid_t>& pids);

  private:
    void getSize(void);

    void findTsSyncByte(void);

    void parseTsPacket(uint64_t offset, uint8_t* packet_ptr, uint32_t packet_size);

    bool setSeek(uint64_t offset);
    void showTsHeader(uint64_t idx, ts_packet_t* packet_ptr);

    std::string     m_filename;

    std::ifstream   m_fid;
    uint64_t        m_fSize;
    uint64_t        m_fidx;

    std::vector<ts_packet_t>      m_ts_packets;
    std::map<uint16_t, ts_pid_t>  m_ts_pids;
    std::list<ts_packet_t>        m_ts_list;

};

#endif // TSFILE_HPP_INCLUDED
