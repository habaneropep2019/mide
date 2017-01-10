// KDat - a tar-based DAT archiver
// Copyright (C) 1998  Sean Vyain, svyain@mail.tds.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef _LoggerWidget_h_
#define _LoggerWidget_h_

#include <qwidget.h>

class QMultiLineEdit;

/**
 * @short A titled logging widget with a save option.
 */
class LoggerWidget : public QWidget {
    Q_OBJECT
    QMultiLineEdit* _mle;
public:
    /**
     * Create a titled logging widget
     *
     * @param title  The title text displayed above the logging window.
     * @param parent The parent widget.
     * @param name   The name of this widget.
     */
    LoggerWidget( const char* title, QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the logging widget.
     */
    ~LoggerWidget();

    /**
     * Append the given text to the end of the logging widget, and position the
     * viewable area at the end of the text.
     *
     * @param text The text to append.
     */
    void append( const char* text );
public slots:
    /**
     * Prompt the user for a file name, and save the contents of the log to
     * that file.
     */
    void save();
};

#endif
