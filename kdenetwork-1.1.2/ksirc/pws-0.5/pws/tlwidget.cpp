#include <stdio.h>

#include <qstack.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qlined.h>
#include <qbttngrp.h>
#include <qpixmap.h>
#include <qchkbox.h>
#include <qframe.h>
#include <qcombo.h>
#include <qpainter.h>

#include <htmlview.h>
#include <html.h>

#include "tlwidget.h"
#include "tlform.h"
#include "qdict.h"



TLObj::TLObj()
{
    align=0;
    width=1;
    height=1;
    vslack=0;
    hslack=0;
    ID="";
}

TLObj::TLObj(const char *_ID)
{
    align=0;
    width=1;
    height=1;
    vslack=0;
    hslack=0;
    ID=_ID;
}