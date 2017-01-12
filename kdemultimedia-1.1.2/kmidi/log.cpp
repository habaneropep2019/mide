/*
 *        kmidi
 *
 * $Id$
 *            Copyright (C) 1997  Bernd Wuebben
 *                 wuebben@math.cornel.edu 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <kapp.h>
#include "log.moc"


myMultiEdit::myMultiEdit(QWidget *parent, const char *name)
  : QMultiLineEdit(parent, name){
}

myMultiEdit::~myMultiEdit(){
}

void myMultiEdit::insertChar(char c){
  QMultiLineEdit::insertChar( c);
}

void myMultiEdit::newLine(){
  QMultiLineEdit::newLine();
}



bool  myMultiEdit::rowYPos(int row, int& yPos){

  return QMultiLineEdit::rowYPos(row,&yPos);
}

LogWindow::LogWindow(QWidget *parent, const char *name)
  : QDialog(parent, name, FALSE)
{
  setCaption(i18n("Info Window"));


  text_window = new myMultiEdit(this,"logwindow");
  text_window->setGeometry(2,5,500, 300);
  text_window->setFocusPolicy ( QWidget::NoFocus );


  dismiss = new QPushButton(this,"dismissbutton");
  dismiss->setGeometry(330,340,70,30);
  dismiss->setText(i18n("Dismiss"));

  connect(dismiss,SIGNAL(clicked()),SLOT(hideit()));
 
  stringlist = new QStrList(TRUE); // deep copies
  stringlist->setAutoDelete(TRUE);

  adjustSize();
  setMinimumSize(width(),height());
  
  sltimer = new QTimer(this);
  connect(sltimer,SIGNAL(timeout()),this,SLOT(updatewindow()));
  timerset = false;

}


LogWindow::~LogWindow() {
}


void LogWindow::hideit(){

  this->hide();

}

void LogWindow::updatewindow(){

  timerset = false;


  //printf("in updatewindow\n");

  if (stringlist->count() != 0){

    /*    printf("inserting .. %d\n", stringlist->count());*/
    text_window->setAutoUpdate(FALSE);
    
    for(stringlist->first();stringlist->current();stringlist->next()){
      /* after a string starting with "~", don't start a new line --gl */
	static int tildaflag = 0;
	static int line = 0, col = 0;
	int futuretilda, len;
	char *s = stringlist->current();
	if (*s == '~') {
	    futuretilda = 1;
	    s++;
	}
	else futuretilda = 0;
	len = strlen(s);
	if (tildaflag && len) {
	    text_window->insertAt(s, line, col);
	    col += len;
	}
	else {
	    line++;
	    text_window->insertLine(s,line);
	    col = len;
	}
	tildaflag = futuretilda;
    }

    text_window->setAutoUpdate(TRUE);
    text_window->setCursorPosition(text_window->numLines(),0,FALSE);

    int y1  = -1;
    int y2  = -1;

    int templine,tempcol;
    
    text_window->getCursorPosition(&templine,&tempcol);	

    bool visible1, visible2;

    visible1 = text_window->rowYPos(templine ,y1);
    visible2 = text_window->rowYPos(templine +1,y2);
    
    if(y1 == -1)
      y1 = 0;
    
    if(y2 == -1)
      y2 = text_window->height();
    
    text_window->repaint(0,y1,text_window->width(),y2);

    stringlist->clear();

  }

}

void LogWindow::insertStr(char* str){


  QString string = str;

  if(string.find("Lyric:",0,TRUE) != -1)
    return;
  
  if(string.find("MIDI file",0,TRUE) != -1){
    stringlist->append(" ");
  }
 
// This makes it difficult to keep track of the cursor when
// a lyric contains a ';' (and also lies about the lyric). --gl
  //--gl int index;
  //  char newl = '\n';
  //--gl if((index = string.find(";",0,TRUE)) != -1){
  //--gl   string.replace(index,1,"\n");
  //--gl }



  stringlist->append(string.data());
 
  if(!timerset){
    sltimer->start(10,TRUE); // sinlge shot TRUE
    timerset = true;
  }

}
void LogWindow::clear(){

  if(text_window){
    
    text_window->clear();

  }

}
		  
void LogWindow::insertchar(char c) {

  QString stuff;

  if(c == '\r' || c == '\n') {
    if(c == '\n') {
      text_window->newLine();
    } 
  }
  else{
    text_window->insertChar(c);
  }
}


void LogWindow::statusLabel(const char* ) {

  /*  statuslabel->setText(n);*/

}



void LogWindow::resizeEvent(QResizeEvent* ){

  int w = width() ;
  int h = height();

  text_window->setGeometry(0,5,w  ,h - 42);
  dismiss->setGeometry(w - 72 , h - 32, 70, 30);
  
}

void LogWindow::enter() {

  char character[3] = "\r\n";

  text_window->append(character);

}

