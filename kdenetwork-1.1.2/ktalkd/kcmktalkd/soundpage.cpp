/*
 * soundpage.cpp - Sound settings for KTalkd
 *
 * Copyright (C) 1998 David Faure, faure@kde.org
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

#include "soundpage.h"
#include <config.h>

#include <stdlib.h> //for setenv

#include <qmsgbox.h> 
#include <qstring.h>

#include <ksimpleconfig.h>
#include <kapp.h>

extern KSimpleConfig *config;
extern KSimpleConfig *announceconfig;

/* Lots of stuff taken from syssound.cpp */

KSoundPageConfig::KSoundPageConfig( QWidget *parent, const char* name ) 
    : KConfigWidget (parent, name)
{
    extprg_edit = new QLineEdit(this);
    extprg_edit->adjustSize();
    extprg_edit->setMinimumWidth(150);
    extprg_label = new QLabel(extprg_edit,i18n("&Announcement program"),this);
    extprg_label->adjustSize();
    extprg_label->setAlignment( ShowPrefix | AlignVCenter );

    client_edit = new QLineEdit(this);
    client_edit->adjustSize();
    client_edit->setMinimumWidth(150);
    client_label = new QLabel(client_edit,i18n("&Talk client"),this);
    client_label->adjustSize();
    client_label->setAlignment( ShowPrefix | AlignVCenter );

    sound_cb = new QCheckBox(i18n("&Play sound"), this);
    sound_list = new QListBox(this);
    sound_label = new QLabel(sound_list,i18n("&Sound File"), this);
    sound_tip = new QLabel(i18n(
               "Additional WAV files can be dropped onto the sound list."
               ),this);  

    QString path = KApplication::kde_sounddir().copy();
    QDir dir;
    dir.setPath(path);
    dir.setNameFilter("*.wav");
    dir.setSorting(QDir::Name);
    dir.setFilter(QDir::Readable | QDir::Files);
    const QStrList * list = dir.entryList();

    sound_list->insertStrList(list);

    audiodrop = new KDNDDropZone(sound_list, DndURL);
        
    btn_test = new QPushButton(i18n("&Test"), this);

    sound_cb->adjustSize();
    btn_test->adjustSize();
    sound_list->setMinimumSize(50, 80);
    sound_tip->adjustSize();
    setMinimumSize(400, 10 + extprg_edit->height() + client_edit->height() +
                   sound_cb->height() + sound_tip->height() + 
                   80 /* for a big list */ + 30); 
    // 400 : otherwise, buttons may overlap

    loadSettings();

    connect(sound_cb, SIGNAL(clicked()), this, SLOT(soundOnOff()));
    connect(btn_test, SIGNAL(clicked()), this, SLOT(playCurrentSound()));
    connect(audiodrop, SIGNAL(dropAction(KDNDDropZone*)),
          SLOT(soundDropped(KDNDDropZone*))); 

}

KSoundPageConfig::~KSoundPageConfig( ) {
    delete extprg_label;
    delete extprg_edit;
    delete client_label;
    delete client_edit;
    delete sound_cb;
    delete sound_label;
    delete sound_list;
    delete sound_tip;
    delete audiodrop;
    // delete audio;  /* not a pointer */
    delete btn_test;

}

void KSoundPageConfig::resizeEvent(QResizeEvent *e) {
    KConfigWidget::resizeEvent(e);
    int h_txt = sound_cb->height(); // taken for the general label height
    int h_edt = client_edit->height();   // taken for the general QLineEdit height

    int spc = h_txt / 3;
    int w = rect().width();

    int h = 10 + spc*2;
    extprg_label->setFixedHeight(h_edt); // same size -> vertical center aligned
    extprg_label->move(10,h);
    extprg_edit->setGeometry(200, h, w-100-10, h_edt);
    h += h_edt + spc;

    client_label->setFixedHeight(h_edt); // same size -> vertical center aligned
    client_label->move(10,h);
    client_edit->setGeometry(200, h, w-100-10, h_edt);
    h += h_edt + spc;

    sound_cb->move(10, h);
    h += h_txt+spc;
    sound_label->move(10, h);
    btn_test->move(rect().width()-btn_test->width()-10, h);
    sound_list->setGeometry(15+sound_label->width(), h,
          btn_test->x()-(15+sound_label->width())-10, rect().height()-h-h_txt-spc*4);
    h += sound_list->height() + spc;
    sound_tip->move(10,h);
}

void KSoundPageConfig::soundDropped(KDNDDropZone *zone){

  QStrList &list = zone->getURLList();
  QString msg;
  int i, len;

  // For now, we do only accept FILES ending with .wav...

  len = list.count();

  for ( i = 0; i < len; i++) {

    QString url = list.at(i);

    if (strcmp("file:", url.left(5))) {      // for now, only file URLs are supported

      QMessageBox::warning(this, klocale->translate("Unsupported URL"),
        i18n("Sorry, this type of URL is currently unsupported"\
              "by the KDE System Sound Module")
                           );

    } else { // Now check for the ending ".wav"

      if (stricmp(".WAV",url.right(4))) {
         msg.sprintf(i18n("Sorry, but \n%s\ndoes not seem "\
                            "to be a WAV--file."), url.data());

        QMessageBox::warning(this, klocale->translate("Improper File Extension"), msg);

      } else {  // Hurra! Finally we've got a WAV file to add to the list

        url = url.right(url.length()-5); // strip the leading "file:"

        if (!addToSound_List(url)) {
          // did not add file because it is already in the list
          msg.sprintf(i18n("The file\n"
                              "%s\n"
                              "is already in the list"), url.data());

          QMessageBox::warning(this, klocale->translate("File Already in List"), msg);

        }
      }
    }
  }
} 

int KSoundPageConfig::findInSound_List(QString sound) {
// Searches for <sound> in sound_list. Returns position or -1 if not found

  bool found = false;

  int i = 0;
  int len = sound_list->count();

  while ((!found) && (i < len)) {

    found = (strcmp(sound,sound_list->text(i))==0);
    i++;
  }
  return (found ? i-1 : -1);
}

bool KSoundPageConfig::addToSound_List(QString sound){ 
// Add "sound" to the sound list, but only if it is not already there 

 bool found = (findInSound_List(sound) != -1);
 if (!found) {   // Fine, the sound is not already in the sound list!

   QString *tmp = new QString(sound); // take a copy...
   sound_list->insertItem(*tmp);
   sound_list->setTopItem(sound_list->count()-1);
 }

 return !found;
} 

void KSoundPageConfig::playCurrentSound()
{
  QString hlp, sname;
  int soundno;

  audio.stop();
  soundno = sound_list->currentItem();
  if (soundno != -1) {
    sname = sound_list->text(soundno);
    if (sname[0] != '/') {
      hlp = KApplication::kde_sounddir().copy();
      hlp += "/";  // Don't forget this -- Bernd
      hlp += sname;
      audio.play((char*)hlp.data());
    } else {
      audio.play((char*)sname.data());
    }
  }
}

void KSoundPageConfig::soundOnOff()
{
    bool b = sound_cb->isChecked();
    sound_label->setEnabled(b);
    sound_list->setEnabled(b);
    btn_test->setEnabled(b);
    sound_tip->setEnabled(b);
}

void KSoundPageConfig::defaultSettings() {

    extprg_edit->setText(KApplication::kde_bindir()+"/ktalkdlg");
    client_edit->setText(KApplication::kde_bindir()+"/kvt -e talk");
       // will be ktalk when ktalk is in CVS.
    sound_cb->setChecked(true);

    // Activate things according to configuration
    soundOnOff();
}

void KSoundPageConfig::loadSettings() {
   
    config->setGroup("ktalkd");
    announceconfig->setGroup("ktalkannounce");

    setenv("KDEBINDIR",KApplication::kde_bindir(),false/*don't overwrite*/); 
           // for the first reading of  the config file

    extprg_edit->setText(config->readEntry("ExtPrg",KApplication::kde_bindir()+"/ktalkdlg"));
    client_edit->setText(announceconfig->readEntry("talkprg",KApplication::kde_bindir()+"/kvt -e talk")); // will be ktalk when ktalk is in CVS

    bool b = announceconfig->readBoolEntry("Sound",true/*default value*/);
    sound_cb->setChecked(b);

    const QString soundFile = announceconfig->readEntry("SoundFile",0L);
    if (soundFile != 0L)
    {
        int pos = findInSound_List(soundFile);
        if (pos != -1) sound_list->setSelected(pos,true);
        else { 
            addToSound_List(soundFile);
            sound_list->setSelected(sound_list->count()-1,true); 
        }
    } else { sound_list->setSelected(0,true); }

    // Activate things according to configuration
    soundOnOff();
}

void KSoundPageConfig::saveSettings() {

    config->setGroup("ktalkd");
    config->writeEntry("ExtPrg", extprg_edit->text());
    config->sync();
    announceconfig->setGroup("ktalkannounce");
    announceconfig->writeEntry("talkprg", client_edit->text());
    announceconfig->writeEntry("Sound", sound_cb->isChecked());
    announceconfig->writeEntry("SoundFile",sound_list->text(sound_list->currentItem()));
    announceconfig->sync();
}

void KSoundPageConfig::applySettings() {
    saveSettings();
}
