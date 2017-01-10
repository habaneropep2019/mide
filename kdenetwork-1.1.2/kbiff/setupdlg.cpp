/*
 * setupdlg.cpp
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the implementation of the setup dialog
 * class for KBiff.
 *
 * $Id: setupdlg.cpp,v 1.22.2.4 1999/08/24 22:17:27 granroth Exp $
 */
#include "setupdlg.h"

#include "Trace.h"

#if QT_VERSION < 140
#include <qgrpbox.h>
#include <qfileinf.h>
#include <qmsgbox.h>
#include <qlined.h>
#include <qchkbox.h>
#include <qpushbt.h>
#include <qcombo.h>
#else
#include <qgroupbox.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qheader.h>
#endif // QT_VERSION < 140

#include <qpixmap.h>
#include <qfont.h>
#include <qlabel.h>
#include <qstrlist.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qdict.h>
#include <qlist.h>

#include <kfiledialog.h>
#include <kapp.h>
#include <ktabctl.h>
#include <ksimpleconfig.h>
#include <kbiffurl.h>
#include <kfm.h>
#include <kprocess.h>
#include <kurllabel.h>

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#define CONFIG_FILE QString(KApplication::localconfigdir() + "/kbiffrc")

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_MAILDIR
#define _PATH_MAILDIR "/var/spool/mail"
#endif

///////////////////////////////////////////////////////////////////////////
// KBiffSetup
///////////////////////////////////////////////////////////////////////////
KBiffSetup::KBiffSetup(QString profile, bool secure)
	: QDialog(0, 0, true, 0)
{
TRACEINIT("KBiffSetup::KBiffSetup()");
	// set the icon just to be cute
	setIcon(ICON("kbiff.xpm"));

	// make sure the profile is *something*
	QString the_profile;
	if (profile.isEmpty())
		the_profile = getSomeProfile();
	else
		the_profile = profile;

	setCaption(i18n("KBiff Setup"));

	// mailbox groupbox
	QGroupBox* profile_groupbox = new QGroupBox(i18n("Profile:"), this);

	QBoxLayout *profile_layout = new QBoxLayout(profile_groupbox,
	                                            QBoxLayout::Down, 12, 5);
	profile_layout->addSpacing(8);

	// combo box to hold the profile names
	comboProfile = new QComboBox(false, profile_groupbox);
	comboProfile->setSizeLimit(10);
	comboProfile->setFixedHeight(comboProfile->sizeHint().height());
	profile_layout->addWidget(comboProfile);

	QBoxLayout *button_layout = new QBoxLayout(QBoxLayout::LeftToRight, 10);
	profile_layout->addLayout(button_layout);

	// add new profile button
	QPushButton *add_button = new QPushButton(i18n("&New..."), profile_groupbox);
	add_button->setFixedHeight(add_button->sizeHint().height());
	connect(add_button, SIGNAL(clicked()), SLOT(slotAddNewProfile()));
	button_layout->addWidget(add_button);
	button_layout->addSpacing(10);

	// rename current profile button
	QPushButton *rename_button = new QPushButton(i18n("&Rename..."),
	                                             profile_groupbox);
	rename_button->setFixedHeight(rename_button->sizeHint().height());
	connect(rename_button, SIGNAL(clicked()), SLOT(slotRenameProfile()));
	button_layout->addWidget(rename_button);
	button_layout->addSpacing(10);

	// delete current profile button
	QPushButton *delete_button = new QPushButton(i18n("&Delete"),
	                                             profile_groupbox);
	delete_button->setFixedHeight(delete_button->sizeHint().height());
	connect(delete_button, SIGNAL(clicked()), SLOT(slotDeleteProfile()));
	button_layout->addWidget(delete_button);

	//  add the profile name stuff into the main layout
	QBoxLayout *main_layout = new QBoxLayout(this, QBoxLayout::Down);
	main_layout->addWidget(profile_groupbox, 1);
	main_layout->addSpacing(5);

	// setup the tabs
	KTabCtl *tabctl = new KTabCtl(this);
	generalTab = new KBiffGeneralTab(the_profile, tabctl);
	newmailTab = new KBiffNewMailTab(the_profile, tabctl);
	mailboxTab = new KBiffMailboxTab(the_profile, tabctl);
	aboutTab   = new KBiffAboutTab(tabctl); 

	connect(comboProfile, SIGNAL(highlighted(const char *)),
	        generalTab, SLOT(readConfig(const char *)));
	connect(comboProfile, SIGNAL(highlighted(const char *)),
	        newmailTab, SLOT(readConfig(const char *)));
	connect(comboProfile, SIGNAL(highlighted(const char *)),
	        mailboxTab, SLOT(readConfig(const char *)));

	// add the tabs
	tabctl->addTab(generalTab, i18n("General"));
	tabctl->addTab(newmailTab, i18n("New Mail"));
	tabctl->addTab(mailboxTab, i18n("Mailbox"));
	tabctl->addTab(aboutTab, i18n("About"));

	// add the tab control to the main layout
	main_layout->addWidget(tabctl, 2);

	QBoxLayout *bottom_layout = new QBoxLayout(QBoxLayout::LeftToRight, 12);

	// and add the bottom buttons to the main layout
	main_layout->addSpacing(5);
	main_layout->addLayout(bottom_layout, 1);
	main_layout->addSpacing(5);
	main_layout->activate();

	// help button
	help = new QPushButton(i18n("Help"), this);
	help->setMinimumSize(help->sizeHint());
	connect(help, SIGNAL(clicked()), SLOT(invokeHelp()));
	bottom_layout->addSpacing(5);
	bottom_layout->addWidget(help);
	bottom_layout->addSpacing(50);

	// ok button
	ok = new QPushButton(i18n("OK"), this);
	ok->setDefault(true);
	ok->setMinimumSize(ok->sizeHint());
	connect(ok, SIGNAL(clicked()), SLOT(slotDone()));
	bottom_layout->addWidget(ok);

	// cancel button
	cancel = new QPushButton(i18n("Cancel"), this);
	cancel->setMinimumSize(cancel->sizeHint());
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));
	bottom_layout->addWidget(cancel);
	bottom_layout->addSpacing(5);

	// are we secure
	isSecure = secure;

	profile_layout->activate();
	main_layout->activate();

	readConfig(the_profile);
}

KBiffSetup::~KBiffSetup()
{
}

const bool KBiffSetup::getSecure() const
{
	return isSecure;
}

const QString KBiffSetup::getProfile() const
{
	return comboProfile->currentText();
}

const KBiffURL KBiffSetup::getCurrentMailbox() const
{
	return mailboxTab->getMailbox();
}

const QList<KBiffURL> KBiffSetup::getMailboxList() const
{
	return mailboxTab->getMailboxList();
}

const char* KBiffSetup::getMailClient() const
{
	return generalTab->getMailClient();
}

const char* KBiffSetup::getNoMailIcon() const
{
	return generalTab->getButtonNoMail();
}

const char* KBiffSetup::getNewMailIcon() const
{
	return generalTab->getButtonNewMail();
}

const char* KBiffSetup::getOldMailIcon() const
{
	return generalTab->getButtonOldMail();
}

const char* KBiffSetup::getNoConnIcon() const
{
	return generalTab->getButtonNoConn();
}

const bool KBiffSetup::getSessionManagement() const
{
	return generalTab->getSessionManagement();
}

const bool KBiffSetup::getDock() const
{
	return generalTab->getDock();
}

const unsigned int KBiffSetup::getPoll() const
{
	return generalTab->getPoll();
}

const char* KBiffSetup::getRunCommandPath() const
{
	return newmailTab->getRunCommandPath();
}

const char* KBiffSetup::getPlaySoundPath() const
{
	return newmailTab->getPlaySoundPath();
}

const bool KBiffSetup::getRunCommand() const
{
	return newmailTab->getRunCommand();
}

const bool KBiffSetup::getPlaySound() const
{
	return newmailTab->getPlaySound();
}

const bool KBiffSetup::getSystemBeep() const
{
	return newmailTab->getSystemBeep();
}

const bool KBiffSetup::getNotify() const
{
	return newmailTab->getNotify();
}

const bool KBiffSetup::getStatus() const
{
	return newmailTab->getStatus();
}

void KBiffSetup::invokeHelp()
{
	kapp->invokeHTMLHelp("kbiff/kbiff.html", "");
}

void KBiffSetup::readConfig(const char* profile)
{
	QStrList profile_list;

	// open the config file
	KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE, true);

	config->setGroup("General");

	// read in the mailboxes
	int number_of_mailboxes = config->readListEntry("Profiles", profile_list);
	delete config;

	// check if we have any mailboxes to read in
	if (number_of_mailboxes > 0)
	{
		comboProfile->clear();

		// load up the combo box
		comboProfile->insertStrList(&profile_list);

		// read in the data from the first mailbox if we don't have a name
		for (int i = 0; i < comboProfile->count(); i++)
		{
			if (QString(profile) == comboProfile->text(i))
			{
				comboProfile->setCurrentItem(i);
				break;
			}
		}
	}
	else
		comboProfile->insertItem(profile);
}

QString KBiffSetup::getSomeProfile() const
{
TRACEINIT("KBiffSetup::getSomeProfile()");
	QStrList profile_list;

	// open the config file
	KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE, true);
	config->setGroup("General");

	// read in the mailboxes
	int number_of_mailboxes = config->readListEntry("Profiles", profile_list);
	delete config;

TRACEF("first profile = %s", profile_list.getLast());
	// get the first mailbox if it exists
	if (number_of_mailboxes > 0)
		return QString(profile_list.getLast());
	else
		return QString("Inbox");
}

void KBiffSetup::saveConfig()
{
	// open the config file for writing
	KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE);

	config->setGroup("General");

	// get the list of profiles
	QStrList profile_list;
	for (int i = 0; i < comboProfile->count(); i++)
		profile_list.append(comboProfile->text(i));

	// write the mailboxes
	config->writeEntry("Profiles", profile_list);

	delete config;
}

///////////////////////////////////////////////////////////////////////
// Protected slots
///////////////////////////////////////////////////////////////////////
void KBiffSetup::slotDone()
{
TRACEINIT("KBiffSetup::slotDone()");
	QString profile = comboProfile->currentText();
	saveConfig();
	generalTab->saveConfig(profile);
	newmailTab->saveConfig(profile);
	mailboxTab->saveConfig(profile);
	accept();
}

void KBiffSetup::slotAddNewProfile()
{
	KBiffNewDlg dlg;

	// popup the name chooser
	dlg.setCaption(i18n("New Profile"));
	if (dlg.exec())
	{
		QString profile_name = dlg.getName();

		// bail out if we already have this name
		for (int i = 0; i < comboProfile->count(); i++)
		{
			if (profile_name == comboProfile->text(i))
				return;
		}

		// continue only if we received a decent name
		if (profile_name.isEmpty() == false)
		{
			comboProfile->insertItem(profile_name, 0);

			saveConfig();
			readConfig(profile_name);
			generalTab->readConfig(profile_name);
			newmailTab->readConfig(profile_name);
			mailboxTab->readConfig(profile_name);
		}
	}
}

void KBiffSetup::slotRenameProfile()
{
	KBiffNewDlg dlg;
	QString title;
	QString old_profile = comboProfile->currentText();
	
	title.sprintf(i18n("Rename Profile: %s"), (const char*)old_profile);
	dlg.setCaption(title);
	// popup the name chooser
	if (dlg.exec())
	{
		QString profile_name = dlg.getName();

		// bail out if we already have this name
		for (int i = 0; i < comboProfile->count(); i++)
		{
			if (profile_name == comboProfile->text(i))
				return;
		}

		// continue only if we received a decent name
		if (profile_name.isNull() == false)
		{
			comboProfile->removeItem(comboProfile->currentItem());
			comboProfile->insertItem(profile_name, 0);

			// remove the reference from the config file
			KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE);
			// nuke the group
			config->deleteGroup(old_profile, true);
			delete config;

			// now save the profile settings
			saveConfig();
			generalTab->saveConfig(profile_name);
			newmailTab->saveConfig(profile_name);
			mailboxTab->saveConfig(profile_name);
		}
	}
}

void KBiffSetup::slotDeleteProfile()
{
	QString title, msg;
	QString profile = comboProfile->currentText();
	
	title.sprintf(i18n("Delete Profile: %s"), (const char*)profile);
	msg.sprintf(i18n("Are you sure you wish to delete this profile?\n"),
	             (const char*)profile);
	
	switch (QMessageBox::warning(this, title, msg,
	                             QMessageBox::Yes | QMessageBox::Default,
	                             QMessageBox::No | QMessageBox::Escape))
	{
		case QMessageBox::Yes:
		{
			comboProfile->removeItem(comboProfile->currentItem());

			saveConfig();

			// remove the reference from the config file
			KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE);
			// nuke the group
			config->deleteGroup(profile, true);
			delete config;

			if (comboProfile->count() == 0)
			{
				readConfig("Inbox");
				generalTab->readConfig("Inbox");
				newmailTab->readConfig("Inbox");
				mailboxTab->readConfig("Inbox");
			}
			else
			{
				readConfig(comboProfile->currentText());
				generalTab->readConfig(comboProfile->currentText());
				newmailTab->readConfig(comboProfile->currentText());
				mailboxTab->readConfig(comboProfile->currentText());
			}

			break;
		}
		case QMessageBox::No:
		default:
			break;
	}
}
///////////////////////////////////////////////////////////////////////
// Protected (non-slot) functions
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// KBiffGeneralTab
///////////////////////////////////////////////////////////////////////
KBiffGeneralTab::KBiffGeneralTab(const char* profile, QWidget *parent)
	: QWidget(parent)
{
TRACEINIT("KBiffGeneralTab::KBiffGeneralTab()");
TRACEF("profile = %s", profile);
	QVBoxLayout *top_layout = new QVBoxLayout(this, 12, 5);
	QGridLayout *top_grid = new QGridLayout(4, 3);
	top_layout->addLayout(top_grid);

	top_grid->setColStretch(2, 1);

	// the poll time (in seconds)
	QLabel* poll_label = new QLabel(i18n("Poll (sec):"), this);
	poll_label->setMinimumSize(poll_label->sizeHint());
	top_grid->addWidget(poll_label, 0, 0);

	// the command to run when clicked
	QLabel *mail_label = new QLabel(i18n("Mail client:"), this);
	mail_label->setMinimumSize(mail_label->sizeHint());
	top_grid->addWidget(mail_label, 1, 0);

	editPoll = new QLineEdit(this);
	editPoll->setMinimumSize(editPoll->sizeHint());
	top_grid->addWidget(editPoll, 0, 1);

	editCommand = new QLineEdit(this);
	editCommand->setMinimumSize(editCommand->sizeHint());
	top_grid->addMultiCellWidget(editCommand, 1, 1, 1, 2);

	// do we dock automatically?
	checkDock = new QCheckBox(i18n("Dock in panel"), this);
	checkDock->setMinimumSize(checkDock->sizeHint());
	top_grid->addMultiCellWidget(checkDock, 2, 2, 1, 2);

	// should we support session management?
	checkNoSession = new QCheckBox(i18n("Use session management"), this);
	checkNoSession->setMinimumSize(checkNoSession->sizeHint());
	top_grid->addMultiCellWidget(checkNoSession, 3, 3, 1, 2);

	// group box to hold the icons together
	QGroupBox* icons_groupbox = new QGroupBox(i18n("Icons:"), this);
	top_layout->addWidget(icons_groupbox);

	// layout to hold the icons inside the groupbox
	QVBoxLayout *icon_layout = new QVBoxLayout(icons_groupbox, 8);
	icon_layout->addSpacing(8);

	QGridLayout *icon_grid = new QGridLayout(2, 4);
	icon_layout->addLayout(icon_grid);

	icon_grid->setColStretch(0, 1);
	icon_grid->setColStretch(1, 1);
	icon_grid->setColStretch(2, 1);

	QBoxLayout *no_layout1 = new QBoxLayout(QBoxLayout::LeftToRight);
	icon_grid->addLayout(no_layout1, 0, 0);

	// "no mail" pixmap button
	QLabel* nomail_label = new QLabel(i18n("No Mail:"), icons_groupbox);
	nomail_label->setMinimumSize(nomail_label->sizeHint());
	no_layout1->addStretch(1);
	no_layout1->addWidget(nomail_label);
	no_layout1->addStretch(1);

	QBoxLayout *old_layout1 = new QBoxLayout(QBoxLayout::LeftToRight);
	icon_grid->addLayout(old_layout1, 0, 1);
	// "old mail" pixmap button
	QLabel* oldmail_label = new QLabel(i18n("Old Mail"), icons_groupbox);
	oldmail_label->setMinimumSize(oldmail_label->sizeHint());
	old_layout1->addStretch(1);
	old_layout1->addWidget(oldmail_label);
	old_layout1->addStretch(1);

	QBoxLayout *new_layout1 = new QBoxLayout(QBoxLayout::LeftToRight);
	icon_grid->addLayout(new_layout1, 0, 2);

	// "new mail" pixmap button
	QLabel* newmail_label = new QLabel(i18n("New Mail:"), icons_groupbox);
	newmail_label->setMinimumSize(newmail_label->sizeHint());
	new_layout1->addStretch(1);
	new_layout1->addWidget(newmail_label);
	new_layout1->addStretch(1);

	QBoxLayout *noconn_layout1 = new QBoxLayout(QBoxLayout::LeftToRight);
	icon_grid->addLayout(noconn_layout1, 0, 3);

	// "no conn" pixmap button
	QLabel* noconn_label = new QLabel(i18n("No Conn.:"), icons_groupbox);
	noconn_label->setMinimumSize(noconn_label->sizeHint());
	noconn_layout1->addStretch(1);
	noconn_layout1->addWidget(noconn_label);
	noconn_layout1->addStretch(1);

	QBoxLayout *no_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	icon_grid->addLayout(no_layout, 1, 0);

	buttonNoMail = new KIconLoaderButton(icons_groupbox);
	buttonNoMail->setFixedSize(50, 50);
	no_layout->addStretch(1);
	no_layout->addWidget(buttonNoMail);
	no_layout->addStretch(1);

	QBoxLayout *old_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	icon_grid->addLayout(old_layout, 1, 1);

	buttonOldMail = new KIconLoaderButton(icons_groupbox);
	buttonOldMail->setFixedSize(50, 50);
	old_layout->addStretch(1);
	old_layout->addWidget(buttonOldMail);
	old_layout->addStretch(1);

	QBoxLayout *new_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	icon_grid->addLayout(new_layout, 1, 2);

	buttonNewMail = new KIconLoaderButton(icons_groupbox);
	buttonNewMail->setFixedSize(50, 50);
	new_layout->addStretch(1);
	new_layout->addWidget(buttonNewMail);
	new_layout->addStretch(1);

	QBoxLayout *noconn_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	icon_grid->addLayout(noconn_layout, 1, 3);

	buttonNoConn = new KIconLoaderButton(icons_groupbox);
	buttonNoConn->setFixedSize(50, 50);
	noconn_layout->addStretch(1);
	noconn_layout->addWidget(buttonNoConn);
	noconn_layout->addStretch(1);

	icon_layout->activate();
	top_layout->activate();

	readConfig(profile);
}

KBiffGeneralTab::~KBiffGeneralTab()
{
}

const bool KBiffGeneralTab::getSessionManagement() const
{
	return checkNoSession->isChecked();
}

const bool KBiffGeneralTab::getDock() const
{
	return checkDock->isChecked();
}

const char* KBiffGeneralTab::getButtonOldMail() const
{
	return buttonOldMail->icon();
}

const char* KBiffGeneralTab::getButtonNewMail() const
{
	return buttonNewMail->icon();
}

const char* KBiffGeneralTab::getButtonNoMail() const
{
	return buttonNoMail->icon();
}

const char* KBiffGeneralTab::getButtonNoConn() const
{
	return buttonNoConn->icon();
}

const char* KBiffGeneralTab::getMailClient() const
{
	return editCommand->text();
}

const int KBiffGeneralTab::getPoll() const
{
	return QString(editPoll->text()).toInt();
}

void KBiffGeneralTab::readConfig(const char* profile)
{
TRACEINIT("KBiffGeneralTab::readConfig()");
	// open the config file
	KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE, true);

	config->setGroup(profile);

	editPoll->setText(config->readEntry("Poll", "60"));
	editCommand->setText(config->readEntry("MailClient", "kmail -check"));
	checkDock->setChecked(config->readBoolEntry("Docked", true));
	checkNoSession->setChecked(config->readBoolEntry("Sessions", true));

	QString no_mail, old_mail, new_mail, no_conn;
	no_mail = config->readEntry("NoMailPixmap", "nomail.xpm");
	old_mail = config->readEntry("OldMailPixmap", "oldmail.xpm");
	new_mail = config->readEntry("NewMailPixmap", "newmail.xpm");
	no_conn = config->readEntry("NoConnPixmap", "noconn.xpm");

	buttonOldMail->setIcon(old_mail);
	buttonOldMail->setPixmap(ICON(old_mail));
	buttonNewMail->setIcon(new_mail);
	buttonNewMail->setPixmap(ICON(new_mail));
	buttonNoMail->setIcon(no_mail);
	buttonNoMail->setPixmap(ICON(no_mail));
	buttonNoConn->setIcon(no_conn);
	buttonNoConn->setPixmap(ICON(no_conn));

	delete config;
}

void KBiffGeneralTab::saveConfig(const char *profile)
{
TRACEINIT("KBiffGeneralTab::saveConfig()");
	// open the config file for writing
	KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE);

	config->setGroup(profile);

	config->writeEntry("Poll", editPoll->text());
	config->writeEntry("MailClient", editCommand->text());
	config->writeEntry("Docked", checkDock->isChecked());
	config->writeEntry("Sessions", checkNoSession->isChecked());
	config->writeEntry("NoMailPixmap", buttonNoMail->icon());
	config->writeEntry("NewMailPixmap", buttonNewMail->icon());
	config->writeEntry("OldMailPixmap", buttonOldMail->icon());
	config->writeEntry("NoConnPixmap", buttonNoConn->icon());
	delete config;
}

///////////////////////////////////////////////////////////////////////
// KBiffNewMailTab
///////////////////////////////////////////////////////////////////////
KBiffNewMailTab::KBiffNewMailTab(const char* profile, QWidget *parent)
	: QWidget(parent)
{
TRACEINIT("KBiffNewMailTab::KBiffNewMailTab()");
	hasAudio = (audioServer.serverStatus() == 0) ? true : false;

	QBoxLayout *top_layout = new QBoxLayout(this, QBoxLayout::Down, 12, 5);

	QGridLayout *grid = new QGridLayout(4, 3);
	top_layout->addLayout(grid);

	// setup the Run Command stuff
	checkRunCommand = new QCheckBox(i18n("Run Command"), this);
	checkRunCommand->setMinimumSize(checkRunCommand->sizeHint());
	grid->addMultiCellWidget(checkRunCommand, 0, 0, 0, 1);

	editRunCommand = new QLineEdit(this);
	grid->addMultiCellWidget(editRunCommand, 1, 1, 0, 1);

	buttonBrowseRunCommand = new QPushButton(i18n("Browse"), this);
	buttonBrowseRunCommand->setMinimumSize(75, 25);
	buttonBrowseRunCommand->setMaximumSize(buttonBrowseRunCommand->sizeHint());
	grid->addWidget(buttonBrowseRunCommand, 1, 2);

	// setup the Play Sound stuff
	checkPlaySound = new QCheckBox(i18n("Play Sound"), this);
	checkPlaySound->setMinimumSize(checkPlaySound->sizeHint());
	grid->addWidget(checkPlaySound, 2, 0);

	buttonTestPlaySound = new QPushButton(this);
	buttonTestPlaySound->setPixmap(ICON("playsound.xpm"));
	buttonTestPlaySound->setMinimumSize(14, 14);
	buttonTestPlaySound->setMaximumSize(18, buttonTestPlaySound->sizeHint().height());
	connect(buttonTestPlaySound, SIGNAL(clicked()), SLOT(testPlaySound()));
	grid->addWidget(buttonTestPlaySound, 3, 0);

	editPlaySound = new QLineEdit(this);
	grid->addWidget(editPlaySound, 3, 1);

	buttonBrowsePlaySound = new QPushButton(i18n("Browse"), this);
	buttonBrowsePlaySound->setMinimumSize(75, 25);
	buttonBrowsePlaySound->setMaximumSize(buttonBrowsePlaySound->sizeHint());
	grid->addWidget(buttonBrowsePlaySound, 3, 2);

	// setup the System Sound stuff
	checkBeep = new QCheckBox(i18n("System Beep"), this);
	checkBeep->setMinimumSize(checkBeep->sizeHint());
	top_layout->addWidget(checkBeep);

	// setup the System Sound stuff
	checkNotify = new QCheckBox(i18n("Notify"), this);
	checkNotify->setMinimumSize(checkNotify->sizeHint());
	top_layout->addWidget(checkNotify);

	checkStatus = new QCheckBox(i18n("Floating Status"), this);
	checkStatus->setMinimumSize(checkStatus->sizeHint());
	top_layout->addWidget(checkStatus);

	top_layout->addStretch(1);

	// connect some slots and signals
	connect(buttonBrowsePlaySound, SIGNAL(clicked()),
	                                 SLOT(browsePlaySound()));
	connect(buttonBrowseRunCommand, SIGNAL(clicked()),
	                                  SLOT(browseRunCommand()));
	connect(checkPlaySound, SIGNAL(toggled(bool)),
	                          SLOT(enablePlaySound(bool)));
	connect(checkRunCommand, SIGNAL(toggled(bool)),
	                           SLOT(enableRunCommand(bool)));

	readConfig(profile);
}

KBiffNewMailTab::~KBiffNewMailTab()
{
}

void KBiffNewMailTab::testPlaySound()
{
TRACEINIT("KBiffNewMailTab::testPlaySound()");
	if (hasAudio)
	{
		TRACE("I have audio!");
		audioServer.play(getPlaySoundPath());
		audioServer.sync();
	}
}

void KBiffNewMailTab::readConfig(const char* profile)
{
TRACEINIT("KBiffNewMailTab::readConfig()");
	// open the config file
	KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE, true);

	TRACEF("profile = %s", profile);
	config->setGroup(profile);

	checkRunCommand->setChecked(config->readBoolEntry("RunCommand", false));
	checkPlaySound->setChecked(config->readBoolEntry("PlaySound", false));
	checkBeep->setChecked(config->readBoolEntry("SystemBeep", true));
	checkNotify->setChecked(config->readBoolEntry("Notify", true));
	checkStatus->setChecked(config->readBoolEntry("Status", true));
	editRunCommand->setText(config->readEntry("RunCommandPath"));
	editPlaySound->setText(config->readEntry("PlaySoundPath"));

	enableRunCommand(checkRunCommand->isChecked());
	enablePlaySound(checkPlaySound->isChecked());

	delete config;
}

void KBiffNewMailTab::saveConfig(const char *profile)
{
TRACEINIT("KBiffNewMailTab::saveConfig()");
	KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE);

	config->setGroup(profile);

	config->writeEntry("RunCommand", checkRunCommand->isChecked());
	config->writeEntry("PlaySound", checkPlaySound->isChecked());
	config->writeEntry("SystemBeep", checkBeep->isChecked());
	config->writeEntry("Notify", checkNotify->isChecked());
	config->writeEntry("Status", checkStatus->isChecked());
	config->writeEntry("RunCommandPath", editRunCommand->text());
	config->writeEntry("PlaySoundPath", editPlaySound->text());

	delete config;
}

const bool KBiffNewMailTab::getRunCommand() const
{
	return checkRunCommand->isChecked();
}

const char* KBiffNewMailTab::getRunCommandPath() const
{
	return editRunCommand->text();
}

const bool KBiffNewMailTab::getPlaySound() const
{
	return checkPlaySound->isChecked();
}

const char* KBiffNewMailTab::getPlaySoundPath() const
{
	return editPlaySound->text();
}

const bool KBiffNewMailTab::getSystemBeep() const
{
	return checkBeep->isChecked();
}

const bool KBiffNewMailTab::getNotify() const
{
	return checkNotify->isChecked();
}

const bool KBiffNewMailTab::getStatus() const
{
	return checkStatus->isChecked();
}

void KBiffNewMailTab::enableRunCommand(bool enable)
{
	editRunCommand->setEnabled(enable);
	buttonBrowseRunCommand->setEnabled(enable);
}

void KBiffNewMailTab::enablePlaySound(bool enable)
{
	editPlaySound->setEnabled(enable);
	buttonBrowsePlaySound->setEnabled(enable);
	buttonTestPlaySound->setEnabled(enable);
}

void KBiffNewMailTab::browseRunCommand()
{
TRACEINIT("KBiffNewMailTab::browseRunCommand()");
	QString command_path = KFileDialog::getOpenFileName();
	if (!command_path.isEmpty() && !command_path.isNull())
	{
		editRunCommand->setText(command_path);
	}
}

void KBiffNewMailTab::browsePlaySound()
{
TRACEINIT("KBiffNewMailTab::browsePlaySound()");
	QString sound_path = KFileDialog::getOpenFileName();
	if (!sound_path.isEmpty() && !sound_path.isNull())
	{
		editPlaySound->setText(sound_path);
	}
}

//////////////////////////////////////////////////////////////////////
// KBiffMailboxTab
//////////////////////////////////////////////////////////////////////
KBiffMailboxAdvanced::KBiffMailboxAdvanced()
	: QDialog(0, 0, true, 0)
{
TRACEINIT("KBiffMailboxAdvanced::KBiffMailboxAdvanced()");
	setCaption(i18n("Advanced Options"));

	QGridLayout *layout = new QGridLayout(this, 6, 3, 12, 5);

	QLabel *mailbox_label = new QLabel(i18n("Mailbox URL:"), this);
	mailbox_label->setMinimumSize(mailbox_label->sizeHint());
	layout->addWidget(mailbox_label, 0, 0);

	QLabel *port_label = new QLabel(i18n("Port:"), this);
	port_label->setMinimumSize(port_label->sizeHint());
	layout->addWidget(port_label, 1, 0);

	preauth = new QCheckBox(i18n("PREAUTH"), this);
	preauth->setMinimumSize(preauth->sizeHint());
	preauth->setEnabled(false);
	layout->addWidget(preauth, 2, 1);

	connect(preauth, SIGNAL(toggled(bool)), SLOT(preauthModified(bool)));

	keepalive = new QCheckBox(i18n("Keep Alive"), this);
	keepalive->setMinimumSize(keepalive->sizeHint());
	keepalive->setEnabled(false);
	layout->addWidget(keepalive, 3, 1);

	connect(keepalive, SIGNAL(toggled(bool)), SLOT(keepaliveModified(bool)));

	async = new QCheckBox(i18n("Asynchronous"), this);
	async->setMinimumSize(async->sizeHint());
	async->setEnabled(false);
	layout->addWidget(async, 4, 1);

	connect(async, SIGNAL(toggled(bool)), SLOT(asyncModified(bool)));

	mailbox = new QLineEdit(this);
	mailbox->setMinimumHeight(25);
	layout->addMultiCellWidget(mailbox, 0, 0, 1, 2);

	port = new QLineEdit(this);
	port->setMinimumSize(50, 25);
	layout->addWidget(port, 1, 1);

	connect(port, SIGNAL(textChanged(const char*)),
	              SLOT(portModified(const char*)));

	QBoxLayout *button_layout = new QBoxLayout(QBoxLayout::LeftToRight, 12);
	layout->addLayout(button_layout, 5, 2);

	QPushButton *ok = new QPushButton(i18n("OK"), this);
	ok->setMinimumSize(ok->sizeHint());
	ok->setDefault(true);
	button_layout->addWidget(ok);
	connect(ok, SIGNAL(clicked()), SLOT(accept()));

	QPushButton *cancel = new QPushButton(i18n("Cancel"), this);
	cancel->setMinimumSize(cancel->sizeHint());
	button_layout->addWidget(cancel);
	connect(cancel, SIGNAL(clicked()), SLOT(reject()));

	layout->activate();
	resize(sizeHint());
}

KBiffMailboxAdvanced::~KBiffMailboxAdvanced()
{
TRACEINIT("KBiffMailboxAdvanced::~KBiffMailboxAdvanced()");
}

const KBiffURL KBiffMailboxAdvanced::getMailbox() const
{
	KBiffURL url(mailbox->text());
	url.setPassword(password);
	return url;
}

const unsigned int KBiffMailboxAdvanced::getPort() const
{
	return QString(port->text()).toInt();
}

void KBiffMailboxAdvanced::setMailbox(const KBiffURL& url)
{
	password = url.passwd();
	KBiffURL new_url(url);
	new_url.setPassword("");
	mailbox->setText(new_url.url());
}

void KBiffMailboxAdvanced::setPort(unsigned int the_port, bool enable)
{
	port->setEnabled(enable);
	port->setText(QString().setNum(the_port));
}

void KBiffMailboxAdvanced::portModified(const char *text)
{
	KBiffURL url = getMailbox();
	url.setPort(QString(text).toInt());
	setMailbox(url);
}

void KBiffMailboxAdvanced::preauthModified(bool is_preauth)
{
TRACEINIT("KBiffMailboxAdvanced::preauthModified()");
	KBiffURL url = getMailbox();
	if (is_preauth)
		url.setSearchPart("preauth");
	else
		url.setSearchPart("");
TRACEF("url = %s", url.url().data());
	setMailbox(url);
}

void KBiffMailboxAdvanced::keepaliveModified(bool is_keepalive)
{
	KBiffURL url = getMailbox();
	if (is_keepalive)
	{
		setAsync(false);
		url.setReference("keepalive");
	}
	else
		url.setReference("");
	setMailbox(url);
}

void KBiffMailboxAdvanced::asyncModified(bool is_async)
{
	KBiffURL url = getMailbox();
	if (is_async)
	{
		setKeepalive(false);
		url.setReference("async");
	}
	else
		url.setReference("");
	setMailbox(url);
}

void KBiffMailboxAdvanced::setPreauth(bool on)
{
	preauth->setEnabled(true);
	preauth->setChecked(on);
}

void KBiffMailboxAdvanced::setKeepalive(bool on)
{
	keepalive->setEnabled(true);
	keepalive->setChecked(on);
}

void KBiffMailboxAdvanced::setAsync(bool on)
{
	async->setEnabled(true);
       	async->setChecked(on);
}

bool KBiffMailboxAdvanced::getPreauth() const
{
	return preauth->isChecked();
}

KBiffMailboxTab::KBiffMailboxTab(const char* profile, QWidget *parent)
	: QWidget(parent), mailboxHash(new QDict<KBiffMailbox>)
{
TRACEINIT("KBiffMailboxTab::KBiffMailboxTab()");
	if (mailboxHash)
		mailboxHash->setAutoDelete(true);

	QHBoxLayout *top_layout = new QHBoxLayout(this, 12, 5);

	QGridLayout *list_layout = new QGridLayout(2, 2);
	top_layout->addLayout(list_layout);

	mailboxes = new QListView(this);
	mailboxes->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	mailboxes->addColumn(i18n("Mailbox:"));
	mailboxes->header()->hide();
TRACE("Set mailboxes");
	list_layout->addMultiCellWidget(mailboxes, 0, 0, 0, 1);
	list_layout->setRowStretch(0, 1);

	connect(mailboxes, SIGNAL(selectionChanged(QListViewItem *)),
	                   SLOT(slotMailboxSelected(QListViewItem *)));

	QPushButton *new_mailbox = new QPushButton(this);
	new_mailbox->setPixmap(ICON("mailbox.xpm"));
	new_mailbox->setFixedSize(35, 20);
	connect(new_mailbox, SIGNAL(clicked()), SLOT(slotNewMailbox()));
	QToolTip::add(new_mailbox, i18n("New Mailbox"));
	list_layout->addWidget(new_mailbox, 1, 0); 

	QPushButton *delete_mailbox = new QPushButton(this);
	delete_mailbox->setPixmap(ICON("delete.xpm"));
	delete_mailbox->setFixedSize(35, 20);
	connect(delete_mailbox, SIGNAL(clicked()), SLOT(slotDeleteMailbox()));
	QToolTip::add(delete_mailbox, i18n("Delete Mailbox"));
	list_layout->addWidget(delete_mailbox, 1, 1); 

	QGridLayout *grid = new QGridLayout(8, 4);
	top_layout->addLayout(grid);

	QLabel *protocol_label = new QLabel(i18n("Protocol:"), this);
	protocol_label->setMinimumSize(protocol_label->sizeHint());

	QLabel *mailbox_label = new QLabel(i18n("Mailbox:"), this);
	mailbox_label->setMinimumSize(mailbox_label->sizeHint());

	QLabel *server_label = new QLabel(i18n("Server:"), this);
	server_label->setMinimumSize(server_label->sizeHint());

	QLabel *user_label = new QLabel(i18n("User:"), this);
	user_label->setMinimumSize(user_label->sizeHint());

	QLabel *password_label = new QLabel(i18n("Password:"), this);
	password_label->setMinimumSize(password_label->sizeHint());

	comboProtocol = new QComboBox(this);
	comboProtocol->insertItem("");
	comboProtocol->insertItem("mbox");
	comboProtocol->insertItem("maildir");	
	comboProtocol->insertItem("imap4");
	comboProtocol->insertItem("pop3");
	comboProtocol->insertItem("mh");
	comboProtocol->insertItem("file");
	comboProtocol->insertItem("nntp");
	comboProtocol->setMinimumSize(comboProtocol->sizeHint());

	connect(comboProtocol, SIGNAL(highlighted(int)),
	                       SLOT(protocolSelected(int)));

	buttonBrowse = new QPushButton(i18n("Browse"), this);
	buttonBrowse->setMinimumSize(75, 25);
	connect(buttonBrowse, SIGNAL(clicked()), SLOT(browse()));

	editMailbox = new QLineEdit(this);
	editMailbox->setMinimumHeight(editMailbox->sizeHint().height());

	editServer = new QLineEdit(this);
	editServer->setMinimumHeight(editServer->sizeHint().height());

	editUser = new QLineEdit(this);
	editUser->setMinimumHeight(editUser->sizeHint().height());

	editPassword = new QLineEdit(this);
	editPassword->setMinimumHeight(editPassword->sizeHint().height());
	editPassword->setEchoMode(QLineEdit::Password);

	checkStorePassword = new QCheckBox(i18n("Store password"), this);
	checkStorePassword->setMinimumSize(checkStorePassword->sizeHint());

	QPushButton *advanced_button = new QPushButton(i18n("Advanced"), this);
	advanced_button->setMinimumSize(advanced_button->sizeHint().width(), 25);
	connect(advanced_button, SIGNAL(clicked()), SLOT(advanced()));

	grid->addWidget(protocol_label, 0, 1);
	grid->addWidget(mailbox_label, 1, 1);
	grid->addWidget(server_label, 3, 1);
	grid->addWidget(user_label, 4, 1);
	grid->addWidget(password_label, 5, 1);

	grid->addWidget(comboProtocol, 0, 2);
	grid->addWidget(buttonBrowse, 0, 3);
	grid->addMultiCellWidget(editMailbox, 1, 1, 2, 3);
	grid->addMultiCellWidget(editServer, 3, 3, 2, 3);
	grid->addMultiCellWidget(editUser, 4, 4, 2, 3);
	grid->addMultiCellWidget(editPassword, 5, 5, 2, 3);
	grid->addMultiCellWidget(checkStorePassword, 6, 6, 2, 3);
	grid->addWidget(advanced_button, 7, 3);

	top_layout->activate();

	readConfig(profile);
}

KBiffMailboxTab::~KBiffMailboxTab()
{
TRACEINIT("KBiffMailboxTab::~KBiffMailboxTab()");
	delete mailboxHash;
}

void KBiffMailboxTab::readConfig(const char* profile)
{
TRACEINIT("KBiffMailboxTab::readConfig()");
	// initialize some variables that need initing
	oldItem = 0;

	// open the config file
	KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE, true);
	config->setDollarExpansion(false);

TRACE("Before clears");
	mailboxHash->clear();
	mailboxes->clear();
TRACE("After clears");

	config->setGroup(profile);

	QStrList mailbox_list;

	int number_of_mailboxes = config->readListEntry("Mailboxes", mailbox_list);

	if (number_of_mailboxes > 0)
	{
		for (unsigned int i = 0; i < mailbox_list.count(); i+=3)
		{
			KBiffMailbox *mailbox = new KBiffMailbox();
			QString key(mailbox_list.at(i));
			mailbox->url = KBiffURL(mailbox_list.at(i+1));
			QString password(scramble(mailbox_list.at(i+2), false));

			if (password.isEmpty())
				mailbox->store = false;
			else
			{
				mailbox->store = true;
				mailbox->url.setPassword(password.data());
			}

			QListViewItem *item = new QListViewItem(mailboxes, key);
			item->setPixmap(0, QPixmap(ICON("mailbox.xpm")));

			mailboxHash->insert(key.data(), mailbox);
		}
	}
	else
	{
		KBiffMailbox *mailbox = new KBiffMailbox();
		mailbox->store = false;
		mailbox->url = defaultMailbox();
		mailboxHash->insert("Default", mailbox);

		QListViewItem *item = new QListViewItem(mailboxes, i18n("Default"));
		item->setPixmap(0, QPixmap(ICON("mailbox.xpm")));
	}

	mailboxes->setSelected(mailboxes->firstChild(), true);
	delete config;
}

void KBiffMailboxTab::saveConfig(const char *profile)
{
TRACEINIT("KBiffMailboxTab::saveConfig()");
	// open the config file
	KSimpleConfig *config = new KSimpleConfig(CONFIG_FILE);
	config->setDollarExpansion(false);

	config->setGroup(profile);

	QStrList mailbox_list;

	for (QListViewItem *item = mailboxes->firstChild();
	     item;
		  item = item->nextSibling())
	{
		KBiffMailbox *mailbox = new KBiffMailbox();
		QString item_text(item->text(0));

		// if this mailbox is the current one, then use the current
		// settings instead of the hash
		if (item == mailboxes->currentItem())
		{
			mailbox->store = checkStorePassword->isChecked();
			mailbox->url   = getMailbox();

			mailboxHash->replace(item_text, mailbox);
		}

		mailbox = mailboxHash->find(item_text);

		QString password(scramble(mailbox->url.passwd()));
		KBiffURL url = mailbox->url;
		url.setPassword("");

		if (mailbox->store == false)
			password = "";
	
TRACEF("mailbox: %s -> %s", item_text.data(), url.url().data());
		mailbox_list.append(item_text);
		mailbox_list.append(url.url());
		mailbox_list.append(password);
	}

	config->writeEntry("Mailboxes", mailbox_list);
	delete config;
}

void KBiffMailboxTab::setMailbox(const KBiffURL& url)
{
TRACEINIT("KBiffMailboxTab::setMailbox()");
	QString prot(url.protocol());

	if (prot == "mbox")
		protocolSelected(1);
	else if (prot == "maildir")
		protocolSelected(2);
	else if (prot == "imap4")
		protocolSelected(3);
	else if (prot == "pop3")
		protocolSelected(4);
	else if (prot == "mh")
		protocolSelected(5);
	else if (prot == "file")
		protocolSelected(6);
	else if (prot == "nntp")
		protocolSelected(7);
	else
		return;

	if (editMailbox->isEnabled())
	{
		QString path(url.path());
		if (((prot == "imap4") || (prot == "nntp")) && !path.isEmpty() && path[0] == '/')
				path.remove(0, 1);

		KBiffURL::decodeURL(path);
		editMailbox->setText(path);
	}

	port = url.port();

	if (editServer->isEnabled())
		editServer->setText(url.host());
	if (editUser->isEnabled())
	{
		QString user(url.user());
		KBiffURL::decodeURL(user);
		editUser->setText(user);
	}
	if (editPassword->isEnabled())
	{
		QString passwd(url.passwd());
		KBiffURL::decodeURL(passwd);
		editPassword->setText(passwd);
	}

	preauth = !strcmp(url.searchPart(), "preauth") ? true : false;
	keepalive = !strcmp(url.reference(), "keepalive") ? true : false;
	async = !strcmp(url.reference(), "async") ? true : false;
}

const KBiffURL KBiffMailboxTab::getMailbox() const
{
	KBiffURL url;

	url.setProtocol(comboProtocol->currentText());

	if (editUser->isEnabled())
	{
		QString user(editUser->text());
		KBiffURL::encodeURLStrict(user);
		url.setUser(user);
	}

	if (editPassword->isEnabled())
	{
		QString passwd(editPassword->text());
		KBiffURL::encodeURLStrict(passwd);
		url.setPassword(passwd);
	}

	if (editServer->isEnabled())
		url.setHost(editServer->text());

	url.setPort(port);

	if (editMailbox->isEnabled())
	{
		QString path(editMailbox->text());
		if (!path.isEmpty() && path[0] != '/')
			path.prepend("/");
		KBiffURL::encodeURL(path);
		url.setPath(path);
	}

	if (preauth)
		url.setSearchPart("preauth");

	if (keepalive)
		url.setReference("keepalive");

	if (async)
		url.setReference("async");

	return url;
}

const QList<KBiffURL> KBiffMailboxTab::getMailboxList() const
{
TRACEINIT("KBiffMailboxTab::getMailboxList()");	
	QList<KBiffURL> url_list;

	for (QListViewItem *item = mailboxes->firstChild();
	     item;
		  item = item->nextSibling())
	{
		KBiffMailbox *mailbox = mailboxHash->find(item->text(0));
		KBiffURL *url = new KBiffURL(mailbox->url);
		url_list.append(url);
	}
	return url_list;
}

void KBiffMailboxTab::slotDeleteMailbox()
{
TRACEINIT("KBiffMailboxTab::slotDeleteMailbox()");
	/* I can't believe QListView doesn't have a 'count' member! */
	int count = 0;
	for (QListViewItem *it = mailboxes->firstChild();
	     it;
		  it = it->nextSibling(), count++);
	if (count == 1)
		return;

	/* need some "Are you sure?" code here */
	QListViewItem *item = mailboxes->currentItem();

	mailboxHash->remove(item->text(0));
	delete item;

	mailboxes->setSelected(mailboxes->firstChild(), true);
}

void KBiffMailboxTab::slotNewMailbox()
{
TRACEINIT("KBiffMailboxTab::slotNewMailbox()");
	KBiffNewDlg dlg;

	// popup the name chooser
	dlg.setCaption(i18n("New Mailbox"));
	if (dlg.exec())
	{
		QString mailbox_name = dlg.getName();

		// continue only if we received a decent name
		if (mailbox_name.isEmpty() == false)
		{
			QListViewItem *item = new QListViewItem(mailboxes, mailbox_name);
			item->setPixmap(0, QPixmap(ICON("mailbox.xpm")));

			KBiffMailbox *mailbox = new KBiffMailbox();
			mailbox->store = false;
			mailbox->url   = defaultMailbox();

			mailboxHash->insert(mailbox_name.data(), mailbox);
			mailboxes->setSelected(item, true);
		}
	}
}

void KBiffMailboxTab::slotMailboxSelected(QListViewItem *item)
{
TRACEINIT("KBiffMailboxTab::slotMailboxSelected()");
	KBiffMailbox *mailbox;

	// if an "old" item exists, save the current info as it
	if (oldItem && oldItem->text(0))
	{
		mailbox = mailboxHash->find(oldItem->text(0));

		if (mailbox)
		{
			// change the hash only if the item is different
			KBiffURL url(getMailbox());
			bool checked = checkStorePassword->isChecked();
			if (mailbox->url.url() != url.url() || mailbox->store != checked)
			{
				mailbox->url   = getMailbox();
				mailbox->store = checkStorePassword->isChecked();
			}
		}
	}

	mailbox = mailboxHash->find(item->text(0));

	if (mailbox)
	{
		setMailbox(mailbox->url);
		checkStorePassword->setChecked(mailbox->store);

		// save this as the "old" item
		oldItem = item;
	}
}

void KBiffMailboxTab::protocolSelected(int protocol)
{
TRACEINIT("KBiffMailboxTab::protocolSelected()");
	comboProtocol->setCurrentItem(protocol);

	switch (protocol)
	{
		case 1: // mbox
		case 2: // maildir
		case 5: // mh
		case 6: // file
			port = 0;
			buttonBrowse->setEnabled(true);
			editMailbox->setEnabled(true);
			editServer->setEnabled(false);
			editUser->setEnabled(false);
			editPassword->setEnabled(false);
			checkStorePassword->setEnabled(false);
			break;
		case 3: // IMAP4
			port = 143;
			editMailbox->setEnabled(true);
			buttonBrowse->setEnabled(false);
			editServer->setEnabled(true);
			editUser->setEnabled(true);
			editPassword->setEnabled(true);
			checkStorePassword->setEnabled(true);
			break;
		case 7: // NNTP
		   port = 119;
			editMailbox->setEnabled(true);
			buttonBrowse->setEnabled(false);
			editServer->setEnabled(true);
			editUser->setEnabled(true);
			editPassword->setEnabled(true);
			checkStorePassword->setEnabled(true);
			break;
		case 4: // POP3
			port = 110;
			editMailbox->setEnabled(false);
			buttonBrowse->setEnabled(false);
			editServer->setEnabled(true);
			editUser->setEnabled(true);
			editPassword->setEnabled(true);
			checkStorePassword->setEnabled(true);
			break;
		default: // blank
			port = 0;
			editMailbox->setEnabled(false);
			buttonBrowse->setEnabled(false);
			editServer->setEnabled(false);
			editUser->setEnabled(false);
			editPassword->setEnabled(false);
			checkStorePassword->setEnabled(false);
			break;
	}
}

void KBiffMailboxTab::browse()
{
	QString path = KFileDialog::getOpenFileName();
	if (!path.isEmpty() && !path.isNull())
	{
		editMailbox->setText(path);
	}
}

void KBiffMailboxTab::advanced()
{
	KBiffMailboxAdvanced advanced_dlg;
	QString prot(getMailbox().protocol());
	
	if (prot == "mbox" || prot == "maildir" || prot == "file" || prot == "mh")
		advanced_dlg.setPort(port, false);
	else
		advanced_dlg.setPort(port);

	if (prot == "imap4")
	{
		advanced_dlg.setPreauth(preauth);
		advanced_dlg.setKeepalive(keepalive);
		advanced_dlg.setAsync(async);
	}

	if ((prot == "pop3") || (prot == "nntp"))
	{
		advanced_dlg.setKeepalive(keepalive);
		advanced_dlg.setAsync(async);
	}

	advanced_dlg.setMailbox(getMailbox());
	if (advanced_dlg.exec())
	{
		port = advanced_dlg.getPort();
		setMailbox(advanced_dlg.getMailbox());
	}
}

const char* KBiffMailboxTab::scramble(const char* password, bool encode)
{
	char *ptr = new char[strlen(password) + 1];
	char *ret_ptr = ptr;

	while (*password)
		*ptr++ = encode ? (*(password++) - 4) : (*(password++) + 4);
	*ptr = '\0';

	return ret_ptr;
}

const KBiffURL KBiffMailboxTab::defaultMailbox() const
{
	QFileInfo mailbox_info(getenv("MAIL"));
	if (mailbox_info.exists() == false)
	{
		QString s(_PATH_MAILDIR);
		s += "/";
		s += getpwuid(getuid())->pw_name;
		mailbox_info.setFile(s);
	}

	QString *default_path = mailbox_info.isDir() ? new QString("maildir:") :
	                                               new QString("mbox:");
	default_path->append(mailbox_info.absFilePath());

	return KBiffURL(*default_path);
}

//////////////////////////////////////////////////////////////////////
// KBiffAboutTab
//////////////////////////////////////////////////////////////////////
KBiffAboutTab::KBiffAboutTab(QWidget *parent)
	: QWidget(parent)
{
TRACEINIT("KBiffAboutTab::KBiffAboutTab()");
	int x, y;

	// load in the kbiff pixmap
	QPixmap logo_pixmap(KApplication::kde_icondir() + '/' + "kbiff.xpm");

	KURLLabel *logo = new KURLLabel(this);
	logo->setPixmap(logo_pixmap);
	logo->move(10,10);
	logo->setFont(QFont("helvetica", 24, QFont::Bold));
	logo->setURL("http://www.pobox.com/~kurt_granroth/kbiff");
	logo->setText(i18n("KBiff"));
	logo->setTextAlignment(Right);
	logo->setUnderline(false);
	logo->setGlow(false);
	logo->setFloat(true);
	logo->setAutoResize(true);
	logo->adjustSize();

	connect(logo, SIGNAL(leftClickedURL(const char*)),
	                 SLOT(homepage(const char*)));

	x = logo_pixmap.width() + 15;
	y = 10;

	QFontMetrics big_fm(QFont("helvetica", 24, QFont::Bold));
	QFontMetrics small_fm(QFont("helvetica", 12));

	y += big_fm.lineSpacing();

	QLabel *version = new QLabel(this);
	version->setFont(QFont("helvetica", 12));
	QString ver_str;
	ver_str.sprintf(i18n("Version %s\n\nCopyright (C) 1999\nKurt Granroth"), "2.3.10a");
	version->setText(ver_str);
	version->setAutoResize(true);
	version->move(x, y);
	version->adjustSize();
	y += version->height();

	KURLLabel *email = new KURLLabel(this);
	email->setText("granroth@kde.org");
	email->setURL("granroth@kde.org");
	email->setFont(QFont("helvetica", 12));
	email->setUnderline(false);
	email->setGlow(false);
	email->setFloat();
	email->move(x, y);
	email->setAutoResize(true);
	email->adjustSize();
	connect(email, SIGNAL(leftClickedURL(const char*)),
	                 SLOT(mailTo(const char*)));
}

KBiffAboutTab::~KBiffAboutTab()
{
}

void KBiffAboutTab::mailTo(const char* url)
{
	// this kludge is necessary until KFM handles mailto: urls
	char *mailer = getenv("MAILER");
	if (mailer)
	{
		KProcess mailto;
		mailto << mailer << "-s" << "\"KBiff\"" << url;
		mailto.start(KProcess::DontCare);
	}
}

void KBiffAboutTab::homepage(const char* url)
{
	KFM kfm;
	kfm.openURL(url);
}

KBiffNewDlg::KBiffNewDlg(QWidget* parent, const char* name)
	: QDialog(parent, name, true, 0)
{
	QGridLayout *layout = new QGridLayout(this, 2, 2, 12, 5);
	layout->activate();
	
	QLabel* label1 = new QLabel(i18n("New Name:"), this);
	label1->setMinimumSize(label1->sizeHint());
	layout->addWidget(label1, 0, 0);

	editName = new QLineEdit(this);
	editName->setFocus();
	editName->setMinimumSize(editName->sizeHint());
	layout->addWidget(editName, 0, 1, 1);

	QBoxLayout *buttons = new QBoxLayout(QBoxLayout::LeftToRight);
	layout->addLayout(buttons, 1, 1);

	// ok button
	QPushButton* button_ok = new QPushButton(i18n("OK"), this);
	connect(button_ok, SIGNAL(clicked()), SLOT(accept()));
	button_ok->setDefault(true);
	button_ok->setMinimumSize(button_ok->sizeHint());
	buttons->addWidget(button_ok);

	// cancel button
	QPushButton* button_cancel = new QPushButton(i18n("Cancel"), this);
	button_cancel->setMinimumSize(button_cancel->sizeHint());
	connect(button_cancel, SIGNAL(clicked()), SLOT(reject()));
	buttons->addWidget(button_cancel);

	// set my name
	setCaption(i18n("New Name"));
}
