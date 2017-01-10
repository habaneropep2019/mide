// -*- C++ -*-

//
//  klpq
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.ivm.de or chris@kde.org
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef klpqspooler_included
#define klpqspooler_included

class QString;
class QListBox;
class QCheckBox;
class KProcess;
class MyRowTable;

class Spooler
{
public:
  Spooler () {}
  virtual ~Spooler () {}

  virtual void    updateCommand ( KProcess *proc, QString queue );
  virtual void    setQueuingCommand ( KProcess *proc, QString queue, bool b );
  virtual QString removeCommand ( QString queue, QString id );
  virtual void    makeTopCommand ( KProcess *proc, QString queue, QString id );
  virtual void    setPrintingCommand ( KProcess *proc, QString queue, bool b);
  virtual void    statusCommand ( KProcess *proc, QString queue );

  virtual void parseUpdate ( MyRowTable *list, QListBox *status, QString lpq_in_buffer );

  /*
  void parseRemove ( ) {}
  void parseMakeTop ( ) {}
  */

  virtual int  parsePrinting ( char *buffer, int len );
  virtual int  parseQueuing ( char *buffer, int len );
  virtual void parseStatus ( char *buffer, int len, QCheckBox *c_queuing, QCheckBox *c_printing );

protected:
  QString lpq_path;
  QString lpc_path;
  QString lprm_path;
};

class SpoolerBsd : public Spooler
{
public:
  SpoolerBsd ();
  virtual  ~SpoolerBsd () {}
};

class SpoolerPpr : public Spooler
{
public:
  SpoolerPpr ();
  virtual ~SpoolerPpr () {}

  void    updateCommand ( KProcess *proc, QString queue );
  void    setQueuingCommand ( KProcess *proc, QString queue, bool b );
  QString removeCommand ( QString queue, QString id );
  void    makeTopCommand ( KProcess *proc, QString queue, QString id );
};

class SpoolerLprng : public Spooler
{
public:
  SpoolerLprng ();
  virtual ~SpoolerLprng () {}

  void parseUpdate ( MyRowTable *list, QListBox *status, QString lpq_in_buffer );

  virtual int  parsePrinting ( char *buffer, int len );
  virtual int  parseQueuing ( char *buffer, int len );
  virtual void parseStatus ( char *buffer, int len, QCheckBox *c_queuing, QCheckBox *c_printing );

};

#endif /* klpqspooler_included */
