/*
 *             BW_LED_Number a very very primitive LED
 * 
 * Copyright: Bernd Johannes Wuebben, wuebben@math.cornell.edu
 * 
 * $Id$
 * 
 * $Log$
 * Revision 1.5  1998/01/02 00:47:17  wuebben
 * Bernd: Monster Commit !! Finished CDDB support and Web intergration
 * Let the games begin
 *
 * Revision 1.4  1997/10/25 15:15:34  wuebben
 * Bernd: fixed the segfault error if you had no permiision to access the cdrom
 * drive
 *
 * Revision 1.2  1997/08/15 22:51:25  wuebben
 * *** empty log message ***
 *
 * Revision 1.2  1997/08/15 22:42:54  wuebben
 * Bernd -- Version 0.4
 * o Support for cdda on Solaris
 * o Support for Irix
 * o New interactive configuration script
 * o  lot of header, define and configuration changes. -- Keep your
 *   fingers crossed .... ;-)
 *
 * Revision 1.1  1997/08/14 23:51:07  wuebben
 * Initial revision
 *
 * Revision 1.1  1997/06/21 23:09:10  wuebben
 * Initial revision
 *
 * Revision 1.2  1997/03/22 22:09:37  wuebben
 * Added support for visibe non-illuminated Segments
 *
 * Revision 1.1  1997/03/22 12:45:25  wuebben
 * Initial revision
 *
 *
 *
 * 
 */


#include "bwlednum.h"
#include "qbitarry.h"
#include "qpainter.h"
#include <stdio.h>

#include "bwlednum.moc"

#define NUM_OF_SEGMENTS 8
#define STOP_CHAR 25


static char segs[14][8] =
{ { 0, 1, 2, 4, 5, 6,25, 0},		// 0 
  { 2, 5,25, 0, 0, 0, 0, 0},		// 1
  { 1, 2, 3, 4, 6,25 ,0, 0},		// 2
  { 1, 2, 3, 5, 6,25, 0, 0},		// 3
  { 0, 3, 2, 5 ,25, 0, 0, 0},		// 4
  { 0, 1, 3, 5, 6,25, 0, 0},		// 5 
  { 0, 1, 3, 4, 5, 6,25, 0},		// 6
  { 1,2 , 5,25, 0, 0, 0, 0},		// 7
  { 0, 1, 2, 3, 4, 5, 6,25},		// 8
  { 0, 1, 2, 3, 5, 6,25, 0},		// 9 
  { 3,25, 0, 0, 0, 0, 0, 0},		// -
  { 7,25, 0, 0, 0, 0, 0, 0},		// .
  { 8, 9,25, 0, 0, 0, 0, 0},		// :
  {25, 0, 0, 0, 0, 0, 0, 0} };		// blank


BW_LED_Number::BW_LED_Number( QWidget *parent, const char *name )
  : QFrame( parent, name ){
    

    offcolor = QColor(100,0,0);
    showOffColon(FALSE);
    smallLED = false;
    current_symbol = ' ';
    old_symbol = ' ';
    old_segments = &segs[13][0];      // nothing
    current_segments = &segs[13][0];  // nothing
    setLEDColor(yellow,black);
    
}

void dump_segments(char * segs){

  printf("dumping segments:");
  for (int i = 0; i <=7; i++){
    printf("%d:",segs[i]);
  }
  printf("\n");
}


BW_LED_Number::~BW_LED_Number(){

}

void  BW_LED_Number::resizeEvent( QResizeEvent * ){

}

void BW_LED_Number::showOffColon(bool off){
 
  show_off_colon = off;

}

void BW_LED_Number::setLEDColor( const QColor& fgColor, const QColor& bgColor ){

  fgcolor = fgColor;
  bgcolor = bgColor;

  QColorGroup old_cg = this->colorGroup();


  QColorGroup new_cg( fgColor, bgColor,
		      fgColor, fgColor, fgColor, 
		      fgColor, fgColor );

        
  this->setPalette(QPalette(new_cg, new_cg, new_cg));

}




static char *getSegments( char s)	
                                        
{
  if (s >= '0' && s <= '9'){
    return segs[s - '0'];
  }

    int j;

    switch ( s ) {
    case '-':
      j = 10;  
      break;
    case 'O':
      j = 0;   
      break;
    case '.':
      j = 11;  
      break;
    case ':':
      j = 12;  
      break;
    default:
      j = 13;  
      break;
    }

    return segs[j];
}

void BW_LED_Number::drawContents( QPainter *p ){

  drawSymbol( p, current_symbol,TRUE );

}


void BW_LED_Number::display(int i ){

 if( (i<0) || (i> 9))
   return;
 display( (char)('0'+ i));

}


void BW_LED_Number::display(char s ){
  
  QPainter p;     

  p.begin( this );
  
  old_symbol = current_symbol;
  old_segments = current_segments;
  current_symbol = s;
  current_segments = getSegments(s);

  drawSymbol(&p,s,FALSE);

  p.end();

}

void BW_LED_Number::setSmallLED(bool a_boolean){

 smallLED = a_boolean;

}


void BW_LED_Number::drawSymbol( QPainter *p,char s,bool repaint ){
  
  //  printf("drawSymbol repaint = %d\n",repaint);

 s = s;

 QPoint  pos;
  
 int xSegment_Length, ySegment_Length, Segment_Length, xAdvance;   
 int Xoffset, Yoffset, space;

 space = 1;

 xSegment_Length  = width()*5/((5 + space) + space) ;

 ySegment_Length   = height()*5/12;

 Segment_Length   = ySegment_Length > xSegment_Length ? xSegment_Length : ySegment_Length;

 xAdvance = Segment_Length*( 5 + space )/5 +1  ;  

 // Xoffset  = ( width() - xAdvance + Segment_Length/5 )/2; // origininal
 Xoffset  = ( width() - xAdvance + Segment_Length/4 )/2;
 Yoffset  = ( height() - Segment_Length*2 )/2;
  
 pos = QPoint( Xoffset , Yoffset );	


 if(repaint){
 
   // this draw the non-illumintated segments

   if(show_off_colon){// we want to show the colon which is actually ugly and
                      // by default not shown.

     for(int l = 0; l <= NUM_OF_SEGMENTS +1; l++){
       drawSegment(pos,(char) l,*p,Segment_Length,TRUE);      //erase segment
     }
   }
   else{
     for(int l = 0; l <= NUM_OF_SEGMENTS -1; l++){
       drawSegment(pos,(char) l,*p,Segment_Length,TRUE);      //erase segment
     }
   }

   // now draw the illuminated segments

   for(int l = 0; l <= NUM_OF_SEGMENTS -1; l++){
     if(current_segments[l] != STOP_CHAR){
     	 drawSegment(pos,current_segments[l],*p,Segment_Length,FALSE);   // draw segment
     }
     else{
       break;
     }
   }
 }
 else{ // we are not repainting ourselves due to a repaint event but rather
       // genuinely changing the symbol that is to be displayed 

   for(int l = 0; l <= NUM_OF_SEGMENTS -1; l++){

     if(current_segments[l] != STOP_CHAR){
       if(!seg_contained_in(current_segments[l],old_segments)) 
	 drawSegment(pos,current_segments[l],*p,Segment_Length,FALSE);   // draw segment
     }
     else{
       break;
     }
   }

 
   for(int k = 0; k <= NUM_OF_SEGMENTS -1; k++){

     if(old_segments[k] != STOP_CHAR){
       if(!seg_contained_in(old_segments[k],current_segments))
	 drawSegment(pos,old_segments[k],*p,Segment_Length,TRUE);      //erase segment
     }
     else{
       break;
     }
   }
 }
}



bool BW_LED_Number::seg_contained_in( char c, char* seg){

  bool result = FALSE;

  while ( *seg != STOP_CHAR){
    //       printf("Comparing %d with %d\n",c,*seg);
    if ( c == *seg )
      result = TRUE;
    seg++;
  }

  return result;
}

void BW_LED_Number::setLEDoffColor(QColor color){

  offcolor = color;
}


void BW_LED_Number::drawSegment( const QPoint &pos, char seg_number, QPainter &p,
			      int Segment_Length, bool erase){


  QPoint pt = pos;
  QColorGroup g = colorGroup();
  QColor lightColor,darkColor;
  if ( erase ){
    
    lightColor = offcolor;
    darkColor  = offcolor;
    
  } else {
    lightColor = g.light();
    darkColor  = g.dark();
  }

  //  int Width = (int) Segment_Length/5 ; // original
  int Width = (int) Segment_Length/4;

  
  QBrush brush(g.light()); 
  QPointArray pts;
  
  
  pt.ry() += (QCOORD)Width/2;
  
  
  if (erase){
    
    p.setBrush(offcolor);
    brush.setColor(offcolor);
    
  }
  else
    p.setBrush(g.light());
  
  if(!smallLED){
    
    switch ( seg_number ) {
    case 0 :
      
      
      if (erase) 
	p.setPen(offcolor);
      
      pts.setPoints(3,pt.x(), pt.y() ,
		    pt.x(), pt.y()-Width +1,
		    pt.x() + Width-1, pt.y());
      p.drawPolygon(pts);
      pts.setPoints(3,pt.x(), pt.y() + Segment_Length -Width - Width/2 -1  ,
		    pt.x() + Width -1 , pt.y()  -Width +Segment_Length - Width/2 -1,
		    pt.x() , pt.y() + Segment_Length - 3*Width/4 -1);
      p.drawPolygon(pts);
      
      if (erase)
	p.setPen(g.light());
      
      p.fillRect(pt.x(),pt.y()+ Width/2 -1, Width , 
		 Segment_Length - Width -Width +1 ,brush);
      
      break;
    case 1 :
      
      p.fillRect(pt.x()+Width,pt.y()- Width , Segment_Length -2* Width, Width ,brush);
      
      if (erase) 
	p.setPen(offcolor);
      
      pts.setPoints(3,pt.x()+1, pt.y()-Width  ,
		    pt.x()+Width, pt.y()-Width  ,
		    pt.x() + Width, pt.y() -1 );
      p.drawPolygon(pts);
      
      pts.setPoints(3,pt.x()+ Segment_Length - Width , pt.y() - Width,
		    pt.x()+  Segment_Length  -1, pt.y() - Width,
		    pt.x() + Segment_Length - Width , pt.y() -1 );
      p.drawPolygon(pts);
      
      if (erase) 
	p.setPen(g.light());
      break;
    case 2 :
      pt.rx() += (QCOORD)(Segment_Length);
      
      
      if (erase) 
	p.setPen(offcolor);
      
      pts.setPoints(3,pt.x() , pt.y() ,
		    pt.x() , pt.y() - Width + 1,    // changes from 1 to 2 
		    pt.x() - Width +1, pt.y() );
      
      p.drawPolygon(pts);
      
      pts.setPoints(3,pt.x() , pt.y() + Segment_Length - Width - Width/2 -1,
		    pt.x() , pt.y() + Segment_Length - 3*Width/4 - 1,
		    pt.x() - Width +1, pt.y() + Segment_Length - Width - Width/2 -1);

      p.drawPolygon(pts);
      
      if (erase)
	p.setPen(g.light());
      
      p.fillRect(pt.x() - Width+1 ,pt.y() + Width/2- 1, Width , 
		 Segment_Length - Width - Width + 1 ,brush);
      
      break;
    case 3 :
      
      pt.ry() += (QCOORD)Segment_Length;
      
      p.setPen(g.background());
      
      pts.setPoints(3,pt.x()-1 , pt.y() - Width/2 -1,
		    pt.x() + Width+2, pt.y()-Width -1 ,//
		    pt.x() + Width+2, pt.y() );
      p.drawPolygon(pts);
      pts.setPoints(3,pt.x() + Segment_Length + 1, pt.y()  - Width/2 -1  ,
		    pt.x() + Segment_Length - Width - 2 , 
		    pt.y() - Width -1,
		    pt.x() + Segment_Length - Width - 2, pt.y() );
      p.drawPolygon(pts);
      
      p.setPen(g.light());
      p.fillRect(pt.x() + Width -1 ,pt.y() - Width, Segment_Length- 2* Width + 3, 
		       Width  ,brush);	    
      
      break;
    case 4 :
      pt.ry() += (QCOORD)(Segment_Length +1);
      p.fillRect(pt.x(), pt.y(), Width , Segment_Length - Width - Width/2 ,brush);
      if (erase) 
	p.setPen(offcolor);
      
      pts.setPoints(3,pt.x(), pt.y(),
		    pt.x(), pt.y()-Width+1,
		    pt.x() + Width-1, pt.y());
      p.drawPolygon(pts);
      pts.setPoints(3,pt.x(), pt.y() + Segment_Length -Width - Width/2  -1 ,
		    pt.x() + Width -1 , pt.y()  -Width +Segment_Length - Width/2 -1 ,
		    pt.x() , pt.y() + Segment_Length - 3*Width/4 -1);
      p.drawPolygon(pts);
      
      if (erase)
	p.setPen(g.light());
      
      break;
    case 5 :
      pt.rx() += (QCOORD)(Segment_Length );
      pt.ry() += (QCOORD)(Segment_Length +1);
      p.fillRect(pt.x() - Width +1  ,pt.y(), Width , 
		 Segment_Length - Width - Width/2  ,brush);
      
      if (erase) 
	p.setPen(offcolor);
      
      pts.setPoints(3,pt.x() , pt.y(),
		    pt.x() , pt.y() - Width  +1,
		    pt.x() - Width +1, pt.y());
      
      p.drawPolygon(pts);
      
      pts.setPoints(3,pt.x() , pt.y() + Segment_Length - Width - Width/2 -1,
		    pt.x() , pt.y() + Segment_Length - 3*Width/4 -1,
		    pt.x() - Width +1, pt.y() + Segment_Length - Width - Width/2 -1);

      p.drawPolygon(pts);
      
      if (erase)
	p.setPen(g.light());
      
      break;
    case 6 :
      pt.ry() += (QCOORD)(Segment_Length*2 );
      p.fillRect(pt.x() + Width  ,pt.y() -Width , Segment_Length -2* Width , 
		 Width  ,brush);
      
      if (erase) 
	p.setPen(offcolor);
      
      pts.setPoints(3,pt.x()+1, pt.y()-1,
		    pt.x() + Width, pt.y() - Width,
		    pt.x() + Width, pt.y() - 1 );
      p.drawPolygon(pts);
      
      pts.setPoints(3, pt.x() + Segment_Length - 1, pt.y()-1,
		    pt.x() + Segment_Length - Width , pt.y() - Width, 
		    pt.x() + Segment_Length - Width , pt.y() - 1 );
             
      p.drawPolygon(pts);
      
      if (erase) 
	p.setPen(g.light());
      
      
      
      break;
    case 7 :
      pt.rx() += (QCOORD)(Segment_Length/2);
      pt.ry() += (QCOORD)(Segment_Length*2);
      p.fillRect(pt.x()  ,pt.y() - Width , Width , Width  ,brush);
      break;
    case 8 :
      pt.ry() += (QCOORD)(Segment_Length/2 + Width/2);
      pt.rx() += (QCOORD)(Segment_Length/2 - Width/2 + 1);
      
      if (!show_off_colon && erase) {
	p.setBrush(bgcolor);
        brush.setColor(bgcolor);
      }

      p.fillRect(pt.x()  ,pt.y() - Width  , Width , Width  ,brush);
      p.moveTo(pt);
      
      if (!show_off_colon && erase) {
	p.setBrush(fgcolor);
        brush.setColor(fgcolor);
      }

      break;
    case 9 :
      pt.ry() += (QCOORD)(3*Segment_Length/2 + Width/2);
      pt.rx() += (QCOORD)(Segment_Length/2 - Width/2 + 1);

      if (!show_off_colon && erase) {
	p.setBrush(bgcolor);
        brush.setColor(bgcolor);
      }
      p.fillRect(pt.x() ,pt.y() - Width  , Width , Width  ,brush);

      if (!show_off_colon && erase) {
	p.setBrush(fgcolor);
        brush.setColor(fgcolor);
      }
      break;
    }
    
  } /*  if (!smallLED) */
  
  else{
    
    pt.ry() += (QCOORD)Width/2;
    
    switch ( seg_number ) {
    case 0 :
      p.fillRect(pt.x(),pt.y()+ Width /2, Width , Segment_Length - Width -Width/2 ,brush);
      break;
    case 1 :
      p.fillRect(pt.x()+Width,pt.y()- Width , Segment_Length -2* Width, Width ,brush);
      break;
    case 2 :
      pt.rx() += (QCOORD)(Segment_Length);
      p.fillRect(pt.x()-Width,pt.y()+ Width/2, Width , 
		 Segment_Length - Width -Width/2 ,brush);
      break;
    case 3 :
      pt.ry() += (QCOORD)Segment_Length;
      p.fillRect(pt.x() + Width ,pt.y() - Width, Segment_Length- 2* Width, Width  ,brush);
      break;
    case 4 :
      pt.ry() += (QCOORD)(Segment_Length );
      p.fillRect(pt.x(), pt.y(), Width , Segment_Length - Width - Width/2 ,brush);
      break;
    case 5 :
      pt.rx() += (QCOORD)(Segment_Length );
      pt.ry() += (QCOORD)(Segment_Length );
      p.fillRect(pt.x() - Width ,pt.y(), Width , 
		 Segment_Length - Width - Width/2  ,brush);
      break;
    case 6 :
      pt.ry() += (QCOORD)(Segment_Length*2);
      p.fillRect(pt.x() + Width  ,pt.y() -Width , Segment_Length -2* Width , 
		 Width  ,brush);
      break;
    case 7 :
      pt.rx() += (QCOORD)(Segment_Length/2);
      pt.ry() += (QCOORD)(Segment_Length*2);
      p.fillRect(pt.x()  ,pt.y() - Width , Width , Width  ,brush);
      break;
    case 8 :
      pt.ry() += (QCOORD)(Segment_Length/2 + Width/2);
      pt.rx() += (QCOORD)(Segment_Length/2 - Width/2 + 1);
      if (!show_off_colon && erase) {
	p.setBrush(bgcolor);
        brush.setColor(bgcolor);
      }

      p.fillRect(pt.x()  ,pt.y() - Width  , Width , Width  ,brush);
      p.moveTo(pt);
      if (!show_off_colon && erase) {
	p.setBrush(fgcolor);
        brush.setColor(fgcolor);
      }

      break;
    case 9 :
      pt.ry() += (QCOORD)(3*Segment_Length/2 + Width/2);
      pt.rx() += (QCOORD)(Segment_Length/2 - Width/2 + 1);
     
      if (!show_off_colon && erase) {
	p.setBrush(bgcolor);
        brush.setColor(bgcolor);
      }
      p.fillRect(pt.x() ,pt.y() - Width  , Width , Width  ,brush);
      if (!show_off_colon && erase) {
	p.setBrush(fgcolor);
        brush.setColor(fgcolor);
      }
      
      break;
    }	

  } /* end smallLED */


}
