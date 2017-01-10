/* -*- C++ -*-
 * Qt widget for the addressbook example.
 * Declaration of the toplevel widget.
 *
 * the KDE addressbook
 * copyright:  (C) Mirko Sucker, 1998, 1999
 * license:    GNU Public License, Version 2
 * mail to:    Mirko Sucker <mirko.sucker@unibw-hamburg.de>
 * requires:   C++-compiler, STL, string class
 * $Revision: 1.3.2.1 $
 */
#ifndef ADDRESSBOOK_TLW_INCL
#define ADDRESSBOOK_TLW_INCL

#include "kab.h"
// There are two ID functions, and namespaces do not work:
#ifdef ID
#undef ID
#endif
#define ID KDE_ID
#include <ktmainwindow.h>
#undef ID

#include <qtimer.h>
#include "debug.h"

/** Here we will construct a top level widget that
  * has an AddressWidget as its main part.
  */

class ABTLWidget : public KTMainWindow // KTopLevelWidget
{
  Q_OBJECT
protected:
  // menubar, statusbar
  KMenuBar* menu;
  QPopupMenu *edit, *file;
  int mailItemID, browseItemID, talkItemID, editItemID, removeItemID,
    exportItemID, searchItemID, printItemID, saveItemID, copyItemID;
  KStatusBar* status;
  // the AddressWidget object
  AddressWidget* widget;
  // a timer to erase the statusbar text
  QTimer* timer;
  bool constructMenu();
  bool constructStatusBar();
  void createConnections();
public:
  enum StatusbarIndex { Text=1, Number };
  ABTLWidget(const char* name=0);
public slots:
  void setNumber(int number, int count);
  void aboutQt();
  void about();
  // ----- next both used to set and erase statusbar:
  void setStatus(const char*);
  void timeOut();
  // ----- called by saveYourself signal:
  void saveOurselfes();
  // ----- enables or disables menu items:
  void enableWidgets(int noOfEntries);
};

#endif // ADDRESSBOOK_TLW_INCL
