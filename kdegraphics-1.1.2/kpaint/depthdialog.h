// -*- c++ -*-

// $Id: depthdialog.h,v 1.2 1998/10/18 19:59:45 habenich Exp $

#ifndef DEPTHDIALOG_H
#define DEPTHDIALOG_H

#include <qdialog.h>
#include <qcombo.h>
#include <qlabel.h>
#include "canvas.h"

// Tags for Colordepth
#define ID_COLOR_1 0
#define ID_COLOR_4 1
#define ID_COLOR_8 2
#define ID_COLOR_15 3
#define ID_COLOR_16 4
#define ID_COLOR_24 5
#define ID_COLOR_32 6

class depthDialog : public QDialog
{
    Q_OBJECT

public:
    depthDialog(Canvas *, QWidget *parent= 0, const char *name= 0);
    QComboBox *depthBox;
};

#endif // DEPTHDIALOG_H
