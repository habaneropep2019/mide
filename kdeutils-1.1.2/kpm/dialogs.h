// dialogs.h			emacs, this is a -*-c++-*- file
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

// misc. handy dialogs for use everywhere

#ifndef DIALOGS_H
#define DIALOGS_H

#include <qdialog.h>
#include <qlined.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qlayout.h>

// ValueDialog: modal dialog for input of a single value

class ValueDialog : public QDialog {
    Q_OBJECT
public:
    ValueDialog(const char *caption, const char *msg, const char *ed_txt);

protected slots:
    void done_dialog();

public:
    QString ed_result;

protected:
    QPushButton *ok, *cancel;
    QLabel *label;
    QLineEdit *lined;
};

// SliderDialog: value dialog with additional slider

class SliderDialog : public QDialog {
    Q_OBJECT
public:
    SliderDialog(const char *caption, const char *msg, int defaultval,
		 int minval, int maxval);

protected slots:
    void done_dialog();
    void slider_change(int val);

public:
    QString ed_result;

protected:
    QSlider *slider;
    QPushButton *ok, *cancel;
    QLabel *label;
    QLineEdit *lined;
};

// MessageDialog: modal dialog that just gives a message
// (like QMessageBox but prettier)

class MessageDialog : public QDialog {
    Q_OBJECT
public:
    MessageDialog(QWidget *parent = 0);

    static void message(const char *caption, const char *text,
			const char *buttonText = 0, QPixmap *icon = 0);
    static QPixmap *warningIcon();

    void setText(const char *text);
    void setButtonText(const char *buttonText);
    void setIcon(QPixmap *pixmap);

    void adjustSize();

private:
    // geometry constants:
    static const int button_minwidth;
    static const int margin_top;		// above text
    static const int margin_middle;	// between text and button
    static const int margin_bottom;	// below button
    static const int margin_side;		// to the left and right of text

    QLabel *label;
    QPushButton *button;
    QLabel *icon;

    static QPixmap *warn_icon;
};

#endif	// DIALOGS_H
