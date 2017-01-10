/* -*- c++ -*- */
/***************************************************************************
 *                             KASettingsTypes.h                           *
 *                            -------------------                          *
 *                         Header file for KArchie                         *
 *                  -A programm to display archie queries                  *
 *                                                                         *
 *                KArchie is written for the KDE-Project                   *
 *                         http://www.kde.org                              *
 *                                                                         *
 *   Copyright (C) Oct 1997 Jörg Habenicht                                 *
 *                  E-Mail: j.habenicht@europemail.com                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          * 
 *                                                                         *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
 *                                                                         *
 ***************************************************************************/

#ifndef KASETTINSTYPES_H
#define KASETTINSTYPES_H

#include <qstring.h>
#include <qstrlist.h>

class NiceLevel
{

public:
  enum mode{ norm = 0, nice = 500, nicer = 1000, verynice = 5000, extremlynice = 10000, nicest = 32765 };
NiceLevel(): internmode(norm) {};
NiceLevel( enum mode input) : internmode(input){};
  //~NiceLevel();

  NiceLevel  &operator=(NiceLevel::mode input) { internmode = input; return *this; }
  void        setLevel(NiceLevel::mode input) { internmode = input; }
  void        setLevel(int input) { internmode = toLevel(input); }
  mode        getLevel() const { return internmode; }
  const char *toString() const { return toString(internmode); }
  int         toLevel()  const { return (int)internmode; }

  static NiceLevel::mode toLevel(int input);
  static const char     *toString(NiceLevel::mode input);

private:
  mode internmode;
};


class SearchMode
{

public:
  enum mode{ none = '\0', exact = '=', substr = 'S' , subcase = 'C', regexp = 'R' , exactsubstr = 's', exactsubcase = 'c' , exactregexp = 'r' };
SearchMode( mode );
SearchMode( char );
  //~SearchMode();

  inline SearchMode& operator=(SearchMode::mode input);
  inline char getChar() const;
  inline mode getMode() const;
  inline void setMode( mode input );
  inline void setMode( char input );

  static SearchMode::mode toMode(char input);
  static char             toChar(SearchMode::mode input);
  static const char      *toString(SearchMode::mode input);

  //  static const QStrList &fullList() { return namelist; }
  
private:
  //  static void initNameList();

  mode internmode;
  //  static QStrList namelist;

};

class SortMode
{
public:
  enum mode{ date, name, weight };
SortMode( mode );
~SortMode();

private:

};

class ConfigEntries
{
public:

  static const char HostConfigGroup[];
  static const char CurrentHostEntry[];
  static const char CurrentHostDefault[];
  static const char CurrentHostNumEntry[];
  static const int  CurrentHostNumDefault;

};

SearchMode& SearchMode::operator=(SearchMode::mode input)
  { internmode = input; return *this; }
char SearchMode::getChar() const
  { return (char)internmode; }
SearchMode::mode SearchMode::getMode() const
  { return internmode; }
void SearchMode::setMode( mode input )
  { internmode = input; }
void SearchMode::setMode( char input )
  { internmode = (mode)input; }

#endif
