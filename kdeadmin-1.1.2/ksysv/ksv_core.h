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

#ifndef KSV_CORE_H
#define KSV_CORE_H

#include <qobject.h>
#include <qmsgbox.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpushbt.h>
#include <kapp.h>
#include <kiconloader.h>
#include "Clipboard.h"
#include "Data.h"
#include "ksv_conf.h"

// convenience macros
#define CLIP KSVCore::clipboard()
#define CONF KSVCore::getConfig()

class KSVCore : public QObject
{
  Q_OBJECT
 
 public:
  KSVCore();
  ~KSVCore();

protected:
  QString topic;
  QString section;

public slots:
  /**
   * Sets a topic and a section for context-sensitive help
   */
  void setHelpTopic( const char* sect, const char* label = 0 );

  /**
   * Invokes kdehelp with the current topic
   * if withTopic == TRUE, otherwise just the
   * general help-file is opened
   */
  void invokeHelp( const bool withTopic = TRUE );

public:
  /**
   * Returns the current topic
   */
  QString helpTopic();

  /**
   * Returns the current section
   */
  QString helpSection();

  /** 
    * static members (utility functions)
    */
 public:
  static const QString getAppName();
  static void msg(QString _text);
  static QPixmap drawDragIcon( QPixmap icon, QBitmap maskIcon, QString overlay, KSVDragData* data );
  static QPixmap drawDragMulti( QPixmap icon, QString overlay );
  
  /**
    * return the global clipboard
    * (not a QClipboard, but my personal
    * one implemented for ksysv)
    */
  static KSVClipBoard* clipboard();

  /**
    * return the global KSVConfig object
    */
  static KSVConfig* getConfig();

  /**
   * Get the global Core-Object
   */
  static KSVCore* getCore();

  /**
   * Sets a QPushbutton to a good size
   */
  static void goodSize( QPushButton* button );
};

#endif
