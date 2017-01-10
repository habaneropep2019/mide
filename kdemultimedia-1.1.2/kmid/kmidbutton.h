#include <qpushbt.h>
#include <qpainter.h>

class KMidButton : public QPushButton
{
protected:
    
    QPixmap *pixmap1,*pixmap2;

    virtual void drawButton(QPainter *paint)
    {
        if ((isOn())&&(pixmap1!=NULL)) paint->drawPixmap(0,0,*pixmap1);
        else if ((!isOn())&&(pixmap2!=NULL)) paint->drawPixmap(0,0,*pixmap2);
    };
    
public:
    
    KMidButton (QWidget *parent,char *name) : QPushButton (parent,name)
    {
        pixmap1=NULL;
        pixmap2=NULL;
    };
    
    ~KMidButton()
    {
        delete pixmap1;
        delete pixmap2;
    };
    
    void setPixmaps(QPixmap *p1,QPixmap *p2)
    {
        pixmap1=new QPixmap(*p1);
        pixmap2=new QPixmap(*p2);
    };
    
};