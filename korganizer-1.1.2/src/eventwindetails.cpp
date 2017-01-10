// 	$Id: eventwindetails.cpp,v 1.26.2.2 1999/03/28 02:44:33 glenebob Exp $	

#include <qmessagebox.h>
#include <qslider.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <kbuttonbox.h>
#include <kiconloader.h>
#include <kapp.h>
#include <kabapi.h>

#include "eventwindetails.h"
#include "eventwindetails.moc"

EventWinDetails::EventWinDetails (QWidget* parent, const char* name, bool todo)
  : QFrame( parent, name, 0 )
{
  isTodo = todo;
  attendeeList.setAutoDelete(TRUE);
    
  resize( 600,400 );
  topLayout = new QVBoxLayout(this, 5);

  initAttendee();
  //initAttach();
  initMisc();

  setMinimumSize( 600, 400 );
  setMaximumSize( 32767, 32767 );

  topLayout->activate();
}

void EventWinDetails::initAttendee()
{

  attendeeGroupBox = new QGroupBox(this);
  attendeeGroupBox->setTitle(i18n("Attendee Information"));
  topLayout->addWidget(attendeeGroupBox);

  QVBoxLayout *layout = new QVBoxLayout(attendeeGroupBox, 10);

  layout->addSpacing(10); // top caption needs some space
  attendeeListBox = new KTabListBox( attendeeGroupBox, "attendeeListBox", 5);
  attendeeListBox->setColumn(0, i18n("Name"), 180);
  attendeeListBox->setColumn(1, i18n("Email"), 180);
  attendeeListBox->setColumn(2, i18n("Role"), 60);
  attendeeListBox->setColumn(3, i18n("Status"), 100);
  attendeeListBox->setColumn(4, i18n("RSVP"), 15, KTabListBox::PixmapColumn);
  attendeeListBox->dict().insert("Y", new QPixmap(Icon("mailappt.xpm")));
  attendeeListBox->dict().insert("N", new QPixmap(Icon("nomailappt.xpm")));
  attendeeListBox->setMinimumSize(QSize(400, 100));
  layout->addWidget(attendeeListBox);
  
  connect(attendeeListBox, SIGNAL(highlighted(int, int)),
	  this, SLOT(attendeeListHilite(int, int)));
  connect(attendeeListBox, SIGNAL(selected(int, int)),
	  this, SLOT(attendeeListAction(int, int)));

  QHBoxLayout *subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);

  attendeeLabel = new QLabel(attendeeGroupBox);
  attendeeLabel->setText(i18n("Attendee Name:"));
  attendeeLabel->setFixedWidth(attendeeLabel->sizeHint().width());
  attendeeLabel->setMinimumHeight(attendeeLabel->sizeHint().height());
  subLayout->addWidget(attendeeLabel);

  attendeeEdit = new QLineEdit(this);
  attendeeEdit->setMinimumWidth(attendeeEdit->sizeHint().width());
  attendeeEdit->setFixedHeight(attendeeEdit->sizeHint().height());
  attendeeEdit->setText( "" );
  subLayout->addWidget(attendeeEdit);

  QLabel *emailLabel = new QLabel(attendeeGroupBox);
  emailLabel->setText(i18n("Email Address:"));
  emailLabel->setFixedWidth(emailLabel->sizeHint().width());
  emailLabel->setMinimumHeight(emailLabel->sizeHint().height());
  subLayout->addWidget(emailLabel);
  
  emailEdit = new QLineEdit(this);
  emailEdit->setMinimumWidth(emailEdit->sizeHint().width());
  emailEdit->setFixedHeight(emailEdit->sizeHint().height());
  emailEdit->setText("");
  subLayout->addWidget(emailEdit);

  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  
  attendeeRoleLabel = new QLabel(attendeeGroupBox);
  attendeeRoleLabel->setText(i18n("Role:"));
  attendeeRoleLabel->setMinimumSize(attendeeRoleLabel->sizeHint());
  attendeeRoleLabel->setAlignment(AlignVCenter|AlignRight);
  subLayout->addWidget(attendeeRoleLabel);

  attendeeRoleCombo = new QComboBox(FALSE, attendeeGroupBox);
  attendeeRoleCombo->insertItem( i18n("Attendee") );
  attendeeRoleCombo->insertItem( i18n("Organizer") );
  attendeeRoleCombo->insertItem( i18n("Owner") );
  attendeeRoleCombo->insertItem( i18n("Delegate") );
  attendeeRoleCombo->setFixedSize(attendeeRoleCombo->sizeHint());
  subLayout->addWidget(attendeeRoleCombo);

  statusLabel = new QLabel(attendeeGroupBox);
  statusLabel->setText( i18n("Status:") );
  statusLabel->setMinimumSize(statusLabel->sizeHint());
  statusLabel->setAlignment(AlignVCenter|AlignRight);
  subLayout->addWidget(statusLabel);

  statusCombo = new QComboBox( FALSE, attendeeGroupBox);
  statusCombo->insertItem( i18n("Needs Action") );
  statusCombo->insertItem( i18n("Accepted") );
  statusCombo->insertItem( i18n("Sent") );
  statusCombo->insertItem( i18n("Tentative") );
  statusCombo->insertItem( i18n("Confirmed") );
  statusCombo->insertItem( i18n("Declined") );
  statusCombo->insertItem( i18n("Completed") );
  statusCombo->insertItem( i18n("Delegated") );
  statusCombo->setFixedSize(statusCombo->sizeHint());
  subLayout->addWidget(statusCombo);

  subLayout->addStretch();

  attendeeRSVPButton = new QCheckBox(this);
  attendeeRSVPButton->setText(i18n("Request Response"));
  attendeeRSVPButton->setFixedSize(attendeeRSVPButton->sizeHint());
  subLayout->addWidget(attendeeRSVPButton);

  KButtonBox *buttonBox = new KButtonBox(attendeeGroupBox);

  addAttendeeButton = buttonBox->addButton(i18n("&Add"));
  connect(addAttendeeButton, SIGNAL(clicked()),
	  this, SLOT(addNewAttendee()));

  addressBookButton = buttonBox->addButton(i18n("Address &Book..."));
  connect(addressBookButton, SIGNAL(clicked()),
	  this, SLOT(openAddressBook()));

  removeAttendeeButton = buttonBox->addButton(i18n("&Remove"));
  connect(removeAttendeeButton, SIGNAL(clicked()),
	  this, SLOT(removeAttendee()));
  buttonBox->layout();

  layout->addWidget(buttonBox);

  layout->activate();
}
    
void EventWinDetails::initAttach()
{

  attachGroupBox = new QGroupBox( this, "User_2" );
  attachGroupBox->setGeometry( 10, 190, 580, 100 );
  attachGroupBox->setMinimumSize( 10, 10 );
  attachGroupBox->setMaximumSize( 32767, 32767 );
  attachGroupBox->setEnabled(FALSE);

  attachFileButton = new QPushButton( this, "PushButton_3" );
  attachFileButton->setGeometry( 20, 200, 100, 20 );
  attachFileButton->setMinimumSize( 10, 10 );
  attachFileButton->setMaximumSize( 32767, 32767 );
  attachFileButton->setText( i18n("Attach...") );
  attachFileButton->setEnabled(FALSE);

  removeFileButton = new QPushButton( this, "PushButton_4" );
  removeFileButton->setGeometry( 20, 260, 100, 20 );
  removeFileButton->setMinimumSize( 10, 10 );
  removeFileButton->setMaximumSize( 32767, 32767 );
  removeFileButton->setText( i18n("Remove") );
  removeFileButton->setEnabled(FALSE);

  attachListBox = new KTabListBox( this, "ListBox_2" );
  attachListBox->setGeometry( 140, 200, 440, 80 );
  attachListBox->setMinimumSize( 10, 10 );
  attachListBox->setMaximumSize( 32767, 32767 );
  attachListBox->setEnabled(FALSE);

  saveFileAsButton = new QPushButton( this, "PushButton_5" );
  saveFileAsButton->setGeometry( 20, 230, 100, 20 );
  saveFileAsButton->setMinimumSize( 10, 10 );
  saveFileAsButton->setMaximumSize( 32767, 32767 );
  saveFileAsButton->setText( i18n("Save As...") );
  saveFileAsButton->setEnabled(FALSE);
}

void EventWinDetails::initMisc()
{

  QGroupBox *groupBox = new QGroupBox(this);
  topLayout->addWidget(groupBox);

  QVBoxLayout *layout = new QVBoxLayout(groupBox, 10);
  
  QHBoxLayout *subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);

  categoriesButton = new QPushButton(groupBox);
  categoriesButton->setText(i18n("Categories..."));
  categoriesButton->setFixedSize(categoriesButton->sizeHint());
  subLayout->addWidget(categoriesButton);

  categoriesLabel = new QLabel(groupBox);
  categoriesLabel->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  categoriesLabel->setText( "" );
  categoriesLabel->setMinimumSize(categoriesLabel->sizeHint());
  subLayout->addWidget(categoriesLabel);

  /*  locationLabel = new QLabel(groupBox);
  locationLabel->setText( i18n("Location:") );
  locationLabel->setFixedSize(locationLabel->sizeHint());
  layout->addWidget(locationLabel);*/

  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);

  priorityLabel = new QLabel(groupBox);
  priorityLabel->setText( i18n("Priority:") );
  priorityLabel->setMinimumWidth(priorityLabel->sizeHint().width());
  priorityLabel->setFixedHeight(priorityLabel->sizeHint().height());
  subLayout->addWidget(priorityLabel);

  priorityCombo = new QComboBox( FALSE, groupBox);
  priorityCombo->insertItem( i18n("Low (1)") );
  priorityCombo->insertItem( i18n("Normal (2)") );
  priorityCombo->insertItem( i18n("High (3)") );
  priorityCombo->insertItem( i18n("Maximum (4)") );
  priorityCombo->setFixedSize(priorityCombo->sizeHint());
  connect(priorityCombo, SIGNAL(activated(int)),
    this, SLOT(setModified()));
  subLayout->addWidget(priorityCombo);

  subLayout->addStretch();
  
  /*  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);

    resourceButton = new QPushButton(groupBox);
  resourceButton->setText( i18n("Resources...") );
  resourceButton->setFixedSize(resourceButton->sizeHint());
  subLayout->addWidget(resourceButton);

  resourcesEdit = new QLineEdit(groupBox);
  resourcesEdit->setText( "" );
  resourcesEdit->setFixedHeight(resourcesEdit->sizeHint().height());
  resourcesEdit->setMinimumWidth(resourcesEdit->sizeHint().width());
  subLayout->addWidget(resourcesEdit);

  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  transparencyLabel = new QLabel( groupBox);
  transparencyLabel->setText( i18n("Transparency:") );
  transparencyLabel->setFixedSize(transparencyLabel->sizeHint());
  subLayout->addWidget(transparencyLabel);

  transparencyAmountLabel = new QLabel(groupBox);
  transparencyAmountLabel->setText( "0" );
  transparencyAmountLabel->setMinimumSize(transparencyAmountLabel->sizeHint());
  subLayout->addWidget(transparencyAmountLabel);*/

  layout->activate();
}


EventWinDetails::~EventWinDetails()
{
  attendeeList.clear();
}


void EventWinDetails::setEnabled(bool enabled)
{
  attendeeEdit->setEnabled(enabled);
  addAttendeeButton->setEnabled(enabled);
  removeAttendeeButton->setEnabled(enabled);
  attachFileButton->setEnabled(enabled);
  saveFileAsButton->setEnabled(enabled);
  addressBookButton->setEnabled(enabled);
  categoriesButton->setEnabled(enabled);
  categoriesLabel->setEnabled(enabled);
  attendeeRoleCombo->setEnabled(enabled);
  //  attendeeRSVPButton->setEnabled(enabled);
  statusCombo->setEnabled(enabled);
  priorityCombo->setEnabled(enabled);
  resourceButton->setEnabled(enabled);
  resourcesEdit->setEnabled(enabled);
}

void EventWinDetails::removeAttendee()
{
  // sanity check
  if ((attendeeListBox->count() == 0) ||
      (attendeeListBox->currentItem() < 0))
    return;
  
  int index = attendeeListBox->currentItem();
  attendeeListBox->removeItem(index);
  attendeeList.remove(index);
  setModified();
  if (attendeeList.count()) {
    index--; if (index < 0) index = 0;
    attendeeListBox->setCurrentItem(index);
  } 
}

void EventWinDetails::attendeeListHilite(int row, int /*col*/)
{
  if (attendeeListBox->count() == 0)
    return;
  /*
  attendeeEdit->setText(attendeeListBox->text(row, 0));
  attendeeRoleCombo->setCurrentItem(attendeeListBox->text(row,2).toUInt());
  statusCombo->setCurrentItem(attendeeListBox->text(row,3).toUInt());
  attendeeRSVPButton->setChecked((attendeeListBox->text(row,4) == "Y") ? 
				 TRUE : FALSE);
  */
}

void EventWinDetails::attendeeListAction(int row, int col)
{
  return;

  /*  switch (col) {
  case 0:
    // do something with the attendee here.
    break;
  case 4:
    if (strcmp(attendeeListBox->text(row, col), "Y") == 0)
      attendeeListBox->changeItemPart("N", row, col);
    else
      attendeeListBox->changeItemPart("Y", row, col);
    break;
    }*/
}

void EventWinDetails::openAddressBook()
{
  KabAPI addrDialog(this);

  if (addrDialog.init() != KabAPI::NoError) {
    QMessageBox::critical(this, i18n("KOrganizer Error"),
			  i18n("Unable to open address book."));
    return;
  }
  string key;
  AddressBook::Entry entry;
  if (addrDialog.exec()) {
    if (addrDialog.getEntry(entry, key) == KabAPI::NoError) {
      // get name -- combo of first and last names
      QString nameStr;
      nameStr = entry.firstname.c_str();
      if (!nameStr.isEmpty()) nameStr += " ";
      nameStr += entry.name.c_str();
      attendeeEdit->setText(nameStr.data());

      // take first email address
      if (!entry.emails.empty() && entry.emails.front().length())
      	emailEdit->setText(entry.emails.front().c_str());
      
    } else {
      QMessageBox::warning(this, i18n("KOrganizer Error"),
			   i18n("Error getting entry from address book."));
    } 
  }
}

void EventWinDetails::dispAttendees()
{
	attendeeListBox->clear();

  Attendee *a;
  QString attStr;
  for (a = attendeeList.first(); a; a = attendeeList.next()) {
    attStr = a->getName() + "\n";
    attStr += (!a->getEmail().isEmpty()) ? a->getEmail().data() : " ";
    attStr += "\n";
    attStr += a->getRoleStr() + "\n";
    attStr += a->getStatusStr() + "\n";
    attStr += (a->RSVP() && !a->getEmail().isEmpty()) ? "Y" : "N";

    attendeeListBox->appendItem(attStr.data());
  }
}

void EventWinDetails::dispAttendee(unsigned int i)
{
  Attendee *a;
  QString attStr;


  // sanity checks
  if (attendeeList.count() == 0)
    return;

  if (i < 0 || i > attendeeList.count() - 1)
    return;

  a = attendeeList.at(i);
  attStr = a->getName() + "\n";
  attStr += (!a->getEmail().isEmpty()) ? a->getEmail().data() : " ";
  attStr += "\n";
  attStr += a->getRoleStr() + "\n";
  attStr += a->getStatusStr() + "\n";
  attStr += (a->RSVP() && !a->getEmail().isEmpty()) ? "Y" : "N";

  if (i >= attendeeListBox->count())
    attendeeListBox->appendItem(attStr.data());
  else
    attendeeListBox->changeItem(attStr.data(), i);  
}

void EventWinDetails::updateAttendee(unsigned int i)
{
  Attendee *a;

  // sanity checks
  if (attendeeList.count() == 0)
    return;

  if (i < 0 || i > attendeeList.count() - 1)
    return;

  a = attendeeList.at(i);
  a->setName(attendeeListBox->text(i, 0));
  a->setEmail(attendeeListBox->text(i, 1));
  if (attendeeListBox->text(i, 2) == "Attendee")
    a->setRole(0);
  else if (attendeeListBox->text(i,2) == "Organizer")
    a->setRole(1);
  else if (attendeeListBox->text(i,2) == "Owner")
    a->setRole(2);
  else if (attendeeListBox->text(i,2) == "Delegate")
    a->setRole(3);
  else
    a->setRole(0);
  a->setStatus(attendeeListBox->text(i,4));
  a->setRSVP((attendeeListBox->text(i,5) == "Y") ? TRUE : FALSE);
}

void EventWinDetails::addNewAttendee()
{
  // don;t do anything on a blank name
  if (QString(attendeeEdit->text()).stripWhiteSpace().isEmpty())
    return;

  Attendee *a;
  
  a = new Attendee(attendeeEdit->text());

  // this is cool.  If they didn't enter an email address,
  // try to look it up in the address book and fill it in for them.
  if (QString(emailEdit->text()).stripWhiteSpace().isEmpty()) {
    KabAPI addrBook;
    string name;
    list<AddressBook::Entry> entries;
    name = attendeeEdit->text();
    if (addrBook.init() == KabAPI::NoError) {
      if (addrBook.getEntryByName(name, entries, 1) == KabAPI::NoError) {
	debug("positive match");
	// take first email address
	if (!entries.front().emails.empty() && 
	    entries.front().emails.front().length())
	  emailEdit->setText(entries.front().emails.front().c_str());
      }
    }
  }

  a->setEmail(emailEdit->text());
  a->setRole(attendeeRoleCombo->currentItem());
  a->setStatus(statusCombo->currentItem());
  a->setRSVP(attendeeRSVPButton->isChecked() ? TRUE : FALSE);
  attendeeList.append(a);
  setModified();
  dispAttendee(attendeeList.count()-1);

  // zero everything out for a new one
  attendeeEdit->setText("");
  emailEdit->setText("");
  attendeeRoleCombo->setCurrentItem(0);
  statusCombo->setCurrentItem(0);
  attendeeRSVPButton->setChecked(TRUE);
}

void EventWinDetails::setModified()
{
  emit modifiedEvent();
}

