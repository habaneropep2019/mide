

#ifndef _SSK_KBOXFILE_
#define _SSK_KBOXFILE_

#include<qobject.h>
#include<qstring.h>
#include<qtimer.h>

class QFileInfo;
class QDateTime;

///
class KMailBox	:	public QObject
{
	Q_OBJECT

private:

/**@name mailbox file properties
*/
//@{
	QString		_file;
	QFileInfo	*_fileInfo;
	unsigned 	_lastSize;
	QDateTime	*_lastModified;
	int		_numMessages;
	bool 		_valid;
//@}

/**@name timer properties
*/
//@{
	int 		_pollTime;
	QTimer		*_timer;
//@}

	/// counts messages in the file.
	int countMail();

	/// checks if the mailbox was changed.
	bool wasTouched();

	/// Checks for a header entry in the input buffer.
	const char *compareHeader(const char *buffer, const char *field);

public:
	///
	KMailBox(const char *file=0, int pollTime=360, bool start=FALSE);
	///
	~KMailBox();

	///
	void setPollTime(int seconds);

	///
	int pollTime() const {return _pollTime;};

	/// checks if mailbox is locked.
	bool isLocked() const;

	/// returns the number of unread messages at last count
	int unreadMessages() const;
	
/**@name slots */
//@{

public slots:

	///
	void reread();

	///
	void touch();

	///
	void setInvalid();
	///
	void start();

public:
	///
	const char *mailbox() const {return _file;};
	///
	void setMailbox(const char *mbox);

//@}

/**@name signals */
//@{

signals:
	///
	void changed( int );

	///
	void fileError();

//@}

};

inline int KMailBox::unreadMessages() const
{
	return _numMessages;
}
#endif
