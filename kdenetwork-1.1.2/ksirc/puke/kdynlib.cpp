/*
  kdynlib.cpp  -  A class for dynamic loading of libraries.

  written 1997 by Matthias Hoelzer
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
  */

// $Id: kdynlib.cpp,v 1.6 1998/12/15 04:59:01 asj Exp $

#include <iostream.h>

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#include "kdynlib.h"
#include <qfile.h>
#include <kapp.h>
#include <klocale.h>
#include <qmsgbox.h>

#include <errno.h>

#ifdef HAVE_DLFCN_H
extern "C" { // this is necessary, because the libc on Alpha/Linux is broken
#include <dlfcn.h> 
}
#endif

#ifdef HAVE_SHLOAD
#include <dl.h>
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif

QString *KDynamicLibrary::last_error = 0;

KDynamicLibrary::KDynamicLibrary() {}
KDynamicLibrary::~KDynamicLibrary() {}

#if !defined(HAVE_DLFCN_H) && !defined(HAVE_SHLOAD)
KDynamicHandle KDynamicLibrary::loadLibrary(QString, LoadOption)
#else
KDynamicHandle KDynamicLibrary::loadLibrary(QString fileName, LoadOption opt)
#endif
{
#if defined(HAVE_DLFCN_H) && defined(HAVE_SHLOAD)
    QFile libfile(fileName);
    if (!libfile.exists()) {
	warning(i18n("No such file \"%s\"!"), 
		 fileName.data());
    }
#endif

#ifdef HAVE_DLFCN_H

    KDynamicHandle handle;

    handle = (KDynamicHandle) dlopen(fileName.data(),
                    RTLD_GLOBAL | ((opt == ResolveLazy) ? RTLD_LAZY : RTLD_NOW));

    if (last_error == 0)
       last_error = new("QString") QString();

    if(handle == NULL){
      warning("Failed to open %s: %s", fileName.data(), dlerror());
      *last_error = dlerror();
    }
    else {
      *last_error = "";
    }

    return handle;

#elif HAVE_SHLOAD

  return (KDynamicHandle)shl_load(fileName.data(),
          (opt == ResolveLazy) ?
		BIND_DEFERRED : BIND_IMMEDIATE || BIND_NONFATAL, 0);
#endif

    return 0; // shouldn't happen
}

#if !defined(HAVE_DLFCN_H) && !defined(HAVE_SHLOAD)
void *KDynamicLibrary::getSymbol(KDynamicHandle, QString)
#else
void *KDynamicLibrary::getSymbol(KDynamicHandle handle, QString symName)
#endif
{
#ifdef HAVE_DLFCN_H
  if (handle){
    void *sym = dlsym(handle, symName.data());
     
    if (!last_error)
       last_error = new("QString") QString();

    if(sym == NULL){
      warning("Failed to find %s: %s", symName.data(), dlerror());
      *last_error = dlerror();
    }
    else {
      *last_error = "";
    }
    return sym;
  }
  else
    return 0L;
#elif  HAVE_SHLOAD
  void *address = 0;
		
  if (handle)
    if (shl_findsym((shl_t *)&handle, symName.data(), TYPE_UNDEFINED, &address) == 0)
      return address;

  return 0L;
#endif

    return 0L; // shouldn't happen
}
  
#if !defined(HAVE_DLFCN_H) && !defined(HAVE_SHLOAD)
void KDynamicLibrary::unloadLibrary(KDynamicHandle)
#else
void KDynamicLibrary::unloadLibrary(KDynamicHandle handle)
#endif
{
#ifdef HAVE_DLFCN_H
    if (handle)
	dlclose(handle);
#endif

#ifdef HAVE_SHLOAD
  if (handle)
    shl_unload((shl_t)handle);
#endif
}
