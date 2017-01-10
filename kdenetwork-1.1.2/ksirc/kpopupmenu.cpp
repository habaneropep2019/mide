
#include "kpopupmenu.h"

#include <kapp.h>
#include <qkeycode.h>

bool KAPopupMenu::visible = false;
KGAccel *KAPopupMenu::KGA = 0x0;

KGAccel::KGAccel(QObject * parent = 0, const char * name=0 ) /*fold00*/
    : QObject(parent, name)
{
    accelDict = new QIntDict<accelItem>;
    accelDict->setAutoDelete(TRUE);

    kapp->installEventFilter(this);
    
}

KGAccel::~KGAccel()
{
  delete accelDict;
}

bool KGAccel::eventFilter(QObject *, QEvent *e) /*FOLD00*/
{
    if(e->type() == Event_KeyPress && KAPopupMenu::popVisible() == false){
    QKeyEvent *ke = Q_KEY_EVENT(e);
    if(ke->state() & (ControlButton | AltButton)){
      emit updateAccel();
      
      int key = ke->key();
      if(ke->state() & ControlButton)
	key += CTRL;
      if(ke->state() & AltButton)
	key += ALT;

      debug("Got: %d xlate to %d, we know it as: %p", ke->key(), key, accelDict->find(key));

      accelItem *ai;
      if((ai = accelDict->find(key))){
	ai->menu()->activated(ai->id());
	ke->accept();
	return TRUE;
      }
    }
  }

  return FALSE;
}

void KGAccel::insertAccel(int key, int id, KAPopupMenu *kp) /*FOLD00*/
{

  removeAccel(key); // clear any old accelerators
  
  // Go Looking for the old accelerator and remove it if needed
  QIntDictIterator<accelItem> it(*accelDict);
  while(it.current()){
    if((it.current()->id() == id) && (it.current()->menu() == kp)){
      debug("Remove: %ld", it.currentKey());
      it.current()->menu()->clearAccel(id);
      accelDict->remove(it.currentKey()); // Moves it ahead one
    }
    else
      ++it;
  }

  accelItem *ai = new accelItem(id, kp);
  accelDict->insert(key, ai);
  
}

void KGAccel::removeAccel(int key) /*FOLD00*/
{
  if(accelDict->find(key)){
    accelDict->find(key)->menu()->clearAccel(accelDict->find(key)->id());
    accelDict->remove(key);
  }
}

void KGAccel::clearAccelForPopup(KAPopupMenu *kp)
{
    QIntDictIterator<accelItem> it(*accelDict);
    while(it.current()){
        if(it.current()->menu() == kp){
            debug("Remove: %ld", it.currentKey());
            accelDict->remove(it.currentKey()); // Moves it ahead one
        }
        else
            ++it;
    }

}

KAPopupMenu::KAPopupMenu ( QWidget * parent=0, const char * name=0 ) /*FOLD00*/
    : QPopupMenu(parent, name)
{
  connect(this, SIGNAL(highlighted(int)),
	  this, SLOT(current(int)));
  connect(this, SIGNAL(aboutToShow()),
          this, SLOT(updateAccel()));

  if(KGA == 0x0){
    KGA = new KGAccel();
  }
  connect(KGA, SIGNAL(updateAccel()),
          this, SLOT(updateAccel()));

  
  current_item = -99;
  read_config = false;
  ccount = -1;

  ignoreAlt = false;
  
}

KAPopupMenu::~KAPopupMenu() /*fold00*/
{
    KGA->clearAccelForPopup(this);
}

void KAPopupMenu::show(){ /*fold00*/
  visible = true;
  QPopupMenu::show();
}

void KAPopupMenu::hide(){ /*fold00*/
  visible = false;
  QPopupMenu::hide();
}

void KAPopupMenu::current(int item) /*fold00*/
{
  current_item = item;
}

void KAPopupMenu::keyPressEvent(QKeyEvent *e) /*fold00*/
{
  if(e->state() & ControlButton | e->state() & AltButton){
    e->accept();
    int accel_key = CTRL + e->key();
    QString text_id = text(current_item);
    int spaces = text_id.find("\t"); // Search for accel key starts
    if(spaces != -1)
      text_id.truncate(spaces); // and remove it

    debug("Inserted: %d", accel_key);
    setAccel(0x0, current_item);
    KGA->insertAccel(accel_key, current_item, this);
    
    changeItem(text_id +  "\t" + keyToString(accel_key), current_item);
    
    setActiveItem(0);
    setActiveItem(current_item);

    if(accel_key == CTRL + Key_Alt){     // Remove the accelerator
      KGA->removeAccel(accel_key);
      accel_key = -1;                   // Write -1 to config so it ignores it
    }

    KConfig *cnf = kapp->getConfig();
    cnf->setGroup("UserDefinedAccel");
    QString key_id = text_id + "_" + name("NoName");
    cnf->writeEntry(key_id, accel_key);

    if(e->state() & AltButton)
        ignoreAlt = true;
    
  }
  else{
    QPopupMenu::keyPressEvent(e);
  }
}

void KAPopupMenu::updateAccel() /*fold00*/
{
  if(read_config == false){
    KConfig *cnf = kapp->getConfig();
    cnf->setGroup("UserDefinedAccel");

    for(int i = 0; i < (int) count(); i++){
      int item = idAt(i);
      QString text_id = text(item);
      int spaces = text_id.find("\t"); // Search for accel key starts
      if(spaces != -1)
	text_id.truncate(spaces); // and remove it
      QString key_id = text_id + "_" + name("NoName");
      int key = cnf->readNumEntry(key_id, -1);
      if(key != -1){
	KGA->insertAccel(key, item, this);
        text_id += "\t" + keyToString(key);
        setAccel(0x0, item);
	changeItem(text_id, item);
      }
    }

    
    read_config = true;
  }
}

void KAPopupMenu::activated(int id) /*FOLD00*/
{
  QMenuItem *mi = findItem(id);
  if(mi != 0x0 && mi->signal() != 0x0)
      mi->signal()->activate();
  else
      warning("Could not activate slot!");

  debug("Got id: %d and QMenuItem: %p", id, mi);
}

void KAPopupMenu::clearAccel(int id) /*fold00*/
{
  QString other = text(id);
  int strip = other.find("\t");
  if(strip != -1)
    other.truncate(strip);
  changeItem(other, id);
}


QString KAPopupMenu::keyToString(int key) /*fold00*/
{
  static QIntDict<char> *ktos = 0x0;
  if(ktos == 0x0){
    ktos = new QIntDict<char>;
    ktos->insert(0x1000, "Escape");
    ktos->insert(0x1001, "Tab");
    ktos->insert(0x1002, "Backtab");
    ktos->insert(0x1003, "Backspace");
    ktos->insert(0x1004, "Return");
    ktos->insert(0x1005, "Enter");
    ktos->insert(0x1006, "Insert");
    ktos->insert(0x1007, "Delete");
    ktos->insert(0x1008, "Pause");
    ktos->insert(0x1009, "Print");
    ktos->insert(0x100a, "SysReq");
    ktos->insert(0x1010, "Home");
    ktos->insert(0x1011, "End");
    ktos->insert(0x1012, "Left");
    ktos->insert(0x1013, "Up");
    ktos->insert(0x1014, "Right");
    ktos->insert(0x1015, "Down");
    ktos->insert(0x1016, "Prior");
    ktos->insert(Key_Prior, "PageUp");
    ktos->insert(0x1017, "Next");
    ktos->insert(Key_Next, "PageDown");
    ktos->insert(0x1020, "Shift");
    ktos->insert(0x1021, "Control");
    ktos->insert(0x1022, "Meta");
    ktos->insert(0x1023, "Alt");
    ktos->insert(0x1024, "CapsLock");
    ktos->insert(0x1025, "NumLock");
    ktos->insert(0x1026, "ScrollLock");
    ktos->insert(0x1030, "F1");
    ktos->insert(0x1031, "F2");
    ktos->insert(0x1032, "F3");
    ktos->insert(0x1033, "F4");
    ktos->insert(0x1034, "F5");
    ktos->insert(0x1035, "F6");
    ktos->insert(0x1036, "F7");
    ktos->insert(0x1037, "F8");
    ktos->insert(0x1038, "F9");
    ktos->insert(0x1039, "F10");
    ktos->insert(0x103a, "F11");
    ktos->insert(0x103b, "F12");
    ktos->insert(0x103c, "F13");
    ktos->insert(0x103d, "F14");
    ktos->insert(0x103e, "F15");
    ktos->insert(0x103f, "F16");
    ktos->insert(0x1040, "F17");
    ktos->insert(0x1041, "F18");
    ktos->insert(0x1042, "F19");
    ktos->insert(0x1043, "F20");
    ktos->insert(0x1044, "F21");
    ktos->insert(0x1045, "F22");
    ktos->insert(0x1046, "F23");
    ktos->insert(0x1047, "F24");
    ktos->insert(0x1048, "F25");
    ktos->insert(0x1049, "F26");
    ktos->insert(0x104a, "F27");
    ktos->insert(0x104b, "F28");
    ktos->insert(0x104c, "F29");
    ktos->insert(0x104d, "F30");
    ktos->insert(0x104e, "F31");
    ktos->insert(0x104f, "F32");
    ktos->insert(0x1050, "F33");
    ktos->insert(0x1051, "F34");
    ktos->insert(0x1052, "F35");
    ktos->insert(0x1053, "Super");
    ktos->insert(0x1054, "Super");
    ktos->insert(0x1055, "Menu");
    ktos->insert(0x20, "Space");
    ktos->insert(0x21, "Exclam");
    ktos->insert(0x22, "QuoteDbl");
    ktos->insert(0x23, "NumberSign");
    ktos->insert(0x24, "Dollar");
    ktos->insert(0x25, "Percent");
    ktos->insert(0x26, "Ampersand");
    ktos->insert(0x27, "Apostrophe");
    ktos->insert(0x28, "ParenLeft");
    ktos->insert(0x29, "ParenRight");
    ktos->insert(0x2a, "Asterisk");
    ktos->insert(0x2b, "Plus");
    ktos->insert(0x2c, "Comma");
    ktos->insert(0x2d, "Minus");
    ktos->insert(0x2e, "Period");
    ktos->insert(0x2f, "Slash");
    ktos->insert(0x30, "0");
    ktos->insert(0x31, "1");
    ktos->insert(0x32, "2");
    ktos->insert(0x33, "3");
    ktos->insert(0x34, "4");
    ktos->insert(0x35, "5");
    ktos->insert(0x36, "6");
    ktos->insert(0x37, "7");
    ktos->insert(0x38, "8");
    ktos->insert(0x39, "9");
    ktos->insert(0x3a, "Colon");
    ktos->insert(0x3b, "Semicolon");
    ktos->insert(0x3c, "Less");
    ktos->insert(0x3d, "Equal");
    ktos->insert(0x3e, "Greater");
    ktos->insert(0x3f, "Question");
    ktos->insert(0x40, "At");
    ktos->insert(0x41, "A");
    ktos->insert(0x42, "B");
    ktos->insert(0x43, "C");
    ktos->insert(0x44, "D");
    ktos->insert(0x45, "E");
    ktos->insert(0x46, "F");
    ktos->insert(0x47, "G");
    ktos->insert(0x48, "H");
    ktos->insert(0x49, "I");
    ktos->insert(0x4a, "J");
    ktos->insert(0x4b, "K");
    ktos->insert(0x4c, "L");
    ktos->insert(0x4d, "M");
    ktos->insert(0x4e, "N");
    ktos->insert(0x4f, "O");
    ktos->insert(0x50, "P");
    ktos->insert(0x51, "Q");
    ktos->insert(0x52, "R");
    ktos->insert(0x53, "S");
    ktos->insert(0x54, "T");
    ktos->insert(0x55, "U");
    ktos->insert(0x56, "V");
    ktos->insert(0x57, "W");
    ktos->insert(0x58, "X");
    ktos->insert(0x59, "Y");
    ktos->insert(0x5a, "Z");
    ktos->insert(0x5b, "BracketLeft");
    ktos->insert(0x5c, "Backslash");
    ktos->insert(0x5d, "BracketRight");
    ktos->insert(0x5e, "AsciiCircum");
    ktos->insert(0x5f, "Underscore");
    ktos->insert(0x60, "QuoteLeft");
    ktos->insert(0x7b, "BraceLeft");
    ktos->insert(0x7c, "Bar");
    ktos->insert(0x7d, "BraceRight");
    ktos->insert(0x7e, "AsciiTilde");
  }

  QString string;

  if(key & CTRL){
    string += "Ctrl+";
    key -= CTRL;
  }
  if(key & ALT){
    string += "Alt+";
    key -= ALT;
  }
  if(key & SHIFT){
    string += "Shift+";
    key -= SHIFT;
  }

  if(ktos->find(key)){
    string += ktos->find(key);
    return string;
  }
  else{
    string += "<" + QString(key) + "?" + ">";
    return string;
  }
}

#include "kpopupmenu.moc"

