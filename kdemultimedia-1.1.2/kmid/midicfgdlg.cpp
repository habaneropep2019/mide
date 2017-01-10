/**************************************************************************

    midicfgdlg.cpp  - The midi config dialog  
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include "midicfgdlg.h"
#include <qpushbt.h>
#include <qlistbox.h>
#include <qlabel.h> 
#include <kfiledialog.h>
#include <kapp.h>
#include <kmsgbox.h>
#include "player/deviceman.h"
#include "version.h"

MidiConfigDialog::MidiConfigDialog(DeviceManager *dm, QWidget *parent,const char *name) : QDialog(parent,name,TRUE)
{
setCaption(i18n("Configure Midi devices"));
setMinimumSize(360,240);
setMaximumSize(360,240);
ok=new QPushButton(i18n("OK"),this);
ok->setGeometry(140,200,100,30);
connect(ok,SIGNAL(clicked()),SLOT(accept()) );
cancel=new QPushButton(i18n("Cancel"),this);
cancel->setGeometry(250,200,100,30);
connect(cancel,SIGNAL(clicked()),SLOT(reject()) );

label=new QLabel(i18n("Select the midi device you want to use :"),this);
label->adjustSize();
label->move(10,10);
mididevices=new QListBox(this,"midideviceslist");
mididevices->setGeometry(10,30,340,60);
connect(mididevices,SIGNAL(highlighted(int)),SLOT(deviceselected(int)));
devman=dm;
char temp[200];
for (int i=0;i<devman->numberOfMidiPorts()+devman->numberOfSynthDevices();i++)
    {
    if (strcmp(devman->type(i),"")!=0)    
        sprintf(temp,"%s - %s",devman->name(i),devman->type(i));
       else
        sprintf(temp,"%s",devman->name(i));

    mididevices->insertItem(temp,i);
    };
selecteddevice=devman->getDefaultDevice();
mididevices->setCurrentItem(selecteddevice);

label2=new QLabel(i18n("Use the midi map :"),this);
label2->adjustSize();
label2->move(10,100);


if (selectedmap!=NULL) delete selectedmap;
if (strcmp(devman->getMidiMapFilename(),"")==0)
    {
    selectedmap=NULL;
    }
   else
    {
    selectedmap=new char[strlen(devman->getMidiMapFilename())+1];
    strcpy(selectedmap,devman->getMidiMapFilename());
    };

if (selectedmap!=NULL) maplabel=new QLabel(selectedmap,this);
   else maplabel=new QLabel(i18n("None"),this);

maplabel->adjustSize();
maplabel->move(10,100+label2->height());

mapbrowse=new QPushButton(i18n("Browse..."),this);
mapbrowse->adjustSize();
mapbrowse->move(170,140);
connect(mapbrowse,SIGNAL(clicked()),SLOT(browseMap()) );

mapnone=new QPushButton(i18n("None"),this);
mapnone->adjustSize();
mapnone->move(170+mapbrowse->width()+10,140);
connect(mapnone,SIGNAL(clicked()),SLOT(noMap()) );

};

void MidiConfigDialog::deviceselected(int idx)
{
selecteddevice=idx;
};

void MidiConfigDialog::browseMap()
{
QString filename;
char *path;
path=new char[strlen(KApplication::kde_datadir())+20];
sprintf(path,"%s/kmid/maps",(const char *)KApplication::kde_datadir());
filename=KFileDialog::getOpenFileName((const char *)path,"*.map",this); 
if (filename.isNull()) return;

if (selectedmap) delete selectedmap;
selectedmap=new char[strlen(filename)+1];
strcpy(selectedmap,filename);
maplabel->setText(selectedmap);
maplabel->adjustSize();

};

void MidiConfigDialog::noMap()
{
if (selectedmap!=NULL) {delete selectedmap;selectedmap=NULL;}; 
maplabel->setText(i18n("None"));
maplabel->adjustSize();
};

int MidiConfigDialog::selecteddevice=0;
char *MidiConfigDialog::selectedmap=NULL;
