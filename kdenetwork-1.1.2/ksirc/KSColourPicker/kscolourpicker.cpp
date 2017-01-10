/**********************************************************************

	--- Qt Architect generated file ---

	File: kscolourpicker.cpp
	Last generated: Tue Jul 28 03:49:33 1998

 *********************************************************************/

#include "kscolourpicker.h"
#include "../config.h"
#include "kconfig.h"
#include "../kspainter.h"

extern KConfig *kConfig;
extern global_config *kSircConfig;

#define Inherited kscolourpickerData

kscolourpicker::kscolourpicker
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  setCaption( i18n("kSirc Colour Picker") );

  PB_Pick->hide();

  SLE_SampleText->setFocusPolicy(QWidget::NoFocus);
  SLE_SampleText->hide();

  if(kSircConfig->colour_background == 0){
     kConfig->setGroup("Colours");
     kSircConfig->colour_background = 
       new("QColor") QColor(kConfig->readColorEntry("Background", new("QColor") QColor(colorGroup().mid())));
  }
 
  {
    QColorGroup normal( *kSircConfig->colour_text, colorGroup().mid(), 
                        colorGroup().light(), colorGroup().dark(),
                        colorGroup().midlight(), 
                        *kSircConfig->colour_text, 
//                        *kSircConfig->colour_background); 
                        QColor( darkGray ));
    QPalette palette( normal, normal, normal );
    SLE_SampleText->setPalette( palette );
  } 

  foregroundColour = -1;
  backgroundColour = -1;

  connect(PB_Pick, SIGNAL(pressed()), this, SLOT(slot_pick()));
  connect(PB_Cancel, SIGNAL(pressed()), this, SLOT(slot_cancel()));

  MappedButtons = new("PushButtons") PushButtons((KSPainter::maxcolour*2)+2);
  int XOffset = 0;
  int YOffset = 0;

  for ( int i=0; i<(KSPainter::maxcolour+1)*2; i++ ) {
    QPushButton *pb = new("QPushButton") QPushButton( this );
    pb->setMinimumSize( 10, 10 );
    pb->setMaximumSize( 32767, 32767 );
    pb->setAutoRepeat( FALSE );
    pb->setAutoResize( FALSE );

    pb->setGeometry( 5+(20*XOffset), 5+YOffset, 20, 20 );

    if ( i == KSPainter::maxcolour ) {
      XOffset = 0;
      YOffset = 20;
    } else {
      XOffset++;
    }

    if ( i != KSPainter::maxcolour && i != (KSPainter::maxcolour*2)+1 ) {
      QString str;
      QColor textColour;

      if (( i >= 1 && i <= 6 ) || i == 10 || i == 12 ||
          ( i >= 18 && i <= 23 ) || i == 27 || i == 29) {
        textColour = QColor( white );
      }

      if ( i > KSPainter::maxcolour ) { 
        str.setNum( i-KSPainter::maxcolour-1 );

        QColorGroup normal( colorGroup().foreground(), 
                            KSPainter::num2colour[i-KSPainter::maxcolour-1],
                            colorGroup().light(), colorGroup().dark(),
                            colorGroup().mid(), textColour,
                            colorGroup().base());

        QPalette palette( normal, normal, normal );
        pb->setPalette( palette );

      } else {
        str.setNum( i );

        QColorGroup normal( colorGroup().foreground(), 
                            KSPainter::num2colour[i],
                            colorGroup().light(), colorGroup().dark(),
                            colorGroup().mid(), textColour,
                            colorGroup().base());

        QPalette palette( normal, normal, normal );
        pb->setPalette( palette );

      }
      pb->setText( str );
      
    }

    connect(pb, SIGNAL(pressed()),this, SLOT(slot_update()));
    MappedButtons->insert( i, pb);
  }

}

kscolourpicker::~kscolourpicker()
{
  delete MappedButtons;
}

void kscolourpicker::slot_update()
{

  int i = MappedButtons->findRef( (QPushButton*)sender() );
  if (i < KSPainter::maxcolour) {
    foregroundColour = i;
  } 
  else if ( i == KSPainter::maxcolour ) {
    foregroundColour = -1;
  } 
  else if ( i == (KSPainter::maxcolour*2)+1 ) {
    backgroundColour = -1;
  } else {
    backgroundColour = i-KSPainter::maxcolour-1;
  }

  if (foregroundColour != -1) {
    PB_Pick->show();
    SLE_SampleText->show();
    if (backgroundColour != -1) {
     QColorGroup normal( KSPainter::num2colour[foregroundColour],
                          colorGroup().mid(),
                          colorGroup().light(), colorGroup().dark(),
                          colorGroup().midlight(),
                          KSPainter::num2colour[foregroundColour],
                          KSPainter::num2colour[backgroundColour]);
      QPalette palette( normal, normal, normal );
      SLE_SampleText->setPalette( palette );
    } else {
       QColorGroup normal( KSPainter::num2colour[foregroundColour],
                          colorGroup().mid(),
                          colorGroup().light(), colorGroup().dark(),
                          colorGroup().midlight(),
                          KSPainter::num2colour[foregroundColour],
//                          *kSircConfig->colour_background);
                          QColor( darkGray ));
      QPalette palette( normal, normal, normal );
      SLE_SampleText->setPalette( palette );
    }
  } else {
    PB_Pick->hide();
    SLE_SampleText->hide();
    QColorGroup normal( *kSircConfig->colour_text, colorGroup().mid(),
                        colorGroup().light(), colorGroup().dark(),
                        colorGroup().midlight(),
                        *kSircConfig->colour_text,
//                        *kSircConfig->colour_background);
                        QColor( darkGray ));
    QPalette palette( normal, normal, normal );
    SLE_SampleText->setPalette( palette );
  }
}

void kscolourpicker::slot_pick()
{
  QString str;

  if (backgroundColour != -1) {
    str.sprintf("%d,%d", foregroundColour, backgroundColour);
  } else {
    str.sprintf("%d", foregroundColour);
  } 

  emit picked( str );
  delete this;
}

void kscolourpicker::slot_cancel()
{
  delete this;
}

