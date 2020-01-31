/*
 * Copyright (c) 2019, 4Embedded.Systems all rights reserved.
 */

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "TsReaderApp.hpp"
#include "TsLogFrame.hpp"
#include "TsReaderFrame.hpp"

#include "options.dbg"
#ifdef DBG_LVL_TSREADERAPP
  #define DBG_LEVEL   DBG_LVL_TSREADERAPP
#endif // DBG_LVL_TSREADERAPP
#include "TsDbg.hpp"


wxIMPLEMENT_APP(TsReaderApp);

bool TsReaderApp::OnInit()
{
  //wxLog::AddTraceMask("thread");
  //wxLog::AddTraceMask("frame");
  //wxLog::AddTraceMask("treectrl");

  try
  {
    TsReaderFrame *readerFrame = new TsReaderFrame("TsReader", wxPoint(50, 50), wxSize(450, 340));
    readerFrame->Show(true);
  }
  catch(...)
  {
    std::cout << "Exception !!!\n";
  }
  return true;
}
