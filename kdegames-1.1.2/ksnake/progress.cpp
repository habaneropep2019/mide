#include "progress.h"

Progress::Progress(QWidget *parent, const char *name)
	: KProgress(0, 300, 300, KProgress::Horizontal, parent, name)
{
  setBackgroundColor("lightgray");
  setBarColor("green1");
  setTextEnabled(FALSE);
}

void Progress::advance()
{
  if (value() == 0) {
    emit restart();
    return;
  }

  else if (value() == 80)
    setBarColor("red1");

  KProgress::advance(-1);
}

void Progress::rewind()
{
  setBarColor("green1");
  KProgress::setValue(300);
}
