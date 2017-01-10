/**********************************************************************

	--- Qt Architect generated file ---

	File: speeddialog.cpp
	Last generated: Sun Dec 21 08:52:31 1997

 *********************************************************************/

#include "speeddialog.h"

#define Inherited speeddialogData

SpeedDialog::SpeedDialog
(
 int tick,
 int step,
 QWidget* parent,
 const char* name
)
  : Inherited( parent, name )
{
  setCaption("Speed setup");
  connect(sliderTick, SIGNAL(valueChanged(int)),
	  lcdTick, SLOT(display(int)));
  connect(sliderStep, SIGNAL(valueChanged(int)),
	  lcdStep, SLOT(display(int)));
  lcdTick->display(tick);
  sliderTick->setValue(tick);
  lcdStep->display(step);
  sliderStep->setValue(step);
}


SpeedDialog::~SpeedDialog()
{
}


void SpeedDialog::updateTick(int tick)
{
  emit stateChange(tick, sliderStep->value());
}

void SpeedDialog::updateStep(int step)
{
  emit stateChange(sliderTick->value(), step);
}

void SpeedDialog::terminate()
{
  delete this;
}

void SpeedDialog::setLimit(int tmin, int tmax, int smin, int smax)
{
  sliderTick->setRange(tmin, tmax);
  sliderStep->setRange(smin, smax);
}

#include "speeddialog.moc"
