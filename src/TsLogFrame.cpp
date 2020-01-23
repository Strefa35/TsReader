
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "TsLogFrame.hpp"

#include "options.dbg"
#ifdef DBG_LVL_TSLOGFRAME
  #define DBG_LEVEL   DBG_LVL_TSLOGFRAME
#endif // DBG_LVL_TSLOGFRAME
#include "TsDbg.hpp"


TsLogFrame::TsLogFrame(wxWindow *parent, const wxString& title)
        : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
  m_TxtCtrl = new wxTextCtrl(this, wxID_ANY, "",
                             wxDefaultPosition, wxDefaultSize,
                             wxTE_MULTILINE | wxTE_READONLY);

  // use fixed width font to align output in nice columns
  wxFont font(wxFontInfo().Family(wxFONTFAMILY_TELETYPE));
  m_TxtCtrl->SetFont(font);
  m_TxtCtrl->SetFocus();

  wxLog::SetActiveTarget(this);
  //wxLog::SetActiveTarget(m_TxtCtrl);

	this->Layout();
	this->Centre(wxBOTH);
}

TsLogFrame::~TsLogFrame()
{
  wxLog::SetActiveTarget(m_oldLogger);
}

void TsLogFrame::DoLogLine(wxTextCtrl *text,
                   const wxString& timestr,
                   const wxString& threadstr,
                   const wxString& msg)
{
  text->AppendText(wxString::Format("%9s %10s %s", timestr, threadstr, msg));
}

void TsLogFrame::DoLogRecord(wxLogLevel level,
                     const wxString& msg,
                     const wxLogRecordInfo& info)
{
  // let the default GUI logger treat warnings and errors as they should be
  // more noticeable than just another line in the log window and also trace
  // messages as there may be too many of them
  if ((level <= wxLOG_Warning) || (level == wxLOG_Trace))
  {
    m_oldLogger->LogRecord(level, msg, info);
    return;
  }

  DoLogLine
  (
    m_TxtCtrl,
    wxDateTime(info.timestamp).FormatISOTime(),
    info.threadId == wxThread::GetMainId()
        ? wxString("[ main ] -> ")
        : wxString::Format("[ %lx ] -> ", info.threadId),
    msg + "\n"
  );
}

