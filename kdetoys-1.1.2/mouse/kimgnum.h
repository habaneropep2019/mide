#ifndef __KIMAGELABEL__
#define __KIMAGELABEL__

#include <qframe.h>

class KImageNumber : public QFrame
{
    Q_OBJECT
public:
    
    KImageNumber(QString font,QWidget * parent=0, const char * name=0);
    
    KImageNumber(QString font, unsigned int num,unsigned int precision,
		 QWidget * parent=0,const char * name=0);
    
    virtual ~KImageNumber();
	
    void paintEvent(QPaintEvent*);
    virtual QSize sizeHint() const;

    
public slots:

    void setValue(double);

protected:

    double val;
    int precision;
    int numDigits;
    QString fontFile;
    QPixmap* fontPix;
};

#endif
