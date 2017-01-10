/* -*- C++ -*-
 *
 *   TracerouteDlg.cpp - Dialog for the traceroute command
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
 * $Id: TracerouteDlg.h,v 1.6 1998/06/28 13:15:22 kalle Exp $
 *
 * $Log: TracerouteDlg.h,v $
 * Revision 1.6  1998/06/28 13:15:22  kalle
 * Fixing...
 * Improved RPM spec file
 * bumped package version number to 1.0pre
 *
 * Revision 1.5  1998/03/01 19:30:21  leconte
 * - added a finger tab
 * - internal mods
 *
 * Revision 1.4  1997/12/12 17:55:51  denis
 * Q_OBJECT; again
 *
 * Revision 1.2  1997/11/23 22:28:16  leconte
 * - Id and Log added in the headers
 * - Patch from C.Czezatke applied (preparation of KProcess new version)
 *
 */


#ifndef __TracerouteDlg_h__
#define __TracerouteDlg_h__

#include <qchkbox.h>
#include <qlayout.h>

#include "CommandDlg.h"

class TracerouteDlg: public CommandDlg
{
  Q_OBJECT

public:
  TracerouteDlg(QString commandName, 
		QWidget* parent = NULL, const char* name = NULL);
  virtual ~TracerouteDlg();
  

protected:
  bool         buildCommandLine(QString);

  // widgets
  QCheckBox   *tracerouteCb1;
  QLineEdit   *tracerouteLe2;
  QLabel      *tracerouteLbl2;
  QFrame      *frame1;

  // layout
  QBoxLayout  *layout3;

};
#endif // __TracerouteDlg_h__
