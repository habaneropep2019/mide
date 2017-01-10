/****************************************************************
Copyright (c) 1998 Sandro Sigala <ssigala@globalnet.it>.
All rights reserved.

Permission to use, copy, modify, and distribute this software
and its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name of the author not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

The author disclaim all warranties with regard to this
software, including all implied warranties of merchantability
and fitness.  In no event shall the author be liable for any
special, indirect or consequential damages or any damages
whatsoever resulting from loss of use, data or profits, whether
in an action of contract, negligence or other tortious action,
arising out of or in connection with the use or performance of
this software.
****************************************************************/

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <ktopwidget.h>

#include "ksmiletris.h"

class KMenuBar;
class QPopupMenu;
class GameWidget;
class KStatusBar;

class GameWindow : public KTopLevelWidget {
	Q_OBJECT

public:
	GameWindow(QWidget *parent=0, const char *name=0);

private:
	enum menuPosition { Top, Bottom, Floating };	// hack
	KMenuBar *menu;
	QPopupMenu *file_popup, *pieces_popup, *options_popup, *help_popup;
	int pauseID, smilesID, symbolsID, iconsID, soundsID;
	KStatusBar *status;
	GameWidget *game;

public slots:
	void menu_newGame();
	void menu_pause();
	void menu_endGame();
	void menu_highScores();
	void menu_smiles();
	void menu_symbols();
	void menu_icons();
	void menu_sounds();

	void updateStats(int level, int points);
	void gameOver();
	void movedMenu(menuPosition);
};

#endif // !GAMEWINDOW_H
