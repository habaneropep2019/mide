/*
* prefdlg.cpp -- Implementation of class KViewPrefDlg.
* Author:	Sirtaj Singh Kang
* Version:	$Id: prefdlg.cpp,v 1.1 1999/01/10 18:16:51 ssk Exp $
* Generated:	Fri Jan  1 16:15:08 EST 1999
*/

#include<assert.h>
#include<qcheckbox.h>
#include<qradiobutton.h>
#include<qlineedit.h>

#include<kapp.h>
#include<kconfig.h>
#include<kaccel.h>
#include<kkeydialog.h>

#include"typolayout.h"
#include"prefdlg.h"

KViewPrefDlg::KViewPrefDlg( KConfig *cfg, KAccel *accel, QWidget *parent )
	: QTabDialog( parent, "PrefDlg", true ),
	_cfg( cfg ),
	_accel( accel ),
	_dict( _accel->keyDict() ),
	_keysDirty( false )
{
	assert( cfg );

// tabdlg settings

	setCaption( i18n( "kView Preferences" ) );

	setCancelButton( i18n( "&Cancel" ) );
	setOkButton( i18n( "&OK" ) );
	setApplyButton( i18n( "&Apply" ) );

	connect( this, SIGNAL(applyButtonPressed()), 
		this, SLOT(applySettings()) );

// General options

	QWidget *optw = new QWidget( this );
	assert( optw );

	KTypoLayout *l = new KTypoLayout( optw );
	l->setGridSize( 1, 2 );

	// load prefs

	KTypoLayout *sl = l->newSubGroup( i18n( "On Load" ), 0, 0 );
	sl->setGridSize( 1, 3 );

	KConfigGroupSaver saver( _cfg, "kview" );
	QString lmode = cfg->readEntry( "LoadMode", "ResizeWindow" );

	sl->startGroup();
	_rw = sl->newRadioButton( i18n("Resize Window"), 0, 0 );
		_rw->setChecked( !stricmp( lmode, "ResizeWindow") );

	_ri = sl->newRadioButton( i18n("Resize Image"), 0, 1 );
		_ri->setChecked( !stricmp( lmode, "ResizeImage") );

	_rn = sl->newRadioButton( i18n("No Resize"), 0, 2 );
		_rn->setChecked( !stricmp( lmode, "ResizeNone") );

	// slideshow prefs
	
	sl = l->newSubGroup( i18n( "Slideshow" ), 0, 1 );
	sl->setGridSize( 3, 2 );

	_loop = sl->newCheckBox( i18n( "Loop" ), 0, 0, 3, 1 );

	_interval = sl->newLineEdit( 
		cfg->readEntry( "SlideInterval", "5" ), 0, 1 );
	sl->newLabel( i18n( "Slide interval (sec)" ), 1, 1, 2, 1 );

	addTab( optw, i18n( "&Options" ) );

// Key bindings
	KKeyChooser *chooser = new KKeyChooser( &_dict, this, false );
	connect( chooser, SIGNAL(keyChange()), this, SLOT(setKeysDirty()) );

	addTab( chooser, i18n( "&Keys" ) );
}

void KViewPrefDlg::applySettings()
{
	KConfigGroupSaver saver( _cfg, "kview" );
	_cfg->writeEntry( "SlideLoop", _loop->isChecked() );
	_cfg->writeEntry( "SlideInterval", _interval->text() );

	const char *lmode = 0;

	if( _rw->isChecked() ) {
		lmode = "ResizeWindow";
	}
	else if ( _ri->isChecked() ) {
		lmode = "ResizeImage";
	}
	else if ( _rn->isChecked() ) {
		lmode = "ResizeNone";
	}
	assert( lmode );

	_cfg->writeEntry( "LoadMode", lmode );

	if ( _keysDirty ) {
		_accel->setKeyDict( _dict );
		_accel->writeSettings( _cfg );
	}
}
