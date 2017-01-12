// -*- c++ -*-

// $Id$

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
