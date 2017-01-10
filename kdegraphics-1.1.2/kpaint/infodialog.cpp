// $Id: infodialog.cpp,v 1.7.4.1 1999/04/08 21:23:52 dfaure Exp $

#include <kdebug.h>
#include <string.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qlined.h>
#include <qimage.h>
#include <qlayout.h>
#include <klocale.h>
#include <kapp.h>
#include "infodialog.h"
#include "formats.h"

extern FormatManager *formatMngr;

imageInfoDialog::imageInfoDialog(Canvas *c, QWidget* parent, const char* name)
  : QDialog(parent, name, TRUE)
{
  QPushButton *okButton;
  QLabel *tmpQLabel;

  QVBoxLayout *mainLayout = new QVBoxLayout(this, 20);

  QHBoxLayout *hbl = new QHBoxLayout();
  mainLayout->addLayout( hbl );

  QVBoxLayout *vbl1 = new QVBoxLayout();
  hbl->addLayout( vbl1 );
  QVBoxLayout *vbl2 = new QVBoxLayout();
  hbl->addLayout( vbl2, 1 );

  tmpQLabel = new QLabel( this, "Label_3" );
  tmpQLabel->setText( klocale->translate( "Colour Depth:" ) );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );
  tmpQLabel->setFixedWidth( tmpQLabel->sizeHint().width() );
  vbl1->addWidget( tmpQLabel, 0, AlignLeft );

  colourDepth = new QLabel( this, "Label_4" );
  colourDepth->setGeometry( 140, 10, 190, 30 );

  QString text;
  text.sprintf("%d", c->pixmap()->depth());
  colourDepth->setText( text );
  colourDepth->setAlignment( 289 );
  colourDepth->setMargin( -1 );
  colourDepth->setFixedWidth( colourDepth->sizeHint().width() );
  vbl2->addWidget( colourDepth, 0, AlignRight );
  colourDepth->setFixedHeight( colourDepth->sizeHint().height() );
  tmpQLabel->setFixedHeight( tmpQLabel->sizeHint().height() );

  tmpQLabel = new QLabel( this, "Label_5" );
  tmpQLabel->setGeometry( 30, 40, 100, 30 );
  tmpQLabel->setText( klocale->translate("Colours Used:") );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );
  tmpQLabel->setFixedWidth( tmpQLabel->sizeHint().width() );
  vbl1->addWidget( tmpQLabel, 0, AlignLeft );

  coloursUsed = new QLabel( this, "Label_6" );
  coloursUsed->setGeometry( 140, 40, 190, 30 );
  text.sprintf("%d", c->pixmap()->convertToImage().numColors());
  coloursUsed->setText( text );
  coloursUsed->setAlignment( 289 );
  coloursUsed->setMargin( -1 );
  coloursUsed->setFixedWidth( coloursUsed->sizeHint().width() );
  vbl2->addWidget( coloursUsed, 0, AlignRight );

  coloursUsed->setFixedHeight( coloursUsed->sizeHint().height() );
  tmpQLabel->setFixedHeight( tmpQLabel->sizeHint().height() );

  //  tmpQLabel = new QLabel( this, "Label_8" );
  //  tmpQLabel->setGeometry( 30, 70, 120, 30 );
  //  tmpQLabel->setText( klocale->translate("Transparent Colour:") );
  //  tmpQLabel->setAlignment( 289 );
  //  tmpQLabel->setMargin( -1 );

  //  QLineEdit* tmpQLineEdit;
  //  tmpQLineEdit = new QLineEdit( this, "LineEdit_1" );
  //  tmpQLineEdit->setGeometry( 170, 70, 100, 30 );
  //  tmpQLineEdit->setText( "" );
  //  tmpQLineEdit->setMaxLength( 32767 );
  //  tmpQLineEdit->setEchoMode( QLineEdit::Normal );
  //  tmpQLineEdit->setFrame( TRUE );

  tmpQLabel = new QLabel( this, "Label_9" );
  tmpQLabel->setGeometry( 30, 120, 100, 30 );
  tmpQLabel->setText( klocale->translate( "Image Width:" ) );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );
  tmpQLabel->setFixedWidth( tmpQLabel->sizeHint().width() );
  vbl1->addWidget( tmpQLabel, 0, AlignLeft );

  QSize sz;
  sz= c->pixmap()->size();

  width = new QLabel( this, "Label_10" );
  width->setGeometry( 150, 120, 100, 30 );
  text.sprintf("%d", sz.width());
  width->setText(text);
  width->setAlignment( 289 );
  width->setMargin( -1 );
  width->setFixedWidth( width->sizeHint().width() );
  vbl2->addWidget( width, 0, AlignRight );
  width->setFixedHeight( width->sizeHint().height() );
  tmpQLabel->setFixedHeight( tmpQLabel->sizeHint().height() );

  tmpQLabel = new QLabel( this, "Label_11" );
  tmpQLabel->setGeometry( 30, 160, 100, 30 );
  tmpQLabel->setText( klocale->translate("Image Height:") );
  tmpQLabel->setAlignment( 289 );
  tmpQLabel->setMargin( -1 );
  tmpQLabel->setFixedWidth( tmpQLabel->sizeHint().width() );
  vbl1->addWidget( tmpQLabel, 0, AlignLeft );

  height = new QLabel( this, "Label_12" );
  height->setGeometry( 150, 160, 100, 30 );

  text.sprintf("%d", sz.height());
  height->setText(text);
  height->setAlignment(289);
  height->setMargin(-1);
  height->setFixedWidth( height->sizeHint().width() );
  vbl2->addWidget( height, 0, AlignRight );

  height->setFixedHeight( height->sizeHint().height() );
  tmpQLabel->setFixedHeight( tmpQLabel->sizeHint().height() );

  okButton= new QPushButton(klocale->translate("Dismiss"), this);
  okButton->setFixedSize( okButton->sizeHint() );
  mainLayout->addSpacing( 5 );
  mainLayout->addWidget( okButton );
//  okButton->move(200, 200);

  resize( minimumSize() );

  connect(okButton, SIGNAL(clicked()), SLOT(accept()) );
}

#include "infodialog.moc"

