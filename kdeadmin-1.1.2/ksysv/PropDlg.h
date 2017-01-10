/*  Yo Emacs, this -*- C++ -*-

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef KSV_PROPDIALOG_H
#define KSV_PROPDIALOG_H

#include <qdialog.h>
#include <qpopmenu.h>
#include <qscrbar.h>
#include <qstring.h>
#include <qpushbt.h>
#include <qlined.h>
#include <qlayout.h>
#include <qspinbox.h>

class KSVPropDlg : public QDialog
{
  Q_OBJECT
public:
  KSVPropDlg( KSVDragData* data, QWidget* parent = 0, char* name = 0);
  ~KSVPropDlg();

protected:
  KSVDragData* datum;
  QLineEdit* name_le;
  QLineEdit* target_le;
  QSpinBox* number_il;

public:

public slots:
  virtual void setEnabled ( bool val );

signals:
  void changed(const KSVDragData*);
  void numberChanged();
  
protected slots:
  void accept();
};

#endif
