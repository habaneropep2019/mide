#include "irclistitem.h"
#include "iostream.h"

#include <stdlib.h>

#include <qbitmap.h>
#include <qimage.h>
#include <qregexp.h>

#include "config.h"

QIntCache<QPixmap> *ircListItem::PaintCache = 0x0;

ircListItem::ircListItem(QString s, const QColor *c, QListBox *lb, QPixmap *p)
  : QObject(),
    QListBoxItem()
    
{

  if(PaintCache == 0x0){
    PaintCache = new("QCache<QPixmap>") QIntCache<QPixmap>;
    PaintCache->setMaxCost(300);
    PaintCache->setAutoDelete(TRUE);
  }

  rtext = s.data();
  setText(s);
  colour = c;
  pm = p;
  parent_lb = lb;

  Wrapping = TRUE;
  frozen = FALSE;

  CacheId = 1;
  
  rows = 1;
  linewidth = 0;
  totalheight = 0;

  revOne = revTwo = -1;
  forceClear = FALSE;

  paint_text = new("QStrListIrcListItem") QStrList();

//  dbuffer = new("QPixmap") QPixmap();
  need_update = TRUE;

  old_height = old_width = 0;
  
  setupPainterText();

}

ircListItem::~ircListItem()
{
  delete paint_text;
  PaintCache->remove(CacheId);
  rtext.truncate(0);
  
}

void ircListItem::paint(QPainter *p)
{
  QPixmap *dbuffer = PaintCache->find(CacheId);
  if(dbuffer == 0x0)
    need_update = TRUE;
    
  if(need_update == TRUE){
    setupPainterText();
    dbuffer = PaintCache->find(CacheId); // Updated by setupPainterText();
    if(dbuffer == 0x0){
      warning("ircListItem::paint() Failed to get a paint pixmap!! Giving up");
      need_update = TRUE;
    }
    else
      need_update = FALSE;
  }

  if(need_update == FALSE)
    p->drawPixmap(0,0, *dbuffer);
}

int ircListItem::height(const QListBox *) const
{
  return  totalheight;
}

int ircListItem::width(const QListBox *) const
{
  return linewidth;
}

int ircListItem::row()
{
  return rows;
}

void ircListItem::setupPainterText()
{
  QPixmap *dbuffer = PaintCache->find(CacheId);
  if(dbuffer == 0x0){
    dbuffer = new("QPixmap") QPixmap();
    CacheId = dbuffer->serialNumber();
    // Should be safe to insert here
    // and use it for the rest of the function
    // since no other inserts are done
    if(PaintCache->insert(CacheId, dbuffer, 1) == false)
      warning("Insert into cache failed");
    // Set the old height and width to 0 to get the pixmap sized
    old_height = old_width = 0;
  }
  
  if(frozen == TRUE){
    need_update = TRUE;
    return;
  }

  lineheight = parent_lb->fontMetrics().lineSpacing();
  QFontMetrics fm = parent_lb->fontMetrics();

  if(pm){
    if ( pm->height() < fm.height() )
      yPos = fm.ascent() + fm.leading()/2;
    else
      yPos = pm->height()/2 - fm.height()/2 + fm.ascent(); 

    xPos = pm->width()+5;
  }
  else{
    yPos = fm.ascent() + fm.leading()/2;
    xPos = 3;
  }

  // Copy permenant contents into temp string so we can mangle it, but always get the orignal back
  itext = rtext;
  itext.detach();

  // Insert selection markers if turned on

  if(revOne >= 0){
    itext.insert(revOne, "~s");
    if(revTwo >= 0){
      int rr = revTwo;
      if(revOne < revTwo)
        rr += 2;
      if(rr > (int) itext.length())
        rr = itext.length();
      itext.insert(rr, "~s");
    }
  }
    
  // Wrapping code is a little slow, and a little silly, but it works.

  // Main idea is this:
  // Go through each character, find it's width and add it, when it goes
  // beyond the max widith, time for new line.
  //
  // We skip over all !<>,<> time constructs.
  
  paint_text->clear();
  int max_width = parent_lb->width()-35;
  int realIndex = 0;
  if((fm.width(itext) > max_width) && (Wrapping == TRUE)){
    int lastp = 0;
    int width = xPos - fm.width(itext[0]);
    uint i;
    int ig = 0;
    for(i = 0; i < itext.length() ; i++){
      if((itext[i] == '~') || (itext[i] == 0x03) &&
	 (((itext[i+1] >= 0x30) && (itext[i+1] <= 0x39)  ||
	   (itext[i] == '~') && ((itext[i+1] >= 0x61) || (itext[i+1] <= 0x7a))))){ // a->z
	if((itext[i+1] >= 0x30) && (itext[i+1] <= 0x39)){
	  i += 2; 
	  ig += 2;
	  if((itext[i] >= 0x30) && (itext[i] <= 0x39)){
	    i++;
	    ig++;
	  }
	  if((itext[i] == ',') && ((itext[i+1] >= 0x30) && (itext[i+1] <= 0x39))){
	    i+=2;
	    ig+=2;
	    if((itext[i] >= 0x30) && (itext[i] <= 0x39)){
	      i++;
	      ig++;
	    }
	  }
	  i--; // Move back on since the i++ moves ahead one.
	}
	else if((itext[i] == '~') && ((itext[i+1] >= 0x61) || (itext[i+1] <= 0x7a))){
	  i += 1;   // Implicit step forward in for loop
	  ig += 2;
	}
      }
      else{
        width += fm.width(itext[i]);
        realIndex++;
	if(width >= max_width){
	  int newi = i;
	  for(; (newi > 0) &&
		(itext[newi] != ' ') &&
		(itext[newi] != '-') &&
		(itext[newi] != '\\'); newi--);
	  if(newi > lastp)
	    i = newi+1;
	  paint_text->append(itext.mid(lastp, i-lastp));
	  ig = 0;
	  width = xPos;
	  lastp = i;
        }
      }
    }
    paint_text->append(itext.mid(lastp, i-lastp));
    rows = paint_text->count();
  }
  else{
    rows = 1;
    //    linewidth = fm.width(itext);
    paint_text->append(itext);
  }
  
  linewidth = parent_lb->width()-35; // set out width to the parent width.
  totalheight =  rows*(lineheight) + 2;

  // Setup the QPixmap's size and colours if it's null.  When sizing
  // changes, QPixmap is changed in updateSize().  rows which is used
  // by width() and height() is set in the prior line sizing, so NEVER
  // EVER call width and height before this point.

  if((old_width != width(0)) ||
     (old_height != height(0)) ||
     (forceClear == TRUE)
    ){
    old_width = width(0);
    old_height = height(0);
    dbuffer->resize(old_width + 35, old_height);
    dbuffer->fill(parent_lb->backgroundColor());
    forceClear = FALSE;
  }
  
  // Print everything to the pixmap so when a paint() comes along
  // we just spit the pixmap out.

  QPainter p;
  if((dbuffer->isNull() == FALSE) && 
     (p.begin(dbuffer) == TRUE)){

    need_update = FALSE;

    p.setFont(parent_lb->font());
    p.setPen(*colour);
    p.setBackgroundColor(parent_lb->backgroundColor());

    if(pm){
      p.drawPixmap(1,0,*pm);
    }
    
    char *txt;
    int row = 0;
    for(txt = paint_text->first(); txt != 0; txt = paint_text->next(), row++){
      KSPainter::colourDrawText(&p, xPos,yPos+lineheight*row, txt);
    }
    p.end();

    if(kSircConfig->transparent == TRUE){
      QImage Imask(dbuffer->width(), dbuffer->height(), 8, 2);
      QImage Ipix = dbuffer->convertToImage();
      Imask.setColor(0, QRgb(0x00000000));
      Imask.setColor(1, QRgb(0x00ffffff));
      for(int i = 0; i < Ipix.height(); i++){
	for(int j = 0; j < Ipix.width(); j++){
	  QRgb c = Ipix.pixel(j, i);
	  if(c != 0x00000000){
	    Imask.setPixel(j, i, 0);
	  }
	  else{
	    Imask.setPixel(j, i, 1);
	  }
	}
      }
      QImage Imask1 = Imask.convertDepth(1);
      QBitmap mask(dbuffer->width(), dbuffer->height());
      mask.convertFromImage(Imask1);
      dbuffer->setMask(mask);
    }

  }
  else{
    cerr << "Start failed!\n";
    if(dbuffer->isNull())
      cerr << "dbuffer is NULL!!!!!!\n";
    dbuffer->resize(1,1);
    need_update = TRUE;
  }

//  if(revOne != -1 || revTwo != -1){
//    debug("revOne: %d RevTwo: %d itext: %s", revOne, revTwo, itext.data());
    //    debug("rtext: %s", rtext.data());
//  }
}

void ircListItem::updateSize(){
  if(PaintCache->find(CacheId))
    setupPainterText();
}

void ircListItem::freeze(bool f){
  frozen = f;
}

void ircListItem::setWrapping(bool _wrap){
  Wrapping = _wrap;
  setupPainterText();
}

inline bool ircListItem::wrapping(){
  return Wrapping;
}

QString ircListItem::getRev(){
    if(revOne == -1 ||
       revTwo == -1)
      return QString();
//    debug("getRev: revOne: %d RevTwo: %d", revOne, revTwo);
    QString seltext = rtext;
    if(revOne < revTwo)
      return seltext.mid(revOne, revTwo - revOne);
    else
      return seltext.mid(revTwo, revOne - revTwo);
}
#include "irclistitem.moc"
