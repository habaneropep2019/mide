// 	$Id: todowingeneral.cpp,v 1.5.2.3 1999/03/28 02:44:42 glenebob Exp $	

#include <qtooltip.h>
#include <kiconloader.h>
#include <qfiledlg.h>

#include "todowingeneral.h"
#include "todowingeneral.moc"

TodoWinGeneral::TodoWinGeneral(QWidget* parent, const char* name)
  : WinGeneral( parent, name)
{
  alarmProgram = "";
  resize( 600,400 );

  initMisc();

  setMinimumSize( 600, 400 );
  setMaximumSize( 32767, 32767 );

  QWidget::setTabOrder(summaryEdit, completedButton);
  QWidget::setTabOrder(completedButton, completedCombo);
  QWidget::setTabOrder(completedCombo, priorityCombo);
  QWidget::setTabOrder(priorityCombo, descriptionEdit);
  QWidget::setTabOrder(descriptionEdit, categoriesButton);
  QWidget::setTabOrder(categoriesButton, privateButton);
  summaryEdit->setFocus();
}

void TodoWinGeneral::initMisc() {
    int ypos = 70;

    completedButton = new QCheckBox(this, "CheckBox_10" );
    completedButton->setGeometry( 10, ypos, 140, 20 );
    completedButton->setText( i18n("Completed") );
    connect(completedButton, SIGNAL(toggled(bool)),
	    this, SLOT(setModified()));

    completedLabel = new QLabel( this, "Label_3" );
    completedLabel->setGeometry( 130, ypos, 70, 20 );
    completedLabel->setText( i18n("% Completed") );
    completedLabel->setAlignment( 289 );
    completedLabel->setMargin( -1 );

    completedCombo = new QComboBox( FALSE, this, "ComboBox_10" );
    completedCombo->setGeometry( 220, ypos, 100, 20 );
    completedCombo->setSizeLimit( 10 );
    completedCombo->insertItem( i18n("0 %") );
    completedCombo->insertItem( i18n("25 %") );
    completedCombo->insertItem( i18n("50 %") );
    completedCombo->insertItem( i18n("75 %") );
    completedCombo->insertItem( i18n("Completed") );
    connect(completedCombo, SIGNAL(activated(int)),
	    this, SLOT(setModified()));

    //    ypos += ystep;

    priorityLabel = new QLabel( this, "Label_3" );
    priorityLabel->setGeometry( 360, ypos, 70, 20 );
    priorityLabel->setText( i18n("Priority") );
    priorityLabel->setAlignment( 289 );
    priorityLabel->setMargin( -1 );

    priorityCombo = new QComboBox( FALSE, this, "ComboBox_10" );
    priorityCombo->setGeometry( 430, ypos, 100, 20 );
    priorityCombo->setSizeLimit( 10 );
    priorityCombo->insertItem( i18n("1 (Highest)") );
    priorityCombo->insertItem( i18n("2") );
    priorityCombo->insertItem( i18n("3") );
    priorityCombo->insertItem( i18n("4") );
    priorityCombo->insertItem( i18n("5 (lowest)") );
    connect(priorityCombo, SIGNAL(activated(int)),
	    this, SLOT(setModified()));

    summaryLabel = new QLabel( this, "Label_1" );
    summaryLabel->setGeometry( 10, 40, 70, 20 );
    summaryLabel->setText( i18n("Summary:") );
    summaryLabel->setAlignment( 289 );
    summaryLabel->setMargin( -1 );

    summaryEdit = new QLineEdit( this, "LineEdit_1" );
    summaryEdit->setGeometry( 80, 40, 510, 20 );
    connect(summaryEdit, SIGNAL(textChanged(const char*)),
	    this, SLOT(setModified()));

    descriptionEdit = new QMultiLineEdit( this, "MultiLineEdit_1" );
    descriptionEdit->setGeometry( 10, 100, 580, 250 );
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

/*
TodoWinGeneral::~TodoWinGeneral()
{
}
  */
