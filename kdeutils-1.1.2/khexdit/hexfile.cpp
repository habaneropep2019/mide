/*
 *   khexdit - a little hex editor
 *   Copyright (C) 1996,97,98  Stephan Kulow
 *
 */


#include "hexfile.h"
#include <qfile.h> 
#include <stdlib.h>

#include <kapp.h>
#include <kdebug.h>
#include <kstring.h>

#include "hexdata.h"
#include "hexfile.moc"

#include <qclipbrd.h> 
#include <qfontmet.h> 
#include <qkeycode.h>
#include <qmsgbox.h> 
#include <qpainter.h>
#include <qpixmap.h>
#include <qwidget.h>

static QColor background(220,220,200); 
static QColor selectColor(180,180,180);
static QBrush RedMarker(QColor(0xff,0,0));
static QBrush GreenMarker(QColor(0xe0,0xff,0xe0));

static QPoint nullPoint(-1,-1);

HexFile::HexFile(const char *filename,QWidget *parent, const char* name) :  
    QWidget(parent,name)
{
    init();
    open(filename);
}

HexFile::HexFile(QWidget *parent)  : QWidget(parent,"HexFile") {
    init();
    filename=new char[20];
    strcpy(filename,i18n("Untitled"));
}

void HexFile::init() {
    UseBig = false;
    sideEdit = LEFT;
    datamap=new QPixmap();
    dispFont=new QFont("courier", 14);
    filename = 0L;
    setFont(*dispFont);
    setBackgroundColor( background );
    metrics = new QFontMetrics(fontMetrics());
    cursorHeight = metrics->ascent();
    lineHeight = metrics->height() + metrics->underlinePos() + 2;
    maxWidth = 0;
    rows = 0;
    draging = false;
    relcur = curx = cury = 0;
    char lineB[] = "ABCDEF0123456789";
    char lineL[] = "abcdef0123456789";
    for (size_t i=0;i<strlen(lineB);i++) {
	char t = (UseBig) ? lineB[i] : lineL[i];
	if (maxWidth < metrics->width(t))
	    maxWidth = metrics->width(t);
    }
    data = new HexData();
    currentByte = 0;
    cursorPosition = 0;
    LineOffset = maxWidth*49+10;
    labelOffset = 0;
    curx = cury = horoff=lineoffset=0;
    setFocusPolicy(QWidget::TabFocus);
    leftM = &RedMarker;
    rightM = &GreenMarker;
    modified = false;
    emit unsaved( false );
    scrollV=new QScrollBar(QScrollBar::Vertical,this);
    scrollV->show();
    scrollH=new QScrollBar(QScrollBar::Horizontal,this);
    scrollH->show();
    minDrag = &nullPoint;
    maxDrag = &nullPoint;
    show();
}

HexFile::~HexFile() {
    delete dispFont;
    delete datamap;
    delete metrics;
    delete scrollV;
    delete scrollH;
    delete [] filename;
}

const char* HexFile::Title() {
    return filename;
}

int HexFile::save() {
    data->save(filename);
    modified = false;
    emit unsaved( false );
    return 0;
}

void HexFile::setFileName(const char *Filename) {
    delete [] filename;
    filename = new char[strlen(Filename)+1];
    strcpy(this->filename, Filename);
}

bool HexFile::open(const char *Filename) {
    if (data->load(Filename) < 0)
	return false;

    delete [] filename;
    filename = new char[strlen(Filename)+1];
    strcpy(this->filename,Filename);
    lineoffset = 0;
    curx = 0;
    cury = 0;
    calcScrolls();
    fillPixmap();
    repaint( false );
    modified = false;
    emit unsaved( false );
    return true;
}

void HexFile::calcScrolls() {
    scrollV->setRange(0,maxLine());
    scrollV->setSteps(1,lines());
    scrollV->setValue(lineoffset/16);
}

int HexFile::maxLine() {
    int ml=(data->size() / 16) - rows + 3;
    return ((ml < 0) ? 0 : ml);
}

int HexFile::lines() {
    return rows;
}

void HexFile::scrolled(int line) {
    static int old_x = -1 , old_y = -1;
    int offset = line*16;
    if (lineoffset == offset && old_y == cury && old_x == curx) 
	return;
    lineoffset=offset;
    old_x = curx;
    old_y = cury;
    if (!datamap->isNull()) {
	fillPixmap();
	repaint( false );
    }
}

void HexFile::moved(int value) {
    horoff=value;
    repaint( false );
}

int HexFile::NormWidth() {
    return maxWidth*65+25;
}

int HexFile::HorOffset() {
    return horoff;
}

const char *HexFile::FileName() {
    return filename;
}

int hexvalue(int key) {
    if (key>='a' && key<='f')
	return key-'a'+10;
    if (key>='A' && key<='F')
	return key-'A'+10;
    return key-'0'; 
}

void HexFile::keyPressEvent (QKeyEvent* e) {
    int ox=curx;
    int oy=cury;
    int new_lineoffset = lineoffset;
    int oldl=lineoffset;
    bool changed = false;
    int oldr=relcur;
    int key = e->ascii();
    if (sideEdit == LEFT) {
	if ((key>='a' && key<='f') || 
	    (key>='A' && key<='F') || 
	    (key>='0' && key<='9')) {
	    
	    int r = data->byteAt(cury*16+curx+lineoffset);
	    
	    if (relcur) 
		data->changeByte(cury*16+curx+new_lineoffset, 
				 r & 0xf0 | hexvalue(key));
	    else 
		data->changeByte(cury*16+curx+new_lineoffset,
				 r & 0x0f | hexvalue(key) << 4);
	    modified = changed = true;
	    emit unsaved( true );
	    relcur++;
	    if (relcur==2) {
		relcur = 0;
		curx++;
	    }
	} else
	    relcur = 0;
    } else if (key && e->key()<0x100) {
	data->changeByte(cury*16+curx+new_lineoffset, key);
	modified = changed = true;
	emit unsaved( true );
	curx++;
    }
    key = e->key();
    switch (key) {
    case Key_Right:
	curx++;
	break;
    case Key_Left:
	curx--;
	break;
    case Key_Up:
	cury--;
	break;
    case Key_Down:
	cury++;
	break;
    case Key_Next:
	scrollV->addPage();
	break;
    case Key_Prior:
	scrollV->subtractPage();
	break;
    case Key_Tab:
    case Key_Backtab:
	changeSide();
	changed=true;
	break;
    }
    if (curx<0) {
	if (new_lineoffset+cury>0) {
	    curx=15;
	    cury--;
	} else 
	    curx=0;
    }
    if (curx>15) {
	curx=0;
	cury++;
    }
    if (cury<0) {
	cury=0;
	if (new_lineoffset>=16) {
	    new_lineoffset-=16;
	}
    }
    if (cury>=rows) {
	cury=rows-1;
	if (new_lineoffset/16<maxLine()) {
	    new_lineoffset +=16;
	}
	lineoffset = new_lineoffset;
	kdebug(KDEBUG_INFO, 1501, "copy");
	QPixmap *pixmap = new QPixmap(datamap->size());
	QPainter painter;
	painter.begin(pixmap);
	painter.drawPixmap(0, 0, *datamap, 0, lineHeight);
	painter.setFont(*dispFont);
       	painter.fillRect(0, pixmap->height() - lineHeight + 1, 
			 width(), lineHeight, 
			 QColor(255,0,0));
	fillLine(&painter, rows);
	fillLine(&painter, rows + 1);
	painter.drawLine(LineOffset,0,LineOffset,height());
	delete datamap;
	datamap = pixmap;
	painter.end();
	repaint(false);
	return;
    }

    if ((unsigned)(curx + new_lineoffset + cury *16) >= data->size()) {
	curx = ox;
	cury = oy;
	new_lineoffset = oldl;
	relcur = oldr;
    }

    if (new_lineoffset != oldl) {
	scrolled(new_lineoffset/16);
	return;
    }

    if (curx != ox || cury != oy || changed || oldr!=relcur) {
	if (changed)
	    fillPixmap();
	repaint( false );
	return;
    }
    
    e->ignore();
}

void HexFile::paintCursor(QPainter *p) 
{
    if (currentByte >= data->size())
	return;
    
    char number[4];
    char hilight[3];
    int w = calcPosition(curx);
    calcCurrentByte(); // just to be sure
    // debug("paintCursor %d %d %ld %x",w, cursorPosition, currentByte, hexdata[currentByte]);
    int offw = 0;

    if (UseBig)
	sprintf(number,"%02X",data->byteAt(currentByte));
    else 
	sprintf(number,"%02x",data->byteAt(currentByte));

    if (sideEdit == LEFT) {
	p->setPen(QColor(0xff,0xff,0xff));
	offw = maxWidth*relcur;
	
	hilight[0] = number[relcur];
	hilight[1] = 0;
    } else {
	p->setPen(QColor(0x0,0x0,0x0));
	hilight[0] = number[0];
	hilight[1] = number[1];
	hilight[2] = 0;
    }

    p->fillRect(w + offw - horoff, cursorPosition - cursorHeight ,
	       (1 + (sideEdit==RIGHT))*maxWidth,
	       cursorHeight + metrics->underlinePos(),
	       *leftM);
    p->drawText(w+offw - horoff,cursorPosition - 2, hilight);

    p->setPen(QColor(0xff,0x0,0));
    p->fillRect(LineOffset + 10 + curx*maxWidth - horoff,
		cursorPosition - lineHeight + 2,
		maxWidth,
		lineHeight - 2,
		*rightM);
    
    if (data->byteAt(currentByte) > 31)
	hilight[0] = data->byteAt(currentByte);
    else 
	hilight[0] = '.';
    hilight[1] = 0;

    p->setPen(QColor(0x20,0x20,0x80));
    p->drawText(LineOffset + 10 + curx * maxWidth - horoff, 
	  cursorPosition - 2, hilight);
}

void HexFile::changeSide() {
    sideEdit = (Side)(LEFT + RIGHT - sideEdit);
    QBrush *tmp=rightM;
    rightM = leftM;
    leftM = tmp;
}

void HexFile::calcCurrentByte() {
    currentByte = lineoffset + cury * 16 + curx;
    // debug("calcCurrentByte %ld %d %d %ld",lineoffset, curx, cury, currentByte);
}

QPoint HexFile::translate(QPoint pos) {
    int neux ,neuy, mx ,cx;
    neuy = pos.y() / lineHeight;
    neux = 0;
    
    mx = pos.x();
    if (mx < LineOffset) {
	cx = labelOffset;
	while ( (mx - cx) > 4*maxWidth && neux < 14) {
	    neux += 2;
	    cx += 5*maxWidth;
	}
	if (mx - cx > 2*maxWidth)
	    neux++;
	return QPoint(neux, neuy);
    } else {
	cx = LineOffset + 10;
	while (mx > cx && neux < 16) {
	    char r = data->byteAt(neuy*16+neux+lineoffset);
	    if (r<32)
		r='.';
	    cx += metrics->width(r);
	    neux++;
	}
	if (neux)
	    neux--;
	return QPoint(neux + 16, neuy);
    }
}

void HexFile::mouseReleaseEvent (QMouseEvent *e) {

    QPoint pos = translate(e->pos());
    curx = pos.x();
    cury = pos.y();
    
    if (pos.x() >= 16) {
	curx-=16;
	if (sideEdit != RIGHT)
	    changeSide();
    } else {
	if (sideEdit != LEFT)
	    changeSide();
    }
    repaint( false );
}

void HexFile::mousePressEvent (QMouseEvent *e) {

    startDrag = translate(e->pos());
    if (indexOf(startDrag) > data->size()) 
	startDrag = nullPoint;

    if (draging) {
	draging = false;
	fillPixmap();
	repaint(false);
	endDrag = nullPoint;
    }
}

inline int max(int i, int j) {
    return (i>j)?i:j;
}

inline int min(int i, int j) {
    return (i<j)?i:j;
}

void HexFile::mouseMoveEvent ( QMouseEvent *e ) {
    QPoint tmp = translate(e->pos());

    if (indexOf(tmp) > data->size())
	return;

    if (tmp == endDrag)
	return;

    QPoint omin = *minDrag;
    QPoint omax = *maxDrag;
    endDrag = tmp;
    
    if (endDrag != startDrag)
	draging = true;
    

    if (startDrag.y() > endDrag.y() || 
	((startDrag.y() == endDrag.y()) && (startDrag.x() > endDrag.x()))) 
    {
	minDrag = &endDrag;
	maxDrag = &startDrag;
    } else {
	maxDrag = &endDrag;
	minDrag = &startDrag;
    }

    QPainter p;
    p.begin(datamap);
    p.setFont(*dispFont);

    if (minDrag->y() != omin.y() || minDrag->x() != omin.x()) {
	for (int y = min(minDrag->y(),omin.y());
	     y < max(minDrag->y(), omin.y()) + 1; 
	     y++)
	    fillLine(&p, y+1);
    }
    if (maxDrag->y() != omax.y() || maxDrag->x() != omax.x()) 
	for (int y = min(maxDrag->y(),omax.y()); 
	     y < max(maxDrag->y(), omax.y()) + 1; 
	     y++)
	    fillLine(&p, y+1);
    p.drawLine(LineOffset,0,LineOffset,height());
    p.end();
    curx = endDrag.x();
    cury = endDrag.y();
    repaint(false);
    copyClipBoard();
}


void HexFile::focusInEvent ( QFocusEvent *) {
    repaint( false );
}

void HexFile::focusOutEvent ( QFocusEvent *) {
    repaint(false);
}

int HexFile::paintLabel( QPainter *p, long int label, int y) {
    char offset[10];
    char txt[2] = " ";

    if (UseBig)
	sprintf(offset,"%08lX ", label);
    else
	sprintf(offset,"%08lx ", label);

    p->setPen(QColor(0,0,0));
    for (int i=0;i<9;i++) {
	// to write a single character (perhaps easier?)
	txt[0]=offset[i];
	p->drawText(5+i*maxWidth, y - 2, txt);
    }

    return 5 + 9 * maxWidth;
}

int HexFile::calcPosition( int field ) {
    cursorPosition = (cury + 1) * lineHeight;
    return field * 2 * maxWidth + labelOffset + (field / 2) * maxWidth;
}

QColor HexFile::colorPosition(int field) {
    if ((field/2) % 2)
	return QColor(0x9f,0x9f,0x20);
    else
	return QColor(0x20,0x9f,0x9f);
}

void HexFile::fillLine(QPainter *p, int line) {
    // debug("fillLine %d",line);
    int w=0, x, i;
    char txt[2]=" ";
    char number[8];
    char currentBuffer[17];
    memset(currentBuffer,' ',17);
    bool marked;

    marked = (draging && line >= minDrag->y()+1 && line <= maxDrag->y()+1); 
    
    p->fillRect(0, (line-1)*lineHeight + 1, 
		datamap->width(), lineHeight, 
		background);
    
    if (data->size()<=(unsigned)(line-1)*16+lineoffset)
	return;

    int label = paintLabel(p, (line-1)*16 + lineoffset, line*lineHeight);
    if (!labelOffset)
	labelOffset = label;

    if (marked && line != minDrag->y()+1)
	p->fillRect(label, (line-1)*lineHeight + 1,
		    LineOffset - label, lineHeight,
		    selectColor);

    for (x = 0; x < 16; x++) {
	
	unsigned int r1;

	if (data->size()<=(unsigned)(line-1)*16+x+lineoffset)
	    r1 = 0;
	else
	    r1=data->byteAt((line-1)*16+x+lineoffset);

	if (UseBig)
	    sprintf(number,"%02X",r1);
	else 
	    sprintf(number,"%02x",r1);

	p->setPen(colorPosition(x));
	
	txt[1]=0; w = calcPosition(x);
	if (marked && line == minDrag->y()+1 && x == minDrag->x()) 
	    p->fillRect(w, (line-1)*lineHeight + 1,
			LineOffset - w, lineHeight,
			selectColor);

	for (i=0;i<2;i++) {
	    txt[0]=number[i];
	    p->drawText( w + i * maxWidth, line*lineHeight - 2, txt);
	}
	
	w += 2*maxWidth;

	if (marked && line == maxDrag->y()+1 && x == maxDrag->x())
	    p->fillRect(w, (line-1)*lineHeight + 1,
			LineOffset - w, lineHeight,
			background);
	
	if ((cury == line-1)  && (curx == x)) {
	    cursorPosition = line*lineHeight;
	}
	    
	if (r1>31)
	    currentBuffer[x]=r1;
	else 
	    currentBuffer[x]='.';
    }
    if (x) {
	currentBuffer[16]=0;
	p->setPen(QColor(0x20,0x20,0x80));
	txt[1]=0; 
	w = LineOffset + 10;
	int h = line * lineHeight - 2;
	
	for (i=0;i<16;i++) {
	    txt[0]=currentBuffer[i];
	    p->drawText( w + i * maxWidth, h, txt);
	}
	p->drawText(LineOffset+10, h, currentBuffer);
	
    } else return;

    return;
}

void HexFile::fillPixmap() {
    // debug("fillPixmap");
    if (!datamap || datamap->isNull())
	return;
    QPainter p(datamap);
    p.fillRect(0,0,datamap->width(), datamap->height(), 
	       QColor(220, 220, 220));
    p.setFont(*dispFont);
    
    for (int y=1; y<=rows +1 ; y++) 
	fillLine(&p, y);
	
    p.drawLine(LineOffset,0,LineOffset,height());
    calcCurrentByte();
}

void HexFile::paintEvent(QPaintEvent *p) {

    bool use_backmapping = false;

    // debug("paintEvent %d %d %d %d",p->rect().left(), p->rect().top(), p->rect().width(), p->rect().height());

    if (!datamap || datamap->isNull())
	return;

    if (p->rect().intersects(QRect(0,0,width()-scrollV->width(), 
				   height() - scrollH->height()))) {

	int width = p->rect().width();
	if (width + p->rect().left() > datamap->width() + horoff)
	    width = datamap->width() - p->rect().left();
	
	if (use_backmapping) {
	    QPixmap pixmap(*datamap);
	    QPainter painter(&pixmap);
	    paintCursor(&painter);
	    
	    bitBlt(this, p->rect().left(), p->rect().top(),
		   &pixmap, 
		   p->rect().left() + horoff, p->rect().top(),
		   width, p->rect().height(), CopyROP);
	} else {
	    bitBlt(this, p->rect().left(), p->rect().top(),
		   datamap, 
		   p->rect().left() + horoff, p->rect().top(),
		   width, p->rect().height(), CopyROP);

	    QPainter painter(this);
	    paintCursor(&painter);
	}
    }
}

ulong HexFile::indexOf(QPoint &p) {
    return lineoffset + 16 * p.y() + p.x();
}

void HexFile::copyClipBoard() {
    QString tmp;
    QClipboard *clp = QApplication::clipboard();
    int start = indexOf(*minDrag);
    int end = indexOf(*maxDrag);
    for (int i=start; i <= end; i+=2) {
	char buffer[6];
	sprintf(buffer, "%02x%02x ",data->byteAt(i), data->byteAt(i+1));
	tmp << buffer;
	if (i % 16 == 0)
	    tmp << "\n";
    }
    clp->setText(tmp);
}

void HexFile::resizeEvent(QResizeEvent *) {
    int scrollVWidth,scrollHHeight;

    if (!scrollV)
	return;
    
    scrollVWidth = scrollHHeight = 15;
    calcScrolls();
    rows = (height() - scrollHHeight) / lineHeight;
    kdebug(KDEBUG_INFO, 1501, "ROWS %d",rows);
    QObject::connect(scrollV, SIGNAL(valueChanged(int)),
		     SLOT(scrolled(int)));
    
    int diff=NormWidth()+scrollVWidth - width();
    if (diff < 0)
	diff = 0;
    scrollH->setRange(0,diff);
    scrollH->setSteps(13,diff);
    scrollH->setValue(0);
    scrollH->setGeometry(0,height()-scrollHHeight,
			 width()-scrollVWidth,scrollHHeight);
    QObject::connect(scrollH, SIGNAL(valueChanged(int)),
		     SLOT(moved(int)));
    
    scrollV->setGeometry(width()-scrollVWidth,0,
			 scrollVWidth,
			 height()-scrollHHeight);
    
    datamap->resize(maxWidth*65+25,height()-scrollHHeight);
    fillPixmap();
    if (width()>datamap->width()+scrollHHeight)
	horoff=0;
}



