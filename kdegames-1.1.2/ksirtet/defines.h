#ifndef KTETRIS_D_H
#define KTETRIS_D_H

#include "kapp.h"
#include "klocale.h"

/* some defines */
#define K_MAIN "main"
#define NET_DEFAULT_NAME "Player" 
#define NET_DEFAULT_PORT "5001"

/* number of highscores */
#define NB_HS    10

/* Strings for configuration file */
#define HS_GRP "High Scores"
#define HS_NAME_KEY "name"
#define HS_SCORE_KEY "score"
#define OP_MENUBAR_VIS "menubar visible"

/* misc defines */
#define CANCEL 0
#define SERVER 1
#define CLIENT 2

/* Layout */
#define BLOCK_W  15
#define BLOCK_H  15
#define XOFF      1
#define YOFF      1
#define NB_W_BLOCKS 10
#define NB_H_BLOCKS 22

#define BOARD_W 2*XOFF + NB_W_BLOCKS*BLOCK_W
#define BOARD_H 2*YOFF + NB_H_BLOCKS*BLOCK_H

#define TOP_H    10
#define BASE_H   10
#define LCD_H    50
#define LCD_W   100 
#define NTILE_H  94
#define NTILE_W  78
#define SIDE_W   10
#define MID_W    30
#define MID_H    7
#define BAR_W    10

#define LABEL_W  LCD_W
#define LABEL_H  30

#define ADD_LABEL( label, str, x, y, w, h) \
    { label = new QLabel(str, this); \
      label->setGeometry(x, y, w, h); }

#define ADD_BUTTON( button, str, x, y, w, h, slot) \
    { button = new QPushButton(str, this); \
      button->setGeometry(x, y, w, h); \
	  connect( button, SIGNAL(clicked()), SLOT(slot) );}

#define ADD_EDIT( ledit, maxL, x, y, w, h) \
    { ledit = new QLineEdit(this); \
      ledit->setGeometry(x, y, w, h); \
      ledit->setMaxLength(maxL); }

#define ERROR( msg ) \
    { QMessageBox ab; \
	  ab.setCaption(i18n("Error")); \
      ab.setText(msg); \
	  ab.setButtonText(i18n("Oops")); \
	  ab.show(); }

/* used in network code */
#define MAX_CLIENTS 10
#define MAX_BUFF    200

#endif
