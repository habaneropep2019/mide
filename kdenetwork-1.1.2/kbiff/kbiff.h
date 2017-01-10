/*
 * kbiff.h
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the declaration of the main KBiff
 * widget.
 *
 * $Id: kbiff.h,v 1.13.2.1 1999/06/04 02:25:09 granroth Exp $
 */
#ifndef KBIFF_H 
#define KBIFF_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qlist.h>

#include <kbiffmonitor.h>
#include <setupdlg.h>

#include <kbiffurl.h>
#include <kapp.h>

// mediatool.h is needed by kaudio.h
extern "C" {
#include <mediatool.h>
} 
#include <kaudio.h>

#include <qlabel.h>
#include <qtimer.h>

#include <notify.h>
#include <status.h>

class KBiff : public QLabel
{
	Q_OBJECT
public:
	KBiff(QWidget *parent = 0);
	virtual ~KBiff();

	const bool isDocked() const;

	void processSetup(const KBiffSetup* setup, bool start);
	void readSessionConfig();

	void setMailboxList(const QList<KBiffURL>& mailbox_list, unsigned int poll = 60);

protected:
	void mousePressEvent(QMouseEvent *);
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);

protected:
	void popupMenu();
	void reset();
	bool isRunning();
	bool isGIF8x(const char* file);
	void executeCommand(const QString& command);

protected slots:
	void saveYourself();
	void invokeHelp();
	void displayPixmap();
	void haveNewMail(const int, const char*);
	void currentStatus(const int, const char*, const KBiffMailState);
	void dock();
	void setup();
	void checkMailNow();
	void readMailNow();
	void readPop3MailNow();
	void stop();
	void start();
	void popupStatus();

protected:
	bool myMUTEX;
	QList<KBiffMonitor> monitorList;
	QList<KBiffNotify>  notifyList;
	KBiffStatusList     statusList;

	// Capability
	bool    hasAudio;
	KAudio  audioServer;
	QTimer  *statusTimer;
	
	// General settings
	QString profile;
	QString mailClient;

	// New mail
	bool    systemBeep;
	bool    runCommand;
	QString runCommandPath;
	bool    playSound;
	QString playSoundPath;
	bool    notify;
	bool    dostatus;

	bool    docked;
	bool    sessions;

	bool    isSecure;

	QString noMailIcon;
	QString newMailIcon;
	QString oldMailIcon;
	QString noConnIcon;

	KBiffStatus *status;
	bool         statusChanged;
};

#endif // KBIFF_H 
