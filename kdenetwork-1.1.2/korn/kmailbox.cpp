
#include<stdlib.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include"kmailbox.h"

#include<qobject.h>
#include<qfileinf.h>
#include<qdatetm.h>
#include<kapp.h>

#include<strings.h>

extern bool realfrom(const char *);

#define MAXSTR (1024)

KMailBox::KMailBox(const char *file, int pollTime, bool sflag)
{
	_valid = true;

	connect(this, SIGNAL(fileError()), this, SLOT(setInvalid()) );

	if(file == 0) {
		_file = getenv("MAIL");	

		if( _file.isEmpty() )
			emit fileError();
	}
	else
		_file = file;

	
	_fileInfo = new QFileInfo(_file.data());

	_lastModified = new QDateTime;
	_lastModified->setTime_t(0);

	_lastSize = 0;
	_numMessages= 0;

//	debug("starting with %u messages", _numMessages);

	_timer = new QTimer(this);
	connect( _timer, SIGNAL(timeout()), this, SLOT(reread()));

	_pollTime= pollTime;

	if( sflag )
		start();
}

KMailBox::~KMailBox()
{
	delete _fileInfo;
	delete _lastModified;
}

void KMailBox::setPollTime(int seconds)
{
	_pollTime = seconds;

	if( _timer ) {
		if(!_timer->isActive())
			_timer->start(seconds*1000);
		else
			_timer->changeInterval(seconds*1000);
	}
}

void KMailBox::reread()
{
	if( wasTouched() ){
//	debug("reread: count start");
		int ml = countMail();
//		debug("reread: count end... %d", ml);
		
		if( ml != _numMessages ) {
			_numMessages = ml;
//			debug("reread: coo! changed to: %d", _numMessages);
			emit changed( _numMessages );
		}
	}
}

void KMailBox::touch()
{
	// to implement
}

void KMailBox::setInvalid()
{
	// to implement
}

void KMailBox::start()
{
	if( _timer && !_timer->isActive() ) {
		reread();
		_timer->start( _pollTime*1000 );	
	}
}

bool KMailBox::isLocked() const
{
	QFileInfo lockFile(_file + QString(".lock"));

	return lockFile.exists();
}

bool KMailBox::wasTouched()
{
	_fileInfo->refresh();

	if(_fileInfo->size() != _lastSize
		|| (_fileInfo->lastModified() > *_lastModified) ) {

		_lastSize = _fileInfo->size();
		*_lastModified = _fileInfo->lastModified();

//		debug("new size: %u", _lastSize);
//		debug("new time: %s", (const char *)_lastModified->toString());

		return true;
	}

	return false;
}

int KMailBox::countMail()
{
	QFile mbox(_file);
	char *buffer = new char[MAXSTR];
	int count=0, msgCount=0;
	bool inHeader = false;
	bool hasContentLen = false;
	bool msgRead = false;
	long contentLength=0;

	if( isLocked() ){
//		debug("countMail: locked. returning.");
		delete[] buffer;
		return _numMessages;
	}

	if(!mbox.open(IO_ReadOnly)) {
		warning(i18n("countMail: file open error"));
		emit fileError();
		delete[]buffer;
		return 0;
	}

	buffer[MAXSTR-1] = 0;

	while(mbox.readLine(buffer, MAXSTR-2) > 0) {

		if( !strchr(buffer, '\n') && !mbox.atEnd() ){
			int c;

			while( (c=mbox.getch()) >=0 && c !='\n' )
				;
		}

		if( !inHeader && realfrom(buffer) ) {
			hasContentLen = false;
			inHeader = true;
			msgRead = false;
		}
		else if ( inHeader ){
			if (compareHeader(buffer, "Content-Length")){
				hasContentLen = true;
				contentLength = atol(buffer+15);
			}

			if (compareHeader(buffer, "Status")) {
				const char *field = buffer;
				field += 7;
				while(field && (*field== ' '||*field == '\t'))
					field++;

				if ( *field == 'N' || *field == 'U' )
					msgRead = false;
				else
					msgRead = true;
			}
			else if (buffer[0] == '\n' ) {
				if( hasContentLen )
					mbox.at( mbox.at() + contentLength);

				inHeader = false;

				if ( !msgRead )
					count++;
			} 
		}//in header

		if(++msgCount >= 100 ) {
			qApp->processEvents();
			msgCount = 0;
		}
	}//while

	mbox.close();

	delete[] buffer;
	return count;

}//countMail

const char *KMailBox::compareHeader(const char *header, const char *field)
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
