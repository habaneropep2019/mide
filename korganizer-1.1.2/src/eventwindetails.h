// 	$Id: eventwindetails.h,v 1.15.2.2 1999/03/28 02:44:34 glenebob Exp $	

#ifndef _EVENTWINDETAILS_H
#define _EVENTWINDETAILS_H

#include <qframe.h>
#include <qlabel.h>
#include <qchkbox.h>
#include <qpushbt.h>
#include <qgrpbox.h>
#include <qlined.h>
#include <ktablistbox.h>
#include <qcombo.h>
#include <qlayout.h>
#include <kapp.h>

#include "kdpevent.h"

class EventWin;
class EditEventWin;
class TodoEventWin;

class EventWinDetails : public QFrame
{
  Q_OBJECT

  friend EventWin;
  friend EditEventWin;
  friend TodoEventWin;

public:

  EventWinDetails(QWidget* parent = 0, const char* name = 0, bool isTodo = FALSE);
  virtual ~EventWinDetails();

public slots:
  virtual void setEnabled(bool);
  void dispAttendees();
  void dispAttendee(unsigned int i);
  void updateAttendee(unsigned int i);

protected slots:
  void addNewAttendee(); 
  void removeAttendee();
  void attendeeListHilite(int, int);
  void attendeeListAction(int, int);
  void openAddressBook();
  void setModified();

signals:
  void nullSignal(QWidget *);
  void modifiedEvent();

protected:
  bool isTodo;
  void initAttendee();
  void initAttach();
  void initMisc();

  QVBoxLayout *topLayout;

  QGroupBox* attendeeGroupBox;
  QLabel* attendeeLabel;
  QLineEdit *attendeeEdit;
  QLineEdit *emailEdit;
  KTabListBox *attendeeListBox;
  QPushButton* addAttendeeButton;
  QPushButton* removeAttendeeButton;
  QGroupBox* attachGroupBox;
  QPushButton* attachFileButton;
  QPushButton* removeFileButton;
  KTabListBox* attachListBox;
  QPushButton* saveFileAsButton;
  QPushButton* addressBookButton;
  QPushButton* categoriesButton;
  QLabel* categoriesLabel;
  QLabel* attendeeRoleLabel;
  QComboBox* attendeeRoleCombo;
  QCheckBox* attendeeRSVPButton;
  QLabel* statusLabel;
  QComboBox* statusCombo;
  QLabel* locationLabel;
  QLabel* priorityLabel;
  QComboBox* priorityCombo;
  QPushButton* resourceButton;
  QLineEdit* resourcesEdit;
  QLabel* transparencyLabel;
  QLabel* transparencyAmountLabel;

  QList<Attendee> attendeeList; // list of attendees

};

#endif
