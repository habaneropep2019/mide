/* -*- c++ -*- */
/***************************************************************************
 *                               KAStatusBar.h                             *
 *                            -------------------                          *
 *                         Header file for KArchie                         *
 *                  -A programm to display archie queries                  *
 *                                                                         *
 *                KArchie is written for the KDE-Project                   *
 *                         http://www.kde.org                              *
 *                                                                         *
 *   Copyright (C) Oct 1997 Jörg Habenicht                                 *
 *                  E-Mail: j.habenicht@europemail.com                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          * 
 *                                                                         *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
 *                                                                         *
 ***************************************************************************/

#ifndef KASTATUSBAR_H
#define KASTATUSBAR_H

#include <kstatusbar.h> 

class KAStatusBar : public KStatusBar
{
  // eine erweiterte Statuszeile.
  // kann dateinamen aufnehmen und informiert werden
Q_OBJECT
public:
KAStatusBar( QWidget *parent = NULL, char *name = NULL );
~KAStatusBar();

  void startLED();
  void stopLED();

public slots:
  void slotChangeStatus( const char * );
  //  void slotStatusReady();
  //  void slotStatusQueryRun();
  //  void slotStatusQuerySetup();
  void slotChangeHost( const char * );
  void slotChangeHits( int );
  void slotChangeProgress( int );
  void slotChangeTries( int );
  void slotConfigChanged();

protected:
  void timerEvent( QTimerEvent * );

private:
  void lightLED();
  void darkLED();
  void toggleLED();

  int ledTimerID;

  static const int statusId;
  static const int hostId;
  static const int hitsId;
  static const int progressId;
  static const int triesId;
  static const int ledId;

  static const char statusTxt[];
  static const char hostTxt[];
  static const char hitsTxt[];
  static const char progressTxt[];
  static const char triesTxt[];

};

#endif
