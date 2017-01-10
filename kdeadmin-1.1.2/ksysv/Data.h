/* Yo, Emacs, this -*- C++ -*-

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

#ifndef KSV_DRAGDATA_H
#define KSV_DRAGDATA_H

#include <qobject.h>
#include <qstring.h>
#include <qpixmap.h>

class KSVDragData : public QObject {
  Q_OBJECT //;

public:
  KSVDragData();

  /**
   * Copy constructor.
   */
  KSVDragData(KSVDragData& o);

  /**
   * assignment operator
   */
  KSVDragData& operator= (KSVDragData& o);

  virtual ~KSVDragData();

  /* class variables */
 protected:
  QString path;
  QString run_level;
  bool num_changed;
  bool name_changed;
  bool other_changed;
  bool file_changed;
  bool new_entry;
  bool runlevel_changed;
  QString old_name;
  QString old_file;
  QString name;
  QString file_name;
  QString _oldRunlevel;
  int nr;
  int oldNr;

  /* conversion functions */
 public:
  virtual QString toString() const;
  virtual void fromString(const QString _data);

  /* other interface functions */
 public:
  bool isChanged() const ;
  bool isNumChanged() const;
  bool isNameChanged() const;
  bool isRunlevelChanged() const;
  bool isNew() const;
  QString oldFilename() const;
  QString oldName() const;
  QString currentName() const;
  QString filename() const;
  QString filenameAndPath() const;
  QString runlevel() const;
  QString oldRunlevel() const;
  int number() const;
  int oldNumber() const;

 public slots:
  void setNameChanged( const bool val = TRUE );
  void setNumChanged( const bool val = TRUE );
  void setFileChanged( const bool val = TRUE );
  void setOtherChanged( const bool val = TRUE );
  void setChanged( const bool val = TRUE );
  void setFilename( const QString _str );
  void setPath( const QString _str );
  void setRunlevel( const QString _str );
  void setNumber( const int val );
  void setName( const QString _str );
  void setNew( const bool val = TRUE );
  void setRunlevelChanged( const bool val = TRUE );

};

#endif
