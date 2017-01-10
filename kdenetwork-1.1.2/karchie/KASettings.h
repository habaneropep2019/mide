/* -*- c++ -*- */
/***************************************************************************
 *                                KASettings.h                             *
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

#ifndef KASETTINGS_H
#define KASETTINGS_H

//#include <ktabctl.h> 
#include <qtabdlg.h> 
#include <qgrpbox.h> 
#include <qcombo.h>
#include <kintegerline.h>

class QFrame;
//class QComboBox;
class QLabel;
class QBoxLayout;
class QGridLayout;
//class KIntegerLine;
class KNumericSpinBox;
class QListBox;
//class QGroupBox;
//class QTabDialog;
class KConfig;

class KAArchieSettings: public QGroupBox
{
Q_OBJECT
public:
KAArchieSettings( const char *title, QWidget *parent=0, const char *name=0 );
~KAArchieSettings();

  const char *getHostName() const { return hostname->currentText(); }
  int getTimeout() const { return timeoutline->value(); }
  int getRetries() const { return triesline->value(); }
  int getMaxWidth() const;

public slots:
  void slotResetSettings();
  void slotApplySettings();

signals:
  void returnPressed();

protected:
  void drawContents ( QPainter * );

private:
  void doLayout();
  void readConfig();
  void writeConfig();

  //  QBoxLayout *mainbox, *hostbox, *timeoutbox, *triesbox;
  QGroupBox *hostbox,
    *timeoutbox,
    *triesbox;
  QComboBox *hostname;
  KIntegerLine *timeoutline,
    *triesline;
  QLabel *hostnamelabel,
    *timeoutlabel,
    *trieslabel;
  //  KConfig *config, *sessionconfig;
  //  uint currentHostId;

 private slots:
 inline void slotRP();
};

class KASearchSettings: public QGroupBox
{
Q_OBJECT
public:
KASearchSettings( const char *title, QWidget *parent=0, const char *name=0 );
~KASearchSettings();

public slots:
  void slotResetSettings();
  void slotApplySettings();

signals:
  void returnPressed();

protected:
  void drawContents ( QPainter * );

private:
  void doLayout();
  void initWidgets();
  void readConfig();
  void writeConfig();

  //  QGridLayout *mainlayout;
  QGroupBox *searchbox,
    //    *sortbox,
    //    *weightbox,
    *nicebox,
    *hitsbox;
  QLabel *searchlabel,
    //    *sortlabel,
    //    *weightlabel,
    *nicelabel,
    *hitslabel;
  QComboBox *searchmode,
    //    *sortmode,
    *nicelevel;
  KIntegerLine *maxhits;
  //  QListBox *weightlist;
  //  KConfig *config;
  int maxlabelwidth;

private slots:
  inline void slotRP();
};

/*
class KAUserSettings: public QGroupBox
{
Q_OBJECT
public:
KAUserSettings( const char *title, QWidget *parent=0, const char *name=0 );


};
*/

//class KASettings: public QTabDialog
class KASettings: public QDialog
{
Q_OBJECT
public:
KASettings(QWidget *parent = 0, const char *name = 0, bool modal=FALSE, WFlags f=0 );
~KASettings();

  const KAArchieSettings &getArchieSettings() const { return *archiehost; }
  const KASearchSettings &getSearchSettings() const { return *searchterms; }
  //  const KAUserSettings   &getUserSettings() const { return *user; }


public slots:

//  void slotDisplaySettings();


private:
//  void okButton();
//  void ResetButton();

//  QDialog *archiedialog, *searchdialog, *userdialog;
  //  QGroupBox *archiehost, *searchterms, *user;
  //  QBoxLayout *archiebox, *searchbox, *userbox;
  QBoxLayout *widgetLayout, *buttonLayout, *frameLayout;
  KAArchieSettings *archiehost;
  KASearchSettings *searchterms;
  //  KAUserSettings *user;
  QPushButton *acceptButton, *restoreButton, *cancelButton;
  QFrame *buttonFrame, *settingsFrame;

private slots:
  void slotReturnPressed();
};

void KAArchieSettings::slotRP()
{ emit returnPressed(); }
void KASearchSettings::slotRP()
{ emit returnPressed(); }

#endif
