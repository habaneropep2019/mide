#include <stdio.h>

#include "kimgnum.h"

#include <qpixmap.h>

KImageNumber::KImageNumber(QString font,QWidget* parent,const char* name) :
    QFrame(parent,name)
{
    val=0.0;
    fontFile=font;
    fontPix=NULL;
    numDigits=-1;
    precision=-1;

    fontPix=new QPixmap(fontFile,"GIF",0);
}

KImageNumber::KImageNumber(QString font,			   
			   unsigned int num,
			   unsigned int prec,
			   QWidget* parent,const char* name) :
    QFrame(parent,name)
{
    val=0.0;
    fontFile=font;
    fontPix=NULL;
    precision=prec;
    numDigits=num;

    fontPix=new QPixmap(fontFile,"GIF",0);
}


KImageNumber::~KImageNumber()
{
    if(fontPix)
	delete fontPix;
}



void KImageNumber::paintEvent(QPaintEvent*)
{
    if(fontPix==NULL)
	fontPix=new QPixmap(fontFile,"GIF",0);

    int w=fontPix->width();
    int each=w/11;

    QString data;
    data.sprintf("%06.1f",val);

    const char* l=data;

    for(unsigned int i=0;i<strlen(l);i++) {

	int wl=l[i]-'0';
	if(l[i]=='.')
	    wl=10;
	
	int x=each*wl;
	
	bitBlt(this,i*each,0,fontPix,x,0,each,fontPix->height()); 
    }
}    

void KImageNumber::setValue(double v)
{
    val=v;
    repaint(false);
}

QSize KImageNumber::sizeHint() const
{
    if(!fontPix)
	return QSize(150,40);
    
    int w=fontPix->width();
    int each=w/11;

    QString data;
    data.sprintf("%06.1f",val);
    
    return QSize(strlen(data)*each,fontPix->height());
}

#include "kimgnum.moc"
