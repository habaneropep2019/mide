#ifndef LINELISTITEM_H
#define LINELISTITEM_H 

#include <qlistbox.h>
#include <qcolor.h>
#include <qstring.h>

class lineListItem : public QListBoxItem
{
 public:
  lineListItem(QListBox *_lb, QColor c = black) : QListBoxItem()
    { 
      colour = c; 
      lb = _lb;
    }

  void setText(char *_str){
    QListBoxItem::setText(_str);
  }

 protected:
  virtual void paint( QPainter * );
  virtual int height( const QListBox * ) const
    {
      return 3;
    }
  virtual int width( const QListBox *lb ) const
    {
      return lb->width();
    }
  virtual const QPixmap *pixmap() { return 0; }

 private:
  QColor colour;
  QListBox *lb;
  QString str;

};


#endif
