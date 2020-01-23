#ifndef TSLOGFRAME_HPP_INCLUDED
#define TSLOGFRAME_HPP_INCLUDED


class TsLogFrame : public wxFrame,
                      private wxLog
{
  public:
    TsLogFrame(const wxString& title);
    virtual ~TsLogFrame();

  protected:
    virtual void DoLogRecord(wxLogLevel level,
                             const wxString& msg,
                             const wxLogRecordInfo& info) wxOVERRIDE;

  private:
    // logging helper
    void DoLogLine(wxTextCtrl *text,
                   const wxString& timestr,
                   const wxString& threadstr,
                   const wxString& msg);

    wxLog*        m_oldLogger;
    wxTextCtrl*   m_TxtCtrl;

};

#endif // TSLOGFRAME_HPP_INCLUDED
