/*
 * Copyright (c) 2019, 4Embedded.Systems all rights reserved.
 */

#ifndef TSTHREAD_HPP_INCLUDED
#define TSTHREAD_HPP_INCLUDED

#include <wx/thread.h>


class TsThread : public wxThread
{
  public:
    TsThread();
    virtual ~TsThread();

  protected:

  private:
};

#endif // TSREADERTHREAD_HPP
