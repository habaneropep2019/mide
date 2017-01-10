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

#include "config.h"

#include <kapp.h>
#include <kmenubar.h>
#include <kmsgbox.h>
#include <kstatusbar.h>
#include <ktopwidget.h>
#include <qaccel.h>
#include <qfile.h>
#include <qpopmenu.h>
#include <qpushbt.h>
#include <qtstream.h>

#include "ksmiletris.h"
#include "gamewindow.h"
#include "gamewidget.h"
#include "scoredialog.h"
#include "newscoredialog.h"

const int default_width = 362;
const int default_height = 460;

GameWindow::GameWindow(QWidget *, const char *name)
        : KTopLevelWidget(name)
{
	setCaption(i18n("Smiletris"));

	file_popup = new QPopupMenu();
	file_popup->insertItem(i18n("&New Game"), this, SLOT(menu_newGame()), CTRL+Key_N);
        pauseID = file_popup->insertItem(i18n("&Pause"), this, SLOT(menu_pause()), Key_F2);
        file_popup->insertItem(i18n("&End Game"), this, SLOT(menu_endGame()), CTRL+Key_E);
	file_popup->insertSeparator();
        file_popup->insertItem(i18n("&High Scores..."), this, SLOT(menu_highScores()));
	file_popup->insertSeparator();
	file_popup->insertItem(i18n("&Quit"), qApp, SLOT(quit()), CTRL+Key_Q);

	pieces_popup = new QPopupMenu();
	smilesID = pieces_popup->insertItem(i18n("&Smiles"), this, SLOT(menu_smiles()));
	symbolsID = pieces_popup->insertItem(i18n("S&ymbols"), this, SLOT(menu_symbols()));

	iconsID = pieces_popup->insertItem(i18n("&Icons"), this, SLOT(menu_icons()));

	options_popup = new QPopupMenu();
	options_popup->insertItem(i18n("&Pieces"), pieces_popup);
	options_popup->insertSeparator();
	soundsID = options_popup->insertItem(i18n("&Sounds"), this, SLOT(menu_sounds()));

	QString about;
	about.sprintf(i18n(
			 "KSmiletris %s\n\n"
			 "by Sandro Sigala <ssigala@globalnet.it>\n\n"
			 "Feedbacks are welcome!\n"), VERSION);
	help_popup = kapp->getHelpMenu(TRUE, about);

	menu = new KMenuBar(this);
	menu->insertItem(i18n("&File"), file_popup);
	menu->insertItem(i18n("&Options"), options_popup);
	menu->insertSeparator();
	menu->insertItem(i18n("&Help"), help_popup);
	setMenu(menu);
	connect(menu, SIGNAL(moved(menuPosition)),
		this, SLOT(movedMenu(menuPosition)));

	status = new KStatusBar(this);
	status->insertItem(i18n("Level: 99"), 1);
	status->insertItem(i18n("Score: 999999"), 2);
	setStatusBar(status);
	status->changeItem("", 1);
	status->changeItem("", 2);

	game = new GameWidget(this);
	setView(game);
	connect(game, SIGNAL(changedStats(int, int)),
		this, SLOT(updateStats(int, int)));
	connect(game, SIGNAL(gameOver()), this, SLOT(gameOver()));

	resize(default_width, default_height + menu->height() + status->height());
	setMaximumSize(width(), height());
	setMinimumSize(width(), height());

	// Read configuration
	KConfig *config = kapp->getConfig();
	config->setGroup("Options");
	PiecesType pieces_type = (PiecesType)config->readNumEntry("Pieces", static_cast<int>(Pieces_Smiles));
	game->setPieces(pieces_type);
	switch (pieces_type) {
	case Pieces_Smiles:
		pieces_popup->setItemChecked(smilesID, true);
		break;
	case Pieces_Symbols:
		pieces_popup->setItemChecked(symbolsID, true);
		break;
	case Pieces_Icons:
		pieces_popup->setItemChecked(iconsID, true);
	}

	game->do_sounds = config->readBoolEntry("Sounds", true);
	options_popup->setItemChecked(soundsID, game->do_sounds);
}

void GameWindow::menu_newGame()
{
	file_popup->setItemChecked(pauseID, false);
	game->newGame();
}

void GameWindow::menu_pause()
{
	if (game->in_game) {
		game->in_pause = !game->in_pause;
		file_popup->setItemChecked(pauseID, game->in_pause);
		game->repaintChilds();
	}
}

void GameWindow::menu_endGame()
{
	if (game->in_game) {
		game->in_game = false;
		game->repaintChilds();
		file_popup->setItemChecked(pauseID, false);
		gameOver();
	}
}

void GameWindow::menu_highScores()
{
	ScoreDialog d(this);
	d.exec();
}

void GameWindow::menu_smiles()
{
	options_popup->setItemChecked(smilesID, true);
	options_popup->setItemChecked(symbolsID, false);
	options_popup->setItemChecked(iconsID, false);
	KConfig *config = kapp->getConfig();
	config->setGroup("Options");
	config->writeEntry("Pieces", static_cast<int>(Pieces_Smiles));
	game->setPieces(Pieces_Smiles);
}

void GameWindow::menu_symbols()
{
	options_popup->setItemChecked(smilesID, false);
	options_popup->setItemChecked(symbolsID, true);
	options_popup->setItemChecked(iconsID, false);
	KConfig *config = kapp->getConfig();
	config->setGroup("Options");
	config->writeEntry("Pieces", static_cast<int>(Pieces_Symbols));
	game->setPieces(Pieces_Symbols);
}

void GameWindow::menu_icons()
{
	options_popup->setItemChecked(smilesID, false);
	options_popup->setItemChecked(symbolsID, false);
	options_popup->setItemChecked(iconsID, true);
	KConfig *config = kapp->getConfig();
	config->setGroup("Options");
	config->writeEntry("Pieces", static_cast<int>(Pieces_Icons));
	game->setPieces(Pieces_Icons);
}

void GameWindow::menu_sounds()
{
	game->do_sounds = !game->do_sounds;
	options_popup->setItemChecked(soundsID, game->do_sounds);
	KConfig *config = kapp->getConfig();
	config->setGroup("Options");
	config->writeEntry("Sounds", game->do_sounds);
}

void GameWindow::updateStats(int level, int points)
{
	QString l, p;
	l.setNum(level);
	p.setNum(points);
	status->changeItem(QString("Level: ") + l, 1);
	status->changeItem(QString("Score: ") + p, 2);
}

void GameWindow::gameOver()
{
	status->changeItem("", 1);
	status->changeItem("", 2);

	KConfig *config = kapp->getConfig();
	config->setGroup("High Score");
	QString s, num, level, score, name;
	int i;
	for (i = 1; i <= 10; ++i) {
		num.setNum(i);
		s = "Pos" + num + "Score";
		score = config->readEntry(s, 0);
		if (game->num_points > score.toInt())
			break;
	}
	if (i <= 10) {
		NewScoreDialog d(this);
		if (d.exec()) {
			for (int j = 10; j > i; --j) {
				num.setNum(j - 1);
				s = "Pos" + num + "Level";
				level = config->readEntry(s, "0");
				s = "Pos" + num + "Score";
				score = config->readEntry(s, "0");
				s = "Pos" + num + "Name";
				name = config->readEntry(s, "Noname");

				num.setNum(j);
				s = "Pos" + num + "Level";
				config->writeEntry(s, level);
				s = "Pos" + num + "Score";
				config->writeEntry(s, score);
				s = "Pos" + num + "Name";
				config->writeEntry(s, name);
			}

			num.detach();
			num.setNum(i);
			s = "Pos" + num + "Level";
			level.detach();
			level.setNum(game->num_level);
			config->writeEntry(s, level);
			s = "Pos" + num + "Score";
			score.setNum(game->num_points);
			config->writeEntry(s, score);
			s = "Pos" + num + "Name";
			config->writeEntry(s, d.name());

			ScoreDialog h(this);
			h.exec();
		}
	}
}

void GameWindow::movedMenu(menuPosition pos)
{
	if (pos == Top) {
		setMinimumSize(default_width, default_height + menu->height() + status->height());
		setMaximumSize(default_width, default_height + menu->height() + status->height());
		resize(width(), height() + menu->height());
	} else {
		setMinimumSize(default_width, default_height + status->height());
		setMaximumSize(default_width, default_height + status->height());
		resize(width(), height() - menu->height());
	}
}
