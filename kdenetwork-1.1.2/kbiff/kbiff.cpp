/*
 * kbiff.cpp
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the implementation of the main KBiff
 * widget
 *
 * $Id: kbiff.cpp,v 1.14.2.1 1999/06/04 02:25:08 granroth Exp $
 */
#include "kbiff.h"

#include "Trace.h"

#include <qmovie.h>
#include <qtooltip.h>

#include <kiconloader.h>
#include <kprocess.h>
#include <kwm.h>

#include "setupdlg.h"
#include "notify.h"
#include "status.h"

KBiff::KBiff(QWidget *parent)
	: QLabel(parent),
	  statusTimer(0),
	  status(0),
	  statusChanged(true)
{
TRACEINIT("KBiff::KBiff()");
	setAutoResize(true);
	setMargin(0);
	setAlignment(AlignLeft | AlignTop);

	// Init the audio server.  serverStatus return 0 when it is ok
	hasAudio = (audioServer.serverStatus() == 0) ? true : false;

	// enable the session management stuff
	connect(kapp, SIGNAL(saveYourself()), this, SLOT(saveYourself()));

	// nuke the list stuff when removed
	monitorList.setAutoDelete(true);
	notifyList.setAutoDelete(true);
	statusList.setAutoDelete(true);

	reset();
}

KBiff::~KBiff()
{
TRACEINIT("KBiff::~KBiff()");
	monitorList.clear();
	notifyList.clear();
	statusList.clear();
}

void KBiff::processSetup(const KBiffSetup* setup, bool run)
{
TRACEINIT("KBiff::processSetup()");
	// General settings
	isSecure    = setup->getSecure();
	profile     = setup->getProfile();
	mailClient  = setup->getMailClient();
	sessions    = setup->getSessionManagement();
	noMailIcon  = setup->getNoMailIcon();
	newMailIcon = setup->getNewMailIcon();
	oldMailIcon = setup->getOldMailIcon();
	noConnIcon  = setup->getNoConnIcon();

	// New mail
	systemBeep     = setup->getSystemBeep();
	runCommand     = setup->getRunCommand();
	runCommandPath = setup->getRunCommandPath();
	playSound      = setup->getPlaySound();
	playSoundPath  = setup->getPlaySoundPath();
	notify         = setup->getNotify();
	dostatus       = setup->getStatus();

	// if we aren't going the status route, we should at least
	// provide a tooltip!
	if (dostatus == false)
		QToolTip::add(this, profile);
	else
		QToolTip::remove(this);

	// set all the new mailboxes
	setMailboxList(setup->getMailboxList(), setup->getPoll());

	// change the dock state if necessary
TRACEF("docked = %d", docked);
TRACEF("setup->getDock() = %d", setup->getDock());
	if (docked != setup->getDock())
		dock();

	if (run)
		start();

	delete setup;
}

void KBiff::setMailboxList(const QList<KBiffURL>& mailbox_list, unsigned int poll)
{
TRACEINIT("KBiff::setMailboxList");
	QList<KBiffURL> tmp_list = mailbox_list;

	myMUTEX = true;
	if (isRunning())
		stop();
	monitorList.clear();
	
	KBiffURL *url;
	for (url = tmp_list.first(); url != 0; url = tmp_list.next())
	{
		TRACEF("Now adding %s", url->url().data());
		KBiffMonitor *monitor = new KBiffMonitor();
		monitor->setMailbox(*url);
		monitor->setPollInterval(poll);
		connect(monitor, SIGNAL(signal_newMail(const int, const char*)),
		        this, SLOT(haveNewMail(const int, const char*)));
		connect(monitor, SIGNAL(signal_currentStatus(const int, const char*, const KBiffMailState)),
		        this, SLOT(currentStatus(const int, const char*, const KBiffMailState)));
		connect(monitor, SIGNAL(signal_noMail()), this, SLOT(displayPixmap()));
		connect(monitor, SIGNAL(signal_oldMail()), this, SLOT(displayPixmap()));
		connect(monitor, SIGNAL(signal_noConn()), this, SLOT(displayPixmap()));
		monitorList.append(monitor);
	}
	myMUTEX = false;
}

const bool KBiff::isDocked() const
{
	return docked;
}

void KBiff::readSessionConfig()
{
TRACEINIT("KBiff::readSesionConfig()");
	KConfig *config = kapp->getSessionConfig();

	config->setGroup("KBiff");

	profile = config->readEntry("Profile", "Inbox");
	docked = config->readBoolEntry("IsDocked", false);
	bool run = config->readBoolEntry("IsRunning", true);

	processSetup(new KBiffSetup(profile), run);
}

///////////////////////////////////////////////////////////////////////////
// Protected Virtuals
///////////////////////////////////////////////////////////////////////////
void KBiff::mousePressEvent(QMouseEvent *event)
{
TRACEINIT("KBiff::mousePressEvent()");
	// regardless of which button, get rid of the status box
	if (status)
	{
		status->hide();
		delete status;
		status = 0;
	}

	// also, ditch the timer
	if (statusTimer)
	{
		statusTimer->stop();
		delete statusTimer;
		statusTimer = 0;
	}

	// check if this is a right click
	if(event->button() == RightButton)
	{
		// popup the context menu
		popupMenu();
	}
	else
	{
		// execute the command
		if (!mailClient.isEmpty())
			executeCommand(mailClient);

		readPop3MailNow();
	}
}

void KBiff::enterEvent(QEvent *e)
{
TRACEINIT("KBiff::enterEvent()");
	QLabel::enterEvent(e);

	// return now if the user doesn't want this feature.
	// *sniff*.. the ingrate.. I worked so hard on this, too... *sob*
	if (dostatus == false)
		return;

	// don't do anything if we already have a timer
	if (statusTimer)
		return;

	// popup the status in one second
	statusTimer = new QTimer(this);
	connect(statusTimer, SIGNAL(timeout()), this, SLOT(popupStatus()));

	TRACE("Starting status timer for 1 second...");
	statusTimer->start(1000, true);
}

void KBiff::leaveEvent(QEvent *e)
{
TRACEINIT("KBiff::leaveEvent()");
	QLabel::leaveEvent(e);

	// stop the timer if it is going
	if (statusTimer)
	{
		statusTimer->stop();
		delete statusTimer;
		statusTimer = 0;
	}

	// get rid of the status box if it is activated
	if (status)
	{
		status->hide();
		delete status;
		status = 0;
	}
}

void KBiff::popupStatus()
{
TRACEINIT("KBiff::popupStatus()");
	if (status)
		delete status;

	// if we don't get rid of the timer, then the very next
	// time we float over the icon, the status box will
	// *not* be activated!
	if (statusTimer)
	{
		statusTimer->stop();
		delete statusTimer;
		statusTimer = 0;
	}

	if (statusChanged)
	{
		statusList.clear();
		KBiffMonitor *monitor;
		for(monitor = monitorList.first(); monitor; monitor = monitorList.next())
		{
			TRACEF("%s: %d\n", (const char*)monitor->getMailbox(), monitor->newMessages());
			statusList.append(new KBiffStatusItem(monitor->getMailbox(), monitor->newMessages()));
		}
		statusChanged = false;
	}

	status = new KBiffStatus(profile, statusList);
	status->popup(QCursor::pos());
}

bool KBiff::isGIF8x(const char* file_name)
{
TRACEINIT("KBiff::isGIF8x()");

	/* The first test checks if we can open the file */
	QFile gif8x(file_name);
	if (gif8x.open(IO_ReadOnly) == false)
		return false;

	/**
	 * The GIF89 format specifies that the first five bytes of
	 * the file shall have the characters 'G' 'I' 'F' '8' '9'.
	 * The GIF87a format specifies that the first six bytes
	 * shall read 'G' 'I' 'F' '8' '7' 'a'.  Knowing that, we
	 * shall read in the first six bytes and test away.
	 */
	char header[6];
	int bytes_read = gif8x.readBlock(header, 6);

	/* Close the file just to be nice */
	gif8x.close();

	/* If we read less than 6 bytes, then its definitely not GIF8x */
	if (bytes_read < 6)
		return false;

	/* Now test for the magical GIF8(9|7a) */
	if (header[0] == 'G' &&
		 header[1] == 'I' &&
		 header[2] == 'F' &&
		 header[3] == '8' &&
		 header[4] == '9' ||
		(header[4] == '7' && header[5] == 'a'))
	{
		/* Success! */
		TRACE("Is GIF8x!");
		return true;
	}

	/* Apparently not GIF8(9|7a) */
	return false;
}

///////////////////////////////////////////////////////////////////////////
// Protected Slots
///////////////////////////////////////////////////////////////////////////
void KBiff::saveYourself()
{
TRACEINIT("KBiff::saveYourself()");
	if (sessions)
	{
		KConfig *config = kapp->getSessionConfig();
		config->setGroup("KBiff");

		config->writeEntry("Profile", profile);
		config->writeEntry("IsDocked", docked);
		config->writeEntry("IsRunning", isRunning());

		config->sync();

		TRACEF("Saving session profile: %s", profile.data());
	}
}

void KBiff::invokeHelp()
{
	kapp->invokeHTMLHelp("kbiff/kbiff.html", "");
}

void KBiff::displayPixmap()
{
TRACEINIT("KBiff::displayPixmap()");
	if (myMUTEX)
		return;
	TRACE("After MUTEX");

	// we will try to deduce the pixmap (or gif) name now.  it will
	// vary depending on the dock and mail state
	QString pixmap_name, mini_pixmap_name;
	bool has_new = false, has_old = false, has_no = true, has_noconn = false;
	KBiffMonitor *monitor;
	for (monitor = monitorList.first();
	     monitor != 0 && has_new == false;
		  monitor = monitorList.next())
	{
		TRACE("Checking mailbox");
		switch (monitor->getMailState())
		{
			case NoMail:
				has_no = true;
				break;
			case NewMail:
				has_new = true;
				break;
			case OldMail:
				has_old = true;
				break;
			case NoConn:
				has_noconn = true;
				break;
			default:
				has_no = true;
				break;
		}
	}
	TRACE("Done checking mailboxes");

	if (has_new)
		pixmap_name = newMailIcon;
	else if (has_old)
		pixmap_name = oldMailIcon;
	else if (has_noconn)
		pixmap_name = noConnIcon;
	else
		pixmap_name = noMailIcon;

	mini_pixmap_name = "mini-" + pixmap_name;

	// Get a list of all the pixmap paths.  This is needed since the
	// icon loader only returns the name of the pixmap -- NOT it's path!
	QStrList *dir_list = kapp->getIconLoader()->getDirList();
	QFileInfo file, mini_file;
	for (unsigned int i = 0; i < dir_list->count(); i++)
	{
		QString here = dir_list->at(i);

		// check if this file exists.  If so, we have our path
		file.setFile(here + '/' + pixmap_name);
		mini_file.setFile(here + '/' + mini_pixmap_name);

		// if we are docked, check if the mini pixmap exists FIRST.
		// if it does, we go with it.  if not, we look for the
		// the regular size one
		if (docked && mini_file.exists())
		{
			file = mini_file;
			break;
		}

		if (file.exists())
			break;
	}

	TRACEF("Displaying %s", file.absFilePath().data());
	// at this point, we have the file to display.  so display it
	if (isGIF8x(file.absFilePath()))
		setMovie(QMovie(file.absFilePath().data()));
	else
		setPixmap(QPixmap(file.absFilePath()));
	adjustSize();
}

void KBiff::currentStatus(const int num, const char* the_mailbox, const KBiffMailState the_state)
{
TRACEINIT("KBiff::currentStatus()");
TRACEF("num = %d mailbox = %s state = %d", num, the_mailbox, the_state);
	statusChanged = true;

	// iterate through all saved notify dialogs to see if "our" one is
	// currently being displayed
	KBiffNotify *notify;
	for (notify = notifyList.first();
	     notify != 0;
		  notify = notifyList.next())
	{
		// if this one is not visible, delete it from the list.  the only
		// way it will again become visible is if the haveNewMail slot
		// gets triggered
		if (notify->isVisible() == false)
		{
			notifyList.remove();
		}
		else
		{
			TRACEF("notify->mailbox = %s", (const char*)notify->getMailbox());
			// if this box is visible (active), we see if it is the one
			// we are looking for
			if (notify->getMailbox() == the_mailbox)
			{
				// yep.  now, if there is new mail, we set the new number in
				// the dialog.  if it is any other state, we remove this
				// dialog from the list
				switch (the_state)
				{
				case NewMail:
					notify->setNew(num);
					break;
				case OldMail:
				case NoMail:
				case NoConn:
				default:
					notifyList.remove();
					break;
				}
			}
		}
	}
}

void KBiff::haveNewMail(const int num, const char* the_mailbox)
{
TRACEINIT("KBiff::haveNewMail()");
	displayPixmap();

	// beep if we are allowed to
	if (systemBeep)
	{
		TRACE("Beep!");
		kapp->beep();
	}

	// run a command if we have to
	if (runCommand)
	{
		// make sure the command exists
		if (!runCommandPath.isEmpty())
		{
			TRACEF("Running %s", runCommandPath.data());
			executeCommand(runCommandPath);
		}
	}

	// play a sound if we have to
	if (playSound && hasAudio)
	{
		// make sure something is specified
		if (!playSoundPath.isEmpty())
		{
		   TRACEF("Playing sound: %s", playSoundPath.data());
			audioServer.play(playSoundPath);
			audioServer.sync();
		}
	}

	// notify if we must
	if (notify)
	{
		TRACEF("Notifying %d in %s", num, the_mailbox);
		KBiffNotify *notify_dlg = new KBiffNotify(num, the_mailbox, mailClient);
		notifyList.append(notify_dlg);
		notify_dlg->show();
	}
}

void KBiff::dock()
{
TRACEINIT("KBiff::dock()");
	// destroy the old window
	if (this->isVisible())
	{
		this->hide();
		this->destroy(true, true);
		this->create(0, true, false);
		kapp->setMainWidget(this);

		// we don't want a "real" top widget if we are _going_ to
		// be docked.
		if (docked)
			kapp->setTopWidget(this);
		else
			kapp->setTopWidget(new QWidget);
	}

	if (docked == false)
	{
		docked = true;

		// enable docking
		KWM::setDockWindow(this->winId());
	}
	else
		docked = false;

	// (un)dock it!
	this->show();
	QTimer::singleShot(1000, this, SLOT(displayPixmap()));
}

void KBiff::setup()
{
TRACEINIT("KBiff::setup()");
	KBiffSetup* setup_dlg = new KBiffSetup(profile);

	if (setup_dlg->exec())
		processSetup(setup_dlg, true);
	else
		delete setup_dlg;
}

void KBiff::checkMailNow()
{
TRACEINIT("KBiff::checkMailNow()");
	KBiffMonitor *monitor;
	for (monitor = monitorList.first();
	     monitor != 0;
		  monitor = monitorList.next())
	{
		monitor->checkMailNow();
	}
}

void KBiff::readMailNow()
{
TRACEINIT("KBiff::readMailNow()");
	KBiffMonitor *monitor;
	for (monitor = monitorList.first();
	     monitor != 0;
		  monitor = monitorList.next())
	{
		monitor->setMailboxIsRead();
	}
}

void KBiff::readPop3MailNow()
{
TRACEINIT("KBiff::readPop3MailNow()");
	KBiffMonitor *monitor;
	for (monitor = monitorList.first();
	     monitor != 0;
		  monitor = monitorList.next())
	{
		if (!strcmp(monitor->getProtocol(), "pop3"))
			monitor->setMailboxIsRead();
	}
}

void KBiff::stop()
{
TRACEINIT("KBiff::stop()");
	KBiffMonitor *monitor;
	for (monitor = monitorList.first();
	     monitor != 0;
		  monitor = monitorList.next())
	{
		monitor->stop();
	}
}

void KBiff::start()
{
TRACEINIT("KBiff::start()");
	myMUTEX = true;
	KBiffMonitor *monitor;
	for (unsigned int i = 0; i < monitorList.count(); i++)
	{
		TRACE("Starting a monitor");
		monitor = monitorList.at(i);
		monitor->start();
	}
	myMUTEX = false;
	QTimer::singleShot(1000, this, SLOT(displayPixmap()));
}

///////////////////////////////////////////////////////////////////////////
// Protected Functions
///////////////////////////////////////////////////////////////////////////
void KBiff::popupMenu()
{
TRACEINIT("KBiff::popupMenu()");
	QPopupMenu *popup = new QPopupMenu(0, "popup");

	// if secure, disable everything but exit
	if (isSecure == false)
	{
		if (docked)
			popup->insertItem(i18n("&UnDock"), this, SLOT(dock()));
		else
			popup->insertItem(i18n("&Dock"), this, SLOT(dock()));
		popup->insertItem(i18n("&Setup..."), this, SLOT(setup()));
		popup->insertSeparator();
		popup->insertItem(i18n("&Help..."), this, SLOT(invokeHelp()));
		popup->insertSeparator();

		int check_id;
		check_id = popup->insertItem(i18n("&Check Mail Now"), this, SLOT(checkMailNow()));
		int read_id;
		read_id = popup->insertItem(i18n("&Read Mail Now"), this, SLOT(readMailNow()));

		if (isRunning())
		{
			popup->setItemEnabled(check_id, true);
			popup->setItemEnabled(read_id, true);
			popup->insertItem(i18n("&Stop"), this, SLOT(stop()));
		}
		else
		{
			popup->setItemEnabled(check_id, false);
			popup->setItemEnabled(read_id, false);
			popup->insertItem(i18n("&Start"), this, SLOT(start()));
		}
		popup->insertSeparator();
	}

	popup->insertItem(i18n("E&xit"), kapp, SLOT(quit()));

	popup->popup(QCursor::pos());
}

void KBiff::reset()
{
TRACEINIT("KBiff::reset()");
	// reset all the member variables
	systemBeep     = true;
	runCommand     = false;
	runCommandPath = "";
	playSound      = false;
	playSoundPath  = "";
	notify         = true;
	dostatus       = true;

	noMailIcon  = "nomail.xpm";
	newMailIcon = "newmail.xpm";
	oldMailIcon = "oldmail.xpm";
	noConnIcon  = "noconn.xpm";

	docked    = false;
	isSecure  = false;

	mailClient  = "xmutt";

	myMUTEX = false;
}

bool KBiff::isRunning()
{
TRACEINIT("KBiff::isRunning()");
	bool is_running = false;
	KBiffMonitor *monitor;
	for (monitor = monitorList.first();
	     monitor != 0;
		  monitor = monitorList.next())
	{
		if (monitor->isRunning())
		{
			is_running = true;
			break;
		}
	}
	return is_running;
}

void KBiff::executeCommand(const QString& command)
{
TRACEINIT("KBiff::executeCommand()");
	/**
	 * The KProcess object expects the first param to be the
	 * command and every param after that to be the command's params.
	 * As a result, though, if you pass it a string with spaces in it,
	 * then it takes the first token of the string as the command
	 * and ignores the rest of it!  We need to pass all tokens from
	 * the command string on as individual parameters to get around
	 * this.
	 */
	KProcess process;
	int index, beg;
	char param[60];
	const char *pom=(const char *)command;
	index=0;
	while(pom[index])
	{
		// simplyfies whitespaces
		while(pom[index] && (pom[index]==' ' || (pom[index]>=9 && pom[index]<=13)))
			index++;

		beg=0;
		if(pom[index]=='"')
		{
			index++;
			while(pom[index] && pom[index]!='"')
			{
				if(pom[index]==92 && pom[index+1])   // '\'
					index++;
				if(beg<59)
				{
					param[beg]=pom[index];
					beg++;
				}
				index++;
			}  
			index++;
		}	
		else
		{
			while(pom[index] && pom[index]>' ')
			{
				if(pom[index]==92 && pom[index+1])   // '\'
					index++;
				if(beg<59)
				{
					param[beg]=pom[index];
					beg++;
				}
				index++;
			}  
		}	
		param[beg]=0;  
		if(beg)
			process << param;  
	}

	process.start(KProcess::DontCare);
}

