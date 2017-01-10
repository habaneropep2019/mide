/************************************************************************

 AHistLineEdit

 $$Id: ahistlineedit.cpp,v 1.12.2.1 1999/02/09 14:46:51 kulow Exp $$

 An extended QLineEdit with history scroll back and focus controls.

 Function:

   Each time a line is saved, to a max of 10 when the user hits enter.
   The arrow keys are used to scroll through the history list, rolling
   arround at the end.

   When focus is gained or lost it emait the approriate signals, This
   is so the toplevel can track who has focus.

 signals:
   gotFocus: duh!
   
   lostFocus: no shit sherlock

 Implementation:

   protected:

     keyPressEvent: Filter key presses looking for up arrow, down
       arrow or enter.  UpArrow saves the current line at the end then
       scroll. No more processing.  DownArrow does the oposite.  Enter
       sves the line, but then passes on the event for normal
       processing.

     focusInEvent: emits neded isngal
     focusOutEvent: ditto

   Variables:
     QStrList: current list of history items.
     current: what I think is the current list item.

*************************************************************************/


#include "ahistlineedit.h"
#include "KSColourPicker/kscolourpicker.h"
#include "config.h"
#include <iostream.h>

extern global_config *kSircConfig;

aHistLineEdit::aHistLineEdit(QWidget *parent, const char *name)
  : QLineEdit(parent, name)
{
  current = 0;
  hist.append(""); // Set the current as blank
}

void aHistLineEdit::keyPressEvent ( QKeyEvent *e )
{
  if ( e->key() != Key_Tab)
    emit notTab();

  if ( e->state() == ControlButton ) {
    QString s;
    s.insert( 0, text() );
    int curPos = cursorPosition();
    switch ( e->key() ) {
    case Key_B:
      s.insert( cursorPosition(), 0xdf );
      setText(s);
      setCursorPosition(curPos + 1);
      break;
    case Key_U:
      s.insert( cursorPosition(), 0xdc );
      setText(s);
      setCursorPosition(curPos + 1);
      break;
    case Key_R:
      s.insert( cursorPosition(), 0x9f );
      setText(s);
      setCursorPosition(curPos + 1);
      break;
    case Key_K:
      s.insert(  cursorPosition(), 0xa9 );
      setText(s);
      setCursorPosition(curPos + 1);
      if ( kSircConfig->ColourPicker ) {
        ColourPickerPopUp();
      }
      break;
    case Key_O:
      s.insert( cursorPosition(), 0xa4 );
      setText(s);
      setCursorPosition(curPos + 1);
      break;
    case Key_I:
      s.insert( cursorPosition(), "~i");
      setText(s);
      setCursorPosition(curPos + 2);
      break;
    default:
      QLineEdit::keyPressEvent(e);
    }
  }
  else if(e->state() == 0){
    switch(e->key()){
    case Key_Up:
      if(hist.at() == current){ // same as Key_Down
	hist.remove(current);
	hist.insert(current, text());
      }
      if(hist.at() < 1){ // Same idea as Key_Down so look there for an
	hist.last();     // explanation
	setText(hist.current());
      }
      else
	setText(hist.prev()); // Set text to prev item
      break;
    case Key_Down:
      if(hist.at() == current){ // If we're leaving the active line, save it
	hist.remove(current);   // remove last line
	hist.insert(current, text()); // update it to the current line
      }
      if(hist.at() >= (int) hist.count()-1){ // If we're moving byond the end
	hist.first();                        // roll to first one
	setText(hist.current());             // set text to first line
      }
      else
	setText(hist.next());                // Set text to the next one
      break;
    case Key_Return:
    case Key_Enter:
      if(hist.count() > 20){
	hist.removeFirst(); // more than 20 entries? ick
        current--;          // backup counter
      }
      if(text() != ""){
	hist.remove(current); // remove the "dummy" entry
	hist.append(text()); // add the current text to the end
	hist.append(""); // set the next line to blank
	current = hist.count() - 1; // move ahead one.
      }
    default:
      QLineEdit::keyPressEvent(e);
    }
  }
  else{
    QLineEdit::keyPressEvent(e);
  }
}

void aHistLineEdit::ColourPickerPopUp()
{
  kscolourpicker *kscp = new("kscolourpicker") kscolourpicker();
  connect(kscp, SIGNAL(picked( QString )), 
          this, SLOT(slot_insert( QString )));
  kscp->show();
}

void aHistLineEdit::slot_insert( QString str )
{
  QString s = text();
  int curPos = cursorPosition();
  s.insert(curPos, str);
  setText(s);
  setCursorPosition(curPos + str.length());
}

void aHistLineEdit::focusInEvent(QFocusEvent *)
{
  emit gotFocus();
}

void aHistLineEdit::focusOutEvent(QFocusEvent *)
{
  emit lostFocus();
}

void aHistLineEdit::mousePressEvent ( QMouseEvent *e )
{
  if(e->button() == MidButton){
    emit pasteText();
  }
  else{
    QLineEdit::mousePressEvent(e);
  }
}
#include "ahistlineedit.moc"
