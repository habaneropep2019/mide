#include <kapp.h>

#include "kgrouptbl.h"
#include "kgrouptbl.moc"

#include "kgroup.h"

#ifndef max
#define max(a,b) ((a>b) ? (a) : (b) )
#endif

KGroupRow::KGroupRow(KGroup *kg, QPixmap *pGroup) {
  this->kg = kg;
  pmGroup = pGroup;
}

void KGroupRow::paint(QPainter *p, int col, int width) {
  int fontpos = (max( p->fontMetrics().lineSpacing(), pmGroup->height()) - p->fontMetrics().lineSpacing())/2;
  switch(col) {
    case 0: {	// pixmap & Filename
      QString tmpS;

      int start = 1+pmGroup->width()+2;
      width -= pmGroup->width()+4;

      p->drawPixmap(1, 0, *pmGroup);
      tmpS.setNum(kg->getGID());
      p->drawText( start, fontpos, width, p->fontMetrics().lineSpacing(), AlignRight, tmpS);
    }
      break;
    case 1:	// size
      p->drawText( 2, fontpos, width-4, p->fontMetrics().lineSpacing(), AlignLeft, kg->getName());
      break;
  }
}

KGroup *KGroupRow::getData() {
  return (kg);
}

KGroupTable::KGroupTable(QWidget *parent, const char *name) : KRowTable(SelectRow, parent, name)
{
  QString pixdir = kapp->kde_datadir() + QString("/kuser/pics/");
  pmGroup = new QPixmap(pixdir + "group.xpm");

  setCellHeight(max( fontMetrics().lineSpacing(), pmGroup->height()));

  setNumCols(2);

  setAutoUpdate(TRUE);
  current = -1;
  sort = -1;
}

KGroupTable::~KGroupTable() {
  clear();
  delete pmGroup;
}

void KGroupTable::setAutoUpdate(bool state) {
  QTableView::setAutoUpdate(state);
}

void KGroupTable::sortBy(int num) {
  if ((sort>-2) && (sort<3))
    sort = num;
}

void KGroupTable::clear() {
  setTopCell( 0 );
  current = -1;
  setNumRows( 0 );
  updateScrollBars();
}

void KGroupTable::insertItem(KGroup *kg) {
  KGroupRow *tmpGroup = new KGroupRow(kg, pmGroup);

  if (sort != -1) {
    bool isinserted = FALSE;

    for (int i=0;i<numRows();i++) {
       KGroupRow *krow;

       krow = (KGroupRow *)getRow(i);

       if (krow == NULL)
         break;

       if (isinserted)
         break;

       switch (sort) {
         case 0:
           if (krow->getData()->getGID() > kg->getGID()) {
             insertRow(tmpGroup, i);
             isinserted = TRUE;
           }
           break;
         case 1:
           if (krow->getData()->getName() > (const char *)kg->getName()) {
             insertRow(tmpGroup, i);
             isinserted = TRUE;
           }
           break;
      }
    }

    if (!isinserted)
      appendRow(tmpGroup);
  } else
    appendRow(tmpGroup);


  if (autoUpdate())
    repaint();
}

int KGroupTable::currentItem() {
  return current;
}

void KGroupTable::setCurrentItem(int item) {
  int old = current;
  current = item;
  updateCell(old, 0);
  updateCell(current, 0, FALSE);
  emit highlighted(current, 0);
}
