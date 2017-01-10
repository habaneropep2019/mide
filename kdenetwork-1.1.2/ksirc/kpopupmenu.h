#ifndef _KPOPUPMENU_H_
#define _KPOPUPMENU_H_

#define INCLUDE_MENUITEM_DEF 1

#include <qpopupmenu.h>
#include <qevent.h>
#include <qintdict.h>

class accelItem;
class KGAccel;
class KAPopupMenu;

class accelItem
{
public:
  accelItem(int i, KAPopupMenu *k){
      identifier = i;
      kp = k;
  }

  int id(){
    return identifier;
  }

  KAPopupMenu *menu(){
      return kp;
  }
  
private:
    int identifier;
    KAPopupMenu *kp;
};

class KGAccel : public QObject
{
    Q_OBJECT
public:
    KGAccel(QObject * parent = 0, const char * name=0 );
    virtual ~KGAccel();

    virtual bool eventFilter(QObject *, QEvent *);
    virtual void insertAccel(int key, int id, KAPopupMenu *);
    virtual void removeAccel(int key);
    virtual void clearAccelForPopup(KAPopupMenu *);

signals:
    void updateAccel();

private:
    QIntDict<accelItem> *accelDict;
    
};

class KAPopupMenu : public QPopupMenu
{
    Q_OBJECT
public:
    KAPopupMenu ( QWidget * parent=0, const char * name=0 );
    virtual ~KAPopupMenu ();

    virtual void show();
    virtual void hide();

    static bool popVisible() { return visible; }

    virtual void activated(int id);
    virtual void clearAccel(int id);

protected slots:
  virtual void current(int id);
  virtual void updateAccel();

protected:
  virtual void keyPressEvent(QKeyEvent *);

  virtual QString keyToString(int);


private:
  int current_item;
  bool read_config;
  
  int ccount;

  bool ignoreAlt;

  static bool visible;
  static KGAccel *KGA;

};

#endif