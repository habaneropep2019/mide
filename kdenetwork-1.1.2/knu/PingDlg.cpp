/*
 *   PingDlg.cpp - Dialog for the ping command
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
 * $Id: PingDlg.cpp,v 1.10 1998/10/14 19:32:59 leconte Exp $
 *
 * $Log: PingDlg.cpp,v $
 * Revision 1.10  1998/10/14 19:32:59  leconte
 * Bertrand: Added mtr support (with a patch to mtr-0.21)
 *
 * Revision 1.9  1998/09/23 16:24:13  bieker
 * Use i18n() instead of _().
 *
 * Revision 1.8  1998/06/28 13:15:19  kalle
 * Fixing...
 * Improved RPM spec file
 * bumped package version number to 1.0pre
 *
 * Revision 1.7  1998/06/09 21:18:01  leconte
 * Bertrand: correction of bug #745 (reported by Duncan Haldane):
 * 	arguments are now added to the command line
 *
 * Revision 1.6  1998/03/01 19:30:16  leconte
 * - added a finger tab
 * - internal mods
 *
 * Revision 1.5  1998/01/03 16:40:05  kulow
 * corrected typos
 *
 * Revision 1.3  1997/12/01 21:11:01  leconte
 * Patches by <neal@ctd.comsat.com>
 *
 * Revision 1.2  1997/11/23 22:28:12  leconte
 * - Id and Log added in the headers
 * - Patch from C.Czezatke applied (preparation of KProcess new version)
 *
 */

#include "PingDlg.h"
#include "PingDlg.moc"


// This is the unit used to separate widgets
#define SEPARATION 10

/*
 * Constructor
 */
PingDlg::PingDlg(QString commandName, 
		 QWidget* parent, const char* name)
  : CommandDlg(commandName, parent, name)
{
  KConfig *kc = kapp->getConfig();

  layout1 = new QBoxLayout(commandBinOK, 
			   QBoxLayout::TopToBottom, SEPARATION);
  CHECK_PTR(layout1);
  
  layout2 = new QBoxLayout(QBoxLayout::LeftToRight, SEPARATION);
  CHECK_PTR(layout2);
  layout1->addLayout(layout2, 0);
  
  // Frame for options
  frame1 = new QFrame(commandBinOK, "frame_1");
  CHECK_PTR(frame1);
  frame1->setFrameStyle(QFrame::Box | QFrame::Sunken);
  layout1->addWidget(frame1, 0);
  
  layout3 = new QBoxLayout(frame1, QBoxLayout::LeftToRight, SEPARATION/2);
  CHECK_PTR(layout3);
  
  // Make the layout of CommandDlg
  layout2->addWidget(commandLbl1);
  layout2->addWidget(commandArgs);
  layout2->addSpacing(2*SEPARATION);
  layout2->addWidget(commandGoBtn);
  layout2->addWidget(commandStopBtn);
  
  // Layout of options
  layout3->addStretch(10);
  
  pingCb1 = new QCheckBox(i18n("Make host &name resolution"), frame1, "cb_1");
  pingCb1->setChecked(TRUE);
  pingCb1->adjustSize();
  pingCb1->setFixedSize(pingCb1->width(), 2*fontMetrics().height());
  layout3->addWidget(pingCb1, 0);
  
  layout3->addStretch(10);
  layout3->activate();
  
  layout1->addWidget(commandTextArea, 10);
  layout1->activate();

  /*
   * Look at the command binary to see which widget to display
   */
  kc->setGroup(configGroupName);
  if (!kc->hasKey("path")) {
    // It's the first execution, 
    // so we have to search for the pathname
    kc->writeEntry("path", commandName);
    checkBinaryAndDisplayWidget();
    if (commandFound) {
      // All is OK : we can enable this tab.
      if (!kc->hasKey("enable")) {
	kc->writeEntry("enable", 1);
      }
    }
  } else {
    checkBinaryAndDisplayWidget();
  }
  
  // Commit changes in configfile (if needed)
  kc->sync();
}

/*
 * Destructor
 */
PingDlg::~PingDlg()
{
}

/**
 * build the command line from widgets
 */
bool
PingDlg::buildCommandLine(QString args)
{
  QString s;
  KConfig *kc = kapp->getConfig();
  
  kc->setGroup(configGroupName);
  s = kc->readEntry("path");
  if (s.isNull()) {
    return FALSE;
  } else {
    //debug("getExecutable = %s", (const char *)s);
    childProcess.clearArguments();
    childProcess.setExecutable(s);

    // Add arguments
    s = (kc->readEntry("arguments")).simplifyWhiteSpace();
    
    if (!s.isEmpty()) {
      while (s.contains(' ', FALSE) != 0) {
	int pos = s.find(' ', 0, FALSE);
	childProcess << s.left(pos);
	s = s.remove(0, pos+1);
      }
      childProcess << s;
    }
    

    if (!pingCb1->isChecked()) {
      childProcess << "-n";
    }
    childProcess << (const char *)args;
    return TRUE;
  }
}


#ifdef PING_CONFIG

/* ******************************************************************** */

/**
 * make a new config object
 *
 * @param parent parent widget
 * @param name name of the widget
 */
PingCfgDlg::PingCfgDlg(const char *tcs,
		       QWidget *parent, const char *name)
  : CommandCfgDlg(tcs, parent, name)
{
}

/*
 * Destructor
 */
PingDlg::~PingDlg()
{
}


/**
 * make a new config widget
 *
 * @param parent parent widget
 * @param makeLayouts name of the widget
 */
QWidget *
PingCfgDlg::makeWidget(QWidget *parent, bool makeLayouts)
{
#define SET_ADJUSTED_FIXED_SIZE(_wdgt) \
                  _wdgt->setFixedSize(_wdgt->sizeHint())

  //debug("PingCfgDlg::makeWidget");
  (void)CommandCfgDlg::makeWidget(parent, FALSE);

  // Widgets
  cfgBG = new QButtonGroup(cfgWidget);
  CHECK_PTR(cfgBG);

#if 0
  
  cfgPingBtn = new QRadioButton(i18n("hos&t"), cfgBG);
  CHECK_PTR(cfgPingBtn);
  SET_ADJUSTED_FIXED_SIZE(cfgPingBtn);

  cfgNslookupBtn = new QRadioButton(i18n("ns&lookup"), cfgBG);
  CHECK_PTR(cfgNslookupBtn);
  SET_ADJUSTED_FIXED_SIZE(cfgNslookupBtn);
#endif

  if (makeLayouts) {
    cfgLayoutTB = new QBoxLayout(cfgWidget, QBoxLayout::TopToBottom, 10);
    CHECK_PTR(cfgLayoutTB);
    
    if (cfgWarning != 0) {
      cfgLayoutTB->addLayout(cfgWarningLayout);
      cfgWarningLayout->addStretch(10);
      cfgWarningLayout->addWidget(cfgWarningPm, 0);
      cfgWarningLayout->addWidget(cfgWarningLbl, 0);
      cfgWarningLayout->addStretch(10);
    }
    cfgLayoutTB->addWidget(cfgBinGB);
    
    cfgLayoutGB = new QGridLayout(cfgBinGB, 3, 3, 10);
    CHECK_PTR(cfgLayoutGB);
    
    cfgLayoutGB->addRowSpacing(0, 0);
    cfgLayoutGB->addWidget(cfgBinNameLbl, 1, 0, AlignRight|AlignVCenter);
    cfgLayoutGB->addWidget(cfgBinNameLE, 1, 1);
    cfgLayoutGB->addWidget(cfgBinNameBrowse, 1, 2);
    cfgLayoutGB->addWidget(cfgBinArgLbl, 2, 0, AlignRight|AlignVCenter);
    cfgLayoutGB->addWidget(cfgBinArgLE, 2, 1);
    cfgLayoutGB->setColStretch(0, 0);
    cfgLayoutGB->setColStretch(1, 10);
    cfgLayoutGB->activate();

    // Our widget
    cfgLayout2 = new QBoxLayout(cfgBG, QBoxLayout::LeftToRight, 10);
    CHECK_PTR(cfgLayout2);
    
    cfgLayout2->addStretch(10);
    cfgLayout2->addWidget(cfgPingBtn);
    cfgLayout2->addStretch(10);
    cfgLayout2->addWidget(cfgNslookupBtn);
    cfgLayout2->addStretch(10);
    cfgLayout2->activate();
    
    cfgLayoutTB->addWidget(cfgBG);

    cfgLayoutTB->addStretch(10);

    cfgWidget->adjustSize();
    cfgLayoutTB->activate();
    cfgWidget->adjustSize();
    cfgWidget->setMinimumSize(cfgWidget->size());
    
    cfgLayoutTB->activate();
  }
  readConfig();
  return (cfgWidget);
#undef SET_ADJUSTED_FIXED_SIZE
}

/**
 * delete the config widget
 */
void
PingCfgDlg::deleteConfigWidget()
{
  //  debug("PingCfgDlg::deleteCondigWidget");
  
  delete cfgLayoutTB;
  delete cfgLayoutGB;
  delete cfgLayout2;
  delete cfgBG;
  delete cfgPingBtn;
  delete cfgNslookupBtn;
}

/**
 * commit changes to the configfile
 * 
 * @return if the change have been done
 */
bool
PingCfgDlg::commitChanges()
{ 
  KConfig *kc = kapp->getConfig();

  (void)CommandCfgDlg::commitChanges();
  if (cfgNslookupBtn->isChecked()) {
    kc->writeEntry("binaryType", "nslookup");
  } else {
    kc->writeEntry("binaryType", "ping");
  }
  return(TRUE);
}

/**
 * cancel changes to the configfile
 */
void
PingCfgDlg::cancelChanges()
{
  // Nothing to do...
}

/**
 * read the configfile
 */
void
PingCfgDlg::readConfig()
{
  QString s;
  KConfig *kc = kapp->getConfig();

  kc->setGroup(configGroupName);
  
  if (kc->hasKey("binaryType")) {
    s = kc->readEntry("binaryType");
    if (!stricmp(s, "nslookup")) {
      cfgNslookupBtn->setChecked(TRUE);
    } else {
      cfgPingBtn->setChecked(TRUE);
    }
  } else {
    cfgPingBtn->setChecked(TRUE);
  }
}


#endif  // PING_CONFIG
