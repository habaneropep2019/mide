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

#include "kiconfiledlg.h"
#include "debug.h"
#include "kicon.h"

KIcon::KIcon(QObject *parent, const QImage *img, const char *filename) : QObject(parent)
{
  kfm = 0L;
  f = 0;

  checkUnNamedBackup(img);

  if(filename != 0)
  {
    open(img, filename);
  }
}

KIcon::~KIcon()
{
}

void KIcon::cleanup()
{
  if(_backup.length() > 0 && QFile::exists(_backup.data()))
  {
    if(!removeFile(_backup))
    {
      QString msg = i18n("There was an error removing temporary backup file:\n");
      msg += _backup.data();
      msg += "\n";
      KMsgBox::message((QWidget*)parent(), i18n("Warning"), msg);
    }
  }

  _backup = "";
  _url = "";
  unlock(f);
  closeKFM();
  emit newname("");
}


void KIcon::open(const QImage *image, const char *xpm)
{
  QImage *img = (QImage*)image;
  QString filename(xpm);

  if(filename.isEmpty())
    return;
  debug("KIcon::open %s", filename.data());
  if(kfm != 0L)
  {
    KMsgBox::message((QWidget*)parent(), i18n("Warning"),
      i18n("We've got a KFM job running.\nPlease wait."), KMsgBox::STOP);
    return;    
  }
  char *t;
  QString tmp = "";

  if(filename.at(0) == '/')
    filename = "file:" + filename;

  debug("KIcon::open - checked for absolute path");

  KURL _turl(filename.data());
  if(_turl.isMalformed()) // try to see if it is a relative filename
  {
    debug("KIcon::open (malformed) %s", filename.data());

    QFileInfo fi(filename.data());
    if(fi.isRelative())
      filename = "file:" + fi.absFilePath();
    _turl = filename.data();

    if(_turl.isMalformed()) // Giving up
    {
      QString msg = i18n("The URL:\n");
      msg += _turl.url();
      msg += i18n("\nseems to be malformed.\n");
      KMsgBox::message((QWidget*)parent(), i18n("ERROR"), msg, KMsgBox::EXCLAMATION);
      return;
    }
  }

  debug("KIcon::open (not malformed) %s", filename.data());

  bool r;
  t = (char*)kapp->checkRecoverFile(_turl.url().data(), r);
  tmp = t;
  if(t)
    delete t;

  if(r)
  {
    debug("KIcon: Found recover file");
    QString msg = i18n("The KDE Icon Editor has found a recover file for:\n");
    msg += filename;
    msg += i18n("\nDo you want to open this?");
    if(KMsgBox::yesNo((QWidget*)parent(), i18n("Information"), msg.data()) == 1)
    {
      debug("Opening recover file for: %s", filename.data());
    }
  }
  else
  {
    t = (char*)kapp->tempSaveName(_turl.url());
    tmp = t;
    if(t)
      delete t;
    //debug("KIcon: tempSaveName: %s", tmp.data());

    debug("isLocalFile   : %d", _turl.isLocalFile());
    debug("hasSubProtocol: %d", _turl.hasSubProtocol());
    debug("Path:         : %s", _turl.path());
    debug("URL:          : %s", _turl.url().data());

    if(_turl.isLocalFile() && !_turl.hasSubProtocol())
    {
      debug("KIcon::open local: %s", _turl.url().data());
      QFileInfo f(_turl.path());
      if(!f.exists() || !f.isReadable())
      {
        QString msg = i18n("The file:\n");
        msg += _turl.path();
        msg += i18n("\ndoesn't exist or isn't readable.\n");
        KMsgBox::message((QWidget*)parent(), i18n("ERROR"), msg, KMsgBox::EXCLAMATION);
        return;
      }
      else
      {
        removeFile(_backup);
        copyFile(_turl.path(), tmp.data());
      }
    } // if local
    else
    {
      debug("KIcon::open non-local: %s %s", _turl.path(), tmp.data());
      if(kfm != 0L)
      {
        QString msg = i18n("The Icon Editor is already downloading a non-local file.\nPlease wait.\n");
        KMsgBox::message((QWidget*)parent(), i18n("Warning"), msg, KMsgBox::STOP);
        return;
      }

      QString src = _turl.url();

      if(!initKFM())
      {
        KMsgBox::message((QWidget*)parent(), i18n("ERROR"), 
          i18n("Couldn't initiate KFM!"), KMsgBox::EXCLAMATION);
        return;
      }

      debug("KIcon::open - before download: %s", tmp.data());
      removeFile(_backup);
      if(!kfm->download(src, tmp))
      {
        QString msg = i18n("There was a KFM error creating a backup for:\n");
        msg += _turl.path();
        msg += "\n";
        KMsgBox::message((QWidget*)parent(), i18n("ERROR"), msg, KMsgBox::EXCLAMATION);
      }

      debug("KIcon::open - after download: %s", tmp.data());

      closeKFM();
      debug("KIcon::open - after closeKFM");
    } // remote
    if(!QFile::exists(tmp))
    {
      QString msg = i18n("There was an error creating a backup for:\n");
      msg += _turl.url();
      msg += "\n";
      KMsgBox::message((QWidget*)parent(), i18n("ERROR"), msg, KMsgBox::EXCLAMATION);
      return;
    }
    debug("KIcon::open - backup created");
  } // not recovered

  FILE *fs = lock(tmp.data());

  if(!fs)
  {
    QString msg = i18n("KIcon: There was a error locking:\n");
    msg += tmp;
    msg += "\n";
    KMsgBox::message((QWidget*)parent(), i18n("ERROR"), msg.data(), KMsgBox::EXCLAMATION);
    removeFile(tmp);
    return;
  }

  debug("KIcon::open - Before loading image");
  if(!img->load(tmp.data()))
  {
    QString msg = i18n("KIcon: There was a QT error loading:\n");
    msg += tmp;
    msg += "\n";
    KMsgBox::message((QWidget*)parent(), i18n("ERROR"), msg.data(), KMsgBox::EXCLAMATION);
    removeFile(tmp);
    return;
  }

  // hack to make pixels opaque
  QString format = QImage::imageFormat(tmp.data());
  if(format == "JFIF")
  {
    *img = img->convertDepth(32);
    for(int y = 0; y < img->height(); y++)
    {
      uint *l = (uint*)img->scanLine(y);
      for(int x = 0; x < img->width(); x++, l++)
      {
        if(*l < 0xff000000) // the dnd encoding stuff turns off the opaque bits
          *l = *l | 0xff000000;
      }
    }
  }

  debug("KIcon::open - Image loaded");
  unlock(f);
  f = fs;
  _backup = tmp;
  _url = _turl.url();

  debug("KIcon: _backup: %s", _backup.data());
  debug("KIcon: _url: %s", _url.data());
  emit loaded(img);
  emit newname(_url.data());
  debug("KIcon::open - done");
}

// Prompts the user for a file to open.
void KIcon::promptForFile(const QImage *img)
{
  if(kfm != 0L)
  {
    KMsgBox::message((QWidget*)parent(), i18n("Warning"),
      i18n("We've got a KFM job running.\nPlease wait."), KMsgBox::STOP);
    return;    
  }
  QString filter = i18n("*|All files (*)\n"
                        "*.xpm|XPM (*.xpm)\n"
                        "*.gif|GIF files (*.gif)\n"
                        "*jpg|JPEG files (*.jpg)\n");
  QString tmpname = KIconFileDlg::getOpenFileURL(_lastdir.data(), filter.data());
  if(tmpname.length() == 0)
    return;
  open(img, tmpname.data());
}

void KIcon::checkUnNamedBackup(const QImage *image)
{
  QImage *img = (QImage*)image;
  bool r;
  char *t = (char*)kapp->checkRecoverFile("UnNamed.xpm", r);
  QString tmp = t;
  if(t)
    delete t;

  if(r)
  {
    debug("KIcon: Found recover file");
    QString msg = i18n("The KDE Icon Editor has found a recover\n");
    msg += i18n("file for an unsaved icon.");
    msg += i18n("\nDo you want to open this?");
    if(KMsgBox::yesNo((QWidget*)parent(), i18n("Information"), msg.data()) == 1)
    {
      debug("Opening recover file for: %s", tmp.data());
      if(!img->load(tmp.data()))
      {
        QString msg = i18n("KIcon: There was a QT error loading:\n");
        msg += tmp;
        msg += "Do you wan't to remove this backup file?\n";
        if(KMsgBox::yesNo((QWidget*)parent(), i18n("Information"), msg.data()) == 1)
          removeFile(tmp);
        return;
      }

      debug("KIcon::checkForUnNamed - Image loaded");
      _backup = tmp;
      emit loaded(img);
      emit newname(tmp.data());
    }
  }
}


// only used for backups under a crash
void KIcon::saveBackup(const QImage *image, const char *filename)
{
  if(!image || image->isNull())
    return;

  if(!filename)
  {
    if(_url.isEmpty())
      filename = "UnNamed.xpm";
    else
      filename = _url.data();
  }

  const char *t = (char*)kapp->tempSaveName(filename);
  QString str = t;
  if(t)
    delete t;

  QImage *img = (QImage*)image, *tmp;
  img->setAlphaBuffer(true);
  tmp = new QImage(img->convertDepth(8, ColorOnly|AvoidDither));
  CHECK_PTR(tmp);
  tmp->setAlphaBuffer(true);
  tmp->save(str.data(), "XPM");
  delete tmp;
}

void KIcon::save(const QImage *image, const char *filename)
{
  debug("KIcon::save");
  if(kfm != 0L)
  {
    KMsgBox::message((QWidget*)parent(), i18n("Warning"),
      i18n("We've got a KFM job running.\nPlease wait."), KMsgBox::STOP);
    return;    
  }

  if(filename == 0)
  {
    if(_url.isEmpty())
    {
      saveAs(image);
      return;
    }
    else
      filename = _url.data();
  }

  QImage *img = (QImage*)image, *tmp;
  img->setAlphaBuffer(true);
  tmp = new QImage(img->convertDepth(8, ColorOnly|AvoidDither));
  CHECK_PTR(tmp);
  tmp->setAlphaBuffer(true);

  const char *t = (char*)kapp->tempSaveName(filename);
  QString str = t;
  if(t)
    delete t;

  QImageIO iio;
  iio.setImage(*img);
  iio.setFileName(str.data());
  iio.setFormat("XPM");
  iio.setDescription("Created by the KDE Icon Editor");
  
  if(iio.write())
  {
    debug("KIcon::save - backup image written: %s", str.data());
    KURL _turl(filename);
    if(_turl.isLocalFile())
    {
/*
      if(QFile::exists(filename))
      {
        QString msg = i18n("The file:\n");
        msg += filename;
        msg += i18n("\nallready exist. Overwrite it?");
        if(!KMsgBox::yesNo((QWidget*)parent(), i18n("Information"), msg.data()) == 1)
        {
          removeFile(str.data());
          delete tmp;
          return;
        }
      }
*/
      if(!copyFile(str, _turl.path()))
      {
        QString msg = i18n("There was an error creating a backup for:\n");
        msg += _url;
        msg += "\n";
        KMsgBox::message((QWidget*)parent(), i18n("ERROR"), msg, KMsgBox::EXCLAMATION);
        delete tmp;
        return;
      }
    } // local
    else
    {
      if(!initKFM())
      {
        KMsgBox::message((QWidget*)parent(), i18n("Warning"),
          i18n("Couldn't initiate KFM"), KMsgBox::EXCLAMATION);
        delete tmp;
        return;
      }
      kfm->copy(str, filename);
      closeKFM();
    }
    if(_backup != str)
    {
      removeFile(_backup);
      _backup = str.copy();
    }
    emit saved();
    emit newmessage(i18n("Done saving"));
    _url = filename;
    emit newname(_url.data());
    refreshDirectory(str);
  }
  else // if(iio.write(...
  {
    QString msg = i18n("There was an error saving:\n");
    msg += _url;
    msg += "\n";
    KMsgBox::message((QWidget*)parent(), i18n("ERROR"), msg, KMsgBox::EXCLAMATION);
    debug("KIcon::save - %s", msg.data());
  }
  delete tmp;
  debug("KIcon::save - done");
}

void KIcon::saveAs(const QImage *image)
{
  debug("KIcon::saveAs");
  if(kfm != 0L)
  {
    KMsgBox::message((QWidget*)parent(), i18n("Warning"),
      i18n("We've got a KFM job running.\nPlease wait."), KMsgBox::STOP);
    return;    
  }
  QString tmpname = KIconFileDlg::getSaveFileName(_lastdir.data(), "*.xpm");
  if(tmpname.length() == 0)
    return;
  save(image, tmpname.data());
  debug("KIcon::saveAs - done");
}

bool KIcon::initKFM(bool allowrestart)
{
  if(kfm != 0L)
    return false;    

  kfm = new KFM();
  CHECK_PTR(kfm);
  if(allowrestart)
    kfm->allowKFMRestart(true);
  if(!kfm->isOK())
  {
    delete kfm;
    return false;
  }
  else
    return true;
}

void KIcon::closeKFM()
{
  if(kfm)
    delete kfm;
  kfm = 0L;
}

FILE *KIcon::lock(const char *file)
{
  FILE *fs = fopen(file, "r+");
  if (!fs) 
  {
    debug("Cannot open file `%s': %s", file, 
	  strerror(errno));
    return 0;
  }

  int rc;
#if !HAVE_FLOCK
  struct flock fl;
  fl.l_type=F_WRLCK;
  fl.l_whence=0;
  fl.l_start=0;
  fl.l_len=0;
#endif


#if HAVE_FLOCK
  rc = flock(fileno(fs), LOCK_NB|LOCK_EX);
#else
  rc = fcntl(fileno(fs), F_SETLK, &fl);
#endif

  if (rc < 0)
  {
    debug("Cannot lock `%s': %s (%d)", file,
	  strerror(errno), errno);
#if HAVE_FLOCK
    rc = flock(fileno(fs), LOCK_UN);
#else
    fl.l_type = F_UNLCK;
    rc = fcntl(fileno(fs), F_SETLK, &fl);
#endif
    return 0;
  }
  debug("Locked %d %s", fileno(fs), file);
  return fs;
}

bool KIcon::unlock(FILE *fs)
{
  if(!fs)
    return false;

  int rc;
#if !HAVE_FLOCK
  struct flock fl;
  fl.l_type=F_UNLCK;
  fl.l_whence=0;
  fl.l_start=0;
  fl.l_len=0;
#endif

#if HAVE_FLOCK
  debug("Using flock");
  rc = flock(fileno(fs), LOCK_UN);
#else
  debug("Using fcntl");
  rc = fcntl(fileno(fs), F_SETLK, &fl);
  //rc = fcntl(fileno(fs), F_SETLK, F_UNLCK);
#endif

  if(rc == 0)
  {
    debug("Unlocked : %d", fileno(fs));
    fclose(fs);
    fs = 0;
    return true;
  }
  debug("Cannot unlock descriptor %d: %s (%d)", fileno(fs), strerror(errno), errno);
  
  return false;
}


