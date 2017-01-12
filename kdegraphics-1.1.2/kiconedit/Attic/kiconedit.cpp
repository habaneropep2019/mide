/*
    kiconedit - a small graphics drawing program for the KDE
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

#include <qwhatsthis.h>
#include <qtooltip.h>
#include <kruler.h>
#include "debug.h"
#include "kiconedit.h"
#include "kdragsource.h"
#include "main.h"

KIconEdit::KIconEdit(const QImage image, const char *name)
 : KTMainWindow(name)
{
  init();
  img = image;
  img.detach();
  grid->load(&img);
  preview->setPixmap(grid->pixmap());
}

KIconEdit::KIconEdit(const char *xpm, const char *name)
 : KTMainWindow(name)
{
  init();
  icon->open(&img, xpm);
  preview->setPixmap(grid->pixmap());
}

void KIconEdit::init()
{
  initMetaObject();
  setCaption(kapp->getCaption());

  printer = new QPrinter;
  CHECK_PTR(printer);

  if(!windowList)
  {
    debug("KIconEdit::KIconEdit() Globalizing windowlist");
    windowList = (WindowList*)memberList;
  }

  kiprops = new KIconEditProperties(this);
  CHECK_PTR(kiprops);
  Properties *pprops = props(this);

  menubar = 0L;
  toolbar = 0L;
  drawtoolbar = 0L;
  palettetoolbar = 0L;
  statusbar = 0L;
  what = 0L;

  msgtimer = new QTimer(this);
  CHECK_PTR(msgtimer);
  connect( msgtimer, SIGNAL(timeout()), SLOT(slotClearStatusMessage()));

#ifdef KWIZARD_VERSION
  icontemplates = new KIconTemplateContainer;
  CHECK_PTR(icontemplates);
  debug("Templates created");
#endif

  gridview = new KGridView(&img, this); //viewport->viewport()); //KIconEditView(this);
  CHECK_PTR(gridview);
  gridview->setShowRulers(pprops->showrulers);

  grid = gridview->grid();

  debug("Grid created");
  grid->setGrid(pprops->showgrid);
  debug("Grid->setGrid done");
  grid->setCellSize(pprops->gridscaling);
  debug("Grid->setCellSize done");

  icon = new KIcon(this, &grid->image());
  CHECK_PTR(icon);

  dropzone = new KDNDDropZone( grid, DndURL);
  CHECK_PTR(dropzone);
  connect( dropzone, SIGNAL( dropAction( KDNDDropZone *) ), 
    this, SLOT( slotDropEvent( KDNDDropZone *) ) );

  dropsite = new KDropSite( grid );
  CHECK_PTR(dropsite);
  connect( dropsite, SIGNAL( dropAction( QDropEvent*) ), 
    this, SLOT( slotQDropEvent( QDropEvent*) ) );

  connect( dropsite, SIGNAL( dragLeave( QDragLeaveEvent*) ), 
    this, SLOT( slotQDragLeaveEvent( QDragLeaveEvent*) ) );

  connect( dropsite, SIGNAL( dragEnter( QDragEnterEvent*) ), 
    this, SLOT( slotQDragEnterEvent( QDragEnterEvent*) ) );

  setupStatusBar();
  setupMenuBar();
  setupToolBar();
  setupDrawToolBar();
  setupPaletteToolBar();
  setupWhatsThis();

  connect( icon, SIGNAL( saved()),
           SLOT(slotSaved()));
  connect( icon, SIGNAL( loaded(QImage *)),
     grid, SLOT(load(QImage *)));
  connect( icon, SIGNAL(opennewwin(const char *)),
           SLOT(slotNewWin(const char *)));
  connect(icon, SIGNAL(newname(const char *)),
           SLOT( slotUpdateStatusName(const char *)));
  connect(icon, SIGNAL(newmessage(const char *)),
           SLOT( slotUpdateStatusMessage(const char *)));

  connect( syscolors, SIGNAL( newcolor(uint)),
     grid, SLOT(setColorSelection(uint)));
  connect( customcolors, SIGNAL( newcolor(uint)),
     grid, SLOT(setColorSelection(uint)));

  connect( grid, SIGNAL( changed(const QPixmap &)),
    preview, SLOT(setPixmap(const QPixmap &)));
  connect( grid, SIGNAL( addingcolor(uint) ),
           SLOT(addColor(uint)));
  connect( grid, SIGNAL( colorschanged(uint, uint*) ),
           SLOT(addColors(uint, uint*)));

  connect(grid, SIGNAL(sizechanged(int, int)),
           SLOT( slotUpdateStatusSize(int, int)));
  connect(grid, SIGNAL(poschanged(int, int)),
           SLOT( slotUpdateStatusPos(int, int)));
  connect(grid, SIGNAL(scalingchanged(int, bool)),
           SLOT( slotUpdateStatusScaling(int, bool)));
  connect( grid, SIGNAL( addingcolor(uint) ),
           SLOT(slotUpdateStatusColors(uint)));
  connect(grid, SIGNAL(colorschanged(uint, uint*)),
           SLOT( slotUpdateStatusColors(uint, uint*)));
  connect(grid, SIGNAL(newmessage(const char *)),
           SLOT( slotUpdateStatusMessage(const char *)));
  connect(grid, SIGNAL(clipboarddata(bool)),
           SLOT( slotUpdatePaste(bool)));
  connect(grid, SIGNAL(selecteddata(bool)),
           SLOT( slotUpdateCopy(bool)));

  debug("Setting tool");
  slotTools(ID_DRAW_FIND);
  debug("Updating statusbar");
  slotUpdateStatusSize(grid->cols(), grid->rows());
  slotUpdateStatusScaling(grid->scaling(), true);
  if(icon->url().length())
    slotUpdateStatusName(icon->url().data());
  slotUpdateCopy(false);
  uint *c = 0, n = 0;
  n = grid->getColors(c);
  slotUpdateStatusColors(n, c);
  setView(gridview);

  if((pprops->winwidth > 0) && (pprops->winheight > 0))
    resize( pprops->winwidth, pprops->winheight );

  debug("Showing");
  show();
}

KIconEdit::~KIconEdit()
{
  debug("KIconEdit - Destructor");
/*  the following should be destructed by the QObject/QWidget destructor
    if(l)
      delete l;
    l = 0L; 
    if(grid)
      delete grid;
    grid = 0L; 
    if(toolsw)
      delete toolsw;
    toolsw = 0L; 
    if(newicon)
      delete newicon;
    newicon = 0L; 
*/
    if(printer)
      delete printer;
    if(what)
      delete what;
    what = 0L;
    if(toolbar)
      delete toolbar;
    toolbar = 0L; 
    if(drawtoolbar)
      delete drawtoolbar;
    drawtoolbar = 0L; 
    if(menubar)
      delete menubar;
    menubar = 0L; 
    if(statusbar)
      delete statusbar;
    statusbar = 0L; 
#ifdef KWIZARD_VERSION
    if(icontemplates)
      delete icontemplates;
    icontemplates = 0L;
#endif 
    delete file;
    delete edit;
    delete view;
    delete tools;
    delete options;
    delete help;
    delete select;
    delete recent;
    delete zoom;

  debug("KIconEdit - Destructor: done");
}

void KIconEdit::closeEvent(QCloseEvent *e)
{
  bool cancel = false;
  if (grid->isModified()) 
  {
    KWM::activate(winId());
    int r = KMsgBox::yesNoCancel(this, i18n("Warning"),
		i18n("The current file has been modified.\nDo you want to save it?"));
    switch(r)
    {
      case 1:
        icon->save(&grid->image());
        //grid->save(grid->drawPath());
        break;
      case 2:
        break;
      case 3:
        cancel = true;
        break;
      default:
        break;
    }
  }
  if(!cancel)
  {
    writeConfig();
    icon->cleanup();
    e->accept();
    delete this;
    if(memberList->count() == 0)
    {
      debug("KIconEdit: Last in list closes the door...");
      kapp->quit();
    }
  }
  else
    e->ignore();
}

void KIconEdit::resizeEvent( QResizeEvent * )
{
  // save size of the application window
  //debug("KIconEdit::resizeEvent()");
  props(this)->winwidth = geometry().width();
  props(this)->winheight = geometry().height();

  updateRects();
  //viewport->updateScrollBars();
}

void KIconEdit::unsaved(bool flag)
{
    setUnsavedData(flag);
}

void KIconEdit::saveGoingDownStatus()
{
  debug("KIconEdit::saveGoingDownStatus");
  if( grid->isModified() )
  {
    debug("Saving backup for modified file %s", icon->url().data());
    icon->saveBackup(&grid->image());
  }
  else
    icon->cleanup();
  debug("KIconEdit::saveGoingDownStatus - done");
}

// this is for exit by request of the session manager
void KIconEdit::saveProperties(KConfig *config )
{
  debug("KIconEdit::saveProperties");
  config->writeEntry("Name", icon->url());
  saveGoingDownStatus();
}

// this is for instances opened by the session manager
void KIconEdit::readProperties(KConfig *config)
{
  debug("KIconEdit::readProperties");
  QString entry = config->readEntry("Name", ""); // no default
  if (entry.isEmpty())
    return;
  icon->open(&grid->image(), entry.data());
}

// this is for normal exits or request from "Options->Save options".
void KIconEdit::writeConfig()
{
/*
  Properties *pprops = props(this);
  
  pprops->maintoolbarstat = toolbar->isVisible();
  pprops->drawtoolbarstat = drawtoolbar->isVisible();
  pprops->statusbarstat = statusbar->isVisible();

  pprops->maintoolbarpos = toolbar->barPos();
  pprops->drawtoolbarpos = drawtoolbar->barPos();
  pprops->menubarpos = menubar->menuBarPos();

  //pprops->showgrid = grid->hasGrid();
  //pprops->gridscaling = grid->cellSize();
*/
  KIconEditProperties::saveProperties(this);

}

QSize KIconEdit::sizeHint() const
{
  if(gridview)
    return gridview->sizeHint();
  else
    return QSize(-1, -1);
}

bool KIconEdit::setupMenuBar()
{
  debug("setupMenuBar");

  Properties *pprops = props(this);

  if(!menubar)
  {
    menubar = new KMenuBar(this);
    CHECK_PTR(menubar);
    setMenu(menubar);
    pprops->menubar = menubar;
  }

  KAccel *keys = pprops->keys; // = new KAccel( this ); 
  CHECK_PTR(keys);

  keys->connectItem( KAccel::New, this, SLOT(slotNew()) );
  keys->connectItem( KAccel::Save , this, SLOT(slotSave()) );
  keys->connectItem( KAccel::Quit, this, SLOT(slotQuit()) );
  keys->connectItem( KAccel::Cut , this, SLOT(slotCut()) );
  keys->connectItem( KAccel::Copy , this, SLOT(slotCopy()) );
  keys->connectItem( KAccel::Paste , this, SLOT(slotPaste()) );
  keys->connectItem( KAccel::Open , this, SLOT(slotOpen()) );
  keys->connectItem( KAccel::Close , this, SLOT(slotClose()) );
  keys->connectItem( KAccel::Print , this, SLOT(slotPrint()) );
  keys->insertItem(i18n("Select All"),   "Select All",   CTRL+Key_A);
  keys->connectItem( "Select All", this, SLOT(slotSelectAll()) );
  keys->readSettings();

  debug("setupMenuBar - read settings");


  recent = new QPopupMenu;
  CHECK_PTR(recent);
  connect( recent, SIGNAL(activated(int)), SLOT(slotOpenRecent(int)));
  for(uint i = 0; i < pprops->recentlist->count(); i++)
    recent->insertItem(pprops->recentlist->at(i));

  int id;

  file = new QPopupMenu;
  CHECK_PTR(file);

  file->insertItem(Icon("newwin.xpm"), i18n("New &Window"), ID_FILE_NEWWIN);
  file->connectItem(ID_FILE_NEWWIN, this, SLOT(slotNewWin()));

  file->insertSeparator();

  id = file->insertItem(Icon("filenew.xpm"), i18n("&New..."), ID_FILE_NEWFILE);
  file->connectItem(ID_FILE_NEWFILE, this, SLOT(slotNew()));

  id = file->insertItem(Icon("fileopen.xpm"), i18n("&Open..."), ID_FILE_OPEN);
  file->connectItem(ID_FILE_OPEN, this, SLOT(slotOpen()));

  file->insertItem(i18n("Open recent"), recent, ID_FILE_RECENT);
  //file->setItemEnabled(ID_FILE_RECENT, false);

  file->insertSeparator();

  id = file->insertItem(Icon("filefloppy.xpm"), i18n("&Save"), ID_FILE_SAVE);
  file->connectItem(ID_FILE_SAVE, this, SLOT(slotSave()));

  file->insertItem(i18n("S&ave as..."), ID_FILE_SAVEAS);
  file->connectItem(ID_FILE_SAVEAS, this, SLOT(slotSaveAs()));

  file->insertSeparator();

  id = file->insertItem(Icon("fileprint.xpm"), i18n("&Print..."), ID_FILE_PRINT);
  file->connectItem(ID_FILE_PRINT, this, SLOT(slotPrint()));

  file->insertSeparator();

  id = file->insertItem(Icon("fileclose.xpm"), i18n("&Close"), ID_FILE_CLOSE);
  file->connectItem(ID_FILE_CLOSE, this, SLOT(slotClose()));

  id = file->insertItem(i18n("&Exit"), ID_FILE_QUIT);
  file->connectItem(ID_FILE_QUIT, this, SLOT(slotQuit()));

  menubar->insertItem(i18n("&File"), file);

  edit = new QPopupMenu;
  CHECK_PTR(edit);

  id = edit->insertItem(Icon("editcut.xpm"), i18n("Cu&t"), ID_EDIT_CUT);
  edit->connectItem(ID_EDIT_CUT, this, SLOT(slotCut()));

  id = edit->insertItem(Icon("editcopy.xpm"), i18n("&Copy"), ID_EDIT_COPY);
  edit->connectItem(ID_EDIT_COPY, this, SLOT(slotCopy()));

  id = edit->insertItem(Icon("editpaste.xpm"), i18n("&Paste"), ID_EDIT_PASTE);
  edit->connectItem(ID_EDIT_PASTE, this, SLOT(slotPaste()));

  id = edit->insertItem(i18n("Paste as &new"), ID_EDIT_PASTE_AS_NEW);
  edit->connectItem(ID_EDIT_PASTE_AS_NEW, grid, SLOT(editPasteAsNew()));

  id = edit->insertItem(i18n("Clea&r"),  this, SLOT(slotClear()));

  edit->insertSeparator();

  id = edit->insertItem(i18n("Select &all"), ID_EDIT_SELECT_ALL);
  edit->connectItem(ID_EDIT_SELECT_ALL, this, SLOT(slotSelectAll()));

  menubar->insertItem(i18n("&Edit"), edit);

  zoom = new QPopupMenu;
  CHECK_PTR(zoom);
  id = zoom->insertItem(i18n("1:1"), ID_VIEW_ZOOM_1TO1);
  id = zoom->insertItem(i18n("1:5"), ID_VIEW_ZOOM_1TO5);
  id = zoom->insertItem(i18n("1:10"), ID_VIEW_ZOOM_1TO10);
  connect( zoom, SIGNAL(activated(int)), SLOT(slotView(int)));

  view = new QPopupMenu;
  CHECK_PTR(view);
  id = view->insertItem(Icon("viewmag+.xpm"), i18n("Zoom &in"), ID_VIEW_ZOOM_IN);
  id = view->insertItem(Icon("viewmag-.xpm"), i18n("Zoom &out"), ID_VIEW_ZOOM_OUT);
  id = view->insertItem(Icon("viewmag.xpm"), i18n("Zoom factor"), zoom);
  connect( view, SIGNAL(activated(int)), SLOT(slotView(int)));
  menubar->insertItem(i18n("&View"), view);

  image = new QPopupMenu;
  CHECK_PTR(image);
#if QT_VERSION >= 140
  id = image->insertItem(Icon("transform.xpm"), i18n("&Resize"), ID_IMAGE_RESIZE);
#endif
  id = image->insertItem(Icon("grayscale.xpm"), i18n("&GrayScale"), ID_IMAGE_GRAYSCALE);
  
  //id = image->insertItem(Icon("kdepalette.xpm"), i18n("&Map to KDE palette"), ID_IMAGE_MAPTOKDE);
  
  connect( image, SIGNAL(activated(int)), SLOT(slotImage(int)));
  menubar->insertItem(i18n("&Icon"), image);

  tools = new QPopupMenu;
  CHECK_PTR(tools);
  tools->insertItem(Icon("paintbrush.xpm"), i18n("Freehand"), ID_DRAW_FREEHAND);
  tools->insertItem(Icon("rectangle.xpm"), i18n("Rectangle"), ID_DRAW_RECT);
  tools->insertItem(Icon("filledrectangle.xpm"), i18n("Filled rectangle"), ID_DRAW_RECT_FILL);
  tools->insertItem(Icon("circle.xpm"), i18n("Circle"), ID_DRAW_CIRCLE);
  tools->insertItem(Icon("filledcircle.xpm"), i18n("Filled circle"), ID_DRAW_CIRCLE_FILL);
  tools->insertItem(Icon("ellipse.xpm"), i18n("Ellipse"), ID_DRAW_ELLIPSE);
  tools->insertItem(Icon("filledellipse.xpm"), i18n("Filled ellipse"), ID_DRAW_ELLIPSE_FILL);
  tools->insertItem(Icon("spraycan.xpm"), i18n("Spray"), ID_DRAW_SPRAY);
  tools->insertItem(Icon("flood.xpm"), i18n("Flood fill"), ID_DRAW_FILL);
  tools->insertItem(Icon("line.xpm"), i18n("Line"), ID_DRAW_LINE);
  tools->insertItem(Icon("eraser.xpm"), i18n("Eraser (Transparent)"), ID_DRAW_ERASE);
  //tools->insertSeparator();
  connect( tools, SIGNAL(activated(int)), SLOT(slotTools(int)));
  menubar->insertItem(i18n("&Tools"), tools);

  options = new QPopupMenu;
  CHECK_PTR(options);
  id = options->insertItem(i18n("&Configure"), ID_OPTIONS_CONFIGURE);
  options->insertSeparator();
  options->insertItem(i18n("Toggle &Grid"), ID_OPTIONS_TOGGLE_GRID);
  if(pprops->showgrid)
    options->setItemChecked(ID_OPTIONS_TOGGLE_GRID, true);
  options->insertItem(i18n("Toggle &toolbar"), ID_OPTIONS_TOGGLE_TOOL1);
  if(pprops->maintoolbarstat)
    options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL1, true);
  options->insertItem(i18n("Toggle &drawing tools"), ID_OPTIONS_TOGGLE_TOOL2);
  if(pprops->drawtoolbarstat)
    options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL2, true);
  options->insertItem(i18n("Toggle &statusbar"), ID_OPTIONS_TOGGLE_STATS);
  if(pprops->statusbarstat)
    options->setItemChecked(ID_OPTIONS_TOGGLE_STATS, true);
  options->insertSeparator();
  options->insertItem(Icon("filefloppy.xpm"), i18n("&Save options"), ID_OPTIONS_SAVE);
  connect( options, SIGNAL(activated(int)), SLOT(slotConfigure(int)));
  menubar->insertItem(i18n("&Options"), options);

  menubar->insertSeparator();

  QString about;
  about.sprintf(i18n("KDE Icon Editor\n\n"
                     "A graphics drawing program for creating\n"
                     "icons using the KDE icon palette\n\n"
                     "Copyright 1998 by Thomas Tanghus\n"
                     "tanghus@kde.org\n"
                     "http://w1.1358.telia.com/~u135800018"));

  help = kapp->getHelpMenu(true,about.data());
  menubar->insertItem(i18n("&Help"), help);

  menubar->setMenuBarPos(pprops->menubarpos);
  updateMenuAccel();
  menubar->show();
  //connect( menubar, SIGNAL(activated(int)), SLOT(slotActions(int)));

  debug("setupMenuBar - done");
  return true;
}

void KIconEdit::updateMenuAccel()
{
  KAccel *keys = props(this)->keys; // = new KAccel( this ); 
  CHECK_PTR(keys);

  keys->changeMenuAccel(file, ID_FILE_NEWFILE, KAccel::New); 
  keys->changeMenuAccel(file, ID_FILE_OPEN, KAccel::Open); 
  keys->changeMenuAccel(file, ID_FILE_SAVE, KAccel::Save); 
  keys->changeMenuAccel(file, ID_FILE_PRINT, KAccel::Print); 
  keys->changeMenuAccel(file, ID_FILE_CLOSE, KAccel::Close); 
  keys->changeMenuAccel(file, ID_FILE_QUIT, KAccel::Quit); 

  keys->changeMenuAccel(edit, ID_EDIT_CUT, KAccel::Cut); 
  keys->changeMenuAccel(edit, ID_EDIT_COPY, KAccel::Copy); 
  keys->changeMenuAccel(edit, ID_EDIT_PASTE, KAccel::Paste); 
  keys->changeMenuAccel(edit, ID_EDIT_SELECT_ALL, "Select All"); 

}

bool KIconEdit::setupToolBar()
{
  Properties *pprops = props(this);
  debug("setupToolBar");

  toolbar = new KToolBar(this);
  CHECK_PTR(toolbar);
  addToolBar(toolbar);
  pprops->maintoolbar = toolbar;

  what = new QWhatsThis;
  QWidget *btwhat = (QWidget*)what->whatsThisButton(toolbar);
  QToolTip::add(btwhat, i18n("What is ...?"));

  QImage i;
  QPixmap p(Icon("image.xpm"));
  i = p;
  i = i.smoothScale(20, 20);
  p = i;
  dragbutton = new KDragSource("image/x-xpm", grid, SLOT(getImage(QImage*)), toolbar);
  dragbutton->setPixmap(p);
  QToolTip::add(dragbutton, i18n("Drag source"));

  toolbar->insertWidget(-1, 22, dragbutton);
  toolbar->insertSeparator();
  toolbar->insertButton(Icon("filenew.xpm"), ID_FILE_NEWFILE,
         SIGNAL(clicked()), this, SLOT(slotNew()), TRUE, i18n("New File"));
  toolbar->insertButton(Icon("fileopen.xpm"),ID_FILE_OPEN,
         SIGNAL(clicked()), this, SLOT(slotOpen()), TRUE, i18n("Open a file"));
  toolbar->insertButton(Icon("filefloppy.xpm"), ID_FILE_SAVE,
         SIGNAL(clicked()), this, SLOT(slotSave()), TRUE, i18n("Save the file"));
  toolbar->insertSeparator();
  toolbar->insertButton(Icon("fileprint.xpm"),ID_FILE_PRINT,
         SIGNAL(clicked()), this, SLOT(slotPrint()), TRUE, i18n("Print icon"));
  toolbar->insertSeparator();
  toolbar->insertButton(Icon("editcut.xpm"),ID_EDIT_CUT,
         SIGNAL(clicked()), this, SLOT(slotCut()), TRUE, i18n("Cut"));
  toolbar->insertButton(Icon("editcopy.xpm"),ID_EDIT_COPY,
         SIGNAL(clicked()), this, SLOT(slotCopy()), TRUE, i18n("Copy"));
  toolbar->insertButton(Icon("editpaste.xpm"),ID_EDIT_PASTE,
         SIGNAL(clicked()), this, SLOT(slotPaste()), TRUE, i18n("Paste"));

  select = new QPopupMenu;
  CHECK_PTR(select);
  select->insertItem(Icon("selectrect.xpm"), ID_SELECT_RECT);
  select->insertItem(Icon("selectcircle.xpm"), ID_SELECT_CIRCLE);
  connect( select, SIGNAL(activated(int)), SLOT(slotTools(int)));

  toolbar->insertButton(Icon("areaselect.xpm"), ID_SELECT, select, true, i18n("Select area"));
  //drawtoolbar->setToggle(ID_DRAW_SELECT, true);

  toolbar->insertSeparator();
#if QT_VERSION >= 140
  toolbar->insertButton(Icon("transform.xpm"),ID_IMAGE_RESIZE, TRUE, 
			  i18n("Resize"));
#endif
  toolbar->insertButton(Icon("grayscale.xpm"),ID_IMAGE_GRAYSCALE, TRUE, 
			  i18n("GrayScale"));
/*
  toolbar->insertButton(Icon("kdepalette.xpm"),ID_IMAGE_MAPTOKDE, TRUE, 
			  i18n("To KDE palette"));
*/
  toolbar->insertSeparator();
  toolbar->insertButton(Icon("viewmag-.xpm"),ID_VIEW_ZOOM_OUT, TRUE, 
			  i18n("Zoom out"));
  toolbar->setDelayedPopup(ID_VIEW_ZOOM_OUT, zoom);
  toolbar->insertButton(Icon("viewmag+.xpm"),ID_VIEW_ZOOM_IN, TRUE, 
			  i18n("Zoom in"));
  toolbar->setDelayedPopup(ID_VIEW_ZOOM_IN, zoom);
  toolbar->insertSeparator();
  toolbar->insertButton(Icon("grid.xpm"),ID_OPTIONS_TOGGLE_GRID, TRUE, 
			  i18n("Toggle grid"));
  toolbar->setToggle(ID_OPTIONS_TOGGLE_GRID, true);
  if(pprops->showgrid)
    ((KToolBarButton*)toolbar->getButton(ID_OPTIONS_TOGGLE_GRID))->on(true);

  toolbar->insertSeparator();

  toolbar->insertWidget(ID_HELP_WHATSTHIS, btwhat->sizeHint().width(), btwhat);

  toolbar->insertButton(Icon("newwin.xpm"),ID_FILE_NEWWIN,
         SIGNAL(clicked()), this, SLOT(slotNewWin()), TRUE,
                          i18n("New Window"));
  toolbar->alignItemRight( ID_FILE_NEWWIN, true);
    
  toolbar->setIconText(pprops->maintoolbartext);
  toolbar->setBarPos(pprops->maintoolbarpos);
  if(pprops->maintoolbarstat)
    toolbar->enable(KToolBar::Show);
  else
    toolbar->enable(KToolBar::Hide);
  //toolbar->show();
  connect( toolbar, SIGNAL(clicked(int)), SLOT(slotView(int)));
  connect( toolbar, SIGNAL(clicked(int)), SLOT(slotImage(int)));
  connect( toolbar, SIGNAL(clicked(int)), SLOT(slotConfigure(int)));

  debug("setupToolBar - done");
  return true;
}

bool KIconEdit::setupDrawToolBar()
{
  debug("setupDrawToolBar");
  Properties *pprops = props(this);
  drawtoolbar = new KToolBar(this);
  CHECK_PTR(drawtoolbar);
  addToolBar(drawtoolbar);
  pprops->drawtoolbar = drawtoolbar;
  drawtoolbar->setFullWidth();

  drawtoolbar->insertButton(Icon("pointer.xpm"), ID_DRAW_FIND, TRUE, i18n("Find pixel"));
  drawtoolbar->setToggle(ID_DRAW_FIND, true);
  drawtoolbar->insertButton(Icon("paintbrush.xpm"), ID_DRAW_FREEHAND, TRUE, i18n("Draw freehand"));
  drawtoolbar->setToggle(ID_DRAW_FREEHAND, true);

  drawtoolbar->insertButton(Icon("line.xpm"), ID_DRAW_LINE, TRUE, i18n("Draw line"));
  drawtoolbar->setToggle(ID_DRAW_LINE, true);
  drawtoolbar->insertButton(Icon("rectangle.xpm"),ID_DRAW_RECT, TRUE, i18n("Draw rectangle"));
  drawtoolbar->setToggle(ID_DRAW_RECT, true);
  drawtoolbar->insertButton(Icon("filledrectangle.xpm"),ID_DRAW_RECT_FILL, TRUE, i18n("Draw filled rectangle"));
  drawtoolbar->setToggle(ID_DRAW_RECT_FILL, true);
  drawtoolbar->insertButton(Icon("circle.xpm"),ID_DRAW_CIRCLE, TRUE, i18n("Draw circle"));
  drawtoolbar->setToggle(ID_DRAW_CIRCLE, true);
  drawtoolbar->insertButton(Icon("filledcircle.xpm"),ID_DRAW_CIRCLE_FILL, TRUE, i18n("Draw filled circle"));
  drawtoolbar->setToggle(ID_DRAW_CIRCLE_FILL, true);
  drawtoolbar->insertButton(Icon("ellipse.xpm"),ID_DRAW_ELLIPSE, TRUE, i18n("Draw ellipse"));
  drawtoolbar->setToggle(ID_DRAW_ELLIPSE, true);
  drawtoolbar->insertButton(Icon("filledellipse.xpm"),ID_DRAW_ELLIPSE_FILL, TRUE, i18n("Draw filled ellipse"));
  drawtoolbar->setToggle(ID_DRAW_ELLIPSE_FILL, true);
  drawtoolbar->insertButton(Icon("spraycan.xpm"),ID_DRAW_SPRAY, TRUE, i18n("Spray"));
  drawtoolbar->setToggle(ID_DRAW_SPRAY, true);
  drawtoolbar->insertButton(Icon("flood.xpm"),ID_DRAW_FILL, TRUE, i18n("Flood fill"));
  drawtoolbar->setToggle(ID_DRAW_FILL, true);
  drawtoolbar->setToggle(ID_DRAW_SPRAY, true);
  drawtoolbar->insertButton(Icon("eraser.xpm"),ID_DRAW_ERASE, TRUE, i18n("Erase"));
  drawtoolbar->setToggle(ID_DRAW_ERASE, true);
    
  drawtoolbar->setIconText(pprops->drawtoolbartext);
  drawtoolbar->setBarPos(pprops->drawtoolbarpos);
  if(pprops->drawtoolbarstat)
    drawtoolbar->enable(KToolBar::Show);
  else
    drawtoolbar->enable(KToolBar::Hide);
  //drawtoolbar->show();
  connect( drawtoolbar, SIGNAL(clicked(int)), SLOT(slotTools(int)));

  debug("setupDrawToolBar - done");
  return true;
}

bool KIconEdit::setupPaletteToolBar()
{
  debug("setupPaletteToolBar");
  //Properties *pprops = props(this);

  QWidget *w = new QWidget(this);
  QBoxLayout *ml = new QVBoxLayout(w);
  QLabel *l = new QLabel(i18n("System Colors"), w);
  l->setFixedSize(l->sizeHint());
  syscolors = new KSysColors(w);
  syscolors->setFixedSize(syscolors->width(), syscolors->height());
  ml->addWidget(l);
  ml->addWidget(syscolors);
  ml->activate();

  int ow = w->sizeHint().width();
  int iw = syscolors->width();

  palettetoolbar = new KToolBar(this, 0, ow+8);
  CHECK_PTR(palettetoolbar);
  addToolBar(palettetoolbar);
  palettetoolbar->enable(KToolBar::Show);
  palettetoolbar->setBarPos(KToolBar::Right);

  w->recreate(palettetoolbar, 0, QPoint(0, 0));

  preview = new Preview(palettetoolbar);
  preview->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  preview->setFixedSize(iw, 60);

  palettetoolbar->insertWidget(ID_PREVIEW, iw, preview);
  palettetoolbar->insertSeparator();
  palettetoolbar->insertWidget(ID_SYSTEM_COLORS, iw, w);
  palettetoolbar->insertSeparator();

  w = new QWidget(palettetoolbar);
  ml = new QVBoxLayout(w);
  l = new QLabel(i18n("Custom Colors"), w);
  l->setFixedSize(l->sizeHint());
  customcolors = new KCustomColors(w);
  customcolors->setFixedSize(iw, customcolors->height());
  ml->addWidget(l);
  ml->addWidget(customcolors);
  ml->activate();

  palettetoolbar->insertWidget(ID_CUSTOM_COLORS, iw, w);

  return true;
}

bool KIconEdit::setupStatusBar()
{
  Properties *pprops = props(this);
  statusbar = new KStatusBar(this);
  CHECK_PTR(statusbar);
  setStatusBar(statusbar);
  pprops->statusbar = statusbar;

  statusbar->insertItem("    -1, -1    ", 0);
  statusbar->insertItem("   32 x 32   ", 1);
  statusbar->insertItem(" 1:1000 ", 2);
  statusbar->insertItem("Colors:       ", 3);
  statusbar->insertItem("", 4);

  if(pprops->statusbarstat)
    statusbar->enable(KStatusBar::Show);
  else
    statusbar->enable(KStatusBar::Hide);
  return true;
}

bool KIconEdit::setupWhatsThis()
{ // QWhatsThis *what is created in setupToolbar

  debug("setupWhatsThis");

  // Create help for the custom widgets

  QString str = i18n("Icon draw grid\n\nThe icon grid is the area where you draw the icons.\n"
                 "You can zoom in and out using the magnifying glasses on the toolbar.\n"
                 "(Tip: Hold the magnify button down for a few seconds to zoom to a predefined scale");
  what->add(grid, str.data());

  str = i18n("Rulers\n\nThis is a visual represantation of the current cursor position");
  what->add(gridview->hruler(), str.data());
  what->add(gridview->vruler(), str.data());

  str = i18n("Statusbar\n\nThe statusbar gives information of the status "
             "of the current icon. The fields are:\n\n"
             "\t- Cursor position\n"
             "\t- Size\n"
             "\t- Zoom factor\n"
             "\t- Number of colors\n"
             "\t- Application messages");
  what->add(statusbar, str.data());

  str = i18n("Preview\n\nThis is a 1:1 preview of the current icon");
  what->add(preview, str.data());

  str = i18n("System colors\n\nHere you can select colors from the KDE Icon Palette.");
  what->add(syscolors, str.data());

  str = i18n("Custom colors\n\nHere you can build a palette of custom colors.\n"
             "Just double-click on a box to edit the color");
  what->add(customcolors, str.data());

  // Create help for the main toolbar

  str = i18n("New\n\nCreate a new icon either from a template or by specifying the size");
  what->add(toolbar->getButton(ID_FILE_NEWFILE), str.data());

  str = i18n("Open\n\nOpen an existing icon");
  what->add(toolbar->getButton(ID_FILE_OPEN), str.data());

  str = i18n("Save\n\nSave the current icon");
  what->add(toolbar->getButton(ID_FILE_SAVE), str.data());

  str = i18n("Print\n\nOpens a print dialog to let you print the current icon."
             " (Doesn't realy work as expected");
  what->add(toolbar->getButton(ID_FILE_PRINT), str.data());

  str = i18n("Cut\n\nCut the current selection out of the icon\n\n"
             "(Tip: You can make both rectangular and circular selections)");
  what->add(toolbar->getButton(ID_EDIT_CUT), str.data());

  str = i18n("Copy\n\nCopy the current selection out of the icon\n\n"
             "(Tip: You can make both rectangular and circular selections)");
  what->add(toolbar->getButton(ID_EDIT_COPY), str.data());

  str = i18n("Paste\n\nPaste the contents of the clipboard into the current icon.\n\n"
             "If the contents is larger than the current icon you can paste it"
             " in a new window.\n\n"
             "(Tip: Select \"Paste transparent pixels\" in the configuration dialog"
             " if you also want to paste transparent)");
  what->add(toolbar->getButton(ID_EDIT_PASTE), str.data());

  str = i18n("Select\n\nSelect a section of the icon using the mouse.\n\n"
             "(Tip: You can make both rectangular and circular selections)");
  what->add(toolbar->getButton(ID_SELECT), str.data());

  str = i18n("Resize\n\nSmoothly resizes the icon while trying to preserve the contents");
  what->add(toolbar->getButton(ID_IMAGE_RESIZE), str.data());

  str = i18n("Gray scale\n\nGray scale the current icon.\n"
             "(Warning: The result is likely to contain colors not in the icon palette");
  what->add(toolbar->getButton(ID_IMAGE_GRAYSCALE), str.data());

  str = i18n("Zoom out\n\nZoom out by one.\n\n"
             "(Tip: Hold the button down for a few seconds to zoom to a predefined scale");
  what->add(toolbar->getButton(ID_VIEW_ZOOM_OUT), str.data());

  str = i18n("Zoom in\n\nZoom in by one.\n\n"
             "(Tip: Hold the button down for a few seconds to zoom to a predefined scale");
  what->add(toolbar->getButton(ID_VIEW_ZOOM_IN), str.data());

  str = i18n("Toggle grid\n\nToggles the grid in the icon edit grid on/off");
  what->add(toolbar->getButton(ID_OPTIONS_TOGGLE_GRID), str.data());

  str = i18n("New window\n\nOpens a new Icon Editor window.");
  what->add(toolbar->getButton(ID_FILE_NEWWIN), str.data());

  str = i18n("Whats is...?\n\nWell since you're reading this I guess you found out ;-)");
  what->add(toolbar->getButton(ID_HELP_WHATSTHIS), str.data());

  str = i18n("Drag source\n\nFrom this button you can drag out a copy of the current icon"
             " and drop it on a drop enabled window, eg. another Icon Editor.\n\n"
             "(Note: The drag source uses the QT XDND protocol which is not"
             " yet standard in KDE)");
  what->add(dragbutton, str.data());

  // Setup help for the tools toolbar

  str = i18n("Free hand\n\nDraw non-linear lines");
  what->add(drawtoolbar->getButton(ID_DRAW_FREEHAND), str.data());

  str = i18n("Flood fill\n\nFill adjoining pixels with the same color with the current color");
  what->add(drawtoolbar->getButton(ID_DRAW_FILL), str.data());

  str = i18n("Rectangle\n\nDraw a rectangle");
  what->add(drawtoolbar->getButton(ID_DRAW_RECT), str.data());

  str = i18n("Filled rectangle\n\nDraw a filled rectangle");
  what->add(drawtoolbar->getButton(ID_DRAW_RECT_FILL), str.data());

  str = i18n("Circle\n\nDraw a circle");
  what->add(drawtoolbar->getButton(ID_DRAW_CIRCLE), str.data());

  str = i18n("Filled circle\n\nDraw a filled circle");
  what->add(drawtoolbar->getButton(ID_DRAW_CIRCLE_FILL), str.data());

  str = i18n("Ellipse\n\nDraw an ellipse");
  what->add(drawtoolbar->getButton(ID_DRAW_ELLIPSE), str.data());

  str = i18n("Filled ellipse\n\nDraw a filled ellipse");
  what->add(drawtoolbar->getButton(ID_DRAW_ELLIPSE_FILL), str.data());

  str = i18n("Line\n\nDraw a straight line vertically, horizontically or in 45 dgr. angles");
  what->add(drawtoolbar->getButton(ID_DRAW_LINE), str.data());

  str = i18n("Spray\n\nDraw scattered pixels in the current color");
  what->add(drawtoolbar->getButton(ID_DRAW_SPRAY), str.data());

  str = i18n("Find\n\nThe color of the pixel clicked on will be the current drawcolor");
  what->add(drawtoolbar->getButton(ID_DRAW_FIND), str.data());

  str = i18n("Erase\n\nErase pixels. Set the pixels to be transparent\n\n"
             "(Tip: If you want to draw transparent with a different tool"
             " then first click on \"Erase\" then on the tool you want to use)");
  what->add(drawtoolbar->getButton(ID_DRAW_ERASE), str.data());

  debug("setupWhatsThis - done");
  return true;
}

void KIconEdit::addRecent(const char *filename)
{
  QString str = filename;
  //debug("addRecent - checking %s", filename);
  Properties *pprops = props(this);
  if(str.isEmpty() || pprops->recentlist->contains(filename))
    return;
  //debug("addRecent - adding %s", filename);

  if( pprops->recentlist->count() < 5)
    pprops->recentlist->insert(0,filename);
  else
  {
    pprops->recentlist->remove(4);
    pprops->recentlist->insert(0,filename);
  }

  recent->clear();

  for ( int i = 0 ;i < (int)pprops->recentlist->count(); i++)
    recent->insertItem(pprops->recentlist->at(i));

  file->setItemEnabled(ID_FILE_RECENT, true);
  //debug("addRecent - done");
}

void KIconEdit::toggleTool(int id)
{
  for(int i = ID_DRAW_FREEHAND; i <= ID_DRAW_ERASE; i++)
  {
    if(i != id)
    {
      if(tools->isItemChecked(i))
        tools->setItemChecked(i, false);
      if(drawtoolbar->isButtonOn(i));
        drawtoolbar->setButton(i, false);
        //((KToolBarButton*)drawtoolbar->getButton(i))->on(false);
    }
    else
    {
      tools->setItemChecked(i, true);
      if(!drawtoolbar->isButtonOn(i));
        //((KToolBarButton*)drawtoolbar->getButton(i))->on(true);
        drawtoolbar->setButton(i, true);
    }
  }
}


