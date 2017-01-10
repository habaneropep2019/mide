/*
 * kbiffmonitor.cpp
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the implementation of KBiffMonitor and
 * associated classes.
 *
 * $Id: kbiffmonitor.cpp,v 1.19.2.2 1999/08/24 22:17:26 granroth Exp $
 */
#include "kbiffmonitor.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <fcntl.h>
#include <errno.h>

#include <kbiffurl.h>

#include <qapp.h>
#include <qstring.h>
#include <qregexp.h>
#include <qdir.h>
#include <qdatetm.h>

#include "Trace.h"

#define MAXSTR (1024)

#if defined (_HPUX_SOURCE)
extern int h_errno;
#endif

static bool real_from(const char *buffer);
static const char *compare_header(const char *header, const char *field);

KBiffMonitor::KBiffMonitor()
	: QObject(),
	  poll(60),
	  oldTimer(0),
	  started(false),
	  newCount(0),
	  oldCount(0),
	  simpleURL(""),
	  protocol(""),
	  mailbox(""),
	  server(""),
	  user(""),
	  password(""),
	  port(0),
	  preauth(false),
	  keepalive(false),
	  mailState(UnknownState),
	  lastSize(0),
	  imap(0),
	  pop(0),
	  nntp(0)
{
TRACEINIT("KBiffMonitor::KBiffMonitor()");
	lastRead.setTime_t(0);
	lastModified.setTime_t(0);
}

KBiffMonitor::~KBiffMonitor()
{
TRACEINIT("KBiffMonitor::~KBiffMonitor()");
	if (imap)
	{
		delete imap;
		imap = 0;
	}
	if (pop)
	{
		delete pop;
		pop = 0;
	}
	if (nntp)
	{
		delete nntp;
		nntp = 0;
	}
}

void KBiffMonitor::start()
{
TRACEINIT("KBiffMonitor::start()");
	started  = true;
	oldTimer = startTimer(poll * 1000);
	emit(signal_checkMail());
}

void KBiffMonitor::stop()
{
TRACEINIT("KBiffMonitor::stop()");
	if (oldTimer > 0)
		killTimer(oldTimer);

	lastSize   = 0;
	oldTimer   = 0;
	mailState  = UnknownState;
	started    = false;
	lastRead.setTime_t(0);
	lastModified.setTime_t(0);
	uidlList.clear();
}

void KBiffMonitor::setPollInterval(const int interval)
{
TRACEINIT("KBiffMonitor::setPollInterval()");
	poll = interval;

	// Kill any old timers that may be running
	if (oldTimer > 0)
	{
		killTimer(oldTimer);

		// Start a new timer will the specified time
		if (started)
		{
			oldTimer = startTimer(interval * 1000);

			emit(signal_checkMail());
		}
	}
}

void KBiffMonitor::setMailbox(const char* url)
{
TRACEINIT("KBiffMonitor::setMailbox()");
	KBiffURL kurl(url);
	setMailbox(kurl);
}

void KBiffMonitor::setMailbox(KBiffURL& url)
{
TRACEINIT("KBiffMonitor::setMailbox()");
	if (imap)
	{
		delete imap;
		imap = 0;
	}
	if (pop)
	{
		delete pop;
		pop = 0;
	}
	if (nntp)
	{
		delete nntp;
		nntp = 0;
	}

	protocol = url.protocol();

	if (protocol == "imap4")
	{
		disconnect(this);

		imap = new KBiffImap;

		connect(this, SIGNAL(signal_checkMail()), SLOT(checkImap()));
		server   = url.host();
		user     = url.user();
		password = url.passwd();

		mailbox  = QString(url.path()).right(strlen(url.path()) - 1); 
		port     = (url.port() > 0) ? url.port() : 143;

		preauth = !strcmp(url.searchPart(), "preauth") ? true : false;
		keepalive = !strcmp(url.reference(), "keepalive") ? true : false;
		bool async = !strcmp(url.reference(), "async") ? true : false;
		imap->setAsync(async);
TRACEF("preauth = %d", preauth);
TRACEF("keepalive = %d", keepalive);
TRACEF("async = %d", imap->isAsync());
		simpleURL = "imap4://" + server + "/" + mailbox;
	}

	if (protocol == "pop3")
	{
		disconnect(this);

		pop = new KBiffPop;

		connect(this, SIGNAL(signal_checkMail()), SLOT(checkPop()));
		server   = url.host();
		user     = url.user();
		password = url.passwd();
		mailbox  = url.user();
		port     = (url.port() > 0) ? url.port() : 110;

		keepalive = !strcmp(url.reference(), "keepalive") ? true : false;
		bool async = !strcmp(url.reference(), "async") ? true : false;
		pop->setAsync(async);
TRACEF("keepalive = %d", keepalive);
TRACEF("async = %d", pop->isAsync());

		simpleURL = "pop3://" + server + "/" + mailbox;
	}

	if (protocol == "mbox")
	{
		disconnect(this);

		connect(this, SIGNAL(signal_checkMail()), SLOT(checkMbox()));
		mailbox = url.path();

		simpleURL = "mbox:" + mailbox;
	}

	if (protocol == "file")
	{
		disconnect(this);

		connect(this, SIGNAL(signal_checkMail()), SLOT(checkLocal()));
		mailbox = url.path();

		simpleURL = "file:" + mailbox;
	}

	if (protocol == "maildir")
	{
		disconnect(this);

		connect(this, SIGNAL(signal_checkMail()), SLOT(checkMaildir()));
		mailbox = url.path();

		simpleURL = "maildir:" + mailbox;
	}

	if (protocol == "mh")
	{
		disconnect(this);

		connect(this, SIGNAL(signal_checkMail()), SLOT(checkMHdir()));
		mailbox = url.path();

		simpleURL = "mh:" + mailbox;
	}

	if (protocol == "nntp")
	{
		disconnect(this);

		nntp = new KBiffNntp;

		connect(this, SIGNAL(signal_checkMail()), SLOT(checkNntp()));
		server   = url.host();
		user     = url.user();
		password = url.passwd();

		mailbox  = QString(url.path()).right(strlen(url.path()) - 1); 
		port     = (url.port() > 0) ? url.port() : 119;

		keepalive = !strcmp(url.reference(), "keepalive") ? true : false;
		bool async = !strcmp(url.reference(), "async") ? true : false;
		nntp->setAsync(async);
TRACEF("keepalive = %d", keepalive);
TRACEF("async = %d", nntp->isAsync());
		simpleURL = "nntp://" + server + "/" + mailbox;
	}
	// decode user, password, and path
	KBiffURL::decodeURL(user);
	KBiffURL::decodeURL(password);
	KBiffURL::decodeURL(mailbox);
}

void KBiffMonitor::setMailboxIsRead()
{
TRACEINIT("KBiffMonitor::setMailboxIsRead()");
	lastRead  = QDateTime::currentDateTime();
	if (mailState == NewMail)
	{
		determineState(OldMail);
		mailState = NewMail;
	}
}

void KBiffMonitor::checkMailNow()
{
TRACEINIT("KBiffMonitor::checkMailNow()");
	emit(signal_checkMail());
}

void KBiffMonitor::setPassword(const char* pass)
{
TRACEINIT("KBiffMonitor::setPassword()");
	password = pass;
}

void KBiffMonitor::timerEvent(QTimerEvent *)
{
TRACEINIT("KBiffMonitor::timerEvent()");
	emit(signal_checkMail());
}

void KBiffMonitor::checkLocal()
{
TRACEINIT("KBiffMonitor::checkLocal()");
	// get the information about this local mailbox
	QFileInfo mbox(mailbox);

	// check if we have new mail
	determineState(mbox.size(), mbox.lastRead(), mbox.lastModified());
}

void KBiffMonitor::checkMbox()
{
TRACEINIT("KBiffMonitor::checkMbox()");
	// get the information about this local mailbox
	QFileInfo mbox(mailbox);

	// see if the state has changed
	if ((mbox.lastModified() != lastModified) || (mbox.size() != lastSize))
	{
		lastModified = mbox.lastModified();
		lastSize     = mbox.size();

		// ok, the state *has* changed.  see if the number of
		// new messages has, too.
		newCount = mboxMessages();

		// if there are any new messages, consider the state New
		if (newCount > 0)
			determineState(NewMail);
		else
		{
			if (oldCount == 0)
				determineState(NoMail);
			else
				determineState(OldMail);
		}
	}

	// handle the NoMail case
	if ((mbox.size() == 0) || (oldCount == 0))
	{
		newCount = 0;
		determineState(NoMail);
		return;
	}
}

void KBiffMonitor::checkPop()
{
TRACEINIT("KBiffMonitor::checkPop()");
	QString command;

	// connect to the server unless it is active already
	if (pop->active() == false)
	{
		if(pop->connectSocket(server, port) == false)
		{
			determineState(NoConn);
			return;
		}

		command = "USER " + user + "\r\n";
		if (pop->command(command) == false)
		{
			pop->close();
			return;
		}

		command = "PASS " + password + "\r\n";
		if (pop->command(command) == false)
		{
			pop->close();
			return;
		}
	}
	
	command = "UIDL\r\n";
	if (pop->command(command) == false)
	{
		command = "STAT\r\n";
		if (pop->command(command) == false)
		{
			command = "LIST\r\n";
			if (pop->command(command) == false)
			{
				// if this still doesn't work, then we
				// close this port
				pop->close();
				return;
			}
		}
	}

	if (command == "UIDL\r\n")
		determineState(pop->getUidlList());
	else
		determineState(pop->numberOfMessages());
	
	if (keepalive == false)
		pop->close();
}

void KBiffMonitor::checkImap()
{
TRACEINIT("KBiffMonitor::checkImap()");
	QString command;
	int seq = 1000;
	bool do_login = false;

	// connect to the server
	if (imap->active() == false)
	{
		if (imap->connectSocket(server, port) == false)
		{
			determineState(NoConn);
			return;
		}

		do_login = true;
	}
	
	// imap allows spaces in usernames... we need to take care of that
	user = imap->mungeUser(user);

	// if we are preauthorized OR we want to keep the session alive, then
	// we don't login.  Otherwise, we do.
	if ((preauth == false) && (do_login == true))
	{
		command = QString().setNum(seq) + " LOGIN " + user + " " + password + "\r\n";
		if (imap->command(command, seq) == false)
			return;
		seq++;
	}

	// this will quite nicely get us the number of new messages
	command = QString().setNum(seq) + " STATUS " + mailbox + " (messages unseen)\r\n";
	if (imap->command(command, seq) == false)
		return;
	seq++;

	// lets not logout if we want to keep the session alive
	if (keepalive == false)
	{
		command = QString().setNum(seq) + " LOGOUT\r\n";
		if (imap->command(command, seq) == false)
			return;
		imap->close();
	}

	// what state are we in?
	if (imap->numberOfMessages() == 0)
	{
		newCount = 0;
		determineState(NoMail);
	}
	else
	{
		newCount = imap->numberOfNewMessages();
		if (newCount > 0)
			determineState(NewMail);
		else
			determineState(OldMail);
	}
}

void KBiffMonitor::checkMaildir()
{
TRACEINIT("KBiffMonitor::checkMaildir()");
	// get the information about this local mailbox
	QDir mbox(mailbox);

	// make sure the mailbox exists
	if (mbox.exists())
	{
		// maildir stores its mail in MAILDIR/new and MAILDIR/cur
		QDir new_mailbox(mailbox + "/new");
		QDir cur_mailbox(mailbox + "/cur");

		// make sure both exist
		if (new_mailbox.exists() && cur_mailbox.exists())
		{
			// check only files
			new_mailbox.setFilter(QDir::Files);
			cur_mailbox.setFilter(QDir::Files);

			// get the number of messages in each
			newCount = new_mailbox.count();
			int cur_count = cur_mailbox.count();

			// all messages in 'new' are new
			if (newCount > 0)
			{
				determineState(NewMail);
			}
			// failing that, we look for any old ones
			else if (cur_count > 0)
			{
				determineState(OldMail);
			}
			// failing that, we have no mail
			else
				determineState(NoMail);
		}
	}
}

void KBiffMonitor::checkNntp()
{
TRACEINIT("KBiffMonitor::checkNntp()");
	QString command;
	bool do_login = false;

	// connect to the server
	if (nntp->active() == false)
	{
		if (nntp->connectSocket(server, port) == false)
		{
			determineState(NoConn);
			return;
		}

		do_login = true;
	}
	
	// if we are preauthorized OR we want to keep the session alive, then
	// we don't login.  Otherwise, we do.
	if ((preauth == false) && (do_login == true))
	{
		if (user.isEmpty() == false)
		{
			command = "authinfo user " + user + "\r\n";
			if (nntp->command(command) == false)
				return;
		}
		if (password.isEmpty() == false)
		{
			command = "authinfo pass " + password + "\r\n";
			if (nntp->command(command) == false)
				return;
		}
	}

	command = "group " + mailbox + "\r\n";
	if (nntp->command(command) == false)
		return;

	// lets not logout if we want to keep the session alive
	if (keepalive == false)
	{
		command = "QUIT\r\n";
		nntp->command(command);
		nntp->close();
	}

	// now, we process the .newsrc file
	QString home(getenv("HOME"));
	QString newsrc_path(home + "/.newsrc");
	QFile newsrc(newsrc_path);
	if (newsrc.open(IO_ReadOnly) == false)
	{
		TRACEF("Could not open %s", (const char*)newsrc_path);
		return;
	}

TRACEF("Opened %s", (const char*)newsrc_path);
	char c_buffer[MAXSTR];
	while(newsrc.readLine(c_buffer, MAXSTR) > 0)
	{
TRACEF("Checking '%s' vs '%s'", c_buffer, (const char*)mailbox);
		// search for our mailbox name
		QString str_buffer(c_buffer);
		if (str_buffer.left(mailbox.length()) != mailbox)
			continue;

		// we now have our mailbox.  this parsing routine is so
		// ugly, however, that I could almost cry.  it assumes way
		// too much.  the "actual" range MUST be 1-something
		// continuously and our read sequence MUST be sequentially in
		// order
		bool range = false;
		int last = 1;
		newCount = 0;
		char *buffer = c_buffer;

		// skip over the mailbox name
		for(; buffer && *buffer != ' '; buffer++);

TRACEF("Range: %s", buffer);
TRACEF("Nntp First: %d\n", nntp->first());
TRACEF("Nntp Last: %d\n", nntp->last());
		// iterate over the sequence until we hit a newline or end of string
		while (buffer && *buffer != '\n' && *buffer != '\0')
		{
			// make sure that this is a digit
			if (!isdigit(*buffer))
			{
				buffer++;
				continue;
			}

			// okay, what digit are we looking at?  atoi() will convert
			// only those digits it recognizes to an it.  this will handily
			// skip spaces, dashes, commas, etc
			char *digit = buffer;
			int current = atoi(digit);

			// if our current digit is greater than is possible, then we
			// should just quit while we're (somewhat) ahead
			if (current > nntp->last())
				break;

			// we treat our sequences different ways if we are in a range
			// or not.  specifically, if we are in the top half of a range,
			// we don't do anything
			if (range == false)
			{
				if (current > last)
					newCount += current - last - 1;
			}
			else
				range = false;

			// set our 'last' one for the next go-round
			last = current;

			// skip over all of these digits
			for(;buffer && isdigit(*buffer); buffer++);

			// is this a range?
			if (*buffer == '-')
				range = true;
		}

		// get the last few new ones
		if (last < nntp->last())
			newCount += nntp->last() - last;

		break;
	}
TRACEF("newCount = %d", newCount);
	// with newsgroups, it is either new or non-existant.  it
	// doesn't make sense to count the number of read mails
	if (newCount > 0)
		determineState(NewMail);
	else
		determineState(OldMail);
}

/*
 * MH support provided by David Woodhouse <David.Woodhouse@mvhi.com>
 */
void KBiffMonitor::checkMHdir()
{
TRACEINIT("KBiffMonitor::checkMHdir()");
	// get the information about this local mailbox
	QDir mbox(mailbox);
	char the_buffer[MAXSTR];
	char *buffer = the_buffer;

	// make sure the mailbox exists
	if (mbox.exists())
	{
		QFile mhseq(mailbox+"/.mh_sequences");
		if (mhseq.open(IO_ReadOnly) == true)
		{
			// Check the .mh_sequences file for 'unseen:'
			
			buffer[MAXSTR-1]=0;
			
			while(mhseq.readLine(buffer, MAXSTR-2) > 0)
			{
				if (!strchr(buffer, '\n') && !mhseq.atEnd())
				{
					// read till the end of the line

					int c;
					while((c=mhseq.getch()) >=0 && c !='\n');
				}
				if (!strncmp(buffer, "unseen:", 7))
				{
					// There are unseen messages
					// we will now attempt to count exactly how
					// many new messages there are

					// an unseen sequence looks something like so:
					// unseen: 1, 5-9, 27, 35-41
					bool range = false;
					int last = 0;

					// initialize the number of new messages
					newCount = 0;

					// jump to the correct position and iterate through the
					// rest of the buffer
					buffer+=7;
					while(*buffer != '\n' && buffer)
					{
						// is this a digit?  if so, it is the first of possibly
						// several digits
						if (isdigit(*buffer))
						{
							// whether or not this is a range, we are guaranteed
							// of at least *one* new message
							newCount++;
							
							// get a handle to this digit.  atoi() will convert
							// only those digits it recognizes to an int.  so
							// atoi("123garbage") would become 123
							char *digit = buffer;

							// if we are in the second half of a range, we need
							// to compute the number of new messages.
							if (range)
							{
								// remember that we have already counted the
								// two extremes.. hence we need to subtract one.
								newCount += atoi(digit) - last - 1;
								range = false;
							}

							// skip over all digits
							for(;buffer && isdigit(*buffer); buffer++);

							// check if we are in a range
							if (*buffer == '-')
							{
								// save the current digit for later computing
								last = atoi(digit);
								range = true;
							}
						}
						else
							buffer++;
					}
					mhseq.close();
					determineState(NewMail);
					return;
				}
			}
			mhseq.close();
		}
			
		// OK. No new messages listed in .mh_sequences. Check if 
		//  there are any old ones.
		//mbox.setFilter(QDir::Files);
		QStrList mails = *mbox.entryList(QDir::Files);

		for (const char * str = mails.first(); str; str = mails.next())
		{
			// Check each file in the directory.
			// If it's a numeric filename, then it's a mail.
			const char * c;

			for (c=str; *c; c++)
			{
				if (!isdigit(*c))
					break;
			}
			if (!(*c))
			{
				// We found a filename which was entirely
				// made up of digits - it's a real mail, so
				// respond accordingly.

			   determineState(OldMail);
				return;
			}	
		}

		// We haven't found any valid filenames. No Mail.
		determineState(NoMail);
	}      
}

void KBiffMonitor::determineState(unsigned int size)
{
TRACEINIT("KBiffMonitor::determineState()");
	// check for no mail
	if (size == 0)
	{
		if (mailState != NoMail)
		{
			mailState = NoMail;
			lastSize  = 0;
			newCount  = 0;
			emit(signal_noMail());
			emit(signal_noMail(simpleURL));
		}

		emit(signal_currentStatus(newCount, simpleURL, mailState));
		return;
	}

	// check for new mail
	if (size > lastSize)
	{
		if (mailState != NewMail)
		{
			mailState = NewMail;
			newCount  = size - lastSize;
			lastSize  = size;
			emit(signal_newMail());
			emit(signal_newMail(newCount, simpleURL));
		}

		emit(signal_currentStatus(newCount, simpleURL, mailState));
		return;
	}

	// if we have *some* mail, but the state is unknown,
	// then we'll consider it old
	if (mailState == UnknownState)
	{
		mailState = OldMail;
		lastSize  = size;
		emit(signal_oldMail());
		emit(signal_oldMail(simpleURL));

		emit(signal_currentStatus(newCount, simpleURL, mailState));
		return;
	}

	// check for old mail
	if (size < lastSize)
	{
		if (mailState != OldMail)
		{
			mailState = OldMail;
			lastSize  = size;
			emit(signal_oldMail());
			emit(signal_oldMail(simpleURL));
		}
	}
	emit(signal_currentStatus(newCount, simpleURL, mailState));
}

void KBiffMonitor::determineState(KBiffUidlList uidl_list)
{
TRACEINIT("KBiffMonitor::determineState()");    
	QString *UIDL;
	unsigned int messages = 0;

TRACEF("uidl_list.count() = %d", uidl_list.count());
	// if the uidl_list is empty then the number of messages = 0  
	if (uidl_list.isEmpty())
	{
		if (mailState != NoMail)
		{
			lastSize  = newCount = 0;
			mailState = NoMail;
			emit(signal_noMail());
			emit(signal_noMail(simpleURL));
		}
	}
	else
	{
		// if a member of uidl_list is not in the old uidlList then we have 
		// new mail
		for (UIDL = uidl_list.first(); UIDL != 0; UIDL = uidl_list.next())
		{
			if (uidlList.find(UIDL) == -1)
				messages++;
		}
TRACEF("new messages = %d", messages);

		// put this before the emit.  don't know why it matters... but it
		// apparently does on some systems!
		uidlList = uidl_list;

		// if there are any new messages, then notify
		if (messages > 0) 
		{
			lastSize  = newCount = messages;
			mailState = NewMail;
			emit(signal_newMail());
			emit(signal_newMail(newCount, simpleURL));
		}
		/*
		else
		{
			mailState = OldMail;
			emit(signal_oldMail());
			emit(signal_oldMail(simpleURL));
		}
		*/
	}
	emit(signal_currentStatus(newCount, simpleURL, mailState));
}
    
void KBiffMonitor::determineState(KBiffMailState state)
{
TRACEINIT("KBiffMonitor::determineState()");
	if ((state == NewMail) && (mailState != NewMail))
	{
		mailState = NewMail;
		emit(signal_newMail());
		emit(signal_newMail(newCount, simpleURL));
	}
	else
	if ((state == NoMail) && (mailState != NoMail))
	{
		mailState = NoMail;
		emit(signal_noMail());
		emit(signal_noMail(simpleURL));
	}
	else
	if ((state == OldMail) && (mailState != OldMail))
	{
		mailState = OldMail;
		emit(signal_oldMail());
		emit(signal_oldMail(simpleURL));
	}
	else
	if ((state == NoConn) && (mailState != NoConn))
	{
		mailState = NoConn;
		emit(signal_noConn());
		emit(signal_noConn(simpleURL));
	}
	emit(signal_currentStatus(newCount, simpleURL, mailState));
}

void KBiffMonitor::determineState(unsigned int size, const QDateTime& last_read, const QDateTime& last_modified)
{
TRACEINIT("KBiffMonitor::determineState()");
TRACEF("mailState = %d", mailState);
TRACEF("last_read = %s", (const char*)last_read.toString());
TRACEF("lastRead = %s", (const char*)lastRead.toString());
TRACEF("last_modified = %s", last_modified.toString().data());
	// Check for NoMail
	if (size == 0)
	{
		// Is this a new state?
		if (mailState != NoMail)
		{
			// Yes, the user has just nuked the entire mailbox
			mailState = NoMail;
			lastRead  = last_read;
			lastSize  = 0;

			// Let the world know of the new state
			emit(signal_noMail());
			emit(signal_noMail(simpleURL));
		}
	}
	else
	// There is some mail.  See if it is new or not.  To be new, the
	// mailbox must have been modified after it was last read AND the
	// current size must be greater then it was before.
	if ((last_modified >= last_read) && (size > lastSize))
	{
		// We have new mail!
		mailState = NewMail;
		lastRead  = last_read;
		lastSize  = size;
		newCount  = 1;

		// Let the world know of the new state
		emit(signal_newMail());
		emit(signal_newMail(1, simpleURL));
	}
	else
	// Finally, check if the state needs to change to OldMail
	if ((mailState != OldMail) && (last_read > lastRead))
	{
		mailState = OldMail;
		lastRead  = last_read;
		lastSize  = size;

		// Let the world know of the new state
		emit(signal_oldMail());
		emit(signal_oldMail(simpleURL));
	}

	// If we get to this point, then the state now is exactly the
	// same as the state when last we checked.  Do nothing at this
	// point.
	emit(signal_currentStatus(newCount, simpleURL, mailState));
}

/**
 * The following function is lifted from unixdrop.cpp in the korn
 * distribution.  It is (C) Sirtaj Singh Kang <taj@kde.org> and is
 * used under the GPL license (and the author's permission).  It has
 * been slightly modified for formatting reasons.
 */
int KBiffMonitor::mboxMessages()
{
	QFile mbox(mailbox);
	char buffer[MAXSTR];
	int count            = 0;
	int msg_count        = 0;
	bool in_header       = false;
	bool has_content_len = false;
	bool msg_read        = false;
	long content_length  = 0;

	oldCount = 0;

	if (mbox.open(IO_ReadOnly) == false)
		return 0;

	buffer[MAXSTR-1] = 0;

	while (mbox.readLine(buffer, MAXSTR-2) > 0)
	{
		// read a line from the mailbox

		if (!strchr(buffer, '\n') && !mbox.atEnd())
		{
			// read till the end of the line if we
			// haven't already read all of it.

			int c;

			while((c=mbox.getch()) >=0 && c !='\n');
		}

		if (!in_header && real_from(buffer))
		{
			// check if this is the start of a message
			has_content_len = false;
			in_header       = true;
			msg_read        = false;
		}
		else if (in_header)
		{
			// check header fields if we're already in one

			if (compare_header(buffer, "Content-Length"))
			{
				has_content_len = true;
				content_length  = atol(buffer + 15);
			}
			// This should handle those folders that double as IMAP or POP
			// folders.  Possibly PINE uses these always
			if (strcmp(buffer, "Subject: DON'T DELETE THIS MESSAGE -- FOLDER INTERNAL DATA\n") == 0)
			{
				oldCount--;
			}
			else
			{
				if (compare_header(buffer, "Status"))
				{
					const char *field = buffer;
					field += 7;
					while (field && (*field== ' ' || *field == '\t'))
						field++;

					if (*field == 'N' || *field == 'U' || *field == 0x0a)
						msg_read = false;
					else
						msg_read = true;
				}
				// Netscape *sometimes* uses X-Mozilla-Status to determine
				// unread vs read mail.  The only pattern I could see for
				// sure, though, was that Read mails started with an '8'.
				// I make no guarantees on this...
				else if (compare_header(buffer, "X-Mozilla-Status"))
				{
					const char *field = buffer;
					field += 17;
					while (field && (*field== ' ' || *field == '\t'))
						field++;

					if (*field == '8')
						msg_read = true;
					else
						msg_read = false;
				}
				else if (buffer[0] == '\n' )
				{
					if (has_content_len)
						mbox.at(mbox.at() + content_length);

					in_header = false;

					oldCount++;

					if (!msg_read)
						count++;
				} 
			}
		}//in header

		if(++msg_count >= 100 )
		{
			qApp->processEvents();
			msg_count = 0;
		}
	}//while

	mbox.close();

	return count;
}

///////////////////////////////////////////////////////////////////////////
// KBiffSocket
///////////////////////////////////////////////////////////////////////////
KBiffSocket::KBiffSocket() : async(false), socketFD(-1), messages(0), newMessages(-1)
{
	FD_ZERO(&socketFDS);

	/*
	 * Set the socketTO once and DO NOT use it in any select call as this
	 * may alter its value!
	 */
	socketTO.tv_sec = SOCKET_TIMEOUT;
	socketTO.tv_usec = 0;
}

KBiffSocket::~KBiffSocket()
{
TRACEINIT("KBiffSocket::~KBiffSocket()");
	close();
}

int KBiffSocket::numberOfMessages()
{
	return messages;
}

int KBiffSocket::numberOfNewMessages()
{
	return (newMessages > -1) ? newMessages : 0;
}

void KBiffSocket::close()
{
TRACEINIT("KBiffSocket::close()");

	if (socketFD != -1)
		::close(socketFD);

	socketFD = -1;
	FD_ZERO(&socketFDS);
}

bool KBiffSocket::connectSocket(const char* host, unsigned int port)
{
TRACEINIT("KBiffSocket::connectSocket()");
	sockaddr_in  sin;
	hostent     *hent; 
	int addr, n;

	// if we still have a socket, close it
	if (socketFD != -1)
		close();

	// get the socket
	socketFD = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	// start setting up the socket info
	memset((char *)&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port   = htons(port);

	// get the address
	if ((addr = inet_addr(host)) == -1)
	{
		// get the address by host name
		if ((hent = gethostbyname(host)) == 0)
		{
			switch (h_errno)
			{
				case HOST_NOT_FOUND: 
TRACE("The specified host is unknown.");
					break;

				case NO_ADDRESS:
TRACE("The requested name is valid but does not have an IP address.");
					break;

				case NO_RECOVERY:
TRACE("A non-recoverable name server error occurred.");
					break;

				case TRY_AGAIN:
TRACE("A temporary error occurred on an authoritative name server.");
					break;

			        default:
TRACE("An unknown error occured when looking up mail server host name.");
			}

			close();
			return false;
		}

		memcpy((void *)&sin.sin_addr, *(hent->h_addr_list), hent->h_length);
	}
	else
		// get the address by IP
		memcpy((void *)&sin.sin_addr, (void *)&addr, sizeof(addr));


	// Set up non-blocking io if requested
	if (async)
	{
		int flags = fcntl(socketFD, F_GETFL);
		if (flags < 0 || fcntl(socketFD, F_SETFL, flags | O_NONBLOCK) < 0)
		{
			async = false;
TRACE("Cannot use asynchronous mode");
		} 
	}

TRACEF("O_NONBLOCK = %d", (fcntl(socketFD, F_GETFL) & O_NONBLOCK) > 0);


	// the socket is correctly setup.  now connect
	if ((n = ::connect(socketFD, (sockaddr *)&sin, sizeof(sockaddr_in))) == -1 && 
	    errno != EINPROGRESS)
	{
TRACE("Could not issue a connection to host");
		close();
		return false;
	}


	// Set the socket in the file descriptor set
	FD_ZERO(&socketFDS);
	FD_SET(socketFD, &socketFDS);

	// For non-blocking io, the connection may need time to finish (n = -1)
	if (n == -1 && async == true)
	{
		struct timeval tv = socketTO;

		// Wait for the connection to come up
		if (select(socketFD+1, NULL, &socketFDS, NULL, &tv) != 1)
		{
TRACE("Connection timed out");
			errno = ETIMEDOUT;
			close();
			return false;
		}
	  
		// The connection has finished. Catch any error in a call to readLine()
	}


	// we're connected!  see if the connection is good
	QString line(readLine());
	if (line.isNull() || ((line.find("200") == -1) && (line.find("OK") == -1) && (line.find("PREAUTH") == -1)))
	{
		if (line.isNull())
TRACE("Could not connect to host");

		close();
		return false;
	}

	// everything is swell
	return true;
}

bool KBiffSocket::active()
{
TRACEINIT("KBiffSocket::active()");
TRACEF("active = %d", socketFD != -1);
	return socketFD != -1;
}

bool KBiffSocket::isAsync()
{
TRACEINIT("KBiffSocket::isAsync()");
TRACEF("async = %d", async);
	return async;
}

void KBiffSocket::setAsync(bool on)
{
TRACEINIT("KBiffSocket::setAsync()");
	int flags = 0;

	async = on; 

	if (active())
	{
		flags = fcntl(socketFD, F_GETFL);

		switch (async)
		{
			case false:
				if (flags >= 0)
					fcntl(socketFD, F_SETFL, flags & ~O_NONBLOCK);
				break;

			case true:
				if (flags < 0 || fcntl(socketFD, F_SETFL, flags | O_NONBLOCK) < 0)
					async = false;
				break;
		}
	}


	if (async != on)
TRACE("Cannot use asynchronous mode");

TRACEF("O_NONBLOCK = %d", (fcntl(socketFD, F_GETFL) & O_NONBLOCK) > 0);
TRACEF("async = %d", async);
}

int KBiffSocket::writeLine(const QString& line)
{
TRACEINIT("KBiffSocket::writeLine()");
	int bytes;

	// Do not try to write to a non active socket. Return error.
	if (!active())
		return -1;

TRACEF("CLIENT> %s", (const char*)line);
	if ((bytes = ::write(socketFD, line, line.size()-1)) <= 0)
		close();

	return bytes;
}

QString KBiffSocket::readLine()
{
TRACEINIT("KBiffSocket::readLine()");
	QString fault, response;
	char buffer;
	ssize_t bytes = -1;

	if (!async)
		while (((bytes = ::read(socketFD, &buffer, 1)) > 0) && (buffer != '\n'))
			response += buffer;
	else
	{
		while ( (((bytes = ::read(socketFD, &buffer, 1)) > 0) && (buffer != '\n')) || 
			((bytes < 0) && (errno == EWOULDBLOCK)) )
		{
			if (bytes > 0)
				response += buffer;
			else
			{
				struct timeval tv = socketTO;
				if (select(socketFD+1,  &socketFDS, NULL, NULL, &tv) != 1)
				{
TRACE("Connection timed out");
					errno = ETIMEDOUT;
					break;
				}
			}
		}
	}

	if (bytes == -1)
	{
TRACEF("Errno = %d", errno);
		// Close the socket and hope for better luck with a new one
		close();

TRACE("Error on read");
		return fault;
	}

TRACEF("SERVER> %s\n", (const char*)response);
	return response;
}

///////////////////////////////////////////////////////////////////////////
// KBiffImap
///////////////////////////////////////////////////////////////////////////
KBiffImap::~KBiffImap()
{
TRACEINIT("KBiffImap::~KBiffImap()");
	close();
}

bool KBiffImap::command(const QString& line, unsigned int seq)
{
TRACEINIT("KBiffIMap::command()");
	int len, match;

	if (writeLine(line) <= 0)
		return false;

	QString ok, response;
	ok.sprintf("%d OK", seq);
	while (response = readLine())
	{
		// if an error has occurred, we get a null string in return
		if (response.isNull())
			return false;

		// if the response is either good or bad, then return
		if (response.find(ok) > -1)
			return true;
		if (response.find("BAD") > -1)
		{
			close();
			return false;
		}
		if (response.find("NO ") > -1)
		{
			close();
			return false;
		}

		// check the number of messages
		QRegExp messages_re("MESSAGES [0-9]*");
		if ((match = messages_re.match(response, 0, &len)) > -1)
			messages = response.mid(match + 9, len - 9).toInt();

		// check for new mail
		QRegExp recent_re("UNSEEN [0-9]*");
		if ((match = recent_re.match(response, 0, &len)) > -1)
			newMessages = response.mid(match + 7, len - 7).toInt();
	}

	close();
	return false;
}

QString KBiffImap::mungeUser(const QString& old_user)
{
TRACEINIT("KBiffImap::mungeUser()");
TRACEF("old_user = %s", old_user.data());
	if (old_user.contains(' ') > 0)
	{
		QString new_user(old_user);

		if (new_user.left(1) != "\"")
			new_user.prepend("\"");
		if (new_user.right(1) != "\"")
			new_user.append("\"");

TRACEF("new_user = %s", new_user.data());
		return new_user;
	}
	else
		return old_user;
}

///////////////////////////////////////////////////////////////////////////
// KBiffPop
///////////////////////////////////////////////////////////////////////////
KBiffPop::~KBiffPop()
{
TRACEINIT("KBiffPop::~KBiffPop()");
	close();
}

void KBiffPop::close()
{
TRACEINIT("KBiffPop::close()");
	command("QUIT\r\n");
	KBiffSocket::close();
}

bool KBiffPop::command(const QString& line)
{
TRACEINIT("KBiffPop::command()");
	if (writeLine(line) <= 0)
		return false;

	QString response;
	response = readLine();

	// check if the response was bad.  if so, return now
	if (response.isNull() || response.left(4) == "-ERR")
	{
		// we used to close the socket here.. but this MAY be
		// because the server didn't understand UIDL.  the server
		// may react better with LIST or STAT so just fail quitely
		// thanks to David Barth (dbarth@videotron.ca)
		return false;
	}

	// if the command was UIDL then build up the newUidlList
	if (line == "UIDL\r\n")
	{
		uidlList.clear();
		for (response = readLine();
		     !response.isNull() && response.left(1) != ".";
		     response = readLine())
		{
			uidlList.append(new QString(response.right(response.length() -
					response.find(" ") - 1)));
		}
	}
	else
	// get all response lines from the LIST command    
	// LIST and UIDL are return multilines so we have to loop around
	if (line == "LIST\r\n")
	{
		for (messages = 0, response = readLine(); 
		     !response.isNull() && response.left(1) != ".";
		     messages++, response = readLine());
	}
	else
	if (line == "STAT\r\n")
	{
		if (!response.isNull())
			sscanf(response.data(), "+OK %d", &messages);
	}

	return !response.isNull();
}

KBiffUidlList KBiffPop::getUidlList() const
{
	return uidlList;
}

///////////////////////////////////////////////////////////////////////////
// KBiffNntp
///////////////////////////////////////////////////////////////////////////
KBiffNntp::~KBiffNntp()
{
TRACEINIT("KBiffNntp::~KBiffNntp()");
	close();
}

bool KBiffNntp::command(const QString& line)
{
TRACEINIT("KBiffNntp::command()");
	int bogus;

	if (writeLine(line) <= 0)
		return false;

	QString response;
	while (response = readLine())
	{
		// return if the response is bad
		if (response.find("500") > -1)
		{
			close();
			return false;
		}

		// find return codes for tcp, user, pass
		QString code(response.left(3));
		TRACEF("code = %s", (const char*)code);
		if ((code == "200") || (code == "281") || (code == "381"))
			return true;

		// look for the response to the GROUP command
		// 211 <num> <first> <last> <group>
		if (code == "211")
		{
			sscanf((const char*)response, "%d %d %d %d",
					&bogus, &messages, &firstMsg, &lastMsg);
			return true;
		}
	}

	close();
	return false;
}

int KBiffNntp::first() const
{
	return firstMsg;
}

int KBiffNntp::last() const
{
	return lastMsg;
}

/////////////////////////////////////////////////////////////////////////
/* The following is a (C) Sirtaj Singh Kang <taj@kde.org> */

#define whitespace(c)    (c == ' ' || c == '\t')

#define skip_white(c)	 while(c && (*c) && whitespace(*c) ) c++
#define skip_nonwhite(c) while(c && (*c) && !whitespace(*c) ) c++

#define skip_token(buf) skip_nonwhite(buf); if(!*buf) return false; \
	skip_white(buf); if(!*buf) return false;

static char *month_name[13] = {
	"jan", "feb", "mar", "apr", "may", "jun",
	"jul", "aug", "sep", "oct", "nov", "dec", NULL
};

static char *day_name[8] = {
	"sun", "mon", "tue", "wed", "thu", "fri", "sat", 0
};

static bool real_from(const char *buffer)
{
	/*
		A valid from line will be in the following format:

		From <user> <weekday> <month> <day> <hr:min:sec> [TZ1 [TZ2]] <year>
	 */

	int day;
	int i;
	int found;

	/* From */

	if(!buffer || !*buffer)
		return false;

	if (strncmp(buffer, "From ", 5))
		return false;

	buffer += 5;

	skip_white(buffer);

	/* <user> */
	if(*buffer == 0) return false;
	skip_token(buffer);

	/* <weekday> */
	found = 0;
	for (i = 0; day_name[i] != NULL; i++)
		found = found || (strnicmp(day_name[i], buffer, 3) == 0);

	if (!found)
		return false;

	skip_token(buffer);

	/* <month> */
	found = 0;
	for (i = 0; month_name[i] != NULL; i++)
		found = found || (strnicmp(month_name[i], buffer, 3) == 0);
	if (!found)
		return false;

	skip_token(buffer);

	/* <day> */
	if ( (day = atoi(buffer)) < 0 || day < 1 || day > 31)
		return false;

	return true;
}

static const char *compare_header(const char *header, const char *field)
{
	int len = strlen(field);

	if (strnicmp(header, field, len))
		return NULL;

	header += len;

	if( *header != ':' )
		return NULL;

	header++;

	while( *header && ( *header == ' ' || *header == '\t') )
		header++;

	return header;
}
