#include <qpixmap.h>
#include <qlayout.h>
#include "editDefaults.h"

#define Inherited QDialog

#include <qlabel.h>
#include <qpushbt.h>
#include <qtabbar.h>

#include <kapp.h>
#include <ktabctl.h>

#include "editDefaults.moc"

#include "userDefaultsPage.h"

editDefaults::editDefaults(QWidget* parent, const char* name)
	: Inherited(parent, name, TRUE, 184320) {
	KTabCtl* qtarch_TabBar_2;
	qtarch_TabBar_2 = new KTabCtl( this, "TabBar_2" );
	qtarch_TabBar_2->setGeometry( 5, 5, 480, 250 );
	qtarch_TabBar_2->setMinimumSize( 0, 0 );
	qtarch_TabBar_2->setMaximumSize( 32767, 32767 );
	qtarch_TabBar_2->setFocusPolicy( QWidget::TabFocus );
	qtarch_TabBar_2->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_TabBar_2->setFontPropagation( QWidget::NoChildren );
	qtarch_TabBar_2->setPalettePropagation( QWidget::NoChildren );
	qtarch_TabBar_2->setShape( QTabBar::RoundedAbove );

  page1 = new userDefaultsPage(this, "user_page");
	qtarch_TabBar_2->addTab(page1, i18n("User"));

	QPushButton* qtarch_PushButton_15;
	qtarch_PushButton_15 = new QPushButton( this, "PushButton_15" );
	qtarch_PushButton_15->setGeometry( 143, 235, 100, 30 );
	qtarch_PushButton_15->setMinimumSize( 100, 30 );
	qtarch_PushButton_15->setMaximumSize( 100, 30 );
	connect( qtarch_PushButton_15, SIGNAL(clicked()), SLOT(ok()) );
	qtarch_PushButton_15->setFocusPolicy( QWidget::TabFocus );
	qtarch_PushButton_15->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_PushButton_15->setFontPropagation( QWidget::NoChildren );
	qtarch_PushButton_15->setPalettePropagation( QWidget::NoChildren );
	qtarch_PushButton_15->setText(i18n("OK"));
	qtarch_PushButton_15->setAutoRepeat( FALSE );
	qtarch_PushButton_15->setAutoResize( FALSE );

	QPushButton* qtarch_PushButton_16;
	qtarch_PushButton_16 = new QPushButton( this, "PushButton_16" );
	qtarch_PushButton_16->setGeometry( 248, 235, 100, 30 );
	qtarch_PushButton_16->setMinimumSize( 100, 30 );
	qtarch_PushButton_16->setMaximumSize( 100, 30 );
	connect( qtarch_PushButton_16, SIGNAL(clicked()), SLOT(cancel()) );
	qtarch_PushButton_16->setFocusPolicy( QWidget::TabFocus );
	qtarch_PushButton_16->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_PushButton_16->setFontPropagation( QWidget::NoChildren );
	qtarch_PushButton_16->setPalettePropagation( QWidget::NoChildren );
	qtarch_PushButton_16->setText(i18n("Cancel"));
	qtarch_PushButton_16->setAutoRepeat( FALSE );
	qtarch_PushButton_16->setAutoResize( FALSE );

	QLabel* qtarch_Label_13;
	qtarch_Label_13 = new QLabel( this, "Label_13" );
	qtarch_Label_13->setGeometry( 5, 235, 133, 30 );
	qtarch_Label_13->setMinimumSize( 0, 30 );
	qtarch_Label_13->setMaximumSize( 32767, 30 );
	qtarch_Label_13->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_13->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_13->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_13->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_13->setText( "" );
	qtarch_Label_13->setAlignment( 289 );
	qtarch_Label_13->setMargin( -1 );

	QLabel* qtarch_Label_14;
	qtarch_Label_14 = new QLabel( this, "Label_14" );
	qtarch_Label_14->setGeometry( 353, 235, 132, 30 );
	qtarch_Label_14->setMinimumSize( 0, 30 );
	qtarch_Label_14->setMaximumSize( 32767, 30 );
	qtarch_Label_14->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_14->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_14->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_14->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_14->setText( "" );
	qtarch_Label_14->setAlignment( 289 );
	qtarch_Label_14->setMargin( -1 );

	QBoxLayout* qtarch_layout_1 = new QBoxLayout( this, QBoxLayout::TopToBottom, 0, 0, NULL );
	qtarch_layout_1->addStrut( 0 );
	qtarch_layout_1->addWidget( qtarch_TabBar_2, 1, 36 );
	QBoxLayout* qtarch_layout_1_2 = new QBoxLayout( QBoxLayout::LeftToRight, 5, NULL );
	qtarch_layout_1->addLayout( qtarch_layout_1_2, 0 );
	qtarch_layout_1_2->addStrut( 0 );
	qtarch_layout_1_2->addWidget( qtarch_Label_13, 1, 36 );
	qtarch_layout_1_2->addWidget( qtarch_PushButton_15, 0, 36 );
	qtarch_layout_1_2->addWidget( qtarch_PushButton_16, 0, 36 );
	qtarch_layout_1_2->addWidget( qtarch_Label_14, 1, 36 );
	resize(290, 290);
	setMinimumSize(290, 320);
	setMaximumSize(290, 320);
}


editDefaults::~editDefaults() {
}

void editDefaults::ok() {
  accept();
}

void editDefaults::cancel() {
  reject();
}

const char *editDefaults::getShell() const {
  return page1->getShell();
}

const char *editDefaults::getHomeBase() const {
  return page1->getHomeBase();
}

bool editDefaults::getCreateHomeDir() const {
  return page1->getCreateHomeDir();
}

bool editDefaults::getCopySkel() const {
  return page1->getCopySkel();
}

bool editDefaults::getUserPrivateGroup() const {
  return page1->getUserPrivateGroup();
}

void editDefaults::setShell(const char *data) {
  page1->setShell(data);
}

void editDefaults::setHomeBase(const char *data) {
  page1->setHomeBase(data);
}

void editDefaults::setCreateHomeDir(bool data) {
  page1->setCreateHomeDir(data);
}

void editDefaults::setCopySkel(bool data) {
  page1->setCopySkel(data);
}

void editDefaults::setUserPrivateGroup(bool data) {
  page1->setUserPrivateGroup(data);
}

