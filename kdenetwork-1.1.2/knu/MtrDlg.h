/* -*- C++ -*-
 *
 *   Mtr.cpp - Dialog for the mtr command
 * 
 *   part of knu: KDE network utilities
 *
 *   Copyright (C) 1997  Bertrand Leconte
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * $Id: MtrDlg.h,v 1.1 1998/10/14 19:32:59 leconte Exp $
 *
 * $Log: MtrDlg.h,v $
 * Revision 1.1  1998/10/14 19:32:59  leconte
 * Bertrand: Added mtr support (with a patch to mtr-0.21)
 *
 */


#ifndef __MtrDlg_h__
#define __MtrDlg_h__

#include <qchkbox.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qheader.h>

#include "CommandDlg.h"

class MtrDlg: public CommandDlg
{
  Q_OBJECT

public:
  MtrDlg(QString commandName, 
		QWidget* parent = NULL, const char* name = NULL);
  virtual ~MtrDlg();

  virtual void installWaitCursor();
  virtual void resetWaitCursor();
  
public slots:
  virtual void slotCmdStdout(KProcess *, char *, int);
  virtual void clearOutput();

protected:
  bool         buildCommandLine(QString);

#if 0
  // widgets
  QCheckBox   *mtrCb1;
  QLineEdit   *mtrLe2;
  QLabel      *mtrLbl2;
  QFrame      *frame1;
#endif
  QListView   *lv;


  // layout
  QBoxLayout  *layout3;


  QListViewItem *lines[256];
  int            numberOfLines;

};
#endif // __MtrDlg_h__
