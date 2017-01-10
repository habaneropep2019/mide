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

#ifndef KSV_CONF_H
#define KSV_CONF_H

#include <qstring.h>
#include <qpoint.h>
#include <qsize.h>
#include <qcolor.h>
#include <kconfig.h>

class KSVConfig : public QObject
{
  Q_OBJECT

 public:
  KSVConfig();
  ~KSVConfig();

 public slots:
  void readConfig();
  void writeConfig();
  void cancelConfig();
  void setPanningFactor( const int val );
  void setShowLog( bool val = FALSE );
  void setShowTools( bool val = FALSE );
  void setShowStatus( bool val = FALSE );
  void setNewColor( const QColor& );
  void setChangedColor( const QColor& );
  void setScriptPath( QString path );
  void setRLPath( QString path );
  void setConfigured( bool val = TRUE );

 public:
  const bool getShowLog() const;
  const bool getShowStatus() const;
  const bool getShowTools() const;
  const int getPanningFactor() const;
  const QSize getSize() const;
  const QPoint getPosition() const;
  const QColor& getNewColor() const;
  const QColor& getChangedColor() const;

  /**
    * Have the necessary config entries
    * been written?
    */
  const bool isConfigured() const;

  const QString getScriptPath() const;
  const QString getRLPath() const;

 protected:
  QString ScriptPath;
  QString RLPath;
  bool DisplaySOut;
  bool showLog;
  bool showTools;
  bool showStatus;
  bool configured;
  KConfig* conf;
  int _panning;
  QColor _newColor;
  QColor _changedColor;
};

#endif
