/**********************************************************************

	--- Qt Architect generated file ---

	File: defaultfilters.cpp
	Last generated: Mon Feb 16 20:17:25 1998

 *********************************************************************/

#include "defaultfilters.h"
#include "../config.h"
#include "../kspainter.h"
#include <qpixmap.h>

#include <kconfig.h>

extern KConfig *kConfig;
extern global_config *kSircConfig;

#define Inherited defaultfiltersdata

DefaultFilters::DefaultFilters
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  kSircColours->setChecked(!kSircConfig->filterKColour);
  mircColours->setChecked(!kSircConfig->filterMColour);

  if(kSircConfig->nickFHighlight == -1){
    NickBColour->hide();
    LabelNickB->hide();
  }

  connect(NickFColour, SIGNAL(activated(int)),
          this, SLOT(slot_setSampleNickColour()));
  connect(NickBColour, SIGNAL(activated(int)),
          this, SLOT(slot_setSampleNickColour()));
  connect(usHighlightColour, SIGNAL(activated(int)),
          this, SLOT(slot_setSampleUsColour()));

  NickFColour->insertItem ( "Disable" );
  NickBColour->insertItem ( "Disable" );
  usHighlightColour->insertItem ( "Disable" );

// Assuming all ComboBox are of the same size and contain the same colours 
// saves some cpu cycles not having to redo all this for each ComboBox

  QPixmap pm( usHighlightColour->width(), usHighlightColour->height()/2 );
  for(int i = 0; i < KSPainter::maxcolour; i++) {
    pm.fill( KSPainter::num2colour[i] );
    usHighlightColour->insertItem ( pm, i+1 );
    NickFColour->insertItem ( pm, i+1 );
    NickBColour->insertItem ( pm, i+1 );
  }

  usHighlightColour->setCurrentItem ( kSircConfig->usHighlight+1 );
  NickFColour->setCurrentItem ( kSircConfig->nickFHighlight+1 );
  NickBColour->setCurrentItem ( kSircConfig->nickBHighlight+1 );

  if(kSircConfig->colour_background == 0){
     kConfig->setGroup("Colours");
     kSircConfig->colour_background = new("QColor") QColor(kConfig->readColorEntry("Background", new("QColor") QColor(colorGroup().mid())));
  }

  SLE_SampleColourNick->setFocusPolicy(QWidget::NoFocus);
  SLE_SampleHighlight->setFocusPolicy(QWidget::NoFocus);

  slot_setSampleNickColour();
  slot_setSampleUsColour();

}


DefaultFilters::~DefaultFilters()
{
}

void DefaultFilters::slot_setSampleNickColour()
{
  QColor foregroundColour;
  QColor backgroundColour;
 
  if (NickFColour->currentItem()-1 != -1) {
    foregroundColour = KSPainter::num2colour[NickFColour->currentItem()-1];
     NickBColour->show();
     LabelNickB->show();

    if (NickBColour->currentItem()-1 != -1) {
      backgroundColour = KSPainter::num2colour[NickBColour->currentItem()-1];
    }
    else
    {
      backgroundColour = *kSircConfig->colour_background;
    }
  }
  else
  {
    foregroundColour = *kSircConfig->colour_text;
    backgroundColour = *kSircConfig->colour_background;
    NickBColour->hide();
    LabelNickB->hide();
  }

  QColorGroup cg = QColorGroup(foregroundColour, colorGroup().mid(),
                               colorGroup().light(), colorGroup().dark(),
                               colorGroup().midlight(),
                               foregroundColour,
                               backgroundColour);

  SLE_SampleColourNick->setPalette(QPalette(cg,cg,cg));
}

void DefaultFilters::slot_setSampleUsColour()
{
  QColor foregroundColour;

  if (usHighlightColour->currentItem()-1 != -1) {
    foregroundColour = KSPainter::num2colour[usHighlightColour->currentItem()-1];
  }
  else
  {
    foregroundColour = *kSircConfig->colour_text;
  }

  QColorGroup cg = QColorGroup(foregroundColour, colorGroup().mid(),
                               colorGroup().light(), colorGroup().dark(),
                               colorGroup().midlight(),
                               foregroundColour,
                               *kSircConfig->colour_background);

  SLE_SampleHighlight->setPalette(QPalette(cg,cg,cg));
}

void DefaultFilters::slot_apply()
{
  kSircConfig->filterKColour = !kSircColours->isChecked();
  kSircConfig->filterMColour = !mircColours->isChecked();
  kSircConfig->nickFHighlight = NickFColour->currentItem()-1;
  kSircConfig->nickBHighlight = NickBColour->currentItem()-1;
  kSircConfig->usHighlight = usHighlightColour->currentItem()-1;

  kConfig->setGroup("Colours");
  kConfig->writeEntry("kcolour", kSircConfig->filterKColour);
  kConfig->writeEntry("mcolour", kSircConfig->filterMColour);
  kConfig->writeEntry("nickfcolour", kSircConfig->nickFHighlight);
  kConfig->writeEntry("nickbcolour", kSircConfig->nickBHighlight); 
  kConfig->writeEntry("uscolour", kSircConfig->usHighlight);

  kConfig->sync();
}

