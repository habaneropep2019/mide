#include "cutmle.h"

void KCutMLE::scrollToBottom(){
  setAutoUpdate(FALSE);
  for(int i=0; i < 100; i++){ // setYOffset oes seem to go to the last line
    pageDown(FALSE);          // This should fit it!
  }                           // What a bad hack!
  setAutoUpdate(TRUE);
}

#include "cutmle.moc"
