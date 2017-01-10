/*
 * notify.cpp
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the implementation of the KBiffNotify
 * widget
 *
 * $Id: notify.cpp,v 1.3.2.2 1999/08/24 22:17:27 granroth Exp $
 */
#include "notify.h"

#include "Trace.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbt.h>

#include <kiconloaderdialog.h>
#include <kapp.h>
#include <kwm.h>
#include <kprocess.h>

KBiffNotify::KBiffNotify(const int num_new, const QString& mailbx, const QString& mail_client)
	: QDialog(0, 0, false, 0)
{
TRACEINIT("KBiffNotify::KBiffNotify()");
TRACEF("%d new messages in %s", num_new, mailbx.data());
	// we do *not* want this window to have focus when it pops up
	KWM::setDecoration(winId(), KWM::normalDecoration | KWM::noFocus);

	setIcon(ICON("kbiff.xpm"));
	setCaption(i18n("You have new mail!"));

	resize(0, 0);

	QVBoxLayout *layout = new QVBoxLayout(this, 12);

	QHBoxLayout *upper_layout = new QHBoxLayout();
	layout->addLayout(upper_layout);

	QLabel *pixmap = new QLabel(this);
	pixmap->setPixmap(ICON("kbiff.xpm"));
	pixmap->setFixedSize(pixmap->sizeHint());
	upper_layout->addWidget(pixmap);

	QVBoxLayout *mailbox_layout = new QVBoxLayout();
	upper_layout->addLayout(mailbox_layout);

	QLabel *congrats = new QLabel(i18n("You have new mail!"), this);
	QFont the_font(congrats->font());
	the_font.setBold(true);
	congrats->setFont(the_font);
	congrats->setMinimumSize(congrats->sizeHint());
	mailbox_layout->addWidget(congrats);

	QString msg;
	msg.sprintf(i18n("New Messages: %d"), num_new);
	msgLabel = new QLabel(msg, this);
	msgLabel->setMinimumSize(msgLabel->sizeHint());
	mailbox_layout->addWidget(msgLabel);

	msg.sprintf(i18n("Mailbox: %s"), mailbx.data());
	QLabel *which_one = new QLabel(msg, this);
	which_one->setMinimumSize(which_one->sizeHint());
	mailbox_layout->addWidget(which_one);

	QPushButton *ok = new QPushButton(i18n("OK"), this);
	ok->setDefault(true);
	connect(ok, SIGNAL(clicked()), SLOT(accept()));

	QPushButton *launch = new QPushButton(i18n("Mailer"), this);
	launch->setFixedSize(launch->sizeHint());
	ok->setFixedSize(launch->sizeHint());
	if (mail_client.isEmpty())
		launch->setEnabled(false);

	connect(launch, SIGNAL(clicked()), SLOT(launchMailClient()));
	connect(launch, SIGNAL(clicked()), SLOT(accept()));

	QHBoxLayout *button_layout = new QHBoxLayout();
	layout->addLayout(button_layout);

	button_layout->addStretch(1);
	button_layout->addWidget(launch);
	button_layout->addWidget(ok);
	button_layout->addStretch(1);

	layout->activate();

	mailbox = mailbx;
	messages = num_new;
	mailClient = mail_client;
}

KBiffNotify::~KBiffNotify()
{
}

void KBiffNotify::setNew(const int num_new)
{
	QString msg;
	msg.sprintf(i18n("New Messages: %d"), num_new);
	msgLabel->setText(msg);
	messages = num_new;
}

// THIS SHOULD BE A CORBA METHOD!
void KBiffNotify::launchMailClient()
{
TRACEINIT("KBiffNotify::launchMailClient()");
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
	const char *pom=(const char *)mailClient;
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
