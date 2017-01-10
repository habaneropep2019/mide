// -*- c++ -*-

// $Id: infodialog.h,v 1.3 1998/01/13 19:01:57 rich Exp $

#ifndef IMAGEINFODIALOG_H
#define IMAGEINFODIALOG_H

#include <qdialog.h>
#include <qcombo.h>
#include <qlabel.h>
#include "canvas.h"

class imageInfoDialog : public QDialog
{
    Q_OBJECT

public:

    imageInfoDialog(Canvas *c, QWidget* parent= 0, const char* name= 0);

private:
    QLabel* colourDepth;
    QLabel* coloursUsed;
    QLabel* width;
    QLabel* height;
};

#endif // IMAGEINFODIALOG_H
