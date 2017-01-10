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

#ifndef KSMILETRIS_H
#define KSMILETRIS_H

const int sprite_width = 32;
const int sprite_height = 32;

enum Sprite {
	Sprite_Bg1,
	Sprite_Bg2,
	Sprite_Bg3,
	Sprite_Bg4,
	Sprite_Bg5,
	Sprite_Bg6,
	Sprite_Bg7,
	Sprite_Bg8,
	Sprite_Bg9,
	Sprite_Bg10,
	Sprite_Bg11,
	Sprite_Bg12,
	Sprite_Bg13,
	Sprite_Bg14,
	Sprite_Bg15,
	Sprite_Bg16,
	Sprite_Block1,
	Sprite_Block2,
	Sprite_Block3,
	Sprite_Block4,
	Sprite_Block5,
	Sprite_Block6,
	Sprite_Block7,
	Sprite_Block8,
	Sprite_Broken,
	Sprite_Broken1,
	Sprite_Broken2,
	Sprite_Broken3,
	Sprite_Broken4,
	Sprite_Broken5,
	Sprite_Broken6,
	Sprite_Broken7,
	Sprite_Broken8,
	Sprite_Cleared,
	NUM_SPRITES
};

const int num_sprites = NUM_SPRITES;
const int num_blocks = 8;
const int num_bgs = 16;

const int scr_width = 8;
const int scr_height = 12;

enum PiecesType {
	Pieces_Smiles,
	Pieces_Symbols,
	Pieces_Icons
};

enum Sound {
	Sound_Break,
	Sound_Clear
};

#endif // !KSMILETRIS_H
