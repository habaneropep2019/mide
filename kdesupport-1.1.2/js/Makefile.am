#
# Automakefile for JavaScript virtual machine.
# Copyright (c) 1998 New Generation Software (NGS) Oy
#
# Author: Markku Rossi <mtr@ngs.fi>
#

#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
# MA 02111-1307, USA
#
#
#
# $Source: /home/kde/kdesupport/js/Makefile.am,v $
# $Id: Makefile.am,v 1.1.1.1 1998/08/09 21:05:33 kulow Exp $
#

lib_LTLIBRARIES = libjs.la
include_HEADERS = js.h jsint.h

libjs_la_SOURCES = bc.c heap.c object.c debug.c compiler.c js.c vm.c \
		vmswitch.c vmjumps.c alloc.c utils.c mrgsort.c \
		regex.c crc32.c \
		b_array.c b_bool.c b_func.c b_core.c b_number.c \
		b_object.c b_string.c b_date.c b_file.c b_math.c \
		b_regexp.c b_system.c b_vm.c

libjs_la_LDFLAGS = -version-info 2:0:2 

noinst_HEADERS = c1swt0.h eswt0.h c1switch.h c2switch.h eswitch.h \
	c1jumps.h c2jumps.h ejumps.h rentrant.h \
	mrgsort.h regex.h md5.h

