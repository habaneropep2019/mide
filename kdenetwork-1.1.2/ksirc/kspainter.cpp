#include "kspainter.h"
#include <stdlib.h>
#include <iostream.h>

#include <qregexp.h>
#include <qapp.h>

const int KSPainter::maxcolour = 16;
const QColor KSPainter::brown    ( 165,  42,  42 );
const QColor KSPainter::orange	 ( 255, 165,   0 );
const QColor KSPainter::lightCyan( 224, 255, 255 );
const QColor KSPainter::lightBlue( 173, 216, 230 );
const QColor KSPainter::pink	 ( 255, 192, 203 );

const QColor KSPainter::num2colour[KSPainter::maxcolour] = { white,
							     black,
							     darkBlue,
							     darkGreen,
							     red,
							     brown,
							     darkMagenta,
							     orange,
							     yellow,
							     green,
							     darkCyan,
							     cyan,
							     blue,
							     pink,
							     gray,
							     lightGray };

int KSPainter::colour2num(const QColor &colour)
{
  for(int i = 0; i < maxcolour; i++)
    if(num2colour[i] == colour)
      return i;

  return -1;
}

void KSPainter::colourDrawText(QPainter *p, int startx, int starty,
				 char *str, int length)
{
  int offset = 0;
  int pcolour;
  char buf[3];
  int loc = 0, i;
  buf[2] = 0;

  PainterState curState(p);

  for(loc = 0; (str[loc] != 0x00) && (loc != length) ; loc++){
    if(str[loc] == 0x03 || str[loc] == '~'){
      i = loc;
      p->drawText(startx, starty, str + offset, i-offset);
      startx += p->fontMetrics().width(str + offset, i-offset);
      offset = i;
      //      lastp = i;
      if((str[i+1] >= 0x30) && (str[i+1] <= 0x39)){
	i++;
	buf[0] = str[i];
	i++;
	if((str[i] >= 0x30) && (str[i] <= 0x39)){
	  buf[1] = str[i];
	  i++;
	}
	else{
	  buf[1] = 0;
	}
	
	pcolour = atoi(buf);
        if(pcolour < maxcolour)
          curState.setFGCol(num2colour[pcolour]);
	else
          i = loc;
        
	if(str[i] == ','){
	  i++;
	  if((str[i] >= 0x30) && (str[i] <= 0x39)){
	    buf[0] = str[i];
	    i++;
	    if((str[i] >= 0x30) && (str[i] <= 0x39)){
	      buf[1] = str[i];
	      i++;
	    }
	    else{
	      buf[1] = 0;
	    }
	    int bcolour = atoi(buf);
	    if(pcolour == bcolour){
	      if(bcolour + 1 < maxcolour)
		bcolour += 1;
	      else
		bcolour -= 1;
	    }
            if(bcolour < maxcolour )
              curState.setBGCol(num2colour[bcolour]);
            
	  }
	}
      }
      else if(str[i] == 0x03){
        i++;
        curState.resetCol();
      }
      else if((str[i] == '~') && ((str[i+1] >= 0x61) || (str[i+1] <= 0x7a))){
	switch(str[i+1]){
        case 'c':
          curState.resetCol();
	  break;
        case 'C':
          curState.resetAll();
          break;
        case 'r':
          curState.toggleRev();
	  break;
        case 's': // Same as ~r but s mens selection
          curState.toggleSel();
          break;
        case 'b':
          curState.toggleBold();
	  break;
        case 'i':
          curState.toggleItalics();
          break;
        case 'u':
          curState.toggleUnderline();
          break;
        case 'p':
          QApplication::beep();
          break;
	case '~':
	  loc++; // Skip ahead 2 characters
	  break;
	default:
	  i-=1;
	  offset -= 1;
	}
	i += 2;
      }
      offset += i - loc;
    }
  }
  p->drawText(startx, starty, str + offset, loc-offset);

}

QString KSPainter::stripColourCodes(QString col, QList<int> *xlate){
  QString noCol;

  if(col.isNull() || col.isEmpty()) // Sanity check
    return noCol;

  col.detach();
  col.replace(QRegExp("~s"), "");

  if(xlate != 0x0){
    xlate->clear();
    xlate->setAutoDelete(TRUE);
  }
//  debug("Processing: %s", col.data());
  for(uint i = 0; i < col.length() ;){
    if(((col[i] == '~') || (col[i] == 0x03)) && ((col[i+1] >= '0') && (col[i+1] <= '9'))){
      i += 2; // Step over the ~ and the first number
      if((col[i] >= '0') && (col[i] <= '9'))
        i ++; // Step over the next number (~12) idea
      if((col[i] == ',') && ((col[i+1] >= '0') && (col[i+1] <= '9'))){
        i+=2; // Step over the , and first number
        if((col[i] >= 0x30) && (col[i] <= 0x39)){
          i++; // Step over the number number
        }
      }
    }
    else if((col[i] == '~') && ((col[i+1] >= 'a') || (col[i+1] <= 'z'))){ // a->z for vairous control codes
      switch(col[i+1]){
      case 'b':
      case 'u':
      case 'r':
      case 's':
      case 'c':
      case 'i':
      case 'C':
      case 'p':
        i+=2; // Move ahead 2 characters (~x == 2)
        break;
      default:
	//        i++; // Move to next character
	noCol += col.mid(i, 1);
	if(xlate != 0x0)
	  xlate->append(new("int") int(i));
	i+=2; // Move ahead to next character
	break;
      }
    }
    else{
      // It's a printable character
      //      cerr << i << "-" << col.mid(i, 1) << " ";
      noCol += col.mid(i, 1);
      if(xlate != 0x0)
        xlate->append(new("int") int(i));

      i++; // Move ahead to next character
    }
  }
//  cerr << endl;
//debug("NoCol: %s", noCol.data());
  return noCol;
  
}

PainterState::PainterState(QPainter *p){
  painter = p;

  qcDefBGCol = p->backgroundColor();
  qcDefFGCol = p->pen().color();
  qfDefFont = p->font();
  qfCurFont = qfDefFont;

  bSelect = FALSE;
  bReverse = FALSE;

  p->setBackgroundMode(OpaqueMode);
  
}

PainterState::~PainterState(){
}

inline void PainterState::resetCol(){
  if(bSelect == FALSE && bSelect == FALSE){
    painter->setPen(qcDefFGCol);
    painter->setBackgroundColor(qcDefBGCol);
  }
  else{
    painter->setPen(qcDefBGCol);
    painter->setBackgroundColor(qcDefFGCol);
  }
}

inline void PainterState::resetAll(){
  if(bSelect == FALSE){
    painter->setPen(qcDefFGCol);
    painter->setBackgroundColor(qcDefBGCol);
    painter->setFont(qfDefFont);
  }
  else{
    painter->setPen(qcDefBGCol);
    painter->setBackgroundColor(qcDefFGCol);
    painter->setFont(qfDefFont);

  }
}

inline void PainterState::setFGCol(const QColor &col){
  if(bSelect == FALSE && bSelect == FALSE){
    painter->setPen(col);
  }
  else
    painter->setBackgroundColor(col);
}
    
inline void PainterState::setBGCol(const QColor &col){
  if(bSelect == FALSE && bSelect == FALSE)
    painter->setBackgroundColor(col);
  else
    painter->setPen(col);
}

inline void PainterState::toggleRev(){
  if(bReverse == FALSE)
    bReverse = TRUE;
  else // bReverse == TRUE
    bReverse = FALSE;
  QColor tmpcol = painter->backgroundColor();
  painter->setBackgroundColor(painter->pen().color());
  painter->setPen(tmpcol);
}

inline void PainterState::toggleSel(){
  if(bSelect == FALSE)
    bSelect = TRUE;
  else // bReverse == TRUE
    bSelect = FALSE;
  QColor tmpcol = painter->backgroundColor();
  painter->setBackgroundColor(painter->pen().color());
  painter->setPen(tmpcol);
}

inline void PainterState::toggleItalics(){
  qfCurFont.setItalic(!qfCurFont.italic());
  painter->setFont(qfCurFont);
}

inline void PainterState::toggleBold(){
  qfCurFont.setBold(!qfCurFont.bold());
  painter->setFont(qfCurFont);
}

inline void PainterState::toggleUnderline(){
  qfCurFont.setUnderline(!qfCurFont.underline());
  painter->setFont(qfCurFont);
}