/*
    KDE Draw - a small graphics drawing program for the KDE
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

#include "debug.h"
#include "knew.h"


QList<KIconTemplate> *KIconTemplateContainer::templatelist = 0;
int KIconTemplateContainer::instances = 0;

void createStandardTemplates(QList<KIconTemplate> *list)
{
    const QString icondir = kapp->kde_datadir().copy() + "/kiconedit/pics/";

    KIconTemplate *it = new KIconTemplate;
    it->path = icondir + "standard.xpm";
    it->title = i18n("Standard file");
    list->append(it);

    it = new KIconTemplate;
    it->path = icondir + "source.xpm";
    it->title = i18n("Source file");
    list->append(it);

    it = new KIconTemplate;
    it->path = icondir + "compressed.xpm";
    it->title = i18n("Compressed file");
    list->append(it);

    it = new KIconTemplate;
    it->path = icondir + "folder.xpm";
    it->title = i18n("Standard folder");
    list->append(it);

    it = new KIconTemplate;
    it->path = icondir + "package.xpm";
    it->title = i18n("Standard package");
    list->append(it);

    it = new KIconTemplate;
    it->path = icondir + "mini-folder.xpm";
    it->title = i18n("Mini folder");
    list->append(it);

    it = new KIconTemplate;
    it->path = icondir + "mini-package.xpm";
    it->title = i18n("Mini package");
    list->append(it);
}

KIconTemplateContainer::KIconTemplateContainer() : QObject()
{
  QString path;
  instances++;
  debug("KIconTemplateContainer: instances %d", instances);
  if(templatelist)
    return;

  debug("KIconTemplateContainer: Creating templates");

  templatelist = new QList<KIconTemplate>;
  templatelist->setAutoDelete(true);

  QStrList names;
  KConfig *k = kapp->getConfig();
  k->setGroup("Templates");
  k->readListEntry("Names", names);
  for(int i = 0; i < (int)names.count(); i++)
  {
    KIconTemplate *it = new KIconTemplate;
    it->path = k->readEntry(names.at(i));
    it->title = names.at(i);
    //debug("Template: %s\n%s", names.at(i), path.data());
    templatelist->append(it);
  }

  if(templatelist->count() == 0)
  {
    createStandardTemplates(templatelist);
  }
}

KIconTemplateContainer::~KIconTemplateContainer()
{
  instances--;
  debug("KIconTemplateContainer: instances %d", instances);
  if(instances == 0)
  {
    debug("KIconTemplateContainer: Deleting templates");
    templatelist->clear();
    delete templatelist;
    debug("KIconTemplateContainer: Deleted templates");
  }
}

QList<KIconTemplate> *KIconTemplateContainer::getTemplates()
{
  KIconTemplateContainer t;
  return t.templatelist;
}

KIconListBoxItem::KIconListBoxItem( KIconTemplate *t )
   : QListBoxItem()
{
  //debug("KIconListBoxItem - %s, %s", t->path.data(), t->title.data());
  icontemplate = t;
  pm.load(t->path.data());
  setText( t->title.data() );
}

void KIconListBoxItem::paint( QPainter *p )    
{
  p->drawPixmap( 3, 0, pm );      
  QFontMetrics fm = p->fontMetrics();      
  int yPos;                       // vertical text position      
  if ( pm.height() < fm.height() )          
    yPos = fm.ascent() + fm.leading()/2;      
  else            
    yPos = pm.height()/2 - fm.height()/2 + fm.ascent();      
  p->drawText( pm.width() + 5, yPos, text() );    
}

int KIconListBoxItem::height(const QListBox *lb ) const    
{
  return QMAX( pm.height(), lb->fontMetrics().lineSpacing() + 1 );    
}
    
int KIconListBoxItem::width(const QListBox *lb ) const
{
  return pm.width() + lb->fontMetrics().width( text() ) + 6;
}

NewSelect::NewSelect(QWidget *parent) : QWidget( parent )
{
  wiz = (KWizard*)parent;
  grp = new QButtonGroup(this);
  connect( grp, SIGNAL(clicked(int)), SLOT(buttonClicked(int)));
  grp->setExclusive(true);
  rbscratch = new QRadioButton(i18n("Create from scratch"), grp);
  rbscratch->setFixedSize(rbscratch->sizeHint());
  rbtempl = new QRadioButton(i18n("Create from template"), grp);
  rbtempl->setFixedSize(rbtempl->sizeHint());

  ml = new QVBoxLayout(this, 10);
  ml->addWidget(grp, 10, AlignLeft);
  l = new QVBoxLayout(grp, 10);
  l->addWidget(rbscratch, 5, AlignLeft);
  l->addWidget(rbtempl, 5, AlignLeft);
  //grp->setMinimumSize(grp->childrenRect().size());
  l->activate();
  ml->activate();
}

NewSelect::~NewSelect()
{
  //should be deleted by QWidget destructor
  //debug("NewSelect - destructor");
  delete ml;
  delete l;
/*
  delete rbscratch;
  delete rbtempl;
  delete grp;
*/
  debug("NewSelect - destructor: done");
}

void NewSelect::buttonClicked(int id)
{
  //debug("Button: %d", id);
  emit iconopenstyle(id);
}

NewFromTemplate::NewFromTemplate(QWidget *parent) : QWidget( parent )
{
  wiz = (KWizard*)parent;
  grp = new QGroupBox(i18n("Templates:"), this);
  templates = new KIconListBox(grp);
  connect( templates, SIGNAL(highlighted(int)), SLOT(checkSelection(int)));
  ml = new QVBoxLayout(this, 10);
  ml->addWidget(grp, 10, AlignLeft);
  l = new QHBoxLayout(grp, 15);
  l->addWidget(templates);
  l->activate();
  ml->activate();
  QList<KIconTemplate> *tlist = KIconTemplateContainer::getTemplates();
  debug("NewFromTemplate: templates %u", tlist->count());
  for(int i = 0; i < (int)tlist->count(); i++)
    templates->insertItem(new KIconListBoxItem(tlist->at(i)));
}

NewFromTemplate::~NewFromTemplate()
{
  debug("NewFromTemplate - destructor");
  delete ml;
  delete l;
  //delete grp;
  debug("NewFromTemplate - destructor: done");
}

void NewFromTemplate::checkSelection(int)
{
  if(templates->currentItem() != -1)
    wiz->getOkButton()->setEnabled(true);
  else
    wiz->getOkButton()->setEnabled(false);
}

KNewIcon::KNewIcon(QWidget *parent) : KWizard( parent, 0, true)
{
  debug("KNewIcon - constructor");
  if(isTopLevel())
    debug("isTopLevel");
  else
    debug("not TopLevel");
  //hide();
  connect(this, SIGNAL(selected(int)), SLOT(checkPage(int)));
  openstyle = -1;
  setOkButton();
  getNextButton()->setEnabled(false);
  getOkButton()->setEnabled(false);
  setCancelButton();
  setEnableArrowButtons(true);

  select = new NewSelect(this);
  connect(select, SIGNAL(iconopenstyle(int)), SLOT(iconOpenStyle(int)));
  //scratch = new NewFromScratch(this);
  scratch = new KResize(this, 0, false, QSize(32, 32));
  connect(scratch, SIGNAL(validsize(bool)), SLOT(checkPage(bool)));
  scratch->hide();
  templ = new NewFromTemplate(this);
  templ->hide();

  p1 = new KWizardPage;
  p1->title = i18n("Select icontype");
  p1->w = select;
  p1->enabled = true;
  addPage(p1);

  p2 = new KWizardPage;
  p2->title = i18n("Create from scratch");
  p2->w = scratch;
  p2->enabled = true;
  addPage(p2);

  setCaption(i18n("Create new icon"));
  resize(400, 250);
}

KNewIcon::~KNewIcon()
{
  debug("KNewIcon - destructor");
  delete select;
  delete scratch;
  delete templ;
  //delete p1;
  //delete p2;
  debug("KNewIcon - destructor: done");
}

void KNewIcon::okClicked()
{
  if(openstyle == Blank)
    emit newicon(scratch->getSize());
  else
    emit newicon(templ->path());
  hide();
#ifdef KWIZARD_VERSION
  setResult(1);
  accept();
#endif
}

void KNewIcon::cancelClicked()
{
  hide();
#ifdef KWIZARD_VERSION
  setResult(0);
  reject();
#endif
}

void KNewIcon::iconOpenStyle(int style)
{
#ifdef KWIZARD_VERSION
  getNextButton()->setEnabled(true);
  openstyle = style;
  if(style == 0)
  {
    setPage(1, scratch);
    setPage(1, QString(i18n("Create from scratch")));
  }
  else
  {
    setPage(1, templ);
    setPage(1, QString(i18n("Create from template")));
  }
#endif
}

void KNewIcon::checkPage(int p)
{
  if(p == 0)
    getOkButton()->setEnabled(false);
  else if(openstyle == Blank)
    scratch->checkValue(0);
  else
    templ->checkSelection(0);
}

void KNewIcon::checkPage(bool ok)
{
  if(ok && openstyle == Blank)
    getOkButton()->setEnabled(true);
  else
    getOkButton()->setEnabled(false);
}


