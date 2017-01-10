#include <qpixmap.h>
#include <kiconloader.h>
#include <kapp.h>

#include "todoview.h"
#include "todoview.moc"

TodoView::TodoView(CalObject *cal, QWidget *parent, const char *name)
    : KTabListBox(parent, name, 5)
{
  calendar = cal;
  // set up filter for events
  lbox.installEventFilter(this);
  // set up the widget to have 4 columns (one hidden), and
  // only a vertical scrollbar
  clearTableFlags(Tbl_hScrollBar);
  clearTableFlags(Tbl_autoHScrollBar);
  // BL: autoscrollbar in not working...
  setTableFlags(Tbl_hScrollBar);
  setAutoUpdate(TRUE);
  adjustColumns();
  // insert pictures for use to show a checked/not checked todo
  dict().insert("CHECKED", new QPixmap(Icon("checkedbox.xpm")));
  dict().insert("EMPTY", new QPixmap(Icon("emptybox.xpm")));
  dict().insert("CHECKEDMASK", new QPixmap(Icon("checkedbox-mask.xpm")));
  dict().insert("EMPTYMASK", new QPixmap(Icon("emptybox-mask.xpm")));

  // this is the thing that lets you edit the todo text.
  editor = new QLineEdit(this);
  editor->hide();
  connect(editor, SIGNAL(returnPressed()),
	  this, SLOT(updateSummary()));
  connect(editor, SIGNAL(returnPressed()),
	  editor, SLOT(hide()));
  connect(editor, SIGNAL(textChanged(const char *)),
	  this, SLOT(changeSummary(const char *)));
  
  connect(this, SIGNAL(selected(int, int)),
    	  this, SLOT(updateItem(int, int)));
  connect(this, SIGNAL(highlighted(int, int)),
	  this, SLOT(hiliteAction(int, int)));

  priList = new QListBox(this);
  priList->hide();
  priList->insertItem("1");
  priList->insertItem("2");
  priList->insertItem("3");
  priList->insertItem("4");
  priList->insertItem("5");
  priList->setFixedHeight(priList->itemHeight()*5+5);
  priList->setFixedWidth(priList->maxItemWidth()+5);
  connect(priList, SIGNAL(highlighted(int)),
	  priList, SLOT(hide()));
  connect(priList, SIGNAL(highlighted(int)),
	  this, SLOT(changePriority(int)));

  QPixmap pixmap;
  rmbMenu1 = new QPopupMenu;
  pixmap = Icon("checkedbox.xpm");
  rmbMenu1->insertItem(pixmap, i18n("New Todo"), this,
		       SLOT(newTodo()));
  pixmap = Icon("delete.xpm");
  rmbMenu1->insertItem(pixmap, i18n("Purge Completed "), this,
		       SLOT(purgeCompleted()));

  rmbMenu2 = new QPopupMenu;
  pixmap = Icon("checkedbox.xpm");
  rmbMenu2->insertItem(pixmap, i18n("New Todo"), this,
		     SLOT (newTodo()));
  rmbMenu2->insertItem(i18n("Edit Todo"), this,
		     SLOT (editTodo()));
  pixmap = Icon("delete.xpm");
  rmbMenu2->insertItem(pixmap, i18n("Delete Todo"), this,
		     SLOT (deleteTodo()));
  rmbMenu2->insertItem(i18n("Purge Completed "), this,
		       SLOT(purgeCompleted()));


  editingFlag = FALSE;
  connect(this, SIGNAL(headerClicked(int)), this, SLOT(headerAction(int)));
  updateConfig();
  prevRow = updatingRow = -1;
}
  
TodoView::~TodoView()
{
  delete rmbMenu1;
  delete rmbMenu2;
}

void TodoView::updateView()
{
  clear();
  sortPriority();
  adjustColumns();
  repaint();
}

void TodoView::updateConfig()
{
  KConfig *config(kapp->getConfig());

  config->setGroup("Fonts");

  QFont newFont(config->readFontEntry("Todo Font", &font()));

  setTableFont(newFont);
  updateView(); // this is somewhat bad because it also resorts the list...
}

// this is basically a method to change which pixmap is displayed when
// an event gets hilighted (black doesn't show up well on select backgrounds)
void TodoView::hiliteAction(int row, int /*col*/)
{
  setUpdatesEnabled(FALSE);
  if (prevRow != -1) {
    if (strcmp("CHECKEDMASK", text(prevRow, 1).data()) == 0)
      changeItemPart("CHECKED", prevRow, 1);
    else if (strcmp("EMPTYMASK", text(prevRow, 1).data()) == 0)
      changeItemPart("EMPTY", prevRow, 1);
  }

  if (strcmp("CHECKED", text(row, 1).data()) == 0)
    changeItemPart("CHECKEDMASK", row, 1);
  else if (strcmp("EMPTY", text(row, 1).data()) == 0)
    changeItemPart("EMPTYMASK", row, 1);
  prevRow = row;
  setUpdatesEnabled(TRUE);
}

void TodoView::updateItem(int row, int col)
{
  QString tmpStr;
  int x=0, y=0;
  int id;

  updatingRow = row;

  ASSERT(row >= 0);
  editingFlag = TRUE;

  tmpStr = text(row, 0);
  id = atol(tmpStr.data());
  aTodo = calendar->getTodo(id);

  editor->hide();
  editor->setText(aTodo->getSummary());

  colXPos(col, &x);
  rowYPos(row, &y);
  y += 17; // correct for header size.

  // first, see if they clicked on the "done/not done column";
  if (col == 1) {
    tmpStr = text(row, 1);
    if (tmpStr == "CHECKED" || tmpStr == "CHECKEDMASK") {      
      aTodo->setStatus(QString("NEEDS ACTION"));
      changeItemPart("EMPTY", row, 1);
    } else {
      aTodo->setDtEnd(QDateTime::currentDateTime());
      aTodo->setStatus(QString("COMPLETED"));
      changeItemPart("CHECKED", row, 1);
    }
  }

  // see if they clicked on the "priority" column
  if (col == 2) {
    priList->move(x, y+2);
    priList->setCurrentItem(aTodo->getPriority()-1);
    priList->setFocus();
    priList->show();
  }

  // they clicked on the "description" column
  if (col == 3) {
    editor->move(x, y+2);
    editor->setFixedWidth(columnWidth(3));
    editor->setFixedHeight(cellHeight(row));
    editor->setFocus();
    editor->show();
  }
  if (col == 4) {
    // handle due date stuff
  }
  adjustColumns();
}
  
void TodoView::changeSummary(const char *newsum)
{
  tmpSummary = newsum;
}

void TodoView::updateSummary()
{
  changeItemPart(tmpSummary, updatingRow, 3);
  aTodo->setSummary(tmpSummary);
  editor->setText(""); // clear it out
}
  
void TodoView::changePriority(int pri)
{
  QString s;
  aTodo->setPriority(pri+1);

  s.sprintf("%d",pri+1);

  changeItemPart(s.data(), updatingRow, 2);
}

void TodoView::doPopup(int row, int)
{
  int id;
  // BL: the row can be some weird number because of the KTabList weirdness
  // this would take care of it for now
  
  if ((row < 0) || ((uint) row > count())) {
    rmbMenu1->popup(QCursor::pos());
    return;
  }

  setCurrentItem(row);
  // BL: this data is problematic

  id = atol(text(row, 0).data());
  aTodo = calendar->getTodo(id);
  
  rmbMenu2->popup(QCursor::pos());
}

void TodoView::newTodo()
{
  QString tmpStr;
  KDPEvent *newEvent;

  newEvent = new KDPEvent;

  newEvent->setStatus(QString("NEEDS ACTION"));
  newEvent->setPriority(1);
  calendar->addTodo(newEvent);

  tmpStr.sprintf("%i\n",newEvent->getEventId());
  tmpStr += "EMPTY\n0\n \n ";
  appendItem(tmpStr.data());
  repaint();
  setCurrentItem(numRows()-1);
  updateItem(currentItem(), 3);
}

void TodoView::deleteTodo()
{
  removeItem(currentItem());
  setCurrentItem(currentItem()-1);
  calendar->deleteTodo(aTodo);
}

void TodoView::editTodo()
{
  if(aTodo!=0) {
    emit editEventSignal(aTodo);
  } else {
    qApp->beep();
  }
}

void TodoView::cleanWindow(QWidget *w)
{
  delete w;
  w = 0L;
}

void TodoView::changeEventDisplay(KDPEvent *which, int action)
{
    updateView();
}


void TodoView::purgeCompleted()
{
  bool allGone = FALSE;

  setUpdatesEnabled(FALSE);
  while (!allGone) {
    allGone = TRUE;
    for (uint i = 0; i < count(); i++) {
      if (!strcmp(text(i, 1).data(),"CHECKED") ||
	  !strcmp(text(i, 1).data(),"CHECKEDMASK")) {
	allGone = FALSE;
	int id = atol(text(i, 0).data());
	aTodo = calendar->getTodo(id);
	removeItem(i);
	calendar->deleteTodo(aTodo);
	break;
      }
    }
  }
  setUpdatesEnabled(TRUE);
}

void TodoView::sortPriority()
{
  QStrList tmpList = makeStrList(PRIORITY);
  clear();
  appendStrList(&tmpList);
}

void TodoView::sortDescription()
{
  QStrList tmpList = makeStrList(DESCRIPTION);
  clear();
  appendStrList(&tmpList);
}

void TodoView::sortCompleted()
{
  QStrList tmpList = makeStrList(COMPLETED);
  clear();
  appendStrList(&tmpList);
}

QStrList TodoView::makeStrList(int sortOrder)
{
  QStrList tmpList;
  QString tmpStr, tmpStr2;
  int month, day;
  QList<KDPEvent> todoList(calendar->getTodoList());

  KDPEvent *aTodo;
  for (aTodo = todoList.first(); aTodo;
       aTodo = todoList.next()) {
    tmpStr.sprintf("%li\n",aTodo->getEventId());
    if (aTodo->getStatusStr() == "NEEDS ACTION")
      tmpStr += "EMPTY\n";
    else
      tmpStr += "CHECKED\n";
    tmpStr2.sprintf("%d\n",aTodo->getPriority());
    tmpStr += tmpStr2 + aTodo->getSummary();
    month = aTodo->getDtEnd().date().month();
    day = aTodo->getDtEnd().date().day();
    // this isn't finished yet.
    //    tmpStr2.sprintf("\n%d/%d",month,day);
    tmpStr += "\n ";
    insortItem(tmpStr, &tmpList, sortOrder);
  }
  return tmpList;
}

void TodoView::insortItem(QString insStr, QStrList *insList, int sortOrder)
{
  QString key1, key2;
  int index;
  switch(sortOrder) {
  case COMPLETED:
      index = insStr.find('\n', 0) + 1;
      key1 = insStr.right(insStr.length()-index);
      for (uint i = 0; i < insList->count(); i++) {
	  key2 = insList->at(i);
	  index = key2.find('\n', 0) + 1;
	  key2 = key2.right(key2.length()-index);
	  if (key1 > key2.data()) {
	      insList->insert(i, insStr);
	      return;
	  }
      }
      insList->append(insStr);
      break;
  case PRIORITY:
    index = insStr.find('\n', 0) + 1;
    index = insStr.find('\n', index) + 1;
    key1 = insStr.right(insStr.length()-index);
    for (uint i = 0; i < insList->count(); i++) {
      key2 = insList->at(i);
      index = key2.find('\n', 0) + 1;
      index = key2.find('\n', index) + 1;
      key2 = key2.right(key2.length()-index);
      if (key1 < key2.data()) {
	insList->insert(i, insStr);
	return;
      }
    }
    insList->append(insStr);
    break;
  case DESCRIPTION:
    index = insStr.find('\n', 0) + 1;
    index = insStr.find('\n', index) + 1;
    index = insStr.find('\n', index) + 1;
    key1 = insStr.right(insStr.length()-index);
    for (uint i = 0; i < insList->count(); i++) {
      key2 = insList->at(i);
      index = key2.find('\n', 0) + 1;
      index = key2.find('\n', index) + 1;
      index = key2.find('\n', index) + 1;
      key2 = key2.right(key2.length()-index);
      if (key1 < key2.data()) {
	insList->insert(i, insStr);
	return;
      }
    }
    insList->append(insStr);
    break;
  } // case
}

void TodoView::headerAction(int column)
{
  switch(column) {
  case 1: // completed
    sortCompleted();
    break;
  case 2: // priority
    sortPriority();
    break;
  case 3: // description
    sortDescription();
    break;
  }
}

void TodoView::doMouseEvent(QMouseEvent *e)
{
  editor->hide();
  priList->hide();
  editingFlag = FALSE;
  int c, r;

  c = findCol(e->x());
  if (count()==0) 
      r=-1;
  else
      r = findRow(e->y());
  // BL: workaround a bug in KTabListBox -- no !!!
  
  if (e->button() == RightButton)
    doPopup(r,c);
  else {
    //if ((y >= 0) && (x >= 0))
    // updateItem(y,x);
    ;
  }
}

// this is an eventfilter for the KTabListBoxTable which holds all
// of the todo items.
bool TodoView::eventFilter(QObject *, QEvent *event)
{
  if (event->type() == Event_MouseButtonPress) {
    QMouseEvent *e = (QMouseEvent *) event;
    setUpdatesEnabled(FALSE);
    doMouseEvent(e);
    setUpdatesEnabled(TRUE);
    return TRUE;
  }
  if (event->type() == Event_Resize) {
      adjustColumns();
  }
  return FALSE;
}

void TodoView::adjustColumns() {
  // resizing the columns, such that the Description (3) is the elastic field
  setColumn(0, "", 0);
  setColumn(1, "", 25, KTabListBox::PixmapColumn);
  setColumn(2, i18n("Pri"), 25);
  if (columnWidth(4)!=0) {
      setColumn(4, i18n("Due"), 40);
      //setColumnWidth(4, 40);
  }
  int descrwidth = width() - 90;
  if (descrwidth < 120) {
      descrwidth = 120;
  }
  setColumn(3, i18n("Todo Descrip."), descrwidth); 
}

void TodoView::showDates(bool show)
{
  static int oldColWidth = 0;
  
  if (!show) {
    oldColWidth = columnWidth(4);
    setColumnWidth(4, 0);
  } else {
    setColumnWidth(4, oldColWidth);
  } 
  repaint();
}

inline void TodoView::showDates()
{
  showDates(TRUE);
}

inline void TodoView::hideDates()
{
  showDates(FALSE);
}
