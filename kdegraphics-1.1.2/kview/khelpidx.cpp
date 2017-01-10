/*
* khelpidx.cpp -- Implementation of class KHelpIndex.
* Author:	Sirtaj Singh Kang
* Version:	$Id: khelpidx.cpp,v 1.3 1999/01/10 18:16:48 ssk Exp $
* Generated:	Thu Sep  3 13:16:48 EST 1998
*/

#include<assert.h>
#include<qfileinf.h>
#include<qregexp.h>

#include<kprocess.h>
#include<kapp.h>

#include"khelpidx.h"

KHelpIndex::KHelpIndex( const char *path ) 
	:_valid	( false ), 
	_index	( 0 ), 
	_path	( new QString ),
	_idxfile ( new QString ),
	_basename( new QString )
{ 
	assert( path != 0 );
	assert( kapp != 0 );

	*_idxfile = kapp->kde_htmldir().copy() + '/' 
		+ klocale->language() + '/' + path;

	QFileInfo info( *_path );

	if( !info.exists() ) {
		// not found: use the default

		*_path = kapp->kde_htmldir().copy()
			+ "/default/" + path;

		info.setFile( *_path );

		if( info.exists() ) {
			_valid = true;
		}
	}
	else {
		_valid = true;
	}

	if( _valid ) {
		readIndex( *_path );
		*_path = info.dirPath();
	}
}

KHelpIndex::~KHelpIndex() 
{
	delete _index;
	delete _path;
	delete _idxfile;
	delete _basename;
}

bool KHelpIndex::readIndex( const char *path )
{
	assert( path );
	
	if ( _index == 0 ) {
		// create index object

		_index = new QDict<QString>;

		if ( _index == 0 ) {
			warning( "KHelpIndex::readIndex: Out of memory" );
			return false;
		}

		_index->setAutoDelete( true );
	}

	// read the index file
	QFile file( path );

	if( !file.exists() || !file.open( IO_ReadOnly ) ) {
		warning( "KHelpIndex::readIndex: Can't read %s",
			path );
		return false;
	}

	char buffer[ 1001 ];
	QRegExp reg( "\\s*==\\s*" );
	int len = 0, pos = 0, linelen = 0;

	while ( (linelen = file.readLine( buffer, 1000 )) > 0 ) {
		pos = reg.match( buffer, 0, &len );

		if ( pos < 0 ) {
			// skip non-conforming lines
			continue;
		}

		buffer[ pos ] = 0;
		buffer[ linelen - 1 ] = 0;

		if( buffer[ pos + len ] == 0 ) {
			continue;
		}

//		debug( "Inserting '%s' at '%s'", buffer + pos + len,
//			buffer );
	
		_index->insert( buffer, 
			new QString( buffer + pos + len ) );
	}

	return true;
}

bool KHelpIndex::invoke( const char *tag )
{
	if( !valid() ) {
		warning( "KHelpIndex::invoke: Attempt to invoke"
			" invalid object." );
		return false;
	}

	QString *p = _index->find( tag );

	if( p == 0 ) {
		return false;
	}

	QString s = *p;

	if( s.isEmpty() ) {
		return false;
	}
	
	s.insert( 0, '/' );
	s.insert( 0, *_path );

	KProcess launcher;
	launcher << "kdehelp" << s;
	launcher.start( KProcess::DontCare );

	return true;
}

#ifdef KHELPIDX_TEST
int main( int argc, char **argv )
{
	assert( argc > 1 );

	KApplication app( argc, argv );

	KHelpIndex idx( argv[ 1 ] );

	idx.invoke( "whatis" );
}
#endif
