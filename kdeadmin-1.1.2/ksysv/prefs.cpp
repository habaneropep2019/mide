/*

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

// KSVGui Preferences Dialog

#include <qlabel.h>
#include <qpushbt.h>
#include <qlayout.h>
#include <kbuttonbox.h>
#include <kfiledialog.h>
#include <kwm.h>
#include "ksv_core.h"
#include "prefs.h"

// include meta-object code
#include "prefs.moc"
#define MINIMAL 2
#define HALF 5
#define FULL 2*HALF
#define MIN_SIZE(A) A->setMinimumSize(A->sizeHint())
#define GOOD_SIZE(A) A->setMinimumWidth( QMAX(80, A->sizeHint().width()) ); \
                     A->setFixedHeight( QMAX(25, A->sizeHint().height()) )

KSVPrefDlg::KSVPrefDlg( QWidget* parent, char* name, WFlags f)
  : QTabDialog(parent, name, true, f)
{
  setCaption(QString().sprintf(i18n("%s - Preferences"),
			       KSVCore::getAppName().data()));

  /******************/
  /* the first page */
  /******************/
  QWidget* pathPage = new QWidget(this);

  QBoxLayout* pathLayout = new QVBoxLayout(pathPage, HALF);

  QLabel* _prefix = new QLabel(i18n("Directory containing your init-scripts (&services)"), pathPage);
  MIN_SIZE(_prefix);
  pathLayout->addWidget(_prefix);

  QBoxLayout* scripts = new QHBoxLayout();
  pathLayout->addLayout(scripts, MINIMAL);
  pathLayout->addSpacing(FULL);

  // script path
  _scriptPath = new QLineEdit(pathPage);
  _prefix->setBuddy(_scriptPath);
  MIN_SIZE(_scriptPath);

  browseScripts = new QPushButton(i18n("Browse..."), pathPage);
  browseScripts->setAutoDefault(FALSE);
  MIN_SIZE(browseScripts);

  scripts->addWidget(_scriptPath, FULL);
  scripts->addWidget(browseScripts, MINIMAL);

  // Runlevel Path
  QLabel* _pRL = new QLabel(i18n("Directory containing your &runlevel-directories"), pathPage);
  MIN_SIZE(_pRL);
  pathLayout->addWidget(_pRL);

  QBoxLayout* RLs = new QHBoxLayout();
  pathLayout->addLayout(RLs, MINIMAL);
  pathLayout->addStretch( 2*FULL );
//   pathLayout->addSpacing(FULL);

  _rlPath = new QLineEdit(pathPage);
  _pRL->setBuddy(_rlPath);
  MIN_SIZE(_rlPath);

  browseRL = new QPushButton(i18n("Browse..."), pathPage);
  browseRL->setAutoDefault(FALSE);
  MIN_SIZE(browseRL);

  RLs->addWidget(_rlPath, FULL);
  RLs->addWidget(browseRL, MINIMAL);

  // some connections
  connect(browseScripts, SIGNAL(clicked()), this, SLOT(getScriptDir()));
  connect(browseRL, SIGNAL(clicked()), this, SLOT(getRLDir()));

  pathLayout->activate();

  /*************/
  /* 2nd page  */
  /*************/
  QWidget* colorPage = new QWidget(this);
  _newColor = new KColorButton(colorPage);
  _changedColor = new KColorButton(colorPage);
  KSVCore::goodSize(_newColor);
  KSVCore::goodSize(_changedColor);

  _newColor->setAutoDefault(FALSE);
  _changedColor->setAutoDefault(FALSE);

  QLabel* newColorLabel = new QLabel(i18n("Color for &new entries"), colorPage);
  QLabel* changedColorLabel = new QLabel(i18n("Color for c&hanged entries"), colorPage);
  newColorLabel->setBuddy(_newColor);
  changedColorLabel->setBuddy(_changedColor);
  MIN_SIZE(newColorLabel);
  MIN_SIZE(changedColorLabel);
//   _newColor->setFixedHeight(newColorLabel->sizeHint().height());
//   _changedColor->setFixedHeight(changedColorLabel->sizeHint().height());
  GOOD_SIZE(_newColor);
  GOOD_SIZE(_changedColor);

  QBoxLayout* colorLayout = new QVBoxLayout(colorPage, HALF);
  QBoxLayout* newColorLayout = new QHBoxLayout();
  QBoxLayout* changedColorLayout = new QHBoxLayout();
  colorLayout->addLayout(newColorLayout, HALF);
  colorLayout->addLayout(changedColorLayout, HALF);

  newColorLayout->addWidget(newColorLabel, 0, AlignLeft);
  newColorLayout->addStretch(FULL);
  newColorLayout->addWidget(_newColor, 0, AlignRight);

  changedColorLayout->addWidget(changedColorLabel, 0, AlignLeft);
  changedColorLayout->addStretch(FULL);
  changedColorLayout->addWidget(_changedColor, 0, AlignRight);

  colorLayout->addStretch(2*FULL);
  colorLayout->activate();

  /*******************/
  /* TabDialog Setup */
  /*******************/
  addTab(pathPage, i18n("&Paths"));
  addTab(colorPage, i18n("&Colors"));

  setOkButton(i18n("OK"));
  setCancelButton(i18n("Cancel"));

  resize(minimumSize());
}

// Destructor

KSVPrefDlg::~KSVPrefDlg() {
  // nothing yet
}

void KSVPrefDlg::reReadConfig() {
  KSVConfig* conf = KSVCore::getConfig();

  _rlPath->setText(conf->getRLPath());
  _scriptPath->setText(conf->getScriptPath());

  _newColor->setColor(conf->getNewColor());
  _changedColor->setColor(conf->getChangedColor());
}

const QString KSVPrefDlg::getRLPath() const {
  return _rlPath->text();
}

const QString KSVPrefDlg::getScriptPath() const {
  return _scriptPath->text();
}

void KSVPrefDlg::getScriptDir() {
  QString tmp = KFileDialog::getDirectory(_scriptPath->text());

  if (!tmp.isEmpty())
    _scriptPath->setText(tmp);

  _scriptPath->setFocus();
}

void KSVPrefDlg::getRLDir() {
  QString tmp = KFileDialog::getDirectory(_rlPath->text());

  if (!tmp.isEmpty())
    _rlPath->setText(tmp);

  _rlPath->setFocus();
}

const QColor KSVPrefDlg::getNewColor() const {
  return _newColor->color();
}

const QColor KSVPrefDlg::getChangedColor() const {
  return _changedColor->color();
}

