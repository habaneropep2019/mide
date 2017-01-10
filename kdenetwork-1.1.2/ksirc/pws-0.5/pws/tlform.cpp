#include "tlform.h"

#include <qwidget.h>
#include <qgrpbox.h>
#include <qlist.h>

#include "typelayout.h"


TLForm::TLForm(const char *ID,const char *title,QWidget *_widget, QWidget *_parent)
    :TLObj(ID)
{
    if (!_widget)
    {
        widget=new("QWidget") QWidget(_parent,ID);
    }
    else
        widget=_widget;
    
    widget->setCaption(title);
    align=AlignCenter;
    layout=new("KTypeLayout") KTypeLayout (this);
}

TLForm::~TLForm()
{
}

TLBook::TLBook(const char *ID,QWidget *_widget=0)
{
    widget=_widget;
    align=AlignCenter;
    layout=new("KBookLayout") KBookLayout (this);
}
