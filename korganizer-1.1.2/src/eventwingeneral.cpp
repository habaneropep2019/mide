// 	$Id: eventwingeneral.cpp,v 1.24.2.2 1999/03/28 02:44:35 glenebob Exp $	

#include <qtooltip.h>
#include <kiconloader.h>
#include <qfiledlg.h>

#include "eventwingeneral.h"
#include "eventwingeneral.moc"

EventWinGeneral::EventWinGeneral(QWidget* parent, const char* name)
  : WinGeneral( parent, name)
{
  alarmProgram = "";
  resize( 600,400 );

  initTimeBox();
  initAlarmBox();
  initMisc();

  setMinimumSize( 600, 400 );
  setMaximumSize( 32767, 32767 );

  QWidget::setTabOrder(summaryEdit, startDateEdit);
  QWidget::setTabOrder(startDateEdit, startTimeEdit);
  QWidget::setTabOrder(startTimeEdit, endDateEdit);
  QWidget::setTabOrder(endDateEdit, endTimeEdit);
  QWidget::setTabOrder(endTimeEdit, noTimeButton);
  QWidget::setTabOrder(noTimeButton, recursButton);
  QWidget::setTabOrder(recursButton, alarmButton);
  QWidget::setTabOrder(alarmButton, freeTimeCombo);
  QWidget::setTabOrder(freeTimeCombo, descriptionEdit);
  QWidget::setTabOrder(descriptionEdit, categoriesButton);
  QWidget::setTabOrder(categoriesButton, privateButton);

  summaryEdit->setFocus();
}


void EventWinGeneral::initTimeBox()
{
  timeGroupBox = new QGroupBox( this, "User_2" );
  timeGroupBox->setTitle(i18n("Appointment Time"));
  timeGroupBox->setGeometry( 10, 70, 580, 80 );

  startLabel = new QLabel( this, "Label_2" );
  startLabel->setGeometry( 70, 90, 70, 20 );
  startLabel->setText( i18n("Start Time:") );
  startLabel->setAlignment( 289 );
  startLabel->setMargin( -1 );

  startDateEdit = new KDateEdit(this);
  startDateEdit->setGeometry( 140, 90, 130, 20 );
  connect(startDateEdit, SIGNAL(dateChanged(QDate)),
    this, SLOT(setModified()));

  endLabel = new QLabel( this, "Label_3" );
  endLabel->setGeometry( 70, 120, 70, 20 );
  endLabel->setText( i18n("End Time:") );
  endLabel->setAlignment( 289 );
  endLabel->setMargin( -1 );

  
  endDateEdit = new KDateEdit(this);
  endDateEdit->setGeometry( 140, 120, 130, 20 );
  connect(endDateEdit, SIGNAL(dateChanged(QDate)),
    this, SLOT(setModified()));

  startTimeEdit = new KTimeEdit(this);
  startTimeEdit->setGeometry( 280, 90, 105, 25 );
  connect(startTimeEdit, SIGNAL(timeChanged(QTime, int)),
    this, SLOT(setModified()));

  endTimeEdit = new KTimeEdit(this);
  endTimeEdit->setGeometry( 280, 120, 105, 25 );
  connect(endTimeEdit, SIGNAL(timeChanged(QTime, int)),
    this, SLOT(setModified()));

  noTimeButton = new QCheckBox(this, "CheckBox_1" );
  noTimeButton->setGeometry( 400, 90, 140, 20 );
  noTimeButton->setText( i18n("No time associated") );

  connect(noTimeButton, SIGNAL(toggled(bool)), 
	  this, SLOT(timeStuffDisable(bool)));
  connect(noTimeButton, SIGNAL(toggled(bool)),
	  this, SLOT(alarmStuffDisable(bool)));
  connect(noTimeButton, SIGNAL(toggled(bool)),
	  this, SLOT(alarmStuffDisable(bool)));
  
  recursButton = new QCheckBox(this);
  recursButton->setGeometry( 400, 120, 140, 20);
  recursButton->setText(i18n("Recurring event"));
}

void EventWinGeneral::initMisc()
{

  summaryLabel = new QLabel( this, "Label_1" );
  summaryLabel->setGeometry( 10, 40, 70, 20 );
  summaryLabel->setText( i18n("Summary:") );
  summaryLabel->setAlignment( 289 );
  summaryLabel->setMargin( -1 );

  summaryEdit = new QLineEdit( this, "LineEdit_1" );
  summaryEdit->setGeometry( 80, 40, 510, 20 );
  connect(summaryEdit, SIGNAL(textChanged(const char*)),
    this, SLOT(setModified()));

  freeTimeLabel = new QLabel( this, "Label_6" );
  freeTimeLabel->setGeometry( 380, 160, 100, 20 );
  freeTimeLabel->setText( i18n("Show Time As:") );
  freeTimeLabel->setAlignment( 289 );
  freeTimeLabel->setMargin( -1 );

  freeTimeCombo = new QComboBox( FALSE, this, "ComboBox_1" );
  freeTimeCombo->setGeometry( 480, 160, 100, 20 );
  freeTimeCombo->setSizeLimit( 10 );
  freeTimeCombo->insertItem( i18n("Busy") );
  freeTimeCombo->insertItem( i18n("Free") );
  connect(freeTimeCombo, SIGNAL(activated(int)),
    this, SLOT(setModified()));

  descriptionEdit = new QMultiLineEdit( this, "MultiLineEdit_1" );
  descriptionEdit->setGeometry( 10, 200, 580, 150 );
  descriptionEdit->insertLine( "" );
  descriptionEdit->setReadOnly( FALSE );
  descriptionEdit->setOverwriteMode( FALSE );
  connect(descriptionEdit, SIGNAL(textChanged()),
    this, SLOT(setModified()));


  ownerLabel = new QLabel( this, "Label_7" );
  ownerLabel->setGeometry( 10, 10, 200, 20 );
  ownerLabel->setText( i18n("Owner:") );
  ownerLabel->setAlignment( 289 );
  ownerLabel->setMargin( -1 );

  privateButton = new QCheckBox( this, "CheckBox_3" );
  privateButton->setGeometry( 520, 360, 70, 20 );
  privateButton->setText( i18n("Private") );
  connect(privateButton, SIGNAL(toggled(bool)),
    this, SLOT(setModified()));

  categoriesButton = new QPushButton( this, "PushButton_6" );
  categoriesButton->setGeometry( 10, 360, 100, 20 );
  categoriesButton->setText( i18n("Categories...") );

  categoriesLabel = new QLabel( this, "LineEdit_7" );
  categoriesLabel->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  categoriesLabel->setGeometry( 120, 360, 390, 20 );

}

void EventWinGeneral::initAlarmBox()
{
  QPixmap pixmap;

  QLabel *alarmBell = new QLabel(this);
  alarmBell->setPixmap(QPixmap(Icon("bell.xpm")));
  alarmBell->adjustSize();
  alarmBell->move(10, 165);

  alarmButton = new QCheckBox( this, "CheckBox_2" );
  alarmButton->setGeometry( 40, 160, 80, 20 );
  alarmButton->setText( i18n("Reminder:") );
  connect(alarmButton, SIGNAL(toggled(bool)),
    this, SLOT(setModified()));

  alarmTimeEdit = new KRestrictedLine(this, "alarmTimeEdit",
				      "1234567890");
  alarmTimeEdit->setGeometry( 120, 160, 50, 20 );
  connect(alarmTimeEdit, SIGNAL(textChanged(const char*)),
    this, SLOT(setModified()));

  alarmIncrCombo = new QComboBox(FALSE, this);
  alarmIncrCombo->insertItem("minute(s)");
  alarmIncrCombo->insertItem("hour(s)");
  alarmIncrCombo->insertItem("day(s)");
  alarmIncrCombo->setMinimumHeight(20);
  alarmIncrCombo->adjustSize();
  alarmIncrCombo->move(180, 155);
  connect(alarmIncrCombo, SIGNAL(activated(int)),
    this, SLOT(setModified()));

  alarmSoundButton = new QPushButton( this, "PushButton_4" );
  alarmSoundButton->setGeometry( 270, 160, 40, 20 );
  pixmap = Icon("playsound.xpm");
  //  alarmSoundButton->setText( i18n("WAV") );
  alarmSoundButton->setPixmap(pixmap);
  alarmSoundButton->setToggleButton(TRUE);
  QToolTip::add(alarmSoundButton, "No sound set");

  alarmProgramButton = new QPushButton( this, "PushButton_5" );
  alarmProgramButton->setGeometry( 320, 160, 40, 20 );
  pixmap = Icon("runprog.xpm");
  //  alarmProgramButton->setText( i18n("PROG") );
  alarmProgramButton->setPixmap(pixmap);
  alarmProgramButton->setToggleButton(TRUE);
  QToolTip::add(alarmProgramButton, "No program set");

  connect(alarmButton, SIGNAL(toggled(bool)),
	  this, SLOT(alarmStuffEnable(bool)));

  connect(alarmSoundButton, SIGNAL(clicked()),
	  this, SLOT(pickAlarmSound()));
  connect(alarmProgramButton, SIGNAL(clicked()),
	  this, SLOT(pickAlarmProgram()));
}

void EventWinGeneral::pickAlarmSound()
{
  QString prefix(KApplication::kde_datadir());
  prefix += "/korganizer/sounds";
  if (!alarmSoundButton->isOn()) {
    alarmSound = "";
    QToolTip::remove(alarmSoundButton);
    QToolTip::add(alarmSoundButton, "No sound set");

    emit modifiedEvent();
  } else {
    QString fileName(QFileDialog::getOpenFileName(prefix.data(),
						  "*.wav", this));
    if (!fileName.isEmpty()) {
      alarmSound = fileName;
      QToolTip::remove(alarmSoundButton);
      QString dispStr = "Playing \"";
      dispStr += fileName.data();
      dispStr += "\"";
      QToolTip::add(alarmSoundButton, dispStr.data());
      emit modifiedEvent();
    }
  }
  if (alarmSound.isEmpty())
    alarmSoundButton->setOn(FALSE);
}

void EventWinGeneral::pickAlarmProgram()
{
  if (!alarmProgramButton->isOn()) {
    alarmProgram = "";
    QToolTip::remove(alarmProgramButton);
    QToolTip::add(alarmProgramButton, "No program set");
    emit modifiedEvent();
  } else {
    QString fileName(QFileDialog::getOpenFileName(0, "*", this));
    if (!fileName.isEmpty()) {
      alarmProgram = fileName;
      QToolTip::remove(alarmProgramButton);
      QString dispStr = "Running \"";
      dispStr += fileName.data();
      dispStr += "\"";
      QToolTip::add(alarmProgramButton, dispStr.data());
      emit modifiedEvent();
    }
  }
  if (alarmProgram.isEmpty())
    alarmProgramButton->setOn(FALSE);
}

EventWinGeneral::~EventWinGeneral()
{
}

void EventWinGeneral::setEnabled(bool enabled)
{

  noTimeButton->setEnabled(enabled);
  recursButton->setEnabled(enabled);

  summaryEdit->setEnabled(enabled);
  startDateEdit->setEnabled(enabled);
  endDateEdit->setEnabled(enabled);

  startTimeEdit->setEnabled(enabled);
  endTimeEdit->setEnabled(enabled);

  alarmButton->setEnabled(enabled);
  alarmTimeEdit->setEnabled(enabled);
  alarmSoundButton->setEnabled(enabled);
  alarmProgramButton->setEnabled(enabled);

  descriptionEdit->setEnabled(enabled);
  freeTimeCombo->setEnabled(enabled);
  privateButton->setEnabled(enabled);
  categoriesButton->setEnabled(enabled);
  categoriesLabel->setEnabled(enabled);

  emit modifiedEvent();
}

void EventWinGeneral::timeStuffDisable(bool disable)
{
  if (disable) {
    startTimeEdit->hide();
    endTimeEdit->hide();
  } else {
    startTimeEdit->show();
    endTimeEdit->show();
  }
  
  emit modifiedEvent();
}

void EventWinGeneral::alarmStuffEnable(bool enable)
{
  alarmTimeEdit->setEnabled(enable);
  alarmSoundButton->setEnabled(enable);
  alarmProgramButton->setEnabled(enable);

  emit modifiedEvent();
}

void EventWinGeneral::alarmStuffDisable(bool disable)
{
  alarmTimeEdit->setEnabled(!disable);
  alarmSoundButton->setEnabled(!disable);
  alarmProgramButton->setEnabled(!disable);

  emit modifiedEvent();
}


