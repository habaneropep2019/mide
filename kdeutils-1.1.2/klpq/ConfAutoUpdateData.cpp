
#include "ConfAutoUpdateData.h"

#include <qlcdnum.h>
#include <qlabel.h>

#include <kapp.h>

ConfAutoUpdateData::ConfAutoUpdateData
(
	QWidget* parent
)
{
	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( parent, "Label_1" );
	tmpQLabel->setGeometry( 16, 24, 180, 24 );
	tmpQLabel->setText( klocale->translate("Update frequency in seconds:") );

	s_freq = new QScrollBar( parent, "ScrollBar_1" );
	s_freq->setGeometry( 24, 48, 160, 16 );
	s_freq->setOrientation( QScrollBar::Horizontal );

	lcd_freq = new QLCDNumber( parent, "LCDNumber_1" );
	lcd_freq->setGeometry( 192, 32, 72, 32 );
	lcd_freq->setFrameStyle( 33 );
	lcd_freq->setSmallDecimalPoint( FALSE );
	lcd_freq->setNumDigits( 5 );
	lcd_freq->setMode( QLCDNumber::DEC );

	b_ok = new QPushButton( parent, "PushButton_1" );
	b_ok->setGeometry( 32, 120, 80, 24 );
	b_ok->setText( klocale->translate("OK") );

	b_cancel = new QPushButton( parent, "PushButton_2" );
	b_cancel->setGeometry( 176, 120, 80, 24 );
	b_cancel->setText( klocale->translate("Cancel") );

	parent->resize( 288, 168 );
}
