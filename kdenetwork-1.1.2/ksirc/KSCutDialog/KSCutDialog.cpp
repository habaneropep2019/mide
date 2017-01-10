/**********************************************************************

	--- Qt Architect generated file ---

	File: KSCutDialog.cpp
	Last generated: Mon Dec 29 21:40:17 1997

 *********************************************************************/

// Notes:
// We HAVE to delete outrselves, toplevel won't do it.

#include "KSCutDialog.h"
#include <qevent.h>
#include <iostream.h>

#define Inherited KSCutDialogData

KSCutDialog::KSCutDialog
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( i18n("Cut Window") );
	IrcMLE->setFocus();
}


KSCutDialog::~KSCutDialog()
{
}

void KSCutDialog::closeEvent(QCloseEvent *)
{
  delete this;
}

void KSCutDialog::setText(QString text)
{
  IrcMLE->setText(text);
  IrcMLE->setCursorPosition(IrcMLE->numLines(), 0);
}

void KSCutDialog::scrollToBot()
{
  IrcMLE->setFocus();
  IrcMLE->scrollToBottom();
}

void KSCutDialog::terminate()
{
  close(1);
}

void KSCutDialog::copy()
{
  IrcMLE->copyText();
}

#include "KSCutDialog.moc"
