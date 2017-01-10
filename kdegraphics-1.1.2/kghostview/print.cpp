/****************************************************************************
**
** A dialog for the selection of priting options.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "print.h"
#include "print.moc"

#include <qaccel.h>
#include <qlayout.h>
#include <qradiobt.h>
#include <qgrpbox.h>
#include <qvalidator.h>


#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <qmsgbox.h>

PrintSetup::PrintSetup( QWidget *parent, const char *name, QString pname,
				QString spooler, QString variable )
	: QDialog( parent, name, TRUE )
{
	setFocusPolicy(QWidget::StrongFocus);
	setCaption( i18n( "Printer setup" ) );
	
	int border = 10;
	
	QBoxLayout *topLayout = new QVBoxLayout( this, border );
	
	topLayout->addStretch( 10 );
	
	QGridLayout *grid = new QGridLayout( 7, 2, 5 );
	topLayout->addLayout( grid );
	

	grid->addRowSpacing(2,5);
	grid->addRowSpacing(4,5);

	grid->setColStretch(0,10);
	grid->setColStretch(1,100);
	
	leName = new QLineEdit( this );
	leName->setFocus();
	leName->setFixedHeight( leName->sizeHint().height() );
	leName->setText( pname );
	
	grid->addWidget( leName, 1, 1 );
	
	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( leName, i18n("&Printer name"), this );
	tmpQLabel->setAlignment( AlignRight | AlignVCenter | ShowPrefix );
	tmpQLabel->setMinimumSize( tmpQLabel->sizeHint() );
	
	grid->addWidget( tmpQLabel, 1, 0 );
	
	leSpool = new QLineEdit( this );
	leSpool->setFixedHeight( leName->sizeHint().height() );
	leSpool->setText( spooler );
	
	grid->addWidget( leSpool, 3, 1 );
	
	tmpQLabel = new QLabel( leSpool, i18n("&Spooler command"), this );
	tmpQLabel->setAlignment(  AlignRight | AlignVCenter | ShowPrefix );
	tmpQLabel->setMinimumSize( tmpQLabel->sizeHint() );
	
	grid->addWidget( tmpQLabel, 3, 0 );
	
	leVar = new QLineEdit( this );
	leVar->setFixedHeight( leName->sizeHint().height() );
	leVar->setText( variable );
	
	grid->addWidget( leVar, 5, 1 );
	
	tmpQLabel = new QLabel( leVar, i18n("&Environment variable"), this );
	tmpQLabel->setAlignment(  AlignRight | AlignVCenter | ShowPrefix );
	tmpQLabel->setMinimumSize( tmpQLabel->sizeHint() );
	
	grid->addWidget( tmpQLabel, 5, 0 );
	
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	tmpQFrame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	tmpQFrame->setMinimumHeight( tmpQFrame->sizeHint().height() );
	
	topLayout->addWidget( tmpQFrame );
	
	// CREATE BUTTONS
	
	KButtonBox *bbox = new KButtonBox( this );
	bbox->addStretch( 10 );
		
	QPushButton* ok = bbox->addButton( i18n("&OK") );
	connect( ok, SIGNAL(clicked()), SLOT(accept()) );
	
	QPushButton* cancel = bbox->addButton( i18n("&Cancel") );
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
		
	bbox->layout();
	topLayout->addWidget( bbox );

	topLayout->activate();
	
	resize( 300,0 );
}

PrintDialog::PrintDialog( QWidget *parent, const char *name, int maxPages,
							bool marked )		
	: QDialog( parent, name, TRUE )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	pgMode = All;
	/*
	printerName.sprintf( "" ); // default printer will be chosen.
	spoolerCommand.sprintf( "lpr" );
	printerVariable.sprintf( "PRINTER" );
	*/

	KConfig *config = KApplication::getKApplication()->getConfig();
	/* Read in the default options. */
	config->setGroup( "Print" );
	printerName=config->readEntry ("Name","");
	spoolerCommand=config->readEntry ("Spool","lpr");
	printerVariable=config->readEntry ("Variable","PRINTER");


	pgMax = maxPages;
	pgStart=0; pgEnd=0;
	
	int border = 10;
	
	QBoxLayout *topLayout = new QVBoxLayout( this, border );
	
	cbFile = new QCheckBox( i18n("Print to file"), this );
	cbFile->setFixedHeight( cbFile->sizeHint().height() );
	
	topLayout->addWidget( cbFile );
	
	QGroupBox *group = new QGroupBox( i18n( "Pages" ), this );
	topLayout->addWidget( group, 10 );
	
	QGridLayout *grid = new QGridLayout( group, 5, 7, 5 );
	
	grid->setRowStretch( 0, 10 );
	grid->setRowStretch( 1, 100 );
	grid->setRowStretch( 2, 0 );
	grid->setRowStretch( 3, 100 );
	grid->setRowStretch( 4, 10 );
	
	grid->setColStretch( 0, 0 );
	grid->setColStretch( 1, 100 );
	grid->setColStretch( 2, 100 );
	grid->setColStretch( 3, 100 );
	grid->setColStretch( 4, 0 );
	grid->setColStretch( 5, 100 );
	grid->setColStretch( 6, 0 );
		
	pgGroup = new QButtonGroup( group );
	pgGroup->hide();
	pgGroup->setExclusive( true );
	connect( pgGroup, SIGNAL( clicked( int ) ), SLOT( slotPageMode( int ) ) );
	
	int widest = 0;
	
	QRadioButton *arb = new QRadioButton( i18n("&All"), group );
	arb->setFixedHeight( arb->sizeHint().height()+6 );
	arb->setChecked( true );
	pgGroup->insert( arb, All );
	
	grid->addWidget( arb, 1, 1 );
	
	if ( arb->sizeHint().width() > widest )
	  widest = arb->sizeHint().width();
	
	QRadioButton *rb = new QRadioButton( i18n("&Current"), group );
	rb->setFixedHeight( rb->sizeHint().height()+6 );
	pgGroup->insert( rb, Current );
	
	grid->addWidget( rb, 1, 2 );
	
	if ( rb->sizeHint().width() > widest ) widest = rb->sizeHint().width();
	
	rb = new QRadioButton( i18n("&Marked"), group );
	rb->setFixedHeight( rb->sizeHint().height()+6 );
	if ( !marked )
	  rb->setEnabled( false );
	else
	  {
	    arb->setChecked ( false );
	    rb->setChecked ( true );
	  }

	pgGroup->insert( rb, Marked );
	
	grid->addWidget( rb, 3, 1 );
	
	if ( rb->sizeHint().width() > widest ) widest = rb->sizeHint().width();
	
	rb = new QRadioButton( i18n("&Range"), group );
	rb->setFixedHeight( rb->sizeHint().height()+6 );
	pgGroup->insert( rb, Range );
	
	grid->addWidget( rb, 3, 2 );
	
	if ( rb->sizeHint().width() > widest ) widest = rb->sizeHint().width();
	
	leStart = new QLineEdit( group );
	leStart->setFixedHeight( rb->sizeHint().height()+6 );
	
	grid->addWidget( leStart, 3, 3 );
	
	lTo = new QLabel( group );
	lTo->setText( i18n("to") );
	lTo->setAlignment( AlignCenter );
	lTo->setMinimumSize( lTo->sizeHint() );
	
	grid->addWidget( lTo, 3, 4 );
	
	leEnd = new QLineEdit( group );
	leEnd->setFixedHeight( rb->sizeHint().height()+6 );
	
	grid->addWidget( leEnd, 3, 5 );
	
	group->setMinimumSize( QSize( 4*widest+25, 4*(rb->sizeHint().height()+6) ) );
	
	cbOrder = new QCheckBox( i18n("Print document in re&verse order"), this );
	cbOrder->setFixedHeight( cbOrder->sizeHint().height() );
	
	topLayout->addWidget( cbOrder );
	
	
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	tmpQFrame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	tmpQFrame->setMinimumHeight( tmpQFrame->sizeHint().height() );
	
	topLayout->addWidget( tmpQFrame );
	
	// CREATE BUTTONS
	
	KButtonBox *bbox = new KButtonBox( this );
	
	QPushButton* setup = bbox->addButton( i18n("&Setup ...") );
	connect( setup, SIGNAL(clicked()), SLOT( setup()) );
	
	
	bbox->addStretch( 10 );

	QPushButton* ok = bbox->addButton( i18n("&OK") );
	connect( ok, SIGNAL(clicked()), SLOT( checkRange() ) );

	QPushButton* cancel = bbox->addButton( i18n("&Cancel") );
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );

	bbox->layout();
	topLayout->addWidget( bbox );
	
	topLayout->activate();
	
	if ( marked )
	  slotPageMode( Marked );
	else
	  slotPageMode( All );

	resize( 250, 0 );
	
}

void PrintDialog::setup()
{
	PrintSetup *ps = new PrintSetup( this, "print setup", printerName,
		 spoolerCommand, printerVariable );
		
		

	if( ps->exec() ) {

	printerName.sprintf( ps->leName->text() );
	spoolerCommand.sprintf( ps->leSpool->text() );
	printerVariable.sprintf( ps->leVar->text() );

	/* Set the default options. */
	KConfig *config = KApplication::getKApplication()->getConfig();

	config->setGroup( "Print" );
	config->writeEntry ("Name", (const char *)printerName);
	config->writeEntry ("Spool", (const char *)spoolerCommand);
	config->writeEntry ("Variable", (const char*)printerVariable);
	config->sync();

        delete ps;
	}
}

void PrintDialog::checkRange()
{
  if ( pgMode == Range ) {
	
    if ( QString( leStart->text() ).toInt() < 1 
	 || QString( leStart->text() ).toInt() > pgMax
	 || QString( leEnd->text() ).toInt() < 1 
	 || QString( leEnd->text() ).toInt() > pgMax ) {
      
      QMessageBox::critical( 0, i18n( "Range error" ),
			     i18n(	"The range of pages entered for printing\n"\
					"can't be understood.\n\n"\
					"The range specified must lie within the page\n"\
					"range for this part of the document.\n"\
					"Check the status bar for this information.\n" ) );
      return;
    }
  }
  
  accept();		
}

void PrintDialog::slotPageMode( int m )
{
  pgMode = m;
  
  switch ( pgMode ) {
    
  case Range:
    
    leStart->setEnabled( true );
    leEnd->setEnabled( true );
    lTo->setEnabled( true );
    break;
    
  default:
    
    leStart->setEnabled( false );
    leEnd->setEnabled( false );
    lTo->setEnabled( false );
    break;
  }
}




