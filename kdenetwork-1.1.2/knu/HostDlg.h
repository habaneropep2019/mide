/* -*- C++ -*-
 *
 *   HostDlg.cpp - Dialog for the host command
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
 * $Id: HostDlg.h,v 1.4 1998/03/01 19:30:14 leconte Exp $
 *
 * $Log: HostDlg.h,v $
 * Revision 1.4  1998/03/01 19:30:14  leconte
 * - added a finger tab
 * - internal mods
 *
 * Revision 1.3  1997/12/07 23:44:29  leconte
 * - handle the binary's name modification dynamicaly (problem reported
 *   by Conrad Sanderson)
 * - added browse button to the option dialog (for binary selection)
 * - code clean-up
 * - better fallback to"nslookup" if "host" is not found
 *
 * Revision 1.2  1997/11/23 22:28:09  leconte
 * - Id and Log added in the headers
 * - Patch from C.Czezatke applied (preparation of KProcess new version)
 *
 */

#ifndef __HostDlg_h__
#define __HostDlg_h__

#include <qchkbox.h>
#include <qcombo.h>
#include <qbttngrp.h>
#include <qradiobt.h>
#include "CommandDlg.h"

class HostDlg: public CommandDlg
{
  Q_OBJECT

public:
  HostDlg(QString commandName,
	  QWidget* parent = NULL, const char* name = NULL);
  virtual ~HostDlg();
 

protected:
  bool         buildCommandLine(QString args);

  // additional widgets
  QLabel      *hostLbl1;
  QComboBox   *hostCb1;
  QFrame      *hostF1;
  QLabel      *hostLbl2;
  QComboBox   *hostCb2;
  QFrame      *frame1;

  // layout
  QBoxLayout  *layout3;       

};


class HostCfgDlg: public CommandCfgDlg
{
  // Q_OBJECT

public:
  HostCfgDlg(const char *tcs, 
	     QWidget* parent = NULL, const char* name = NULL);
  //virtual  ~HostDlgCfg();

  /**
   *
   */
  virtual QWidget *makeWidget(QWidget* parent, bool makeLayouts = TRUE);
  virtual void deleteConfigWidget();

  /**
   * commit changes to the configfile
   * 
   * @return if the change have been done
   * @see cancelChanges
   */
  virtual bool commitChanges();
  
  /**
   * cancel changes to the configfile
   *
   * @see commitChanges
   */
  virtual void cancelChanges();

  /**
   * This is called just before the OptionDlg is shown
   */
  void readConfig();

protected:

  // The configWidget
  QButtonGroup *cfgBG;
  QRadioButton *cfgHostBtn, *cfgNslookupBtn;
  QBoxLayout   *cfgLayout2;
};

#endif // __HostDlg_h__

