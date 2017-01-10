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

#ifndef NPIECEWIDGET_H
#define NPIECEWIDGET_H

#include <qframe.h>

#include "ksmiletris.h"

class NextPieceWidget : public QFrame {
public:
	NextPieceWidget(QPixmap *s, bool *game, bool *pause,
			QWidget *parent=0, const char *name=0);

	void setBackgroundSprite(Sprite s) { bg_sprite = s; }
	void setNextPieceSprites(Sprite *s) { next_piece_sprites = s; }

private:
	QPixmap *sprites;
	bool *in_game, *in_pause;
	Sprite bg_sprite;
	Sprite *next_piece_sprites;

protected:
	void drawContents(QPainter *p);
};

#endif // !NPIECEWIDGET_H
