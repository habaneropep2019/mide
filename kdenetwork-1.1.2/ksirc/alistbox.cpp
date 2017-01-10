/*******************************************************************

 aListBox

 $$Id: alistbox.cpp,v 1.20.2.1 1999/02/09 14:46:52 kulow Exp $$

 List box that outputs a right click mouse so I can popup a qpopupmenu.

 Does special sorting, and maintains a two part list, one for ops,
 other for users.

 nothing special.

*******************************************************************/

#include "alistbox.h"
#include "linelistitem.h"
#include "config.h"
#include <qscrbar.h>

aListBox::~aListBox() {
}
void aListBox::mousePressEvent(QMouseEvent *e) /*fold00*/
{

  QListBox::mousePressEvent(e);

  if(e->button() == RightButton){
    emit rightButtonPress(findItem(mapFromGlobal(cursor().pos()).y()));
  }
}


void aListBox::clear() /*FOLD00*/
{
  QListBox::clear();
}


void aListBox::inSort ( nickListItem *lbi) /*fold00*/
{
  int insert;
  bool found;
  insert = searchFor(lbi->text(), found, lbi->op());
  if(found == TRUE){
    debug("%s is already in nick list!", lbi->text());
    return;
  }
  insertItem(lbi, insert);
//  for(uint index = 0; index < count(); index++){
//    debug("%d is %s", index, text(index));
//  }

}

void aListBox::inSort ( const char * text, bool top)  /*fold00*/
{
  nickListItem *nli = new("nickListItem") nickListItem();
  nli->setText(text);
  if(top == TRUE)
    nli->setOp(TRUE);
  inSort(nli);
}

int aListBox::findSep() /*fold00*/
{
  uint i = 0;
  for(; i < count(); i++)
    if(item(i)->op() == FALSE)
      break; // stop now

  return i;

}
int aListBox::searchFor(QString nick, bool &found, bool top) /*fold00*/
{
  int min = 0, max = 0;
  int current = 0, compare = 0;
  int real_max = 0;
  int insert;

  found = FALSE;

  // If there's nothing in the list, don't try and search it, etc
  
  if(count() == 0){
    insert = 0;
  }
  else{
    int sep = findSep();
    if(sep >= 0){
      if(top == TRUE){
        min = 0;
        max = (sep >= 1) ? sep - 1 : 0;
      }
      else{
        min = sep;
        max = count() - 1;
      }
    }
    else
      current = -1;

    real_max = max;
    current = (min + max)/2; // + (max-min)%2;
    insert = current;
    int last_current = -1;
    uint loop = 0;           // Most loops should be log_2 count(), but...
    do {
      if(current == last_current){
//        debug("Insert looping on %s", nick.data());
        //      current++;
        break; // we're looping, so stop
      }
      if(current >= max)
        break; // Don't go too far
      last_current = current;

      compare = strcasecmp(text(current), nick.data());
      if(compare < 0){
        min = current;
	insert = current + 1;
//	debug("1 < 0: %s is greater then: %s, min: %d max: %d current: %d", nick.data(), text(current), min, max, current);
      }
      else if(compare > 0){
        max = current;
 	insert = current;
//	debug("1 > 0: %s is less then: %s, min: %d max: %d current: %d", nick.data(), text(current), min, max, current);
      }
      else {// We got a match?
	insert = current;
        min = current;
        found = TRUE;
        break;
      }
      current = (min + max)/2;
      loop++; // Infinite loop detector increment
    } while(max != min && loop < count());

    if(current >= real_max - 1){
      compare = strcasecmp(text(real_max), nick.data());
      if(compare < 0){
	min = current;
	insert = real_max + 1;
//	debug("End check got one!");
      }
      else if (compare == 0){// We got a match
	insert = real_max + 1;
	min = real_max;
	found = TRUE;
      }
    }

    // Sanity check
    if((top == TRUE && insert > sep) ||
       (top == FALSE && insert < sep)){
      insert = sep;
    }

    if(loop == count())
    {
//        debug("Loop inifitly on: %s", nick.data());
    }

    if(found == TRUE){
//      debug("Found %s", nick.data());
      return min; // We found one, so return the number found
    }
  }
//  debug("%s is at %d", nick.data(), insert);
  return insert;
       
}
bool aListBox::isTop(int index) /*fold00*/
{
  if(index >= findSep())
    return FALSE;
  else
    return TRUE;
}

void aListBox:: setPalette ( const QPalette &p ) /*fold00*/
{
  QListBox::setPalette(p);
  ((QScrollBar*) QTableView::verticalScrollBar())->setPalette(p_scroll);
  ((QScrollBar*) QTableView::horizontalScrollBar())->setPalette(p_scroll);
}

int aListBox::findNick(QString str) /*fold00*/
{
  bool found;
  int index;
  index = searchFor(str, found, TRUE);
  if(found == TRUE)
    return index;
  index = searchFor(str, found, FALSE);
  if(found == TRUE)
    return index;
//  debug("Did not find: %s", str.data());
  return -1;
}

nickListItem *aListBox::item(int index){ /*fold00*/
  return (nickListItem *) QListBox::item(index);
}

nickListItem::nickListItem() /*fold00*/
  : QListBoxItem()
{
  is_op = FALSE;
  is_voice = FALSE;
  is_away = FALSE;
  is_ircop = FALSE;
}

nickListItem::~nickListItem() /*FOLD00*/
{
  string.truncate(0);
}

bool nickListItem::op() /*fold00*/
{
  return is_op;
}

bool nickListItem::voice() /*fold00*/
{
  return is_voice;
}

void nickListItem::setOp(bool _op) /*fold00*/
{
  is_op = _op;
}

void nickListItem::setVoice(bool _voice) /*fold00*/
{
  is_voice = _voice;
}
void nickListItem::setAway(bool _away) /*FOLD00*/
{
  is_away = _away;
}

void nickListItem::setIrcOp(bool _ircop) /*FOLD00*/
{
  is_ircop = _ircop;
}

void nickListItem::paint(QPainter *p) /*FOLD00*/
{
  QFontMetrics fm = p->fontMetrics();
  int yPos;                       // vertical text position
  QPen pen = p->pen();
  QFont font = p->font();
  if(is_voice == TRUE)
    p->setPen(*kSircConfig->colour_chan);
  if(is_op == TRUE)
    p->setPen(*kSircConfig->colour_error);
  if(is_away == TRUE)
    p->setPen(p->pen().color().dark(150));
  if(is_ircop == TRUE){
    QFont bfont = font;
    bfont.setBold(TRUE);
    p->setFont(bfont);
  }
  yPos = fm.ascent() + fm.leading()/2;
  p->drawText( 3, yPos, text() );
  p->setPen(pen);
  p->setFont(font);
}

int nickListItem::height(const QListBox *lb ) const /*fold00*/
{
  return lb->fontMetrics().lineSpacing() + 1;
}

int nickListItem::width(const QListBox *lb ) const /*fold00*/
{
  return lb->fontMetrics().width( 
				 text() 
				 ) + 6;
}

const char* nickListItem::text() const /*fold00*/
{
  return string;
}

const QPixmap* nickListItem::pixmap() const /*fold00*/
{
  return 0l;
}

void nickListItem::setText(const char *str) /*FOLD00*/

{
  string = str;
}

nickListItem &nickListItem::operator= (const nickListItem &nli) /*FOLD00*/
{
  string = nli.string;
  is_op = nli.is_op;
  is_voice = nli.is_voice;
  return (*this);
}
#include "alistbox.moc"
