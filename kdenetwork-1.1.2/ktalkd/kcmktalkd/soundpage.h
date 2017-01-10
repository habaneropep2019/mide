/*
 * soundpage.h
 *
 * Copyright (c) 1998 David Faure
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __KCONTROL_KSOUNDPAGE_H__
#define __KCONTROL_KSOUNDPAGE_H__

#include <qdir.h>
/* has to be before everything because of #define Unsorted 0 in X11/X.h !! */

#include <qobject.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qbutton.h>
#include <qchkbox.h> 

#include <kcontrol.h>
#include <mediatool.h>
#include <kaudio.h>
#include <drag.h>

class KSoundPageConfig : public KConfigWidget
{
  Q_OBJECT

public:
    KSoundPageConfig( QWidget *parent=0, const char* name=0 );
    ~KSoundPageConfig( );
    virtual void loadSettings();
    virtual void saveSettings();
    virtual void applySettings();
    virtual void defaultSettings();

protected:
    void  resizeEvent(QResizeEvent *e);

private slots:
    void soundOnOff();
    void soundDropped(KDNDDropZone *zone);
    void playCurrentSound();
 
private:
    QLabel *extprg_label;
    QLineEdit *extprg_edit;
    QLabel *client_label;
    QLineEdit *client_edit;
    QCheckBox *sound_cb;
    QLabel *sound_label;
    QListBox *sound_list;
    QLabel *sound_tip;
    KDNDDropZone *audiodrop;
    KAudio audio;
    QPushButton *btn_test;

    int findInSound_List(QString sound);
    bool addToSound_List(QString sound);
};

#endif

