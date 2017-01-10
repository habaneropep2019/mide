/**************************************************************************

    kdisptext.h  - The widget that displays the karaoke/lyrics text
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include <qwidget.h>
#include "player/player.h"

struct kdispt_ev 
{
    SpecialEvent *spev;
    int xpos;
    int width;
    kdispt_ev *next;
};

struct kdispt_line
{
    kdispt_ev *ev;
    int num;
    int ypos;
    kdispt_line *next;
};

class QFont;
class QPainter;
class QFontMetrics;
class QScrollBar;

class KDisplayText : public QWidget
{
    Q_OBJECT
private:
    QScrollBar *textscrollbar;
    
    
    QFontMetrics *qfmetr;
    //const QFont qtextfont(const "courier",18);
    QFont *qtextfont;
    QPainter *qpaint;
    
    int typeoftextevents;
    
    kdispt_line *linked_list_[2];
    int nlines_[2];
    kdispt_line *first_line_[2];
    kdispt_line *cursor_line_[2];
    kdispt_ev *cursor_[2];
    
    kdispt_line *linked_list; 
    kdispt_line *cursor_line;
    kdispt_ev *cursor;
    
    kdispt_line *first_line; // Pointer to first text at first visible line
    
    int nlines;     // Total number of lines
    int nvisiblelines; // Number of visible lines
    int font_height; // Height of the font
    
    void RemoveLine(kdispt_line *tmpl);
    void RemoveLinkedList(void);
    
    int IsLineFeed(char c,int type=0);
    virtual void paintEvent(QPaintEvent *qpe);
    void drawFrame(void);
    
protected:
    void resizeEvent(QResizeEvent *qre);
    
public:
    KDisplayText(QWidget *parent,const char *name);
    ~KDisplayText();
    
    void PreDestroyer(void);
    void ClearEv(bool totally=true);
    void AddEv(SpecialEvent *ev);
    void calculatePositions(void);
    
    void CursorToHome(void);
    
    void ChangeTypeOfTextEvents(int type); 
    int ChooseTypeOfTextEvents(void);
    void PaintIn(int type);
    void gotomsec(ulong i);
    
    QFont *getFont(void);
    void fontChanged(void);
    
    void saveLyrics(FILE *fh);

public slots:
    void ScrollText(int i);
    void ScrollDown();
    void ScrollUp();
    void ScrollPageDown();
    void ScrollPageUp();
    
};
