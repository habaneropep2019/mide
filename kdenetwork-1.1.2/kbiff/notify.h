/*
 * notify.h
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the declaration of the KBiffNotify
 * widget.
 *
 * $Id: notify.h,v 1.1.2.1 1999/06/04 02:25:12 granroth Exp $
 */
#ifndef KBIFFNOTIFY_H 
#define KBIFFNOTIFY_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qdialog.h>
#include <qstring.h>

class QLabel;

class KBiffNotify : public QDialog
{
	Q_OBJECT
public:
	KBiffNotify(const int num_new, const QString& mailbx, const QString& mail_client = "");
	virtual ~KBiffNotify();

	const QString getMailbox() { return mailbox; }
	const int newMessages() { return messages; }

	void setNew(const int num_new);

protected slots:
	void launchMailClient();

protected:
	QString mailbox;
	QString mailClient;
	QLabel* msgLabel;
	int     messages;
};

#endif // KBIFFNOTIFY_H
