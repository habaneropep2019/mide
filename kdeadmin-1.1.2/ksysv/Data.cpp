/*

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <qpainter.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include "Data.h"
#include <kapp.h>

// include meta-object code
#include "Data.moc"

#define MAX_POS 13

KSVDragData::KSVDragData() : QObject() {
  path = "Empty";
  run_level = "Empty";
  name = "Empty";
  old_file = "Empty";
  num_changed = FALSE;
  name_changed = FALSE;
  other_changed = FALSE;
  file_changed = FALSE;
  new_entry = FALSE;
  runlevel_changed = FALSE;
  nr = 0;
  oldNr = 0;
  old_name = "Empty";
  file_name = "Empty";
  _oldRunlevel = "Empty";
}

KSVDragData::KSVDragData( KSVDragData& o ) : QObject() {
  fromString(o.toString());
}

KSVDragData& KSVDragData::operator=( KSVDragData& o ) {
  if (this != &o) {
    fromString(o.toString());
  }

  return *this;
}

KSVDragData::~KSVDragData() {
}

QString KSVDragData::toString() const {
  QString data = path;
  data += "|";
  data += file_name;
  data += "|";
  data += QString().setNum(nr);
  data += "|";
  data += run_level;
  data += "|";
  data += old_file;
  data += "|";
  data += QString().setNum(oldNr);
  data += "|";
  data += QString().setNum((int)num_changed);
  data += "|";
  data += QString().setNum((int)name_changed);
  data += "|";
  data += QString().setNum((int)file_changed);
  data += "|";
  data += QString().setNum((int)other_changed);
  data += "|";
  data += name;
  data += "|";
  data += old_name;
  data += "|";
  data += QString().setNum((int)new_entry);
  data += "|";
  data += _oldRunlevel;

  return data;
}

void KSVDragData::fromString(const QString _data) {
  int pos[MAX_POS];

  pos[0] = _data.find('|');
  for (int i = 1; i < MAX_POS; ++i)
    pos[i] = _data.find('|', pos[i-1] + 1);

  path = _data.mid(0, pos[0]);
  file_name = _data.mid(pos[0] + 1, pos[1] - pos[0] -1);
  nr = _data.mid(pos[1] + 1, pos[2] - pos[1] - 1).toInt();
  run_level = _data.mid(pos[2] + 1, pos[3] - pos[2] - 1);
  old_file = _data.mid(pos[3] + 1, pos[4] - pos[3] - 1);
  oldNr = _data.mid(pos[4] + 1, pos[5] - pos[4] - 1).toInt();
  num_changed = _data.mid(pos[5] + 1, pos[6] - pos[5] - 1).toInt();
  name_changed = _data.mid(pos[6] + 1, pos[7] - pos[6] - 1).toInt();
  file_changed = _data.mid(pos[7] + 1, pos[8] - pos[7] - 1).toInt();
  other_changed = _data.mid(pos[8] + 1, pos[9] - pos[8] - 1).toInt();
  name = _data.mid(pos[9] + 1, pos[10] - pos[9] -1);
  old_name = _data.mid(pos[10] + 1, pos[11] - 1 - pos[10]);
  new_entry = _data.mid(pos[11] + 1, pos[12] -1 - pos[11]).toInt();
  _oldRunlevel = _data.mid(pos[12] + 1, _data.length() - 1 - pos[12]);
}

bool KSVDragData::isChanged() const {
  return (num_changed || name_changed || file_changed);// || other_changed);
}

bool KSVDragData::isNumChanged() const {
  return num_changed;
}

bool KSVDragData::isNameChanged() const {
  return name_changed;
}

QString KSVDragData::oldFilename() const {
  return old_file;
}

QString KSVDragData::filename() const {
  return file_name;
}

QString KSVDragData::filenameAndPath() const {
  QString result = path;
  
  if (result.right(1) != "/")
    result += "/";
  
  return result + file_name;
}

QString KSVDragData::runlevel() const {
  return run_level;
}

int KSVDragData::number() const {
  return nr;
}

int KSVDragData::oldNumber() const {
  return oldNr;
}

void KSVDragData::setChanged( const bool val ) {
  num_changed = val;
  file_changed = val;
  other_changed = val;
  name_changed = val;
}

void KSVDragData::setFilename( const QString _str ) {
  if (!file_changed)
    old_file = file_name;

  file_name = _str;
  file_changed = TRUE;
}

void KSVDragData::setPath( const QString _str ) {
  path = _str;

  other_changed = TRUE;
}

void KSVDragData::setRunlevel( const QString _str ) {
  if (!runlevel_changed)
    _oldRunlevel = run_level;

  run_level = _str;

  runlevel_changed = TRUE;
}

void KSVDragData::setNumber( const int val ) {
  if (!num_changed)
    oldNr = nr;

  nr = val;
  num_changed = TRUE;
}

void KSVDragData::setFileChanged( const bool val ) {
  file_changed = val;
}

void KSVDragData::setNameChanged( const bool val ) {
  name_changed = val;
}

void KSVDragData::setOtherChanged( const bool val ) {
  other_changed = val;
}

void KSVDragData::setNumChanged( const bool val ) {
  num_changed = val;
}

void KSVDragData::setName( const QString _str ) {
  if (!name_changed)
    old_name = name;

  name = _str;
  name_changed = TRUE;
}

QString KSVDragData::currentName() const {
  return name;
}

QString KSVDragData::oldName() const {
  return old_name;
}

bool KSVDragData::isNew() const {
  return new_entry;
}

void KSVDragData::setNew( const bool val ) {
  new_entry = val;
}

QString KSVDragData::oldRunlevel() const {
  return _oldRunlevel;
}

bool KSVDragData::isRunlevelChanged() const {
  return runlevel_changed;
}

void KSVDragData::setRunlevelChanged( const bool val ) {
  runlevel_changed = val;
}
