#ifndef KSCUTMLE_H
#define KSCUTMLE_H

#include <qmlined.h>

class KCutMLE : public QMultiLineEdit
{
  Q_OBJECT
public:
  KCutMLE ( QWidget * parent=0, const char * name=0 ) :
    QMultiLineEdit(parent, name)
    {
    }

  void scrollToBottom();

};


#endif
