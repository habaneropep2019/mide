/*
  main.cpp - The KControl module for ktalkd

  Copyright (C) 1998 by David Faure, faure@kde.org
  
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
   
  */


#include "soundpage.h"
#include "answmachpage.h"
#include "forwmachpage.h"
#include <kcontrol.h>
#include <ksimpleconfig.h>

KSimpleConfig *config;
KSimpleConfig *announceconfig;

class KKTalkdApplication : public KControlApplication
{
public:

    KKTalkdApplication(int &argc, char **argv, const char *name);
    
    virtual void init();
    virtual void apply();
    virtual void defaultValues();
    virtual void help();
    
private:

    KSoundPageConfig *soundpage;
    KAnswmachPageConfig *answmachpage;
    KForwmachPageConfig *forwmachpage;
};

KKTalkdApplication::KKTalkdApplication(int &argc, char **argv, 
                                       const char *name)
    : KControlApplication(argc, argv, name), soundpage(0), answmachpage(0)
{
    if (runGUI())
	{
	    if (!pages || pages->contains("soundpage"))
		addPage(soundpage = new KSoundPageConfig(dialog, "soundpage"),
			i18n("&Announcement"), "");
	    if (!pages || pages->contains("answmachpage"))
		addPage(answmachpage = new KAnswmachPageConfig(dialog, "answmachpage"),
			i18n("Ans&wering machine"), "");
	    if (!pages || pages->contains("forwmachpage"))
		addPage(forwmachpage = new KForwmachPageConfig(dialog, "forwmachpage"),
			i18n("&Forward"), "");
	    
	    if (soundpage || answmachpage || forwmachpage) {
		dialog->show();
	    }
	    else {
		fprintf(stderr, i18n("usage: kcmktalkd [-init | soundpage | answmachpage | forwmachpage ]\n"));
		justInit = true;
	    }
	    
	}
}

void KKTalkdApplication::help()
{
   invokeHTMLHelp("ktalkd/ktalkd.html","");
}

void KKTalkdApplication::init()
{
}

void KKTalkdApplication::defaultValues() 
{
    if (soundpage)
        soundpage->defaultSettings();
    if (answmachpage)
        answmachpage->defaultSettings();    
    if (forwmachpage)
        forwmachpage->defaultSettings();    
}

void KKTalkdApplication::apply()
{
    if (soundpage)
        soundpage->saveSettings();
    if (answmachpage)
        answmachpage->saveSettings();
    if (forwmachpage)
        forwmachpage->saveSettings();
}

int main(int argc, char **argv)
{
    config = new KSimpleConfig(KApplication::localconfigdir() + "/ktalkdrc");
    announceconfig = new KSimpleConfig(KApplication::localconfigdir() + "/ktalkannouncerc");
    KKTalkdApplication app(argc, argv, "kcmktalkd");
    app.setTitle(i18n("KTalkd Configuration"));

    int ret;
    if (app.runGUI())	
        ret = app.exec();
    else
        ret = 0;

    delete config;
    delete announceconfig;
    return ret;
}
