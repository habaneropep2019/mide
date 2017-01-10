// -*- c++ -*-

// $Id: formatdialog.h,v 1.2 1997/08/30 18:22:37 kdecvs Exp $

#ifndef FORMATDIALOG_H
#define FORMATDIALOG_H

#include <qdialog.h>
#include <qcombo.h>
#include <qlabel.h>

class formatDialog : public QDialog
{
    Q_OBJECT

public:

    formatDialog(const char *format, QWidget* parent = NULL, const char* name = NULL);

    QComboBox* fileformat;
};

#endif // FORMATDIALOG_H
