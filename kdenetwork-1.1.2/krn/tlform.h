#ifndef TL_FORM_H
#define TL_FORM_H

#include <qlayout.h>
#include <qlist.h>

#include "tlwidget.h"

class KTypeLayout;

class TLForm: public TLObj
{
public:

    TLForm(){};
    TLForm(const char *ID,const char *title,QWidget *_widget=0,QWidget *_parent=0);
    virtual ~TLForm ();
    KTypeLayout *layout;
private:
};

class TLBook: public TLForm
{
public:
    TLBook(const char *ID,QWidget *_widget=0);
};

#endif