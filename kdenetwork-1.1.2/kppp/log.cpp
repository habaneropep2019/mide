/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: log.cpp,v 1.4 1999/01/07 21:24:24 porten Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * This file contributed by: Mario Weilguni, <mweilguni@sime.com>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "log.h"
#include <stdio.h>
#include <stdlib.h>

void PRINTDEBUG(char *file, int line, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  
  // strip newlines
  int len = strlen(fmt);
  char *sfmt = (char*)malloc(len+1);
  strcpy(sfmt, fmt);
  while(len && sfmt[len-1] == '\n')
    sfmt[--len] = 0;

  fprintf(stderr, "[%s:%s:%d]: ", "kppp", file, line);
  vfprintf(stderr, sfmt, ap);
  fprintf(stderr, "\n");
  free(sfmt);
  va_end(ap);
}
