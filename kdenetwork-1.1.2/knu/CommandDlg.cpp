/*
 *   CommandDlg.cpp - Command Widget
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
 * $Id: CommandDlg.cpp,v 1.9 1999/01/22 20:31:07 leconte Exp $
 *
 * $Log: CommandDlg.cpp,v $
 * Revision 1.9  1999/01/22 20:31:07  leconte
 * Preparing kde1.1
 * Knu should now be able to handle long responses from nslookup
 *
 * Revision 1.8  1998/11/30 22:58:33  leconte
 * Bertrand: more than 1024 byte output from child did core. Corrected
 * 	for all tabs except mtr.
 *
 * Revision 1.7  1998/10/14 19:32:55  leconte
 * Bertrand: Added mtr support (with a patch to mtr-0.21)
 *
 * Revision 1.6  1998/09/23 16:24:07  bieker
 * Use i18n() instead of _().
 *
 * Revision 1.5  1998/04/17 21:20:12  leconte
 * Bertrand: french translation update
 *
 * Revision 1.4  1998/03/01 19:30:11  leconte
 * - added a finger tab
 * - internal mods
 *
 * Revision 1.3  1997/12/07 23:44:16  leconte
 * - handle the binary's name modification dynamicaly (problem reported
 *   by Conrad Sanderson)
 * - added browse button to the option dialog (for binary selection)
 * - code clean-up
 * - better fallback to"nslookup" if "host" is not found
 *
 * Revision 1.2  1997/11/23 22:28:05  leconte
 * - Id and Log added in the headers
 * - Patch from C.Czezatke applied (preparation of KProcess new version)
 *
 */

#include <errno.h>
#include <ctype.h>

#include <kfiledialog.h>

#include <kapp.h>
#include <qregexp.h>
#include <kmsgbox.h>
#include <qsocknot.h>
#include <qfileinf.h>
#include <qpixmap.h>
#include <qmsgbox.h> 


#include "CommandDlg.h"
#include "CommandDlg.moc"

#ifdef HAVE_CONFIG
#include <config.h>
#endif

// This is the unit used to separate widgets
#define SEPARATION 10

// defined in knu.cpp
extern bool test_for_exec(QString);



/*
 * We want to remove the ampersand in a string
 */ 
QString removeAmpersand(const QString &s1)
{
  QString s2 = s1.copy();
  
  s2.replace(QRegExp("&"), "" );
  return(s2);
}


/**
 * Constructor
 */
CommandDlg::CommandDlg(QString, 
		       QWidget* parent,	const char* name)
	: QWidget(parent, name)
{
  int widgetHeight = 2*fontMetrics().height();

  configGroupName = removeAmpersand(name);

  /*
   * We build a pseudo-top level widget to be able to hide it if we don't
   * find the binary
   */
  commandBinOK = new QFrame(this);
  CHECK_PTR(commandBinOK);
  
  commandArgs = new QLineEdit(commandBinOK, "lineedit_1" );
  //CB commandArgs = new QComboBox(TRUE, this, "combobox_1" );//CB
  //CB commandArgs->setInsertionPolicy(QComboBox::AtTop);//CB
  commandArgs->setMaximumSize(QLayout::unlimited, widgetHeight);
  connect(commandArgs, SIGNAL(returnPressed()), 
	  this, SLOT(slotLauchCommand()));
  connect(commandArgs, SIGNAL(textChanged(const char*)), 
	  this, SLOT(slotEnableGoBtn(const char*)));
  commandArgs->setMaxLength(1024);
  commandArgs->setEchoMode(QLineEdit::Normal);
  commandArgs->setFrame(TRUE);
  
  commandLbl1 = new QLabel(commandArgs, i18n("H&ost:"), commandBinOK);
  CHECK_PTR(commandLbl1);
  commandLbl1->setFixedSize(commandLbl1->sizeHint());
    
    
  commandGoBtn = new QPushButton(commandBinOK, "pushbutton_3" );
  commandGoBtn->setFixedSize(70, widgetHeight);
  connect(commandGoBtn, SIGNAL(clicked()),
	  this, SLOT(slotLauchCommand()));
  commandGoBtn->setText( i18n("&Go!") );
  commandGoBtn->setEnabled(FALSE);
  commandGoBtn->setAutoDefault(TRUE);
  isGoBtnEnabled=FALSE;
  //BL commandGoBtn->setFocusPolicy(QWidget::StrongFocus);
    
  commandStopBtn = new QPushButton(commandBinOK, "pushbutton_4" );
  commandStopBtn->setFixedSize(70, widgetHeight);
  connect(commandStopBtn, SIGNAL(clicked()), 
	  this, SLOT(slotStopCommand()));
  commandStopBtn->setText(i18n("&Stop"));
  commandStopBtn->setEnabled(FALSE);
  
  commandTextArea = new QMultiLineEdit(commandBinOK, "multilineedit_1" );
  commandTextArea->setReadOnly(TRUE);
  commandTextArea->setFocusPolicy(QWidget::NoFocus);
  
  /*
   * This is the widget if we don't find the command
   */
  commandBinNonOK = new QWidget(this);
  CHECK_PTR(commandBinNonOK);
  layoutNoBin = new QBoxLayout(commandBinNonOK, 
			       QBoxLayout::TopToBottom, SEPARATION);
  CHECK_PTR(layoutNoBin);
  
  commandLblNoBin = new QLabel(i18n("This command binary was not found. \n"
				 "You can give its path "
				 "in the Edit->Preferences... menu."),
			       commandBinNonOK);
  CHECK_PTR(commandLblNoBin);
  commandLblNoBin->setAlignment(AlignCenter);
  
  layoutNoBin->addWidget(commandLblNoBin);
  layoutNoBin->activate();
}


/*
 * Destructor
 */
CommandDlg::~CommandDlg()
{
  slotStopCommand();
}



/**
 * resize
 */
void
CommandDlg::resizeEvent(QResizeEvent *) 
{
  commandBinOK->resize(width(), height());
  commandBinNonOK->resize(width(), height());
}


/**
 * checkBinary
 */
void 
CommandDlg::checkBinaryAndDisplayWidget()
{
  //debug("%s::checkBinaryAndDisplayWidget()", name());
  KConfig *kc = kapp->getConfig();
  kc->setGroup(configGroupName);
  if (::test_for_exec(kc->readEntry("path"))) {
    commandFound = TRUE;
  } else {
    commandFound = FALSE;
  }

  if (commandFound) {
    commandBinNonOK->hide();
    commandBinOK->show();
  } else {
    commandBinOK->hide();
    commandBinNonOK->show();
  }
}

/**
 * This is called when the tab is selected, so we
 * can manage the focus
 */
void
CommandDlg::tabSelected()
{
  if (commandFound) {
    commandArgs->setFocus(); 
  }
}

/**
 * This is called when the tab is deselected, so we can
 * kill the command if needed
 */
void
CommandDlg::tabDeselected()
{
  slotStopCommand();
}

/**
 * This slot is connected to the lineedit, so we
 * can check if there is something in it to enable
 * the go button.
 */
void
CommandDlg::slotEnableGoBtn(const char *args)
{
  if (strlen(args) == 0) {
    if (isGoBtnEnabled) {
      isGoBtnEnabled = FALSE;
      commandGoBtn->setEnabled(isGoBtnEnabled);
    }
  } else {
    if (!isGoBtnEnabled) {
      isGoBtnEnabled = TRUE;
      commandGoBtn->setEnabled(isGoBtnEnabled);
    }
  }
}


/*
 * This function check if we can call the sub-program
 * (we make some basic checks on length and special characters)
 */
bool
CommandDlg::checkInput(QString *args)
{
  char c;
  int l;
  const char *p;
  
  l = args->length();
  if (l > 128) {
    return(FALSE);
  }
  p = (const char *)*args;
  while ((c = *p++)) {
    if ((!isalnum(c)) 
	&& (c!='.') && (c != '-') && (c != '/') && (c != '_')) {
      return(FALSE);
    }
  }
  return(TRUE);
}

/**
 * build the command line from widgets
 */
bool
CommandDlg::buildCommandLine(QString)
{
  warning("CommandDlg::buildCommandLine must be derived");
  return FALSE;
}


/*
 * this is called from the Go button
 */
void
CommandDlg::slotLauchCommand()
{
  //QString str;
  QString args;

  if (childProcess.isRunning()) {
    return;
  }

  args = commandArgs->text();
  //CB args = commandArgs->currentText();//CB
  if (strlen(args) == 0) {
    // nothing to do (this should not be possible)
  } else {

    // Check the input
    if (!checkInput(&args)) {
      //warning("input not valid");
      commandArgs->selectAll();
      KApplication::beep();
      return;
    }

    // Install the "Stop" button, and hide "Go!"
    commandGoBtn->setEnabled(FALSE);
    commandStopBtn->setEnabled(TRUE);

    // Install waitCursor
    installWaitCursor();

    // separate commands with CR/LF
    if (commandTextArea->numLines() > 1) {
      int line;
      line = QMAX(commandTextArea->numLines()-2, 0);
      if (strlen(commandTextArea->textLine(line)) > 0) {
	commandTextArea->append("");
      }
    }

    //  Process creation
    if (!buildCommandLine(args)) {
      QString errorString;
      debug("buildCommandLine = FALSE");
      // Same message in MtrDlg.cpp
      errorString.sprintf(i18n("\nYou have a problem in your\n" 
			    "%s/%src\nconfiguration file.\n"
			    "In the [%s] group,\nI can't "
			    "find a valid \"path=\" entry.\n\n"
			    "Please use Edit->Preferences... menu\n"
			    "to configure it again.\n"), 
			  (const char *)kapp->localconfigdir(), 
			  (const char *)kapp->appName(), 
			  (const char *)removeAmpersand(this->name()));
      KMsgBox::message(this, i18n("Error in pathname"), 
		       errorString, KMsgBox::STOP);
      slotProcessDead(NULL);
      return;
    }

    connect(&childProcess, SIGNAL(processExited(KProcess *)), 
	    SLOT(slotProcessDead(KProcess *)));

    connect(&childProcess, SIGNAL(receivedStdout(KProcess *, char *, int)), 
	    this, SLOT(slotCmdStdout(KProcess *, char *, int)));
    connect(&childProcess, SIGNAL(receivedStderr(KProcess *, char *, int)), 
	    this, SLOT(slotCmdStdout(KProcess *, char *, int)));

    // Test pour mtr: pb stdin?
//    if (!childProcess.start(KProcess::NotifyOnExit, KProcess::AllOutput)) {
    if (!childProcess.start(KProcess::NotifyOnExit, KProcess::All)) {
      // Process not started
      debug("Process not started");
      slotProcessDead(NULL);
      return;
    }
  }
}


/*
 * This is called by the Stop button
 */
void
CommandDlg::slotStopCommand()
{
  if (childProcess.isRunning()) {
    childProcess.kill(15);
  }
}


/*
 * Clean up all the things after the death of the command 
 * (gop button, cursor, ...)
 */
void
CommandDlg::slotProcessDead(KProcess *)
{
  // disconnect KProcess
  disconnect(&childProcess, SIGNAL(receivedStdout(KProcess *, char *, int)), 
	     this, SLOT(slotCmdStdout(KProcess *, char *, int)));
  disconnect(&childProcess, SIGNAL(receivedStderr(KProcess *, char *, int)), 
	     this, SLOT(slotCmdStdout(KProcess *, char *, int)));
  disconnect(&childProcess, SIGNAL(processExited(KProcess *)), 
	     this, SLOT(slotProcessDead(KProcess *)));
  
  // put back the "Go!" button up
  commandGoBtn->setEnabled(TRUE);
  commandStopBtn->setEnabled(FALSE);

  resetWaitCursor();
  
  // to be ready for a new command
  commandArgs->selectAll();
  commandArgs->setFocus();
}

/*
 * Read the output of the command on a socket and append
 * it to the multilineedit.
 *
 * This is called by the main select loop in Qt (QSocketNotifier)
 */
void
CommandDlg::slotCmdStdout(KProcess *, char *buffer, int buflen)
{
  int  line, col;
  char *p;
  QString *receivedLine;
  
  if (buflen <= 0) {
    receivedLine = new QString("--- nothing ---\n");
  } else {
    //buffer[buflen] = 0;		// mark eot
    //debug("stdout> %s", buffer);
    receivedLine = new QString(buffer, buflen+1);
  }

  // goto end of data
  line = QMAX(commandTextArea->numLines() - 1, 0);
  p = (char*)commandTextArea->textLine(line);
  col = 0;
  if (p != NULL) {
    col = strlen(p);
  }
  commandTextArea->setCursorPosition(line, col);
  
  // and insert text here
  commandTextArea->insertAt(*receivedLine, line, col);
  commandTextArea->setCursorPosition(commandTextArea->numLines(), 0);

  delete receivedLine;
}

/**
 * clear the output (slot)
 */
void
CommandDlg::clearOutput()
{
  commandTextArea->clear();
}

/**
 * install wait cursor on the main widget
 */
void
CommandDlg::installWaitCursor()
{
  commandTextArea->setCursor(waitCursor);
}

/**
 * reset wait cursor on the main widget
 */
void
CommandDlg::resetWaitCursor()
{
  commandTextArea->setCursor(arrowCursor);
}


/* ******************************************************************** */

CommandCfgDlg::CommandCfgDlg(const char *tcs, 
			     QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  // We will have to give this back to the ConfigWindow
  tabCaptionString = tcs;
  configGroupName = removeAmpersand(tcs);
}

QWidget *
CommandCfgDlg::makeWidget(QWidget *parent, bool makeLayouts)
{
#define SET_ADJUSTED_FIXED_SIZE(_wdgt) \
             _wdgt->setFixedSize(_wdgt->sizeHint());
	     
  KConfig   *kc = kapp->getConfig();
  int widgetWidth =  parent->fontMetrics().width("----------------------");
  int widgetHeight = 2*parent->fontMetrics().height();

  cfgWidget = new QWidget(parent);

  // We will have to give this back to the ConfigWindow
  kc->setGroup(configGroupName);

  // Widgets creation
  cfgBinGB = new QGroupBox(i18n("Binary"), cfgWidget);
  CHECK_PTR(cfgBinGB);

  cfgBinNameLE = new QLineEdit(cfgBinGB);
  CHECK_PTR(cfgBinNameLE);
  cfgBinNameLE->setMinimumSize(widgetWidth, widgetHeight);
  cfgBinNameLE->setMaximumSize(QLayout::unlimited, widgetHeight);
  
  cfgBinNameLbl = new QLabel(cfgBinNameLE, i18n("Path&name:"), cfgBinGB);
  CHECK_PTR(cfgBinNameLbl);
  SET_ADJUSTED_FIXED_SIZE(cfgBinNameLbl);

  cfgBinArgLE = new QLineEdit(cfgBinGB);
  CHECK_PTR(cfgBinArgLE);
  cfgBinArgLE->setMinimumSize(widgetWidth, widgetHeight);
  cfgBinArgLE->setMaximumSize(QLayout::unlimited, widgetHeight);
  
  cfgBinArgLbl = new QLabel(cfgBinArgLE, i18n("Additional &arguments:"),
			    cfgBinGB);
  CHECK_PTR(cfgBinArgLbl);
  SET_ADJUSTED_FIXED_SIZE(cfgBinArgLbl);

  cfgBinNameBrowse = new QPushButton(i18n("Browse..."), cfgBinGB);
  CHECK_PTR(cfgBinNameBrowse);
  cfgBinNameBrowse->setFixedSize(70, widgetHeight);
  connect(cfgBinNameBrowse, SIGNAL(clicked()), SLOT(slotBrowse()));
  
  /*
   * Have we to display a warning???
   */
  cfgWarning = 0;
  if (kc->readNumEntry("enable", 1) == 0) {
    // We have to display a warning
    cfgWarning = new QFrame(cfgWidget);
    CHECK_PTR(cfgWarning);

    cfgWarningPm = new QLabel(cfgWarning);
    CHECK_PTR(cfgWarningPm);
    cfgWarningPm->setPixmap(QMessageBox::standardIcon(QMessageBox::Warning, 
						      style()));
    SET_ADJUSTED_FIXED_SIZE(cfgWarningPm);

    cfgWarningLbl = new QLabel(i18n("This command had been disabled in "
				 "the configuration file."), cfgWarning);
    CHECK_PTR(cfgWarningLbl);
    SET_ADJUSTED_FIXED_SIZE(cfgWarningLbl);
    SET_ADJUSTED_FIXED_SIZE(cfgWarning);
    
    cfgWarningLayout = new QBoxLayout(cfgWarning, QBoxLayout::LeftToRight, 5);
    CHECK_PTR(cfgWarningLayout);
    cfgWarningLayout->addStretch(10);
    cfgWarningLayout->addWidget(cfgWarningPm, 0);
    cfgWarningLayout->addWidget(cfgWarningLbl, 0);
    cfgWarningLayout->addStretch(10);
  }
  
  // Layouts
  if (makeLayouts) {
    cfgLayoutTB = new QBoxLayout(cfgWidget, QBoxLayout::TopToBottom, 10);
    CHECK_PTR(cfgLayoutTB);
    
    if (cfgWarning != 0) {
      cfgLayoutTB->addWidget(cfgWarning);
      cfgWarningLayout->activate();
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

    cfgLayoutTB->addStretch(10);

    cfgWidget->adjustSize();
    cfgLayoutTB->activate();
    cfgWidget->adjustSize();
    cfgWidget->setMinimumSize(cfgWidget->size());
  }
  // Now, we read the configfile
  readConfig();
  
  return cfgWidget;
#undef SET_ADJUSTED_FIXED_SIZE
}

/**
 * Pseudo Destructor
 */
void
CommandCfgDlg::deleteWidget()
{
  delete cfgLayoutGB;
  delete cfgLayoutTB;
  if (cfgWarning != 0) {
    delete cfgWarning;
    cfgWarning = 0;
  }
  delete cfgBinNameLbl;
  delete cfgBinArgLbl;
  delete cfgBinNameLE;
  delete cfgBinArgLE;
}

CommandCfgDlg::~CommandCfgDlg()
{
}

/**
 * commit changes to the configfile
 * 
 * @return if the change have been done
 */
bool
CommandCfgDlg::commitChanges()
{
  QString s;
  KConfig *kc = kapp->getConfig();

  kc->setGroup(configGroupName);
  
  kc->writeEntry("path", cfgBinNameLE->text());
  kc->writeEntry("arguments", cfgBinArgLE->text());

  return(TRUE);
}

/**
 * cancel changes to the configfile
 */
void
CommandCfgDlg::cancelChanges()
{
  // nothing to do
}

/**
 * read the configfile
 */
void
CommandCfgDlg::readConfig()
{
  QString s;
  KConfig *kc = kapp->getConfig();

  kc->setGroup(configGroupName);
  
  if (kc->hasKey("path")) {
    s = kc->readEntry("path");
    cfgBinNameLE->setText(s);
  }
  if (kc->hasKey("arguments")) {
    s = kc->readEntry("arguments");
    cfgBinArgLE->setText(s);
  }
}

/**
 * read the configfile
 */
void
CommandCfgDlg::slotBrowse()
{
  QString f = KFileDialog::getOpenFileName( 0, 0L, this );
  if ( f.isNull() )
    return;
  
  cfgBinNameLE->setText(f.data());
}
