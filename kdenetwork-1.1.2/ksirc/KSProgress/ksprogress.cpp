/**********************************************************************


 *********************************************************************/

#include "ksprogress.h"

#define Inherited ksprogressData

KSProgress::KSProgress
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
  setCaption("KSProgress");
  id = "";
  canceled = FALSE;
  progress->setOrientation(KProgress::Horizontal);
}


void KSProgress::setID(QString _id)
{
  id = _id;
}

void KSProgress::setRange(int minValue, int maxValue)
{
  progress->setRange(minValue, maxValue);
}

void KSProgress::setTopText(QString text)
{
  fileName->setText(text);
}

void KSProgress::setBotText(QString text)
{
  transferStatus->setText(text);
}

void KSProgress::setValue(int value)
{
  progress->setValue(value);
}

void KSProgress::cancelPressed()
{
  canceled = TRUE;
  emit cancel();
  emit cancel(id);
}

KSProgress::~KSProgress()
{
}
