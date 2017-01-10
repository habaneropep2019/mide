/*
  This file is part of klpq (C) 1998 Christoph Neerfeld
*/
/**********************************************************************

	--- Qt Architect generated file ---

	File: FirstStart.cpp
	Last generated: Sat Feb 7 00:45:21 1998

 *********************************************************************/

#include <kapp.h>

#include "FirstStart.h"
#include "FirstStart.moc"

FirstStart::FirstStart(	QWidget* parent, const char* name)
  : FirstStartData( parent, name )
{
  setCaption( klocale->translate("Select your spooling system") );
  connect( b_ok, SIGNAL(clicked()), this, SLOT(accept()) );
}


FirstStart::~FirstStart()
{
}

QString FirstStart::getSpooler()
{
  if( rb_ppr->isChecked() )
    return "PPR";
  if( rb_lprng->isChecked() )
    return "LPRNG";
  return "BSD";
}
