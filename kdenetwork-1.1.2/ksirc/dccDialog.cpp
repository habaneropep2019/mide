/**********************************************************************

	--- Qt Architect generated file ---

	File: dccDialog.cpp
	Last generated: Wed Dec 17 21:00:35 1997

 *********************************************************************/

#include "dccDialog.h"

#define Inherited dccDialogData

dccDialog::dccDialog
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( i18n("DCC Files Offered") );
	connect(getButton, SIGNAL(clicked()),
		this, SIGNAL(getFile()));
	connect(forgetButton, SIGNAL(clicked()),
		this, SIGNAL(forgetFile()));
}


dccDialog::~dccDialog()
{
}
#include "dccDialog.moc"
