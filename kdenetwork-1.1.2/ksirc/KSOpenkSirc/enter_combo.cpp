#include "enter_combo.h"

void EnterCombo::keyPressEvent( QKeyEvent *e ){
  if(e->key() == Key_Return || e->key() == Key_Enter)
    emit(enterPressed());
  else
    QComboBox::keyPressEvent(e);
}

#include "enter_combo.moc"
