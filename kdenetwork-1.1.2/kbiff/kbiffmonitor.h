/*
 * kbiffmonitor.h
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * $Id: kbiffmonitor.h,v 1.9.2.1 1999/06/04 02:25:10 granroth Exp $
 *
 */
#ifndef KBIFFMONITOR_H
#define KBIFFMONITOR_H

#include <sys/time.h>

#include <qobject.h>
#include <qlist.h>
#include <qfileinf.h>

class KBiffURL;
class QString;

/**
 * @internal
 * Internal class to store UIDL list
 */
class KBiffUidlList : public QList<QString>
{
protected:
	int compareItems(GCI s1, GCI s2)
	{
		QString *str1, *str2;
		str1 = (QString *)s1;
		str2 = (QString *)s2;
		if((*str1) == (*str2))
			return 0;
		else
			return -1;
	}
};

/**
 * @internal
 */

#define SOCKET_TIMEOUT 5

class KBiffSocket
{
public:
	KBiffSocket();
	virtual ~KBiffSocket();

	bool connectSocket(const char* host, unsigned int port);
	bool active();

	bool isAsync();
	void setAsync(bool on);

	int numberOfMessages();
	int numberOfNewMessages();

	void close();

protected:
	QString readLine();
	int writeLine(const QString& line);

	bool    async;

	struct timeval socketTO;

	int socketFD;
	fd_set socketFDS;
	int messages;
	int newMessages;
};

/**
 * @internal
 */
class KBiffImap : public KBiffSocket
{
public:
	virtual ~KBiffImap();

	bool command(const QString& line, unsigned int seq);
	QString mungeUser(const QString& old_user);
};

/**
 * @internal
 */
class KBiffPop : public KBiffSocket
{
public:
	virtual ~KBiffPop();

	void close();

	bool command(const QString& line);
	KBiffUidlList getUidlList() const;

protected:
	KBiffUidlList  uidlList;    
};

/**
 * @internal
 */
class KBiffNntp : public KBiffSocket
{
public:
	virtual ~KBiffNntp();

	bool command(const QString& line);
	int first() const;
	int last() const;
protected:
	int firstMsg;
	int lastMsg;
};

typedef enum
{
	NewMail,
	NoMail,
	OldMail,
	NoConn,
	UnknownState
} KBiffMailState;

/**
 * A "biff"-like class that can monitor local and remote mailboxes for new
 * mail.  KBiffMonitor currently supports six protocols.
 * 
 * <UL>
 * <LI>mbox</LI> Unix style mailbox files
 * <LI>pop3</LI> POP3 
 * <LI>imap4</LI> imap4 
 * <LI>maildir</LI> Mailboxes in maildir format
 * <LI>mh</LI> Mailboxes in MH format
 * <LI>file</LI> Simple files (no parsing)
 * <LI>nntp</LI> USENET newsgroups
 * </UL>
 *
 * A typical usage would look like so:
 *
 * <PRE>
 *    KBiffMonitor mon;
 *    mon.setMailbox("imap4://user:password@some.host.net/mailbox");
 *    mon.setPollInterval(15);
 *    mon.start();
 *
 *    connect(&mon, SIGNAL(signal_newMail()), this, SLOT(processNewMail()));
 *    connect(&mon, SIGNAL(signal_oldMail()), this, SLOT(processOldMail()));
 *    connect(&mon, SIGNAL(signal_noMail()), this, SLOT(processNoMail()));
 *    connect(&mon, SIGNAL(signal_noConn()), this, SLOT(processNoConn()));
 * </PRE>
 *
 * @short A "biff" class that monitors local and remote mailboxes
 * @author Kurt Granroth <granroth@kde.org>
 * @version $Id: kbiffmonitor.h,v 1.9.2.1 1999/06/04 02:25:10 granroth Exp $
 */
class KBiffMonitor : public QObject
{

	Q_OBJECT
public:

	/**
	 * Constructor.  Does not take any arguments
	 */
	KBiffMonitor();

	/**
	 * Destructor.
	 */
	virtual ~KBiffMonitor();

	/**
	 * Returns the current state of the mailbox (NewMail, OldMail, NoMail, or
	 * UnknownState)
	 */
	KBiffMailState getMailState() const { return mailState; }

	/**
	 * Returns the current mailbox being monitored
	 */
	const QString getMailbox() const { return simpleURL; }

	/**
	 * Returns the type of mailbox being monitored
	 */
	const char* getProtocol() const { return protocol; }
	/**
	 * Returns <CODE>true</CODE> is KBiffMonitor is currently monitoring
	 * a mailbox.
	 */
	bool isRunning() { return started; }

	/**
	 * Returns the number of new messages for the current mailbox
	 */
	int newMessages() { return newCount; }

public slots:
	/**
	 * Sets the mailbox to monitor.  It uses a KBiffURL to specify the
	 * protocol, host, username, password, port and path (depending on
	 * protocol type).  KBiffMonitor recognizes four protocols:
	 * 
	 * <UL>
	 * <LI>mbox</LI> Unix style mailbox files
	 * <LI>pop3</LI> POP3 
	 * <LI>imap4</LI> IMAP4 
	 * <LI>maildir</LI> Mailboxes in maildir format
	 * <LI>mh</LI> Mailboxes in MH format
	 * <LI>nttp</LI> USENET newsgroups
	 * </UL>
	 *
	 * Some examples:
	 * <PRE>
	 * mbox:/var/spool/mail/granroth
	 * </PRE>
	 *
	 * This would monitor a local file called '/var/spool/mail/granroth'
	 *
	 * <PRE>
	 * pop3://granroth:password@host.net:1234
	 * </PRE>
	 *
	 * This would monitor POP3 mailbox 'granroth' on server 'host.net'
	 * using 1234 as the port and 'password' as the password.
	 *
	 * <PRE>
	 * imap4://granroth:password@host.net/Mail/mailbox
	 * </PRE>
	 *
	 * This would monitor IMAP4 mailbox 'Mail/mailbox' on server 'host.net'
	 * with 'granroth' as the user and 'password' as the password.
	 */
	void setMailbox(KBiffURL& url);

	/**
	 * Overloaded for convenience
	 */
	void setMailbox(const char* url);

	/**
	 * Sets the password for the POP3 and IMAP4 protocols.
	 */
	void setPassword(const char* password);

	/**
	 * Set the interval between mailbox reads.  This is in seconds.
	 */
	void setPollInterval(const int interval);

	/**
	 * Start monitoring the mailbox
	 */
	void start();

	/**
	 * Stop monitoring the mailbox
	 */
	void stop();

	/**
	 * Fakes KBiffMonitor into thinking that the mailbox was just read
	 */
	void setMailboxIsRead();

	/**
	 * Forces a mailbox check
	 */
	void checkMailNow();

signals:
	/**
	 * This will get <CODE>emit</CODE>ed when new mail arrives
	 */
	void signal_newMail();

	/**
	 * This will get <CODE>emit</CODE>ed when new mail arrives
	 */
	void signal_newMail(const int num_new, const char* mailbox);

	/**
	 * This will get <CODE>emit</CODE>ed when no mail exists
	 */
	void signal_noMail();

	/**
	 * This will get <CODE>emit</CODE>ed when no mail exists
	 */
	void signal_noMail(const char* mailbox);

	/**
	 * This will get <CODE>emit</CODE>ed when the mailbox is read
	 */
	void signal_oldMail();

	/**
	 * This will get <CODE>emit</CODE>ed when the mailbox is read
	 */
	void signal_oldMail(const char* mailbox);

	/**
	 * This will get <CODE>emit</CODE>ed when no connection can be established
	 */
	void signal_noConn();

	/**
	 * This will get <CODE>emit</CODE>ed when no connection can be established
	 */
	void signal_noConn(const char* mailbox);

	/**
	 * This will get <CODE>emit</CODE>ed everytime the mailbox
	 * should be checked (determined by @ref #setPollInterval)
	 */
	void signal_checkMail();

	/**
	 * This will get <CODE>emit</CODE>ed everytime the mailbox is
	 * checked.  It contains the current mailbox name, state, and number
	 * of new messages
	 */
	void signal_currentStatus(const int, const char*, const KBiffMailState);

protected:
	void timerEvent(QTimerEvent *);

protected slots:
	void checkLocal();
	void checkMbox();
	void checkPop();
	void checkMaildir();
	void checkImap();
	void checkMHdir();
	void checkNntp();

protected:
	// protected (non-slot) functions
	void determineState(unsigned int size, const QDateTime& last_read,
	                    const QDateTime& last_modified);
	void determineState(unsigned int size);
    
	void determineState(KBiffUidlList uidl_list);
	void determineState(KBiffMailState state);
	int  mboxMessages();

private:
	// General stuff
	int     poll;
	int     oldTimer;
	bool    started;
	int     newCount;
	int     oldCount;

	// Mailbox stuff
	QString simpleURL;
	QString protocol;
	QString mailbox;
	QString server;
	QString user;
	QString password;
	int     port;
	bool    preauth;
	bool    keepalive;

	// State variables
	KBiffMailState mailState;
	unsigned int   lastSize;
	QDateTime      lastRead;
	QDateTime      lastModified;
	KBiffUidlList  uidlList;

	// Socket protocols
	KBiffImap      *imap;
	KBiffPop       *pop;
	KBiffNntp      *nntp;
};

#endif // KBIFFMONITOR_H
