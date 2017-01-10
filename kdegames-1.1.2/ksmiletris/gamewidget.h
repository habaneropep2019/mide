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

#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <qwidget.h>

#include "ksmiletris.h"

class ScreenWidget;
class MirrorWidget;
class NextPieceWidget;
class QTimer;
class KAudio;

class GameWidget : public QWidget {
	Q_OBJECT

signals:
	void changedStats(int, int);
	void gameOver();

public:
	bool in_game, in_pause;
	bool do_sounds;
	int num_level;
	int num_points;

	GameWidget(QWidget *parent=0, const char *name=0);
	~GameWidget();

	void setPieces(PiecesType type);

	Sprite& ref(int x, int y) {
		return map[y*scr_width + x];
	}

	Sprite type(Sprite s) {
		if (s >= Sprite_Broken1 && s < Sprite_Broken1 + num_blocks)
			return (Sprite)(Sprite_Block1 + s - Sprite_Broken1);
		return s;
	}

	void repaintChilds();

private:
	KAudio *audio;
	QPixmap *sprites;
	ScreenWidget *screen;
	MirrorWidget *mirror;
	NextPieceWidget *next;
	int xpos, ypos;
	Sprite piece[4];
	Sprite *map;
	Sprite *mirror_sprites;
	Sprite next_piece[4];
	Sprite bg_sprite;
	int timer_interval;
	bool fast_mode;
	QTimer *timer;

	int num_pieces_level;

	void playSound(Sound s);
	void loadSprites();
	void loadSprite(Sprite spr, const char *path);
	void loadMaskedSprite(Sprite spr, char *path1, const char *path2);
	void newBlock();
	void putPiece();
	void getPiece();
	void newPiece();
	void nextPiece();
	void compact();
	void broke(int x, int y, bool *xmap);
	void recBroke(int x, int y, bool *xmap);
	void checkSolePiece(int x, int y, bool *xmap);
	void checkPiece(int x, int y);
	void blockPiece();
	void updateMirror();

public slots:
	void newGame();
	void keyUp();
	void keyDown();
	void keyLeft();
	void keyRight();
	void keySpace();
	void timeout();
};

#endif // !GAMEWIDGET_H
