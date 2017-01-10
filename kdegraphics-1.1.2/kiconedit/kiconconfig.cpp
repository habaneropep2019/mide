/*
    KDE Icon Editor - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#include <kcolordlg.h>
#include "debug.h"
#include "kiconconfig.h"
#include "kiconfiledlg.h"
#include "pics/logo.xpm"

KTemplateEditDlg::KTemplateEditDlg(QWidget *parent) : QDialog(parent, 0, true)
{
  ok = cancel = browse = 0L;
  QGroupBox *grp = new QGroupBox(i18n("Template"), this);
  lb_name = new QLabel(i18n("Description:"), this);
  lb_name->setFixedHeight(lb_name->sizeHint().height());
  lb_path = new QLabel(i18n("Path:"), this);
  lb_path->setFixedHeight(lb_path->sizeHint().height());

  ln_name = new QLineEdit(this);
  ln_name->setFixedHeight(20);
  connect( ln_name, SIGNAL(textChanged(const char *)), SLOT(slotTextChanged(const char*)) );

  ln_path = new QLineEdit(this);
  ln_path->setFixedHeight(20);
  connect( ln_path, SIGNAL(textChanged(const char *)), SLOT(slotTextChanged(const char*)) );

  QBoxLayout *ml = new QVBoxLayout(this, 10);
  //ml->addWidget(grp, 10, AlignLeft);
  QBoxLayout *l = new QVBoxLayout(grp, 25);
  l->addWidget(lb_name);
  l->addWidget(ln_name);
  l->addWidget(lb_path);
  l->addWidget(ln_path);

  KButtonBox *bbox = new KButtonBox( this );
	
  ok = bbox->addButton( i18n("&OK") );
  connect( ok, SIGNAL(clicked()), SLOT(accept()) );
  ok->setEnabled(false);
	
  cancel = bbox->addButton( i18n("&Cancel") );
  connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
	
  bbox->addStretch( 10 );
		
  browse = bbox->addButton( i18n("&Browse...") );
  connect( browse, SIGNAL(clicked()), SLOT(slotBrowse()) );
		
  bbox->layout();
  bbox->setMaximumHeight(bbox->sizeHint().height());

  ml->addWidget(grp, 10, AlignLeft);
  ml->addWidget( bbox );

  l->activate();
  ml->activate();
}

void KTemplateEditDlg::slotTextChanged(const char *)
{
  QString name = ln_name->text(), path = ln_path->text();
  if(name.length() && path.length())
  {
    if(ok) ok->setEnabled(true);
  }
  else
  {
    if(ok) ok->setEnabled(false);
  }
}

void KTemplateEditDlg::setName(QString name)
{
  ln_name->setText(name.data());
}

const char *KTemplateEditDlg::getName()
{
  return ln_name->text();
}

void KTemplateEditDlg::setPath(QString path)
{
  ln_path->setText(path.data());
}

const char *KTemplateEditDlg::getPath()
{
  return ln_path->text();
}

void KTemplateEditDlg::slotBrowse()
{
  QString path = KIconFileDlg::getOpenFileName("/", "*.xpm");
  if(!path.isEmpty())
    setPath(path);
}

KTemplateConfig::KTemplateConfig(QWidget *parent) : QWidget(parent)
{
  debug("KTemplateConfig constructor");
  btadd = btremove = btedit = 0L;
  grp = new QGroupBox(i18n("Templates:"), this);
  templates = new KIconListBox(grp);
  connect( templates, SIGNAL(highlighted(int)), SLOT(checkSelection(int)));

  ml = new QVBoxLayout(this);
  ml->addWidget(grp);
  l = new  QVBoxLayout(grp, 15);
  l->addWidget(templates);

  tlist = KIconTemplateContainer::getTemplates();

  for(int i = 0; i < (int)tlist->count(); i++)
    templates->insertItem(new KIconListBoxItem(tlist->at(i)));

  KButtonBox *bbox = new KButtonBox( grp );
	
  btadd = bbox->addButton( i18n("&Add") );
  connect( btadd, SIGNAL(clicked()), SLOT(add()) );
	
  btedit = bbox->addButton( i18n("&Edit") );
  connect( btedit, SIGNAL(clicked()), SLOT(edit()) );
  btedit->setEnabled(false);
	
  btremove = bbox->addButton( i18n("&Remove") );
  connect( btremove, SIGNAL(clicked()), SLOT(remove()) );
  btremove->setEnabled(false);
		
  bbox->addStretch( 10 );
		
  bbox->layout();
  bbox->setMaximumHeight(bbox->sizeHint().height());
  l->addWidget( bbox );

  l->activate();
  ml->activate();

  debug("KTemplateConfig constructor - done");
}

void KTemplateConfig::saveSettings()
{
  debug("KTemplateConfig::saveSettings");

  for(int i = 0; i < (int)templates->count(); i++)
  {
    if(!tlist->containsRef(templates->iconTemplate(i)))
    {
      KIconTemplate *it = new KIconTemplate;
      it->path = templates->path(i);
      //it->path.detach();
      it->title = templates->text(i);
      //it->title.detach();
      tlist->append(it);
      debug("Name: %s\nPath: %s", templates->text(i), templates->path(i).data());
      debug("Name: %s\nPath: %s", it->path.data(), it->path.data());
    }
  }
  debug("KTemplateConfig::saveSettings - done");

}

void KTemplateConfig::checkSelection(int)
{
  debug("KTemplateConfig::checkSelection");
  if(templates->currentItem() != -1)
  {
    if(btremove) btremove->setEnabled(true);
    if(btedit) btedit->setEnabled(true);
  }
  else
  {
    if(btremove) btremove->setEnabled(false);
    if(btedit) btedit->setEnabled(false);
  }
  debug("KTemplateConfig::checkSelection - done");
}

void KTemplateConfig::remove()
{
  templates->removeItem(templates->currentItem());
}

void KTemplateConfig::add()
{
  KTemplateEditDlg dlg(this);
  if(dlg.exec())
  {
    KIconTemplate *it = new KIconTemplate;
    it->path = dlg.getPath();
    it->title = dlg.getName();
    //tlist->append(it);
    templates->insertItem(new KIconListBoxItem(it));
  }
}

void KTemplateConfig::edit()
{
  KTemplateEditDlg dlg(this);
  dlg.setPath(templates->path(templates->currentItem()).data());
  dlg.setName(templates->text(templates->currentItem()));
  if(dlg.exec())
  {
    KIconTemplate *it = new KIconTemplate;
    it->path = dlg.getPath();
    it->title = dlg.getName();
    tlist->append(it);
    templates->insertItem(new KIconListBoxItem(it));
  }
}

KBackgroundConfig::KBackgroundConfig(QWidget *parent) : QWidget(parent)
{
  debug("KBackgroundConfig - constructor");
  initMetaObject();

  lb_ex = 0L;

  pprops = KIconEditProperties::getProperties(parent);
  color = pprops->backgroundcolor;
  pixpath = pprops->backgroundpixmap;
  pix.load(pixpath.data());
  if(pix.isNull())
  {
    debug("BGPIX: %s not valid!", pixpath.data());
    QPixmap pmlogo((const char**)logo);
    pix = pmlogo;
  }

  QGroupBox *grp1 = new QGroupBox(i18n("Color or pixmap"), this);

  btngrp = new QButtonGroup(grp1);
  connect( btngrp, SIGNAL(clicked(int)), SLOT(slotBackgroundMode(int)));
  btngrp->setExclusive(true);
  btngrp->setFrameStyle(QFrame::NoFrame);

  int w = 0, h = 0;
  QRadioButton *rbc = new QRadioButton(i18n("Colored background"), btngrp);
  rbc->setFixedSize(rbc->sizeHint());
  w = rbc->width();
  h = rbc->height();

  QRadioButton *rbp = new QRadioButton(i18n("Pixmap background"), btngrp);
  rbp->setFixedSize(rbp->sizeHint());
  if(w < rbp->width())
    w = rbp->width();

  rbc->move(5, 5);
  rbp->move(5, 10+h);

  btngrp->setFixedSize(w+10, 15+(2*h));

  btngrp->insert(rbc, 0);
  btngrp->insert(rbp, 1);

  KButtonBox *bbox = new KButtonBox( grp1, KButtonBox::VERTICAL );

  btcolor = bbox->addButton( i18n("&Color...") );
  btcolor->setFixedSize(btcolor->sizeHint());

  btpix = bbox->addButton( i18n("&Pixmap...") );
  btpix->setFixedSize(btpix->sizeHint());

  bbox->setMaximumHeight(bbox->sizeHint().height());

  connect(btcolor, SIGNAL(clicked()), SLOT(selectColor()));
  connect(btpix, SIGNAL(clicked()), SLOT(selectPixmap()));

  QRadioButton *rbb = new QRadioButton(i18n("&Built-in"), grp1);
  rbb->setFixedSize(rbb->sizeHint());


  QGroupBox *grp2 = new QGroupBox(i18n("Example"), this);

  lb_ex = new QLabel(grp2);
  lb_ex->setMinimumHeight((btngrp->height()/2)*3);
  lb_ex->setFrameStyle(QFrame::Panel|QFrame::Sunken);

  QBoxLayout *ml = new QVBoxLayout(this);

  QBoxLayout *l1 = new  QHBoxLayout(grp1, 20);
  QBoxLayout *l1r = new  QVBoxLayout();
  QBoxLayout *l2 = new  QVBoxLayout(grp2, 15);

  l1->addWidget(btngrp, 0, AlignLeft);
  l1->addLayout(l1r);
  l1r->addWidget(bbox, 0, AlignLeft);
  l1r->addWidget(rbb, 0, AlignLeft);
  l2->addWidget(lb_ex);

  bgmode = pprops->backgroundmode;
  if(bgmode == QWidget::FixedPixmap)
  {
    btngrp->setButton(1);
    btcolor->setEnabled(false);
    lb_ex->setBackgroundPixmap(pix);
  }
  else
  {
    btngrp->setButton(0);
    btpix->setEnabled(false);
    lb_ex->setBackgroundColor(color);
  }

  ml->addWidget(grp1);
  ml->addWidget(grp2, 10);
  bbox->layout();
  l1->activate();
  l2->activate();
  ml->activate();

  debug("KBackgroundConfig - constructor done");
}

KBackgroundConfig::~KBackgroundConfig()
{

}

void KBackgroundConfig::slotBackgroundMode(int id)
{
  if(id == 0)
  {
    bgmode = QWidget::FixedColor;
    btpix->setEnabled(false);
    btcolor->setEnabled(true);
    if(lb_ex)
      lb_ex->setBackgroundColor(color);
  }
  else
  {
    bgmode = QWidget::FixedPixmap;
    btpix->setEnabled(true);
    btcolor->setEnabled(false);
    if(lb_ex)
      lb_ex->setBackgroundPixmap(pix);
  }
}

void KBackgroundConfig::saveSettings()
{
  debug("KBackgroundConfig::saveSettings");
  Properties *pprops = props(this);
  pprops->backgroundmode = bgmode;
  pprops->backgroundpixmap = pixpath;
  pprops->backgroundcolor = color;
  debug("KBackgroundConfig::saveSettings - done");
}

void KBackgroundConfig::selectColor()
{
  QColor c;
  if(KColorDialog::getColor(c))
  {
    lb_ex->setBackgroundColor(c);
    color = c;
  }
}

void KBackgroundConfig::selectPixmap()
{
  QString path = KIconFileDlg::getOpenFileName("/", "*.xpm");
  if(!path.isEmpty())
  {
    QPixmap p(path.data());
    if(!p.isNull())
    {
      lb_ex->setBackgroundPixmap(p);
      pixpath = path;
    }
  }
}

KToolBarConfig::KToolBarConfig(QWidget *parent) : QWidget(parent)
{
  debug("KToolBarConfig - constructor");
  initMetaObject();

  pprops = props(parent);

  btngrptool = new QButtonGroup(i18n("Main Toolbar"), this);
  btngrptool->setExclusive(true);

  btngrpdraw = new QButtonGroup(i18n("Drawing tools"), this);
  btngrpdraw->setExclusive(true);

  QLabel *lb = new QLabel(
      i18n("Note: Other settings of the toolbars can be reached\n"
           "from the toolbar \"handle\" or from the \"Options\" menu"), this);
  lb->setFixedSize(lb->sizeHint());

  QBoxLayout *ml = new  QVBoxLayout(this, 10);
  QBoxLayout *lh = new QHBoxLayout();

  ml->addLayout(lh);
  ml->addWidget(lb);

  QBoxLayout *l1 = new  QVBoxLayout(btngrptool, 10);
  QBoxLayout *l2 = new  QVBoxLayout(btngrpdraw, 10);

  lh->addWidget(btngrptool);
  lh->addWidget(btngrpdraw);

  l1->addSpacing(btngrptool->fontMetrics().height());
  l2->addSpacing(btngrptool->fontMetrics().height());

  rb = new QRadioButton(i18n("Show Icon"), btngrptool);
  rb->setFixedSize(rb->sizeHint());
  l1->addWidget(rb, 0, AlignLeft);
  //connect(rbp, SIGNAL(toggled(bool)), SLOT(pasteMode(bool)));

  rb = new QRadioButton(i18n("Show Icon and Text"), btngrptool);
  rb->setFixedSize(rb->sizeHint());
  l1->addWidget(rb, 0, AlignLeft);

  rb = new QRadioButton(i18n("Show Text"), btngrptool);
  rb->setFixedSize(rb->sizeHint());
  l1->addWidget(rb, 0, AlignLeft);

  rb = new QRadioButton(i18n("Show Icon"), btngrpdraw);
  rb->setFixedSize(rb->sizeHint());
  l2->addWidget(rb, 0, AlignLeft);
  //connect(rbp, SIGNAL(toggled(bool)), SLOT(pasteMode(bool)));

  rb = new QRadioButton(i18n("Show Icon and Text"), btngrpdraw);
  rb->setFixedSize(rb->sizeHint());
  l2->addWidget(rb, 0, AlignLeft);

  rb = new QRadioButton(i18n("Show Text"), btngrpdraw);
  rb->setFixedSize(rb->sizeHint());
  l2->addWidget(rb, 0, AlignLeft);

  ml->addStretch(1);

  l1->activate();

  l2->activate();

  ml->activate();

  mstat = pprops->maintoolbartext;
  dstat = pprops->drawtoolbartext;
  btngrptool->setButton(mstat);
  btngrpdraw->setButton(dstat);

  connect(btngrptool, SIGNAL(clicked(int)), SLOT(mainToolBar(int)));
  connect(btngrpdraw, SIGNAL(clicked(int)), SLOT(drawToolBar(int)));

  debug("KToolBarConfig - constructor done");
}

KToolBarConfig::~KToolBarConfig()
{

}

void KToolBarConfig::saveSettings()
{
  debug("KToolBarConfig::saveSettings");

  pprops->maintoolbartext = mstat;
  pprops->drawtoolbartext = dstat;

  pprops->maintoolbar->setIconText(mstat);
  pprops->drawtoolbar->setIconText(dstat);

  debug("KToolBarConfig::saveSettings - done");
}

void KToolBarConfig::drawToolBar(int i)
{
  dstat = i;
}

void KToolBarConfig::mainToolBar(int i)
{
  mstat = i;
}

KMiscConfig::KMiscConfig(QWidget *parent) : QWidget(parent)
{
  debug("KMiscConfig - constructor");
  initMetaObject();

  pprops = props(parent);

  QGroupBox *grp1 = new QGroupBox(i18n("Paste mode"), this);

  QRadioButton *rbp = new QRadioButton(i18n("Paste transparent pixels"), grp1);
  rbp->setFixedSize(rbp->sizeHint());
  connect(rbp, SIGNAL(toggled(bool)), SLOT(pasteMode(bool)));

  QRadioButton *rbr = new QRadioButton(i18n("Show rulers"), grp1);
  rbr->setFixedSize(rbr->sizeHint());
  connect(rbr, SIGNAL(toggled(bool)), SLOT(showRulers(bool)));

  QBoxLayout *ml = new QVBoxLayout(this);

  QBoxLayout *l1 = new  QVBoxLayout(grp1, 20);

  l1->addWidget(rbp, 0, AlignLeft);
  l1->addWidget(rbr, 0, AlignLeft);

  pastemode = pprops->pastetransparent;
  if(pastemode)
    rbp->setChecked(true);

  showrulers = pprops->showrulers;
  if(showrulers)
    rbr->setChecked(true);

  ml->addWidget(grp1);
  ml->addStretch(1);
  l1->activate();
  ml->activate();

  debug("KMiscConfig - constructor done");
}

KMiscConfig::~KMiscConfig()
{

}

void KMiscConfig::saveSettings()
{
  debug("KMiscConfig::saveSettings");
  pprops->pastetransparent = pastemode;
  pprops->showrulers = showrulers;
  debug("KMiscConfig::saveSettings - done");
}

void KMiscConfig::pasteMode(bool mode)
{
  pastemode = mode;
}

void KMiscConfig::showRulers(bool mode)
{
  showrulers = mode;
}

KIconConfig::KIconConfig(QWidget *parent) : KNoteBook(parent, 0, true)
{
  pprops = KIconEditProperties::getProperties(parent);
  debug("KIconConfig - constructor");
  initMetaObject();
  setCaption("Configure ");
  //keys = k; //new KAccel(parent);
  //dict = new QDict<KKeyEntry>( k->keyDict() );
  //debug("Keys: %u", k->keyDict().count());
  //debug("Keys: %u", dict->count());
  //CHECK_PTR(dict);
  pprops->keydict = pprops->keys->keyDict();
  debug("Keys: %u", pprops->keydict.count());

  setCancelButton();
  setOkButton();
  //setDirectionsReflectsPage(true);
  setEnablePopupMenu(true);
  setEnableArrowButtons(true);

  debug("KIconConfig - constructor");

  keychooser = new KKeyChooser(&pprops->keydict, this);
  debug("KeyChooser created");
  keychooser->setMinimumSize(300, 275);

  debug("KIconConfig - constructor");

  connect( this, SIGNAL(cancelclicked()), SLOT(reject()) );
  connect( this, SIGNAL(okclicked()), SLOT(saveSettings()) );

  QTab *tab = new QTab;            // create a QTab to hold the tab data
  tab->label = i18n("Keys");
  tab->enabled = true;
  addTab( tab );
  KWizardPage *p = new KWizardPage;
  p->w = keychooser;
  p->title = i18n("Configure keys");
  p->enabled = true;
  addPage( p );

  temps = new KTemplateConfig(this);

  tab = new QTab;            // create a QTab to hold the tab data
  tab->label = i18n("Templates");
  tab->enabled = true;
  addTab( tab );
  p = new KWizardPage;
  p->w = temps;
  p->title = i18n("Configure Icon templates");
  p->enabled = true;
  addPage( p );

  backs = new KBackgroundConfig(this);

  tab = new QTab;            // create a QTab to hold the tab data
  tab->label = i18n("Appearance");
  tab->enabled = true;
  addTab( tab );
  p = new KWizardPage;
  p->w = backs;
  p->title = i18n("Configure background");
  p->enabled = true;
  addPage( p );

  toolbars = new KToolBarConfig(this);

  p = new KWizardPage;
  p->w = toolbars;
  p->title = i18n("Toolbars");
  p->enabled = true;
  addPage( p );

  misc = new KMiscConfig(this);

  tab = new QTab;            // create a QTab to hold the tab data
  tab->label = i18n("Miscellaneous");
  tab->enabled = true;
  addTab( tab );
  p = new KWizardPage;
  p->w = misc;
  p->title = i18n("Icon grid");
  p->enabled = true;
  addPage( p );

  setSizes();

  debug("KIconConfig - constructor - done");
}

KIconConfig::~KIconConfig()
{
  //delete dict;
}

void KIconConfig::saveSettings()
{
  debug("KIconEditConfig::saveSettings");

  pprops->keys->setKeyDict(pprops->keydict);
  debug("KIconEditConfig::saveSettings - keys saved");
  temps->saveSettings();
  backs->saveSettings();
  misc->saveSettings();
  toolbars->saveSettings();
  debug("KIconEditConfig::saveSettings - done");
  accept();
}


