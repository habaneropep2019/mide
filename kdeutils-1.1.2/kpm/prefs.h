// prefs.h				emacs, this is written in -*-c++-*-
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997, 1998

#ifndef PREFS_H
#define PREFS_H

#include <qdialog.h>
#include <qradiobt.h>
#include <qlabel.h>
#include <qbttngrp.h>

class Preferences : public QDialog {
    Q_OBJECT
public:
    Preferences();

public slots:
    void update_boxes();
    void update_reality(int);
    void closed();

signals:
    void prefs_change();
};

#endif	// PREFS_H
