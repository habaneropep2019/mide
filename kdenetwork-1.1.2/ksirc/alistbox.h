#ifndef ALISTBOX_H
#define ALISTBOX_H

class aListBox;
class nickListItem;

#include <qobject.h>
#include <qwidget.h>
#include <qlistbox.h>
#include <qevent.h>

#include "irclistitem.h"

class nickListItem : public QListBoxItem
{
 public:
  nickListItem();
  virtual ~nickListItem();

  int height ( const QListBox * ) const;
  int width ( const QListBox * ) const;
  const char* text () const;
  const QPixmap* pixmap () const;
  
  bool op();
  bool voice();

  void setOp(bool _op = FALSE);
  void setVoice(bool _voice = FALSE);
  void setAway(bool _away = FALSE);
  void setIrcOp(bool _ircop = FALSE);

  void setText(const char *str);

  nickListItem &operator= ( const nickListItem & nli );
  
 protected:
  virtual void paint ( QPainter * );

 private:
  bool is_op;
  bool is_voice;
  bool is_away;
  bool is_ircop;

  QString string;
};

class aListBox : public QListBox
{
  Q_OBJECT;

public:
  aListBox(QWidget *parent = 0, const char *name = 0) : QListBox(parent,name)
    {
      clear();
      p_scroll = palette().copy();
    }

  virtual ~aListBox();

  void clear();

  void inSort ( nickListItem *);
  void inSort ( const char * text, bool top=FALSE);

  nickListItem *item(int index);

  bool isTop(int index);

  virtual void setPalette ( const QPalette & );

  int findNick(QString str);

signals:
   void rightButtonPress(int index);

protected:
  virtual void mousePressEvent ( QMouseEvent * );
  virtual int findSep();
  virtual int searchFor(QString nick, bool &found, bool top);

private:
  QPalette p_scroll;


};

#endif
