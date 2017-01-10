#include "kspainter.h"
#include <stdlib.h>   

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
				 char *str, int length = -1)
{
  int offset = 0;
  int pcolour;
  char buf[3];
  int loc = 0, i;
  buf[2] = 0;
  bool ReverseText = FALSE;

  // Default pen (colour)

  const QPen qpDefPen = p->pen();

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
	  p->setPen(num2colour[pcolour]);
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
	    if(bcolour < maxcolour ){
	      p->setBackgroundColor(num2colour[bcolour]);
	      p->setBackgroundMode(OpaqueMode);
	    }

	  }
	}
      }
      else if(str[i] == 0x03){
	i++;
	p->setPen(qpDefPen);
	p->setBackgroundMode(TransparentMode);
      }
      else if((str[i] == '~') && ((str[i+1] >= 0x61) || (str[i+1] <= 0x7a))){
	QFont fnt = p->font();
        QColor temppen;
	switch(str[i+1]){
	case 'c':
	  p->setPen(qpDefPen);
	  p->setBackgroundMode(TransparentMode);
	  break;
	case 'C':
	  p->setPen(qpDefPen);
	  p->setBackgroundMode(TransparentMode);
	  fnt.setBold(FALSE);
	  fnt.setItalic(FALSE);
	  fnt.setUnderline(FALSE);
	  ReverseText = TRUE; // Force reverse removed, all fall through.
	  // FALL THROUGH.
        case 'r':
          if(ReverseText == TRUE) {
            ReverseText = FALSE;
            p->setBackgroundMode(TransparentMode);
          }
          else {
            ReverseText = TRUE;
            p->setBackgroundMode(OpaqueMode);
          }
          temppen = p->pen().color();
          p->setPen( p->backgroundColor() );
          p->setBackgroundColor( temppen );
          break;
	case 'b':
	  if(fnt.bold() == TRUE)
	    fnt.setBold(FALSE);
	  else
	    fnt.setBold(TRUE);
	  break;
	case 'i':
	  if(fnt.italic() == TRUE)
	    fnt.setItalic(FALSE);
	  else
	    fnt.setItalic(TRUE);
	  break;
	case 'u':
	  if(fnt.underline() == TRUE)
	    fnt.setUnderline(FALSE);
	  else
	    fnt.setUnderline(TRUE);
	  break;
	case '~':
	  loc++; // Skip ahead 2 characters
	  break;
	default:
	  i-=1;
	  offset -= 1;
	}
	p->setFont(fnt);
	i += 2;
      }
      offset += i - loc;
    }
  }
  p->drawText(startx, starty, str + offset, loc-offset);

}

