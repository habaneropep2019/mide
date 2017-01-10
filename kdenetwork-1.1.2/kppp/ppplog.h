/*
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: ppplog.h,v 1.3 1998/11/20 21:07:57 porten Exp $
 *
 * (c) 1998 Mario Weilguni <mweilguni@kde.org>
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
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

#ifndef __PPPL__H__
#define __PPPL__H__

class QStrList;

int PPPL_MakeLog(QStrList &list);
void PPPL_AnalyseLog(QStrList &list, QStrList &result);
void PPPL_ShowLog();

#endif

