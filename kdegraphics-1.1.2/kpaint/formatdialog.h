// -*- c++ -*-

// $Id$

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
