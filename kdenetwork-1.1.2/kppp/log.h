/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: log.h,v 1.4 1998/10/05 17:23:03 mario Exp $
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

#ifndef __LOG__H__
#define __LOG__H__

#include <assert.h>
#include <stdarg.h>

extern void PRINTDEBUG(char *, int, const char *, ...);

#define Fatal(fmt, args...) { PRINTDEBUG(__FILE__, __LINE__, fmt, ##args); exit(1); }

#ifndef MY_DEBUG
#define Debug(fmt, args...) ((void)0);
#else
#define Debug(fmt, args...) PRINTDEBUG(__FILE__, __LINE__, fmt, ##args)
#endif

#endif
