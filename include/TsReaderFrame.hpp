#ifndef TSREADERFRAME_HPP_INCLUDED
#define TSREADERFRAME_HPP_INCLUDED

#include <wx/treectrl.h>

#include "TsFile.hpp"
#include "TsLogFrame.hpp"

class TsReaderFrame : public wxFrame
{
  public:
    TsReaderFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~TsReaderFrame();

  protected:

  private:
    void createStatusBar();
    void createMenuBar();

    void parseTsFile(wxString&  fileName);

    void parseSection(wxTreeItemId& tree_root, uint16_t pid, uint8_t* sect_ptr, uint16_t sect_size);
    void parsePacket(wxTreeItemId& tree_root, ts_packet_t* packet_ptr);

    void parsePat(wxTreeItemId& tree_root, uint8_t* sect_ptr, uint16_t sect_size);

    bool preparePacketsTree(wxTreeItemId& tree_root, std::vector<ts_packet_t>& packets);
    bool preparePacketsTree(wxTreeItemId& tree_root, std::list<ts_packet_t>& packets);

    void prepareSectionsTree(wxTreeItemId& tree_root, std::vector<ts_packet_t>& packets);

    bool preparePidsTree(std::map<uint16_t, ts_pid_t>& pids);

    void setPidNames(void);
    void setPidName(uint16_t pid, const char* name);
    std::string getPidName(uint16_t pid);

    void OnOpen(wxCommandEvent& event);
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLogger(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();

    wxMenuBar*    m_menuBar;
    wxStatusBar*  m_statusBar;

    wxGauge*      m_progress;

    TsLogFrame*   m_logger;

    wxTreeCtrl*   m_TreeCtrl;
    wxTreeItemId  m_TreeCtrlRoot;

    TsFileBase*   m_tsFile;

    std::map<uint16_t, std::string> m_pidName;

};


#endif
