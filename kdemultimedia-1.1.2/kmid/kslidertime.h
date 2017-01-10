/**************************************************************************

    kslidertime.h  - A widget that displays time tags under a KSlider
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include <kslider.h>

class QWidget;
class KSlider;

class KSliderTime : public QWidget
{
  Q_OBJECT
private:

KSlider *kslider;

int fontheight;
public:

  KSliderTime( KSlider *ksl, QWidget *parent = NULL, const char *name = NULL );

  int getFontHeight(void) {return fontheight;};

private:

  virtual void paintEvent( QPaintEvent * );  
  void drawTimeMarks(QPainter *painter);

  char *formatMillisecs(int ms,char *tmp);
};
