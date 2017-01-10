/**********************************************************************

	--- Qt Architect generated file ---

	File: ssfeprompt.cpp
	Last generated: Thu Jan 15 20:49:56 1998

 *********************************************************************/

#include "ssfeprompt.h"
#include "kplunger/kplunger.h"
#include <kapp.h>

#define Inherited ssfepromptdata

ssfePrompt::ssfePrompt
(
 QString prompttext,
 QWidget* parent,
 const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( i18n("Prompt") );
	prompt->setText(prompttext);
	SLine->setFocus();
}


ssfePrompt::~ssfePrompt()
{
}

void ssfePrompt::terminate()
{
  done(0);
}

QString ssfePrompt::text()
{
  QString text;
  text = SLine->text();
  return text;
}

void ssfePrompt::setPassword(bool pass)
{
  if(pass == TRUE)
    SLine->setEchoMode(QLineEdit::Password);
  else
    SLine->setEchoMode(QLineEdit::Normal);
}
#include "ssfeprompt.moc"
