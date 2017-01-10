/*
  kdynlib.h  -  A class for dynamic loading of libraries.

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

/* $Id: kdynlib.h,v 1.3 1998/10/19 10:38:59 kulow Exp $ */

#ifndef _K_DYNAMIC_LIBRARY_
#define _K_DYNAMIC_LIBRARY_

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#ifdef HAVE_SHLOAD
#include <dl.h>
#endif

#include <qstring.h>

typedef void* KDynamicHandle;

class KDynamicLibrary
{
public:

    enum LoadOption {ResolveLazy, ResolveStrict};
  
    KDynamicLibrary();
    ~KDynamicLibrary();

    static KDynamicHandle loadLibrary(QString libName, LoadOption opt);
    static void unloadLibrary(KDynamicHandle);
    static void *getSymbol(KDynamicHandle handle, QString symName);

    static QString *last_error;
};


#endif
