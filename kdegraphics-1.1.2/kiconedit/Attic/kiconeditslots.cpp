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

#include <qdragobject.h>
#include <qstrlist.h>
#include <qprintdialog.h>
#include <qpaintdevicemetrics.h>
#include "debug.h"
#include "kiconedit.h"
#include "pics/logo.xpm"

void KIconEdit::addColors(uint n, uint *c)
{
  //debug("KIconTools::addColors()");
  customcolors->clear();
  for(uint i = 0; i < n; i++)
    addColor(c[i]);
}

void KIconEdit::addColor(uint color)
{
  //debug("KIconTools::addColor()");
  if(!syscolors->contains(color))
    customcolors->addColor(color);
}

void KIconEdit::slotNewWin()
{
  slotNewWin((const char*)0);
}

void KIconEdit::slotNewWin(const char *url)
{
  debug("KIconEdit::openNewWin() - %s", url);
  KIconEdit *w = new KIconEdit(url, "kiconedit");
  CHECK_PTR(w);
}

void KIconEdit::slotNew()
{
  bool cancel = false;
  if (grid->isModified()) 
  {
    int r = KMsgBox::yesNoCancel(this, i18n("Warning"), 
      i18n("The current file has been modified.\nDo you want to save it?"));
    switch(r)
    {
      case 1:
        icon->save(&grid->image());
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
#ifndef KWIZARD_VERSION
    KResize n(this, 0, true, QSize(32, 32));
    if(n.exec())
    {
      grid->editClear();
      const QSize s = n.getSize();
      debug("Size: %d x %d", s.width(), s.height());
      grid->setSize(s);
      grid->setModified(false);
      icon->cleanup();
    }
#else
    KNewIcon newicon(this);
    if(newicon.exec())
    {
      int r = newicon.openStyle();
      if(r == KNewIcon::Blank)
      {
        grid->editClear();
        const QSize s = newicon.templateSize();
        debug("Size: %d x %d", s.width(), s.height());
        grid->setSize(s);
        grid->setModified(false);
        icon->cleanup();
      }
      else if(r == KNewIcon::Template)
      {
        QString str = newicon.templatePath();
        icon->open(&grid->image(), str.data());
        icon->cleanup();
      }
    }
#endif
  }
}

void KIconEdit::slotOpen()
{
  bool cancel = false;
  if (grid->isModified()) 
  {
    int r = KMsgBox::yesNoCancel(this, i18n("Warning"), 
	i18n("The current file has been modified.\nDo you want to save it?"));
    switch(r)
    {
      case 1:
        icon->save(&grid->image());
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
    icon->promptForFile(&grid->image());
}

void KIconEdit::slotClose()
{
  close();
}

void KIconEdit::slotQuit()
{
  debug("KIconEdit: Closing %u windows", memberList->count());
  KIconEdit *ki = 0L;
  while((ki = (KIconEdit*)memberList->getFirst()) != 0)
  {
    CHECK_PTR(ki);
    memberList->getFirst()->close();
  }
}

void KIconEdit::slotSave()
{
  debug("KIconEdit: Saving %s", icon->url().data());
  icon->save(&grid->image());
}

void KIconEdit::slotSaveAs()
{
  debug("KIconEdit: Saving %s", icon->url().data());
  icon->saveAs(&grid->image());
}

void KIconEdit::slotPrint()
{
  file->setItemEnabled(ID_FILE_PRINT, false);
  toolbar->setItemEnabled(ID_FILE_PRINT, false);

  if ( QPrintDialog::getPrinterSetup(printer) )
  {
    int margin = 10, yPos = 0;
    printer->setCreator("KDE Icon Editor");
    QPainter p;
    p.begin( printer );
    QFontMetrics fm = p.fontMetrics();
    QPaintDeviceMetrics metrics( printer ); // need width/height

    p.drawText( margin, margin + yPos, metrics.width(), fm.lineSpacing(),
                        ExpandTabs | DontClip, icon->url().data() );
    yPos = yPos + fm.lineSpacing();
    p.drawPixmap( margin, margin + yPos, grid->pixmap() );
    p.end();
  }
  file->setItemEnabled(ID_FILE_PRINT, true);
  toolbar->setItemEnabled(ID_FILE_PRINT, true);
}

void KIconEdit::slotView( int id )
{
  switch(id)
  {
    case ID_VIEW_ZOOM_1TO1:
      //slotUpdateStatusScaling(1, false);
      grid->zoomTo(1);
      //slotUpdateStatusScaling(1, true);
      toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
      view->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
      break;
    case ID_VIEW_ZOOM_1TO5:
      //slotUpdateStatusScaling(5, false);
      grid->zoomTo(5);
      //slotUpdateStatusScaling(5, true);
      break;
    case ID_VIEW_ZOOM_1TO10:
      //slotUpdateStatusScaling(10, false);
      grid->zoomTo(10);
      //slotUpdateStatusScaling(10, true);
      break;
    case ID_VIEW_ZOOM_OUT:
      if(!grid->zoom(Out))
      {
        toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
        view->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
      }
      break;
    case ID_VIEW_ZOOM_IN:
      grid->zoom(In);
      toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
      view->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
      break;
    default:
      break;
  }
}

void KIconEdit::slotCopy()
{
  grid->editCopy();
}

void KIconEdit::slotCut()
{
  grid->editCopy(true);
}

void KIconEdit::slotPaste()
{
  toggleTool(ID_DRAW_FIND);
  grid->setTool(KIconEditGrid::Find);
  grid->editPaste();
}

void KIconEdit::slotClear()
{
  grid->editClear();
}

void KIconEdit::slotSelectAll()
{
  //toggleTool(ID_DRAW_SELECT);
  grid->setTool(KIconEditGrid::SelectRect);
  grid->editSelectAll();
}

void KIconEdit::slotOpenRecent(int id)
{
  debug("Opening recent file: %d", id);
  Properties *pprops = props(this);
  debug("Recent: %s", pprops->recentlist->at((uint)id));
  icon->open(&grid->image(), pprops->recentlist->at(id));
}

void KIconEdit::slotConfigure(int id)
{
  switch(id)
  {
    case ID_OPTIONS_CONFIGURE:
    {
      KIconConfig c(this);
      if(c.exec())
      {
        Properties *pprops = props(this);
        updateMenuAccel();
        gridview->setShowRulers(pprops->showrulers);
        if(pprops->backgroundmode == QWidget::FixedPixmap)
        {
          QPixmap pix(pprops->backgroundpixmap.data());
          if(pix.isNull())
          {
            QPixmap pmlogo((const char**)logo);
            pix = pmlogo;
          }
          //viewport->viewport()->setBackgroundPixmap(pix);
        }
        //else
          //viewport->viewport()->setBackgroundColor(pprops->backgroundcolor);
      }
      //KKeyDialog::configureKeys( keys );
      break;
    }
    case ID_OPTIONS_TOGGLE_GRID:
      if(options->isItemChecked(ID_OPTIONS_TOGGLE_GRID))
      {
        options->setItemChecked(ID_OPTIONS_TOGGLE_GRID, false);
        ((KToolBarButton*)toolbar->getButton(ID_OPTIONS_TOGGLE_GRID))->on(false);
        //toolbar->setButton(ID_OPTIONS_TOGGLE_GRID, false);
        props(this)->showgrid = false;
        grid->setGrid(false);
      }
      else
      {
        options->setItemChecked(ID_OPTIONS_TOGGLE_GRID, true);
        ((KToolBarButton*)toolbar->getButton(ID_OPTIONS_TOGGLE_GRID))->on(true);
        //toolbar->setButton(ID_OPTIONS_TOGGLE_GRID, true);
        props(this)->showgrid = true;
        grid->setGrid(true);
      }
      break;
    case ID_OPTIONS_TOGGLE_TOOL1:
      if(options->isItemChecked(ID_OPTIONS_TOGGLE_TOOL1))
        options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL1, false);
      else
        options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL1, true);
      enableToolBar(KToolBar::Toggle, 0);
      break;
    case ID_OPTIONS_TOGGLE_TOOL2:
      if(options->isItemChecked(ID_OPTIONS_TOGGLE_TOOL2))
        options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL2, false);
      else
        options->setItemChecked(ID_OPTIONS_TOGGLE_TOOL2, true);
      enableToolBar(KToolBar::Toggle, 1);
      break;
    case ID_OPTIONS_TOGGLE_STATS:
      if(options->isItemChecked(ID_OPTIONS_TOGGLE_STATS))
        options->setItemChecked(ID_OPTIONS_TOGGLE_STATS, false);
      else
        options->setItemChecked(ID_OPTIONS_TOGGLE_STATS, true);
      enableStatusBar();
      break;
    case ID_OPTIONS_SAVE:
      writeConfig();
      break;
    default:
      break;
  }
}

void KIconEdit::slotTools( int id )
{
  switch(id)
  {
    case ID_SELECT_RECT:
      grid->setTool(KIconEditGrid::SelectRect);
      break;
    case ID_SELECT_CIRCLE:
      grid->setTool(KIconEditGrid::SelectCircle);
      break;
    case ID_DRAW_FREEHAND:
      toggleTool(id);
      grid->setTool(KIconEditGrid::Freehand);
      break;
    case ID_DRAW_ELLIPSE:
      toggleTool(id);
      grid->setTool(KIconEditGrid::Ellipse);
      break;
    case ID_DRAW_ELLIPSE_FILL:
      toggleTool(id);
      grid->setTool(KIconEditGrid::FilledEllipse);
      break;
    case ID_DRAW_CIRCLE:
      toggleTool(id);
      grid->setTool(KIconEditGrid::Circle);
      break;
    case ID_DRAW_CIRCLE_FILL:
      toggleTool(id);
      grid->setTool(KIconEditGrid::FilledCircle);
      break;
    case ID_DRAW_RECT:
      toggleTool(id);
      grid->setTool(KIconEditGrid::Rect);
      break;
    case ID_DRAW_RECT_FILL:
      toggleTool(id);
      grid->setTool(KIconEditGrid::FilledRect);
      break;
    case ID_DRAW_ERASE:
      toggleTool(id);
      grid->setTool(KIconEditGrid::Eraser);
      break;
    case ID_DRAW_LINE:
      toggleTool(id);
      grid->setTool(KIconEditGrid::Line);
      break;
    case ID_DRAW_FIND:
      toggleTool(id);
      grid->setTool(KIconEditGrid::Find);
      break;
    case ID_DRAW_FILL:
      toggleTool(id);
      grid->setTool(KIconEditGrid::FloodFill);
      break;
    //case ID_DRAW_SELECT:
    case ID_DRAW_SPRAY:
      toggleTool(id);
      grid->setTool(KIconEditGrid::Spray);
      break;
    default:
        if(id >= 500 && id <= 600)
          KMsgBox::message(this, i18n("Warning"), i18n("Sorry - not implemented."));
      break;
  }
}

void KIconEdit::slotImage( int id )
{
  switch(id)
  {
#if QT_VERSION >= 140
    case ID_IMAGE_RESIZE:
      grid->editResize();
      break;
#endif
    case ID_IMAGE_GRAYSCALE:
      grid->grayScale();
      break;
    case ID_IMAGE_MAPTOKDE:
      grid->mapToKDEPalette();
      break;
    default:
      break;
  }
}

void KIconEdit::slotSaved()
{
  grid->setModified(false);
}

void KIconEdit::slotUpdateStatusPos(int x, int y)
{
  QString str;
  str.sprintf("%d, %d", x, y);
  statusbar->changeItem( str.data(), 0);
}

void KIconEdit::slotUpdateStatusSize(int x, int y)
{
  QString str;
  str.sprintf("%d x %d", x, y);
  statusbar->changeItem( str.data(), 1);
  //viewport->updateScrollBars();
}

void KIconEdit::slotUpdateStatusScaling(int s, bool show)
{
  props(this)->gridscaling = s;
  QString str;
  if(show)
  {
    toolbar->setItemEnabled(ID_VIEW_ZOOM_IN, true);
    toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, true);
  }
  else
  {
    toolbar->setItemEnabled(ID_VIEW_ZOOM_IN, false);
    toolbar->setItemEnabled(ID_VIEW_ZOOM_OUT, false);
  }

  str.sprintf("1:%d", s);
  statusbar->changeItem( str.data(), 2);
  //viewport->updateScrollBars();
}

void KIconEdit::slotUpdateStatusColors(uint)
{
  QString str;
  str.sprintf(i18n("Colors: %u"), grid->numColors());
  statusbar->changeItem( str.data(), 3);
}

void KIconEdit::slotUpdateStatusColors(uint n, uint *)
{
  QString str;
  str.sprintf(i18n("Colors: %u"), n);
  statusbar->changeItem( str.data(), 3);
}

void KIconEdit::slotUpdateStatusMessage(const char *msg)
{
  msgtimer->start(10000, true);
  statusbar->changeItem( msg, 4);
}

void KIconEdit::slotClearStatusMessage()
{
  statusbar->changeItem( "", 4);
}

void KIconEdit::slotUpdateStatusName(const char *name)
{
  QString cap = "", str = name;
  if(!str.isEmpty())
  {
    cap += name;
    cap += " - ";
    addRecent(name);
  }
  cap += kapp->getCaption();
  setCaption(cap.data());
}

void KIconEdit::slotUpdatePaste(bool state)
{
  edit->setItemEnabled(ID_EDIT_PASTE, state);
  toolbar->setItemEnabled(ID_EDIT_PASTE, state);
  edit->setItemEnabled(ID_EDIT_PASTE_AS_NEW, state);
  toolbar->setItemEnabled(ID_EDIT_PASTE_AS_NEW, state);
}

void KIconEdit::slotUpdateCopy(bool state)
{
  edit->setItemEnabled(ID_EDIT_COPY, state);
  toolbar->setItemEnabled(ID_EDIT_COPY, state);
  edit->setItemEnabled(ID_EDIT_CUT, state);
  toolbar->setItemEnabled(ID_EDIT_CUT, state);
}

void KIconEdit::slotOpenBlank(const QSize s)
{
  icon->cleanup();
  grid->loadBlank( s.width(), s.height());
}

void KIconEdit::slotDropEvent( KDNDDropZone * _dropZone )
{
  debug("KIconEdit::slotDropEvent");
  QStrList & list = _dropZone->getURLList();
  char *s;
  bool loadedinthis = false;
  
  for ( s = list.first(); s != 0L; s = list.next() )
  {
    // Load the first file in this window
    debug("KIconEdit:slotDropEvent - %s", s);
    //s == list.getFirst();
    if (!grid->isModified() && !loadedinthis) 
    {
      icon->open( &grid->image(), s );
      loadedinthis = true;
    }
    else 
    {
      slotNewWin(s);
    }
  }    
}

void KIconEdit::slotQDropEvent( QDropEvent *e )
{
  debug("Got QDropEvent!");
  gridview->setAcceptDrop(false);
  QImage image;
  QStrList list;
  char *s;
  bool loadedinthis = false;
  
  if ( QImageDrag::decode( e, image ) )
  {
    debug("Image decoded");
    if(!image.isNull())
    {
      image = image.convertDepth(32);
      //image.setAlphaBuffer(true);
      for(int y = 0; y < image.height(); y++)
      {
        uint *l = (uint*)image.scanLine(y);
        for(int x = 0; x < image.width(); x++, l++)
        {
          if(*l < OPAQUE_MASK) // the dnd encoding stuff turns off the opaque bits
          {
            *l = *l | OPAQUE_MASK;
          }
        }
      }
      grid->load(&image);
    }
    else
      debug("Image is invalid");
  }
#if QT_VERSION > 140
  else if(QUrlDrag::decode( e, list ) )
  {
    for ( s = list.first(); s != 0L; s = list.next() )
    {
      // Load the first file in this window
      debug("KIconEdit:slotQDropEvent - %s", s);
      //s == list.getFirst();
      if (!grid->isModified() && !loadedinthis) 
      {
        icon->open( &grid->image(), s );
        loadedinthis = true;
      }
      else 
      {
        slotNewWin(s);
      }
    }
  } 
#endif   
}

void KIconEdit::slotQDragLeaveEvent( QDragLeaveEvent * /*e*/ )
{
  debug("Got QDragLeaveEvent!");
  gridview->setAcceptDrop(false);
}

void KIconEdit::slotQDragEnterEvent( QDragEnterEvent *e )
{
  debug("Got QDragEnterEvent!");
  if( QImageDrag::canDecode( e ) )
  {
    e->accept();
    gridview->setAcceptDrop(true);
  }
#if QT_VERSION > 140
  else if( QUrlDrag::canDecode( e ) )
  {
    gridview->setAcceptDrop(true);
    e->accept();
  }
#endif
  else
    e->ignore();
}




