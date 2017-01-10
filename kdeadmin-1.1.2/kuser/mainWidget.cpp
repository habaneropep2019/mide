#include "maindlg.h"
#include "mainWidget.moc"

#include "globals.h"
#include <qtooltip.h>
#include <kmsgbox.h>
#include <ktoolbar.h>
#include <kiconloader.h>

#include "misc.h"

mainWidget::mainWidget(const char *name) : KTMainWindow(name) {
  md = new mainDlg(this);
  md->init();

  QPopupMenu *file = new QPopupMenu;
  CHECK_PTR( file );
  file->insertItem(i18n("&Save"),  md, SLOT(save()) );
  file->insertSeparator();
  file->insertItem(i18n("&Preferences..."),  md, SLOT(properties()) );
  file->insertSeparator();
  file->insertItem(i18n("&Quit"),  md, SLOT(quit()) );

  QPopupMenu *user = new QPopupMenu;
  CHECK_PTR(user);
  user->insertItem(i18n("&Edit..."), md, SLOT(useredit()) );
  user->insertItem(i18n("&Delete..."), md, SLOT(userdel()) );
  user->insertItem(i18n("&Add..."), md, SLOT(useradd()) );
  user->insertItem(i18n("&Set password..."), md, SLOT(setpwd()) );

  QPopupMenu *group = new QPopupMenu;
  CHECK_PTR(group);
  group->insertItem(i18n("&Edit..."), md, SLOT(grpedit()) );
  group->insertItem(i18n("&Delete..."), md, SLOT(grpdel()) );
  group->insertItem(i18n("&Add..."), md, SLOT(grpadd()) );

  QString tmp;
  tmp.sprintf(i18n("KUser version %s\n"
		"KDE project\n"
		"This program was created by\n"
		"Denis Pershin\n"
		"dyp@inetlab.com\n"
		"Copyright 1997(c)"), _KU_VERSION);
  QPopupMenu *help = kapp->getHelpMenu(true, tmp);

  menubar = new KMenuBar( this );
  CHECK_PTR( menubar );
  menubar->insertItem(i18n("&File"), file );
  menubar->insertItem(i18n("&User"), user );
  menubar->insertItem(i18n("&Group"), group );
  menubar->insertSeparator();
  menubar->insertItem(i18n("&Help"), help );

  setMenu(menubar);

  toolbar = new KToolBar(this, "toolbar");
  QPixmap pix;

  pix = kapp->getIconLoader()->loadIcon("useradd.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(useradd()), TRUE, i18n("Add user"));
  pix = kapp->getIconLoader()->loadIcon("userdel.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(userdel()), TRUE, i18n("Delete user"));
  pix = kapp->getIconLoader()->loadIcon("useredit.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(useredit()), TRUE, i18n("Edit user"));
  
  toolbar->insertSeparator();
  
  pix = kapp->getIconLoader()->loadIcon("grpadd.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(grpadd()), TRUE, i18n("Add group"));
  pix = kapp->getIconLoader()->loadIcon("grpdel.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(grpdel()), TRUE, i18n("Delete group"));
  pix = kapp->getIconLoader()->loadIcon("grpedit.xpm");
  toolbar->insertButton(pix, 0, SIGNAL(clicked()), md, SLOT(grpedit()), TRUE, i18n("Edit group"));
  toolbar->setBarPos(KToolBar::Top);

  addToolBar(toolbar);
  
  sbar = new KStatusBar(this);
  sbar->insertItem("Reading config", 0);
  
  setStatusBar(sbar);

  setView(md);

  resize(500, 400);

  // restore geometry settings
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup( "Appearance" );
  QString geom = config->readEntry( "Geometry" );
  if (!geom.isEmpty()) {
    int width, height;
    sscanf( geom, "%dx%d", &width, &height );
    resize( width, height );
  }
  sbar->changeItem(i18n("Ready"), 0);
}

mainWidget::~mainWidget() {
  if (md)
    delete md;

  if (sbar)
    delete sbar;

  if (toolbar)
    delete toolbar;

  if (menubar)
    delete menubar;
}

void mainWidget::resizeEvent (QResizeEvent *) {
  // save size of the application window
  KConfig *config = KApplication::getKApplication()->getConfig();
  config->setGroup( "Appearance" );
  QString geom;
  geom.sprintf( "%dx%d", geometry().width(), geometry().height() );
  config->writeEntry( "Geometry", geom );
  updateRects();
}

void mainWidget::closeEvent (QCloseEvent *) {
  md->quit();
}
