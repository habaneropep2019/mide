/***************************************************************************
 *                            KASettingsTypes.cpp                          *
 *                            -------------------                          *
 *                         Source file for KArchie                         *
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

#include "KASettingsTypes.h"

#define NICELEVEL_BIAS 10 /* offset of nicelevel recognition is 10 percent */
#define NICELEVEL_COMPUTE_OFFSET  (100 - NICELEVEL_BIAS)

NiceLevel::mode
NiceLevel::toLevel( int input )
{
  //debug("input ist %i",input);
  long tmp;
  tmp = NICELEVEL_COMPUTE_OFFSET * (int)NiceLevel::nicest / 100;
  //debug("tmp ist %ld", tmp);
  if (input > tmp)
    return NiceLevel::nicest;
  tmp = NICELEVEL_COMPUTE_OFFSET * (int)NiceLevel::extremlynice / 100;
  if (input > tmp)
    return NiceLevel::extremlynice;
  tmp = NICELEVEL_COMPUTE_OFFSET * (int)NiceLevel::verynice / 100;
  if (input > tmp)
    return NiceLevel::verynice;
  tmp = NICELEVEL_COMPUTE_OFFSET * (int)NiceLevel::nicer / 100;
  if (input > tmp)
    return NiceLevel::nicer;
  tmp = NICELEVEL_COMPUTE_OFFSET * (int)NiceLevel::nice / 100;
  if (input > tmp)
    return NiceLevel::nice;
  
  return NiceLevel::norm;
}


const char *
NiceLevel::toString(NiceLevel::mode input)
{
  switch (input) {
  case norm: return "norm"; break;
  case nice: return "nice"; break;
  case nicer: return "nicer"; break;
  case verynice: return "very nice"; break;
  case extremlynice: return "extremly nice"; break;
  case nicest: return "nicest"; break;
  default: return (char*)0;
  }
}


//QStrList SearchMode::namelist;

SearchMode::SearchMode( mode init )
  :internmode( init )
{
}

SearchMode::SearchMode( char init )
  :internmode( (mode)init )
{
}

SearchMode::mode
SearchMode::toMode(char input)
  { return (SearchMode::mode)input; }

char
SearchMode::toChar(SearchMode::mode input)
  { return (char)input; }

const char *
SearchMode::toString(SearchMode::mode input)
{
  switch(input) {
  case exact: return "exact";
  case substr: return "sub string";
  case subcase: return "sub string case insensitive";
  case regexp: return "regular expression";
  case exactsubstr: return "exact, then substr";
  case exactsubcase: return "exact, then subcase";
  case exactregexp: return "exact, then regexp";
  case none: // fall through
  default: return "none";
  }
}


const char ConfigEntries::HostConfigGroup[]     = "Hostconfig";
const char ConfigEntries::CurrentHostEntry[]    = "CurrentHost";
const char ConfigEntries::CurrentHostDefault[]  = "archie.sura.net";
const char ConfigEntries::CurrentHostNumEntry[] = "CurrentHostId";
const int  ConfigEntries::CurrentHostNumDefault = 0;
