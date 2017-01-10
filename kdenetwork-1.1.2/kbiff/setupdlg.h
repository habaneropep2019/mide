/*
 * setupdlg.h
 * Copyright (C) 1999 Kurt Granroth <granroth@kde.org>
 *
 * This file contains the setup dialog and related widgets
 * for KBiff.  All user configuration is done here.
 *
 * $Id: setupdlg.h,v 1.15.2.1 1999/06/04 02:25:13 granroth Exp $
 */
#ifndef SETUPDLG_H
#define SETUPDLG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif //HAVE_CONFIG_H

#include <qlistview.h>

#include <qwidget.h>
#include <qdialog.h>

// mediatool.h is needed by kaudio.h
extern "C" {
#include <mediatool.h>
} 
#include <kaudio.h>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QComboBox;

#include <kiconloaderdialog.h>
#include <kbiffurl.h>

struct KBiffMailbox
{
	KBiffURL url;
	bool store;
};

class KBiffGeneralTab;
class KBiffNewMailTab;
class KBiffMailboxTab;
class KBiffAboutTab;
class KBiffNewDlg;

class KBiffSetup : public QDialog
{
	Q_OBJECT
public:
	KBiffSetup(QString name = "", bool secure = false);
	virtual ~KBiffSetup();

	const QString getProfile() const;
	const KBiffURL getCurrentMailbox() const;
	const QList<KBiffURL> getMailboxList() const;

	const char* getMailClient() const;
	const char* getRunCommandPath() const;
	const char* getPlaySoundPath() const;
	const char* getNoMailIcon() const;
	const char* getNewMailIcon() const;
	const char* getOldMailIcon() const;
	const char* getNoConnIcon() const;
	const bool getSystemBeep() const;
	const bool getRunCommand() const;
	const bool getPlaySound() const;
	const bool getNotify() const;
	const bool getStatus() const;
	const bool getDock() const;
	const bool getSessionManagement() const;
	const bool getSecure() const;
	const unsigned int getPoll() const;

public slots:
	void invokeHelp();

	void readConfig(const char *profile);
	void saveConfig();

protected:
	QString getSomeProfile() const;

protected slots:
	void slotDone();
	void slotAddNewProfile();
	void slotRenameProfile();
	void slotDeleteProfile();

private:
	bool isSecure;

	// "outer" dialog
	QComboBox   *comboProfile;
	QPushButton *help;
	QPushButton *ok;
	QPushButton *cancel;

	// tabs
	KBiffGeneralTab *generalTab;
	KBiffNewMailTab *newmailTab;
	KBiffMailboxTab *mailboxTab;
	KBiffAboutTab   *aboutTab;
};

class KBiffGeneralTab : public QWidget
{
	Q_OBJECT
public:
	KBiffGeneralTab(const char* profile = 0, QWidget *parent=0);
	virtual ~KBiffGeneralTab();

	const char* getButtonNewMail() const;
	const char* getButtonOldMail() const;
	const char* getButtonNoMail() const;
	const char* getButtonNoConn() const;
	const char* getMailClient() const;
	const int   getPoll() const;
	const bool  getDock() const;
	const bool  getSessionManagement() const;

public slots:
	void readConfig(const char *profile);
	void saveConfig(const char *profile);

private:
	QLineEdit* editPoll;
	QLineEdit* editCommand;
	QCheckBox* checkDock;
	QCheckBox* checkNoSession;

	KIconLoaderButton *buttonNoMail;
	KIconLoaderButton *buttonOldMail;
	KIconLoaderButton *buttonNewMail;
	KIconLoaderButton *buttonNoConn;
};

class KBiffNewMailTab : public QWidget
{
	Q_OBJECT
public:
	KBiffNewMailTab(const char* profile = 0, QWidget *parent=0);
	virtual ~KBiffNewMailTab();

	const bool getRunCommand() const;
	const char* getRunCommandPath() const;
	const bool getPlaySound() const;
	const char* getPlaySoundPath() const;
	const bool getSystemBeep() const;
	const bool getNotify() const;
	const bool getStatus() const;

public slots:
	void readConfig(const char *profile);
	void saveConfig(const char *profile);

protected slots:
	void enableRunCommand(bool);
	void enablePlaySound(bool);
	void browseRunCommand();
	void browsePlaySound();
	void testPlaySound();

private:
	QLineEdit *editRunCommand;
	QLineEdit *editPlaySound;

	QCheckBox *checkRunCommand;
	QCheckBox *checkPlaySound;
	QCheckBox *checkBeep;
	QCheckBox *checkNotify;
	QCheckBox *checkStatus;

	QPushButton *buttonBrowsePlaySound;
	QPushButton *buttonTestPlaySound;
	QPushButton *buttonBrowseRunCommand;

	bool    hasAudio;
	KAudio  audioServer;
};

class KBiffMailboxAdvanced : public QDialog
{
	Q_OBJECT
public:
	KBiffMailboxAdvanced();
	virtual ~KBiffMailboxAdvanced();

	const KBiffURL getMailbox() const;
	const unsigned int getPort() const;
	bool getPreauth() const;

	void setPort(unsigned int the_port, bool enable = true);
	void setMailbox(const KBiffURL& url);
	void setPreauth(bool on);
	void setKeepalive(bool on);
	void setAsync(bool on);

protected slots:
	void portModified(const char* text);
	void preauthModified(bool toggled);
	void keepaliveModified(bool toggled);
	void asyncModified(bool toggled);

private:
	QString    password;
	QLineEdit *mailbox;
	QLineEdit *port;
	QCheckBox *preauth;
	QCheckBox *keepalive;
	QCheckBox *async;
};

class KBiffMailboxTab : public QWidget
{
	Q_OBJECT
public:
	KBiffMailboxTab(const char* profile = 0, QWidget *parent=0);
	virtual ~KBiffMailboxTab();

	void setMailbox(const KBiffURL& url);

	const KBiffURL getMailbox() const;
	const QList<KBiffURL> getMailboxList() const;

public slots:
	void readConfig(const char *profile);
	void saveConfig(const char *profile);

protected slots:
	void slotDeleteMailbox();
	void slotNewMailbox();
	void slotMailboxSelected(QListViewItem *item);

	void protocolSelected(int protocol);
	void browse();
	void advanced();

protected:
	const char* scramble(const char* password, bool encode = true);
	const KBiffURL defaultMailbox() const;

private:
	QDict<KBiffMailbox> *mailboxHash;
	QListViewItem       *oldItem;

	unsigned int port;
	bool         preauth;
	bool         keepalive;
	bool         async;
	QComboBox   *comboProtocol;
	QLineEdit   *editMailbox;
	QLineEdit   *editServer;
	QLineEdit   *editUser;
	QLineEdit   *editPassword;
	QCheckBox   *checkStorePassword;
	QPushButton *buttonBrowse;
	QListView   *mailboxes;
};

class KBiffAboutTab : public QWidget
{
	Q_OBJECT
public:
	KBiffAboutTab(QWidget *parent=0);
	virtual ~KBiffAboutTab();

protected slots:
	void mailTo(const char*);
	void homepage(const char*);
};

class KBiffNewDlg : public QDialog
{
	Q_OBJECT
public:
	KBiffNewDlg(QWidget* parent = 0, const char* name = 0);

	const char* getName() const
		{ return editName->text(); }

private:
	QLineEdit *editName;
};

#endif // SETUPDLG_H
