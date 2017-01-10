#include <kapp.h>

#include "kusertbl.h"
#include "kusertbl.moc"

#include "kuser.h"

#ifndef max
#define max(a,b) ((a>b) ? (a) : (b) )
#endif

KUserRow::KUserRow(KUser *aku, QPixmap *pUser) {
  ku = aku;
  pmUser = pUser;
}

void KUserRow::paint( QPainter *p, int col, int width ) {
  int fontpos = (max( p->fontMetrics().lineSpacing(), pmUser->height()) - p->fontMetrics().lineSpacing())/2;
  switch(col) {
    case 0: {	// pixmap & Filename
      int start = 1 + pmUser->width() + 2;
      width -= 2 + pmUser->width();

      p->drawPixmap(1, 0, *pmUser);
      p->drawText(start, fontpos, width, p->fontMetrics().lineSpacing(), AlignLeft, ku->getName());
    }
      break;
    case 1:	// size
      p->drawText(2, fontpos, width-4, p->fontMetrics().lineSpacing(), AlignLeft, ku->getFullName());
      break;
  }
}

KUser *KUserRow::getData() {
  return (ku);
}

KUserTable::KUserTable(QWidget *parent, const char *name) : KRowTable(SelectRow, parent, name) {
  QString pixdir = kapp->kde_datadir() + QString("/kuser/pics/");
  pmUser = new QPixmap(pixdir + "user.xpm");

  setCellHeight( max( fontMetrics().lineSpacing(), pmUser->height()) );

  setNumCols( 2 );

  setAutoUpdate(TRUE);
  current = -1;
  sort = -1;
}

KUserTable::~KUserTable() {
  clear();
  delete pmUser;
}

void KUserTable::setAutoUpdate(bool state) {
  QTableView::setAutoUpdate(state);
}

void KUserTable::sortBy(int num) {
  if ((sort > -2)&&(sort < 3))
    sort = num;
}

void KUserTable::clear() {
  setTopCell(0);
  current = -1;
  setNumRows(0);
  updateScrollBars();
}

void KUserTable::insertItem(KUser *aku) {
  KUserRow *tmpUser = new KUserRow(aku, pmUser);

  if (sort != -1) {
    bool isinserted = FALSE;

    for (int i=0;i<numRows();i++) {
       KUserRow *krow;

       krow = (KUserRow *)getRow(i);

       if (krow == NULL)
         break;

       if (isinserted)
         break;

       switch (sort) {
         case 0:
           if (krow->getData()->getName() > (const char *)aku->getName()) {
             insertRow(tmpUser, i);
             isinserted = TRUE;
           }
           break;
         case 1:
           if (krow->getData()->getFullName() > (const char *)aku->getFullName()) {
             insertRow(tmpUser, i);
             isinserted = TRUE;
           }
           break;
      }
    }

    if (!isinserted) {
      appendRow(tmpUser);
    }
  } else
    appendRow(tmpUser);

  if (autoUpdate())
    repaint();
}

int KUserTable::currentItem() {
  return current;
}

void KUserTable::setCurrentItem(int item) {
  int old = current;
  current = item;
  updateCell(old, 0);
  updateCell(current, 0, FALSE);
  emit highlighted(current, 0);
}
