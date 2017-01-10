/***************************************************************************
                          KMDIMgr.h  -  description                              
                             -------------------                                         

    version              :                                   
    begin                : Mon Oct 19 18:42:15 CDT 1998
                                           
    copyright            : (C) 1998 by Timothy Whitfield and Glover George                         
    email                : timothy@ametro.net and dime@ametro.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#ifndef KMDIMGR
#define KMDIMGR

#include <ktoolbar.h>

#include "KMDIMgrBase.h"

// KMDIMgr adds functionality above and beyond KMDIMgrBase
class KMDIMgr : public KMDIMgrBase
{
    Q_OBJECT

public:
    KMDIMgr (QWidget* p=0L, const char *name = 0, 
                KToolBar *toolbar=0 );
    virtual ~KMDIMgr();

public slots:
    KMDIWindow* addWindow(QWidget *,int flag , const char* icon=0L); 
    void slotWindowMaximized(KMDIWindow *);
    void slotWindowRestored(KMDIWindow *);
private:

protected:
    KToolBar *top_toolbar;
    int toolMin,toolMax,toolClose;
};

#endif
