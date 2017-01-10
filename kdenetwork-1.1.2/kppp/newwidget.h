/////////////////////////////////////////////////////////////////////////////
//
// functions generating layout-aware widgets
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __NEWWIDGET__H__
#define __NEWWIDGET__H__

#include <qwidget.h>
#include <qlabel.h>
#include <qradiobt.h>
#include <qchkbox.h>
#include <qlined.h>

#define L_FIXEDW 1
#define L_FIXEDH 2
#define L_FIXED  (L_FIXEDW | L_FIXEDH)

QLabel *newLabel(const char *text, QWidget *parent);
QRadioButton *newRadioButton(const char *text, QWidget *parent);
QCheckBox *newCheckBox(const char *text, QWidget *parent);
QLineEdit *newLineEdit(int visiblewidth, QWidget *parent);

#endif
