/*
 * status.cpp
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the implementation of the KBiffStatus
 * widget
 *
 * $Id: status.cpp,v 1.4.2.2 1999/07/16 10:12:19 porten Exp $
 */
#include "status.h"

#include "Trace.h"
#include <kapp.h>

#include <qlistview.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpoint.h>

KBiffStatus::KBiffStatus(const QString& profile, KBiffStatusList& list)
	: QFrame(0, 0, WStyle_Customize | WStyle_NoBorder),
	  _listView(new QListView(this))
{
TRACEINIT("KBiffStatus::KBiffStatus()");
	setFrameStyle(WinPanel|Raised);
	resize(0, 0);
	QLabel *profile_label = new QLabel(profile, this);
	profile_label->setFrameStyle(QFrame::Box | QFrame::Raised);
	profile_label->setMinimumSize(profile_label->sizeHint());
	profile_label->setAlignment(AlignCenter);

	_listView->addColumn(i18n("Mailbox"));
	_listView->addColumn(i18n("New"));
	_listView->setColumnAlignment(1, AlignRight);
	_listView->setSorting(1, FALSE);
	_listView->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
	_listView->setVScrollBarMode(QScrollView::AlwaysOff);
	_listView->setHScrollBarMode(QScrollView::AlwaysOff);
	_listView->header()->hide();

	KBiffStatusList *ptr = &list;
	for(unsigned int i = 0; i < ptr->count(); i++)
	{
		const KBiffStatusItem *item = ptr->at(i);
		new QListViewItem(_listView, item->mailbox(), item->newMessages());
	}

	_listView->setMinimumSize(_listView->sizeHint());
	int list_height = (_listView->firstChild()->height() * ptr->count()) + 10;
	TRACEF("list_height = %d", list_height);
	_listView->setMinimumSize(_listView->sizeHint().width() + 5, list_height);

	QVBoxLayout *layout = new QVBoxLayout(this, 0, 0);
	layout->addWidget(profile_label);
	layout->addWidget(_listView);
	layout->activate();
}

KBiffStatus::~KBiffStatus()
{
}

void KBiffStatus::popup(const QPoint& pos)
{
	QWidget *desktop = KApplication::desktop();
	int x = 0, y = 0;

	// verify that the width is within the desktop
	if ((pos.x() + width()) > desktop->width())
	{
		x = pos.x() - width();
		x = (x < 0) ? 0 : x;
	}

	// verify that that height is within tolerances
	if ((pos.y() + height()) > desktop->height())
	{
		//y = desktop->height() - height();
		y = pos.y() - height();
		y = (y < 0) ? 0 : y;
	}

	// now that we have *real* co-ordinates, we show it
	move(x, y);
	show();
}

KBiffStatusItem::KBiffStatusItem(const QString& mailbox, const int num_new)
	: _mailbox(mailbox),
	  _newMessages(QString().setNum(num_new))
{
}
