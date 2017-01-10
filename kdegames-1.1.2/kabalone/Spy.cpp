/* Class Spion
 *
 * Josef Weidendorfer, 10/97
 */

#ifdef SPION

#include <qgrpbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <kapp.h>
#include <kwm.h>
#include "Spion.h"

Spion::Spion(Board& b)
  :board(b)
{
  top = new QVBoxLayout(this, 5);
  
  QLabel *l = new QLabel(this);
  l->setText( klocale->translate("Actual examined position:") );
  l->setFixedHeight( l->sizeHint().height() );
  l->setAlignment( AlignVCenter | AlignLeft );
  top->addWidget( l );

  QHBoxLayout* b1 = new QHBoxLayout();
  top->addLayout( b1, 10 );
	
  for(int i=0;i<BoardCount;i++) {
    QVBoxLayout *b2 = new QVBoxLayout();
    b1->addLayout( b2 );

    actBoard[i] = new BoardWidget(board,this);
    actLabel[i] = new QLabel(this);
    actLabel[i]->setText("---");
    // actLabel[i]->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    actLabel[i]->setAlignment( AlignHCenter | AlignVCenter);
    actLabel[i]->setFixedHeight( actLabel[i]->sizeHint().height() );

    b2->addWidget( actBoard[i] );
    b2->addWidget( actLabel[i] );
    connect( actBoard[i], SIGNAL(mousePressed()), this, SLOT(nextStep()) );
  }

  l = new QLabel(this);
  l->setText( klocale->translate("Best move so far:") );
  l->setFixedHeight( l->sizeHint().height() );
  l->setAlignment( AlignVCenter | AlignLeft );
  top->addWidget( l );

  b1 = new QHBoxLayout();
  top->addLayout( b1, 10 );
	
  for(int i=0;i<BoardCount;i++) {
    QVBoxLayout *b2 = new QVBoxLayout();
    b1->addLayout( b2 );

    bestBoard[i] = new BoardWidget(board,this);
    bestLabel[i] = new QLabel(this);
    bestLabel[i]->setText("---");
    //    bestLabel[i]->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    bestLabel[i]->setAlignment( AlignHCenter | AlignVCenter);
    bestLabel[i]->setFixedHeight( bestLabel[i]->sizeHint().height() );

    b2->addWidget( bestBoard[i] );
    b2->addWidget( bestLabel[i] );
    connect( bestBoard[i], SIGNAL(mousePressed()), this, SLOT(nextStep()) );
  }
  
  connect( &board, SIGNAL(update(int,int,Move&,bool)), 
	   this, SLOT(update(int,int,Move&,bool)) );
  connect( &board, SIGNAL(updateBest(int,int,Move&,bool)), 
	   this, SLOT(updateBest(int,int,Move&,bool)) );
  top->activate();
  setCaption(klocale->translate("Spion"));
  // KWM::setDecoration(winId(), 2);
  resize(500,300);
}

void Spion::keyPressEvent(QKeyEvent *)
{
	nextStep();
}

void Spion::nextStep()
{
  next = true;
}

void Spion::clearActBoards()
{
  for(int i=0;i<BoardCount;i++) {
    actBoard[i]->clearPosition();
    actBoard[i]->draw();
    actLabel[i]->setText("---");
  }
}

void Spion::update(int depth, int value, Move& m, bool wait)
{
	next = false;
	
	if (depth>BoardCount-1) return;
	board.showMove(m,3);
	actBoard[depth]->copyPosition();
	actBoard[depth]->draw();
	board.showMove(m,0);	

	if (!wait) {
	  actLabel[depth]->setText("---");
	  return;
	}

	if (depth<BoardCount-1) {
	  board.playMove(m);
	  actBoard[depth+1]->copyPosition();
	  actBoard[depth+1]->draw();
	  actLabel[depth+1]->setNum(value);
	  board.takeBack();

	  if (depth<BoardCount-2) {
	    actBoard[depth+2]->clearPosition();
	    actBoard[depth+2]->draw();
	  }
	}

	while(!next)
	  kapp->processEvents();
}

void Spion::updateBest(int depth, int value, Move& m, bool cutoff)
{
  if (depth>BoardCount-1) return;
  board.showMove(m,3);
  bestBoard[depth]->copyPosition();
  bestBoard[depth]->draw();
  board.showMove(m,0);

  if (depth<BoardCount-1) {
    board.playMove(m);
    bestBoard[depth+1]->copyPosition();
    bestBoard[depth+1]->draw();

    QString tmp;
    tmp.setNum(value);
    if (cutoff) tmp += " (CutOff)";
    bestLabel[depth+1]->setText(tmp);
    board.takeBack();
  }
}

#include "Spy.moc"

#endif





