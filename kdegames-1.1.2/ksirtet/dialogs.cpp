#include "dialogs.h"
#include "defines.h"
#include "net.h"

#include <stdlib.h>

#include <qpushbt.h>
#include <qfileinf.h>
#include <qmsgbox.h>

#include <kapp.h>
#include <kconfig.h>

#include "dialogs.moc"

/*********************/
/* HighScore dialogs */
WHighScores::WHighScores( bool show, int score,
						  QWidget *parent, const char *name)
: QDialog(parent, name, TRUE)
{
	kconf = kapp->getConfig();
	
	showHS(show, score);
}

void WHighScores::showHS( bool show, int score)
{
	int i;
	range = -1;
	QString str1, str2;
	QLabel *label;
	
	kconf->setGroup(HS_GRP);
	/* set highscore ? */
	if ( !show ) {
        /* a better score ?? */
        for (i=0; i<NB_HS; i++) {
			str1.sprintf("%s%i", HS_SCORE_KEY, i);
            if ( score > kconf->readNumEntry(str1) ) {
				range = i;
				break;
			}
		}
		if (range == -1)
			return;

		for (i=NB_HS; i>range; i--) {
			str1.sprintf("%s%i", HS_SCORE_KEY, i);
			str2.sprintf("%s%i", HS_SCORE_KEY, i-1);
			kconf->writeEntry(str1, kconf->readEntry(str2));
			str1.sprintf("%s%i", HS_NAME_KEY, i);
            str2.sprintf("%s%i", HS_NAME_KEY, i-1);
            kconf->writeEntry(str1, kconf->readEntry(str2));
		}
		
		str1.sprintf("%s%i", HS_SCORE_KEY, range);
		kconf->writeEntry(str1, score);
	}
			
	setCaption(i18n("High Scores"));

	/* set dialog layout */
	int frame_w = 10; int frame_h = 10;
	int dec1_h = 20; int dec2_h = 10; int dec3_h = 20;
	
	QFont f1( font() );
	QFontMetrics fm1( f1 );
	int nb_w = 2*fm1.maxWidth(); int nb2_w = 10*fm1.maxWidth();
	int button_h = fm1.height() + 2*10;
	int button_w = fm1.width(i18n("Close")) +2*15;
	int space_w = fm1.width(" ");
	
	f1.setBold(TRUE);
	QFontMetrics fm2(f1);
	int label_h = QMAX(fm1.height(), fm2.height()) + 2*2;
	int name_label_w = 10*fm2.maxWidth();
	
	QFont f2( font() );
	QFontInfo info(f2);
	f2.setPointSize(info.pointSize()+6);
	f2.setBold(TRUE);
	QFontMetrics fm3(f2);
	int title_w = fm3.width(i18n("Hall of Fame"));
	int title_h = fm3.height();

	int H = 2*frame_h + title_h + dec1_h + NB_HS*label_h + (NB_HS-1)*dec2_h
		    + dec3_h + button_h;
	int W = QMAX(title_w+10, name_label_w + 3*space_w + nb_w + nb2_w)
		    + 2*frame_w;
	setFixedSize(W, H);
	
	ADD_LABEL(label, i18n("Hall of Fame"),
			  (W-title_w)/2, frame_h, title_w, title_h);
	label->setFont(f2);
	label->setAlignment(AlignCenter);
	
	int temp = frame_h + title_h + dec1_h;
	int dec = label_h + dec2_h;
	int temp2;
	#define HDEC temp+i*dec
	for (i=0; i<NB_HS; i++) {
		temp2 = frame_w;
		str1.sprintf("%s%i", HS_NAME_KEY, i);
		ADD_LABEL(label, kconf->readEntry(str1), temp2, HDEC,
				  name_label_w, label_h);
		label->setAlignment( AlignLeft | AlignVCenter);
		label->setFont(f1);
		if ( !show && range==i ) {
			lb = label;
			label->hide(); 
			ADD_EDIT(qle, 10, temp2, HDEC, name_label_w, label_h);
			connect(qle, SIGNAL(returnPressed()), SLOT(writeName()));
		}
		temp2 += name_label_w + 2*space_w;
		ADD_LABEL(label, "-", temp2, HDEC, nb_w, label_h);
		temp2 += nb_w + space_w;
		str1.sprintf("%s%i", HS_SCORE_KEY, i);
		ADD_LABEL(label, kconf->readEntry(str1), temp2, HDEC,
				  nb2_w, label_h);
        label->setAlignment( AlignRight | AlignVCenter);
	}
	
	ADD_BUTTON(pb, i18n("Close"),
			   (W-button_w)/2, H-frame_h-button_h,
			   button_w, button_h, accept());
	
	if ( !show ) pb->hide();

	exec();
}

void WHighScores::writeName()
{
	QString str = qle->text();
	if ( str.isNull() )
		str = i18n("Anonymous");

	QString str2;
	str2.sprintf("%s%i", HS_NAME_KEY, range);
	kconf->writeEntry(str2, str);
	
	/* show the entered highscore */
	qle->hide();
	pb->show();
	lb->setText(str);
	lb->show();
}

/******************/
/* Options dialog */
Options::Options( QWidget *parent, const char *name )
: QDialog(parent, name, TRUE)
{
	QLabel *label;
	ADD_LABEL(label, i18n("No option yet !"), 10, 10, 90, LABEL_H);
	QPushButton *pb;
	ADD_BUTTON(pb, i18n("OK"), 35, 130, 50, 30, accept());
	
	resize(120,170);
	exec();
}

/***********************/
/* network game Dialog */
NetDialog::NetDialog( QWidget *parent, const char *name )
: QDialog(parent, name, TRUE)
{
	timer = new QTimer();
	connect( timer, SIGNAL(timeout()), SLOT(timeout()) );
	
	net_obj = 0;
	
	/* create title label */
	ADD_LABEL(labTitle, i18n("Network Dialog"), 10, 10, 150, LABEL_H);
		
	/* create the choice buttons (SERVER, CLIENT & CANCEL) */
	ADD_BUTTON(butServer, i18n("Server"), 35, 50, 100, 30, server());
	ADD_BUTTON(butClient, i18n("Client"), 35, 90, 100, 30, client());
	ADD_BUTTON(butCancel, i18n("Cancel"), 35, 130, 100, 30, cancel());

	/* create labels and edit lines for ASK_ADD */
	ADD_LABEL(labNameE, i18n("Player name :"), 10, 40, 100, LABEL_H);
	labNameE->hide();
	ADD_LABEL(labAddE, i18n("Server address :"), 10, 60, 100, LABEL_H);
	labAddE->hide();
	ADD_LABEL(labPortE, i18n("Port :"), 10, 90, 100, LABEL_H);
	labPortE->hide();

	ADD_LABEL(labName, "", 120, 40, 100, LABEL_H); labName->hide();
	ADD_LABEL(labAdd, "", 120, 60, 200, LABEL_H); labAdd->hide();
	ADD_LABEL(labPort, "", 120, 90, 50, LABEL_H); labPort->hide();

	ADD_EDIT(edName, 10, 120, 40, 100, 20); edName->hide();
	ADD_EDIT(edAdd, 40, 120, 60, 200, 20); edAdd->hide();
	ADD_EDIT(edPort, 4, 120, 90, 40, 20); edPort->hide();
	
	/* create buttons and label for CONNECT & WAIT_PLAYERS */
	ADD_BUTTON(butConnect, i18n("Connect"), 200, 130, 100, LABEL_H, connecting());
	butConnect->hide();

	ADD_LABEL(labState, i18n("Connecting ..."), 160, 130, 200, LABEL_H);
	labState->setAlignment( AlignCenter ); labState->hide();
	
	ADD_BUTTON(butPlay, i18n("Play !"),  200, 130, 100, LABEL_H, play());
	butPlay->hide();

	/* variable that indicates in which state we are */
	state = ASK_MODE;
	
	adjustSize();
}


NetDialog::~NetDialog()
{
	delete timer;
}


void NetDialog::showConnect(int new_state)
{
	int i;
	QString str;
	QLabel *label;
	
	state = new_state;
	
	/* ASK_MODE : is called if cancel in ASK_ADD */
	if ( state==ASK_MODE ) {
		/* change title label */
		labTitle->setText(i18n("Network Dialog"));
		labTitle->resize(150, LABEL_H);
		/* hide labels & edLines used in ASK_ADD */
		labNameE->hide();
		labAddE->hide();
		labPortE->hide();
		if ( net_obj->isServer() )
			labAdd->hide();
		else
			edAdd->hide();
		edPort->hide();
		edName->hide();
		/* hide connect button and show server/client buttons */
		butConnect->hide();
		butServer->show();
		butClient->show();
	}
	
	/* ASK_ADD */
	if ( state==ASK_ADD ) {
		str = i18n("Network game");
		/* change the title label */
		str += " : ";
		if ( net_obj->isServer() )
			str += i18n("Server mode");
		else 
			str += i18n("Client mode");
		labTitle->resize(200, LABEL_H); 
		labTitle->setText(str);
		
		/* hide SERVER & CLIENT buttons */
		butServer->hide();
		butClient->hide();
		
		/* show the two labels "server address" & "port" */
		labNameE->show();
		labAddE->show();
		labPortE->show();
		
		/* show enventually the label for address in server mode */
		if ( net_obj->isServer() ) {
			labAdd->setText(net_obj->t_address);
			labAdd->show();
		}
		
		/* show edit lines */
		edAdd->setText(net_obj->t_address);
		if ( net_obj->isClient() )
			edAdd->show();
		edPort->setText(net_obj->t_port);
		edPort->show();
		edName->setText(net_obj->t_name);
		edName->show();
			
		/* show CONNECT button */
		butConnect->show();
	}

	/* RE_ASK_ADD */
	if ( state==RE_ASK_ADD ) {
		/* hide labels formed from edit lines & reshow edits */
		if ( net_obj->isClient() ) {
			labAdd->hide();
			edAdd->show();
		}
		labPort->hide();
		edPort->show();
		labName->hide();
		edName->show();

		/* hide state label and show connect button */
		labState->hide();
		butConnect->show();
	}
	
	/* CONNECT */
	if ( state==CONNECT ) {
		/* hide edit lines & create the corresponding labels */
		if ( net_obj->isClient() ) {
			edAdd->hide();
			net_obj->t_address = edAdd->text();
			labAdd->setText(net_obj->t_address);
			labAdd->show();
		}
		edPort->hide();
		/* t_port has been updated in net_obj->checkParam */
		labPort->setText(net_obj->t_port);
		labPort->show();
		edName->hide();
		net_obj->t_name = edName->text();
		labName->setText(net_obj->t_name);
		labName->show();
		
		/* hide CONNECT button & show the corresponding label */
		butConnect->hide();
		labState->show();
		
		oldNbPlayers = 0;
	}
	
	/* WAIT_PLAYERS */
	if ( state==WAIT_PLAYERS ) {
		/* update state label */
		if ( net_obj->getNbPlayers()==0 ) {
			labState->setText(i18n("Waiting for other player(s) ..."));
			labState->show();
			butPlay->hide();
		}
		/* delete labels & create the new list */
		for (i=1; i<=net_obj->getNbPlayers(); i++) {
			net_obj->deleteLabel(i);
			net_obj->getPlayerInfos(net_obj->client_name, i);
			ADD_LABEL( label, net_obj->client_name, 
					   20, 150+i*30, 300, LABEL_H );
				label->show();
			net_obj->createLabel(i, label);
		}
		/* if the number of players is >= 2 */
		/* delete state label & create "PLAY" button if not already */
		if ( net_obj->getNbPlayers()>=1 ) {
			labState->hide();
			butPlay->show();
		} 
		oldNbPlayers = net_obj->getNbPlayers();
	}

	adjustSize();
}


void NetDialog::createNetObject(bool server)
{
	if ( server ) net_obj = new ServerNetObject();
	else net_obj = new ClientNetObject();
	
	showConnect(ASK_ADD);
}


void NetDialog::connecting()
{
	/* test and set address & port */
	if ( !net_obj->checkParam( edName->text(), edAdd->text(), edPort->text(),
							   serror) ) {
		ERROR(serror);
		return;
	}
	
	/* update dialog in connecting mode */
	showConnect(CONNECT);
	
	/* in the case of a server_net_object : the connection socket (#0) may
	 * already exist (if != -1) ; in this case it is not necessary to create
	 * it and possibly if the port used is the same, it is not necessary
	 * to bind/listen it. */
//	if ( net_obj->isClient() || !net_obj->connectSocketExists() ) {
		/* create the first socket */
		if ( !net_obj->createSocket(serror) ) {
			ERROR(serror);
			showConnect(RE_ASK_ADD);
			return;
		}
//	}
	
//	if ( net_obj->isClient() || !net_obj->connectSocketExists()
//		 || !net_obj->samePort() ) {
		net_obj->setPort();
		/* connect socket (ie connect or listen according to the mode) */
		if ( !net_obj->connectSocket(serror) ) {
			ERROR(serror);
			showConnect(RE_ASK_ADD);
			return;
		}
//	}

	/* update dialog in waiting mode */
	showConnect(WAIT_PLAYERS);
	
	/* waiting loop */
	timer->start(NET_TIMEOUT);
}


void NetDialog::timeout()
{
	if ( !net_obj->dialogTimeout(serror) ) {
		if ( !serror.isEmpty() ) {
			timer->stop();
			ERROR(serror);
			reject();
		}
	} else {
		if ( net_obj->isClient() )
			accept();
		else
			/* number of clients has changed */
			showConnect(WAIT_PLAYERS);
	}
}


void NetDialog::cancel()
{
	if ( state==ASK_ADD )
		showConnect(ASK_MODE);
	else {
		if (net_obj) {
			delete net_obj;
			net_obj = 0;
		}
		reject();
	}
}


void NetDialog::play()
{
	for (int i=0; i<=net_obj->getNbPlayers(); i++)
		net_obj->deleteLabel(i);
	
	accept();
}
