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

#include <mediatool.h>
#include <kaudio.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <qaccel.h>
#include <qbitmap.h>
#include <qframe.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <stdlib.h>
#ifdef HAVE_USLEEP
#include <unistd.h>
#endif

#include "ksmiletris.h"
#include "gamewidget.h"
#include "screenwidget.h"
#include "mirrorwidget.h"
#include "npiecewidget.h"

GameWidget::GameWidget(QWidget *parent, const char *name)
        : QWidget(parent, name)
{
	audio = new KAudio();
	in_game = false;
	in_pause = false;

	QAccel *accel = new QAccel(this);
	accel->connectItem(accel->insertItem(Key_Up), this, SLOT(keyUp()));
	accel->connectItem(accel->insertItem(Key_Down), this, SLOT(keyDown()));
	accel->connectItem(accel->insertItem(Key_Left), this, SLOT(keyLeft()));
	accel->connectItem(accel->insertItem(Key_Right), this, SLOT(keyRight()));
	accel->connectItem(accel->insertItem(Key_Space), this, SLOT(keySpace()));

	loadSprites();
	setPieces(Pieces_Smiles);

	map = new Sprite[scr_width * scr_height];
	mirror_sprites = new Sprite[scr_width];

	screen = new ScreenWidget(sprites, &in_game, &in_pause, this);
	screen->move(10, 10);
	screen->setScreenSprites(map);

	mirror = new MirrorWidget(sprites, &in_game, &in_pause, this);
	mirror->move(10, 407);
	mirror->setMirrorSprites(mirror_sprites);

	next = new NextPieceWidget(sprites, &in_game, &in_pause, this);
	next->move(278, 10);
	next->setNextPieceSprites(next_piece);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

GameWidget::~GameWidget()
{
	delete audio;
	delete [] sprites;
	delete [] map;
	delete [] mirror_sprites;
}

void GameWidget::playSound(Sound s)
{
	if (!do_sounds || audio->serverStatus() != 0)
		return;

	QString name;
	switch (s) {
	case Sound_Break:
		name = "break.wav";
		break;
	case Sound_Clear:
		name = "clear.wav";
		break;
	}

	QString fname = kapp->kde_datadir() + "/ksmiletris/wav/" + name;
//	audio->stop();
	audio->play(fname);
//	audio->sync();
}

void GameWidget::setPieces(PiecesType type)
{
	QString prefix;

	switch (type) {
	case Pieces_Smiles:
		prefix = "block-a";
		loadSprite(Sprite_Cleared, "cleared-a.bmp");
		break;
	case Pieces_Symbols:
		prefix = "block-b";
		loadSprite(Sprite_Cleared, "cleared-b.bmp");
		break;
	case Pieces_Icons:
		prefix = "block-c";
		loadSprite(Sprite_Cleared, "cleared-c.bmp");
	}

	for (int i = 0; i < num_blocks; ++i) {
		QString n;
		n.setNum(i + 1);
		loadSprite((Sprite)(Sprite_Block1 + i), prefix + n + ".bmp");
	}
	QPixmap pm(32, 32);
	for (int i = 0; i < num_blocks; ++i) {
		QPainter p;
		p.begin(&pm);
		p.drawPixmap(0, 0, sprites[Sprite_Block1 + i]);
		p.drawPixmap(0, 0, sprites[Sprite_Broken]);
		p.end();
		sprites[Sprite_Broken1 + i] = pm;
	}

	if (in_game) repaintChilds();
}

void GameWidget::loadSprites()
{
	sprites = new QPixmap[num_sprites];
	loadSprite(Sprite_Bg1, "bg1.bmp");
	loadSprite(Sprite_Bg2, "bg2.bmp");
	loadSprite(Sprite_Bg3, "bg3.bmp");
	loadSprite(Sprite_Bg4, "bg4.bmp");
	loadSprite(Sprite_Bg5, "bg5.bmp");
	loadSprite(Sprite_Bg6, "bg6.bmp");
	loadSprite(Sprite_Bg7, "bg7.bmp");
	loadSprite(Sprite_Bg8, "bg8.bmp");
	loadSprite(Sprite_Bg9, "bg9.bmp");
	loadSprite(Sprite_Bg10, "bg10.bmp");
	loadSprite(Sprite_Bg11, "bg11.bmp");
	loadSprite(Sprite_Bg12, "bg12.bmp");
	loadSprite(Sprite_Bg13, "bg13.bmp");
	loadSprite(Sprite_Bg14, "bg14.bmp");
	loadSprite(Sprite_Bg15, "bg15.bmp");
	loadSprite(Sprite_Bg16, "bg16.bmp");
	loadMaskedSprite(Sprite_Broken, "broken.bmp", "broken-mask.bmp");
}

void GameWidget::loadSprite(Sprite spr, const char *path)
{
	if (!sprites[spr].load(kapp->kde_datadir() + "/ksmiletris/data/" + path))
		fatal("Cannot open data files.\nHave you correctly installed KSmiletris?");
}

void GameWidget::loadMaskedSprite(Sprite spr, char *path1, const char *path2)
{
	QBitmap bmp;
	if (!sprites[spr].load(kapp->kde_datadir() + "/ksmiletris/data/" + path1))
		fatal("Cannot open data files.\nHave you correctly installed KSmiletris?");
	if (!bmp.load(kapp->kde_datadir() + "/ksmiletris/data/" + path2))
		fatal("Cannot open data files.\nHave you correctly installed KSmiletris?");
	sprites[spr].setMask(bmp);
}

void GameWidget::newGame()
{
	in_pause = false;
	fast_mode = false;

	num_level = 1;
	num_pieces_level = 2;
	num_points = 0;

	bg_sprite = (Sprite)(Sprite_Bg1 + random() % num_bgs);
	screen->setBackgroundSprite(bg_sprite);
	mirror->setBackgroundSprite(bg_sprite);
	next->setBackgroundSprite(bg_sprite);
	for (int i = 0; i < scr_width; ++i)
		mirror_sprites[i] = bg_sprite;
	for (int i = 0; i < 4; ++i)
		next_piece[i] = bg_sprite;
	for (int y = 0; y < scr_height; ++y)
		for (int x = 0; x < scr_width; ++x)
			ref(x, y) = bg_sprite;

	newPiece();
	nextPiece();
	updateMirror();

	in_game = true;
	repaintChilds();
	emit changedStats(num_level, num_points);
	timer_interval = 700;
	timer->start(timer_interval);
}

void GameWidget::repaintChilds()
{
	screen->repaint(false);
	mirror->repaint(false);
	next->repaint(false);
}

void GameWidget::putPiece()
{
	if (piece[0] != bg_sprite) ref(xpos + 0, ypos + 0) = piece[0];
	if (piece[1] != bg_sprite) ref(xpos + 1, ypos + 0) = piece[1];
	if (piece[2] != bg_sprite) ref(xpos + 0, ypos + 1) = piece[2];
	if (piece[3] != bg_sprite) ref(xpos + 1, ypos + 1) = piece[3];
	updateMirror();
	screen->repaint(false);
}

void GameWidget::getPiece()
{
	if (piece[0] != bg_sprite) ref(xpos + 0, ypos + 0) = bg_sprite;
	if (piece[1] != bg_sprite) ref(xpos + 1, ypos + 0) = bg_sprite;
	if (piece[2] != bg_sprite) ref(xpos + 0, ypos + 1) = bg_sprite;
	if (piece[3] != bg_sprite) ref(xpos + 1, ypos + 1) = bg_sprite;
}

void GameWidget::newPiece()
{
	static int pieces[][4] = {{1, 0, 1, 1},
				  {1, 1, 1, 0},
				  {1, 1, 0, 1},
				  {0, 1, 1, 1}};
	int p = random() % 4;
	for (int i = 0; i < 4; ++i)
		if (pieces[p][i])
			next_piece[i] = (Sprite)(Sprite_Block1 + random()%num_pieces_level);
		else
			next_piece[i] = bg_sprite;
	next->repaint(false);
}

void GameWidget::nextPiece()
{
	piece[0] = next_piece[0];
	piece[1] = next_piece[1];
	piece[2] = next_piece[2];
	piece[3] = next_piece[3];
	newPiece();
	xpos = (scr_width - 2) / 2;
	ypos = 0;
	if ((piece[0] != bg_sprite && ref(xpos + 0, ypos + 0) != bg_sprite)
	 || (piece[1] != bg_sprite && ref(xpos + 1, ypos + 0) != bg_sprite)
	 || (piece[2] != bg_sprite && ref(xpos + 0, ypos + 1) != bg_sprite)
	 || (piece[3] != bg_sprite && ref(xpos + 1, ypos + 1) != bg_sprite)) {
		in_game = false;
		repaintChilds();
		KMsgBox::message(this, "KSmileTris", "Game Over");
		emit gameOver();
	}

	putPiece();
}

void GameWidget::updateMirror()
{
	for (int x = 0; x < scr_width; ++x)
		mirror_sprites[x] = bg_sprite;
	mirror_sprites[xpos] = piece[2] == bg_sprite ? piece[0] : piece[2];
	mirror_sprites[xpos+1] = piece[3] == bg_sprite ? piece[1] : piece[3];
	mirror->repaint(false);
}

void GameWidget::keyUp()
{
	if (!in_game || in_pause) return;

	getPiece();
	if ((piece[0] == bg_sprite && ref(xpos + 0, ypos + 0) != bg_sprite)
	 || (piece[1] == bg_sprite && ref(xpos + 1, ypos + 0) != bg_sprite)
	 || (piece[2] == bg_sprite && ref(xpos + 0, ypos + 1) != bg_sprite)
	 || (piece[3] == bg_sprite && ref(xpos + 1, ypos + 1) != bg_sprite)) {
		putPiece();
		return;
	}
	 
	Sprite npiece[4];
	npiece[2] = piece[0];
	npiece[0] = piece[1];
	npiece[3] = piece[2];
	npiece[1] = piece[3];
	for (int i = 0; i < 4; ++i)
		piece[i] = npiece[i];
	putPiece();
}

void GameWidget::keyDown()
{
	if (!in_game || in_pause) return;

	getPiece();
	if ((piece[0] == bg_sprite && ref(xpos + 0, ypos + 0) != bg_sprite)
	 || (piece[1] == bg_sprite && ref(xpos + 1, ypos + 0) != bg_sprite)
	 || (piece[2] == bg_sprite && ref(xpos + 0, ypos + 1) != bg_sprite)
         || (piece[3] == bg_sprite && ref(xpos + 1, ypos + 1) != bg_sprite)) {
		putPiece();
		return;
	}

	Sprite npiece[4];
	npiece[0] = piece[2];
	npiece[1] = piece[0];
	npiece[2] = piece[3];
	npiece[3] = piece[1];
	for (int i = 0; i < 4; ++i)
		piece[i] = npiece[i];
	putPiece();
}

void GameWidget::keyLeft()
{
 	if (!in_game || in_pause || xpos == 0) return;

	getPiece();
	if ((piece[0] != bg_sprite && ref(xpos - 1, ypos + 0) != bg_sprite)
	 || (piece[1] != bg_sprite && ref(xpos + 0, ypos + 0) != bg_sprite)
	 || (piece[2] != bg_sprite && ref(xpos - 1, ypos + 1) != bg_sprite)
         || (piece[3] != bg_sprite && ref(xpos + 0, ypos + 1) != bg_sprite)) {
		putPiece();
		return;
	}

	--xpos;
	putPiece();
}

void GameWidget::keyRight()
{
 	if (!in_game || in_pause || xpos == scr_width - 2) return;

	getPiece();
	if ((piece[0] != bg_sprite && ref(xpos + 1, ypos + 0) != bg_sprite)
	 || (piece[1] != bg_sprite && ref(xpos + 2, ypos + 0) != bg_sprite)
	 || (piece[2] != bg_sprite && ref(xpos + 1, ypos + 1) != bg_sprite)
         || (piece[3] != bg_sprite && ref(xpos + 2, ypos + 1) != bg_sprite)) {
		putPiece();
		return;
	}

	++xpos;
	putPiece();
}

void GameWidget::keySpace()
{
	if (!in_game || in_pause || fast_mode) return;
	fast_mode = true;
	timer->changeInterval(50);
}

void GameWidget::broke(int x, int y, bool *xmap)
{
	xmap[y*scr_width + x] = true;
	if (ref(x, y) >= Sprite_Broken1) {
		// Clear the piece
		playSound(Sound_Clear);
		ref(x, y) = Sprite_Cleared;
		num_points += 20;
	} else {
		// Break the piece
		playSound(Sound_Break);
		ref(x, y) = (Sprite)(Sprite_Broken1 + ref(x, y) - Sprite_Block1);
		num_points += 10;
	}
	int level = num_points / 1000 + 1;
	if (level > num_level) {
		num_level = level;
		num_pieces_level = (num_level/2 + 2) > num_blocks ?
			num_blocks : num_level/2 + 2;
		timer_interval = timer_interval - 25;
		if (timer_interval < 50)
			timer_interval = 50;
		timer->changeInterval(timer_interval);
	}

	emit changedStats(num_level, num_points);

#ifdef HAVE_USLEEP
	screen->repaint(false);
	usleep(75 * 1000);
#endif
}

void GameWidget::recBroke(int x, int y, bool *xmap)
{
	int t = type(ref(x, y));

	broke(x, y, xmap);
	// X-1, Y
	if (x > 0 && !xmap[y*scr_width + x - 1] && type(ref(x-1, y)) == t)
		recBroke(x-1, y, xmap);
	// X+1, Y
	if (x < scr_width-1 && !xmap[y*scr_width + x + 1] && type(ref(x+1, y)) == t)
		recBroke(x+1, y, xmap);
	// X, Y-1
	if (y > 0 && !xmap[(y-1)*scr_width + x] && type(ref(x, y-1)) == t)
		recBroke(x, y-1, xmap);
	// X, Y+1
	if (y < scr_height-1 && !xmap[(y+1)*scr_width + x] && type(ref(x, y+1)) == t)
		recBroke(x, y+1, xmap);

	// X-1, Y+1
	if (x > 0 && y < scr_height-1 && !xmap[(y+1)*scr_width + x - 1] && type(ref(x-1, y+1)) == t)
		recBroke(x-1, y+1, xmap);
	// X+1, Y+1
	if (x < scr_width-1 && y < scr_height-1 && !xmap[(y+1)*scr_width + x + 1] && type(ref(x+1, y+1)) == t)
		recBroke(x+1, y+1, xmap);
	// X-1, Y-1
	if (x > 0 && y > 0 && !xmap[(y-1)*scr_width + x - 1] && type(ref(x-1, y-1)) == t)
		recBroke(x-1, y-1, xmap);
	// X+1, Y-1
	if (x < scr_width-1 && y > 0 && !xmap[(y-1)*scr_width + x + 1] && type(ref(x+1, y-1)) == t)
		recBroke(x+1, y-1, xmap);
}

void GameWidget::checkSolePiece(int x, int y, bool *xmap)
{
	if (y < scr_height-1 && type(ref(x, y+1)) == type(ref(x, y)))
		recBroke(x, y, xmap);
}

void GameWidget::checkPiece(int x, int y)
{
	bool xmap[scr_width * scr_height];
	for (int yy = 0; yy < scr_height; ++yy)
		for (int x = 0; x < scr_width; ++x)
			xmap[yy*scr_width + x] = false;
	checkSolePiece(x, y, xmap);
}

void GameWidget::compact()
{
 recheck:
	bool moved = true;
	while (moved) {
		moved = false;
		for (int x = 0; x < scr_width; ++x)
			for (int y = scr_height - 1; y > 0; --y)
				if (ref(x, y) == bg_sprite
				 || ref(x, y) == Sprite_Cleared) {
					int i;
					for (i = y-1; i >= 0; --i)
						if (ref(x, i) != bg_sprite
						&&  ref(x, i) != Sprite_Cleared) {
							ref(x, y) = ref(x, i);
							ref(x, i) = bg_sprite;
							moved = true;
							checkPiece(x, y);
							goto recheck;
							break;
						}
					if (i < 0)
						ref(x, y) = bg_sprite;
				}
	}			
}

void GameWidget::blockPiece()
{
	if (fast_mode) {
		timer->changeInterval(timer_interval);
		fast_mode = false;
	}	
	putPiece();
	bool xmap[scr_width * scr_height];
	for (int yy = 0; yy < scr_height; ++yy)
		for (int x = 0; x < scr_width; ++x)
			xmap[yy*scr_width + x] = false;
	if (piece[2] != bg_sprite)
		checkSolePiece(xpos, ypos+1, xmap);
	if (piece[3] != bg_sprite)
		checkSolePiece(xpos+1, ypos+1, xmap);
	if (piece[2] == bg_sprite)
		checkSolePiece(xpos, ypos, xmap);
	if (piece[3] == bg_sprite)
		checkSolePiece(xpos+1, ypos, xmap);
	compact();
	nextPiece();
}

void GameWidget::timeout()
{
	if (!in_game || in_pause)
		return;

	getPiece();
	if (ypos == scr_height - 2) {
		blockPiece();
		return;
	}

	if ((piece[0] != bg_sprite && ref(xpos + 0, ypos + 1) != bg_sprite)
	 || (piece[1] != bg_sprite && ref(xpos + 1, ypos + 1) != bg_sprite)
	 || (piece[2] != bg_sprite && ref(xpos + 0, ypos + 2) != bg_sprite)
         || (piece[3] != bg_sprite && ref(xpos + 1, ypos + 2) != bg_sprite))
		blockPiece();
	else {
		++ypos;
		putPiece();
	}

}
