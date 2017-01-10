/***************************************************************************
 *                              KAQueryList.cpp                            *
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

#include "KAQueryList.moc"
#include "KAQueryList.h"

#define min(x1,x2) ((x1<x2)?x1:x2)

int
KAHostList::compareItems( GCI item1, GCI item2 )
{
  return strcmp(((KAQueryFile *)item1)->getHost(), ((KAQueryFile *)item2)->getHost());
}

int
KADomainList::compareItems( GCI item1, GCI item2 )
{
  uint step = 0, step1, step2, ret;
  /* domains werden von hinten nach vorne sortiert im Namen */
  const char *host1 = (const char *)((KAQueryFile *)item1)->getHost();
  const char *host2 = (const char *)((KAQueryFile *)item2)->getHost();
  //  debug("compare %s against %s", host1, host2);
  // go to the end of the strings, pointing to '\0'
  char *index1 = (char *)host1 + strlen(host1);
  char *index2 = (char *)host2 + strlen(host2);
  char *oldindex1, *oldindex2;

  while ((index1 != host1) && (index2 != host2)) {
    oldindex1 = index1;
    oldindex2 = index2;
    // go reverse to the begin
    while ((index1!=host1) && ('.' != *index1))
      index1--;
    while ((index2!=host2) && ('.' != *index2))
      index2--;

    step1 = oldindex1 - index1;
    step2 = oldindex2 - index2;
    step = (step1<step2)?step1:step2;
    //    debug("index1 %s, index2 %s, len1 %i, len2 %i, step %i", index1, index2, step1, step2, step);

    /*
    // go reverse to the front and count the steps
    while ((index1!=host1) && ('.' != *index1) &&
	   (index2!=host2) && ('.' != *index2)) {
      step++; index1--; index2--; 
    }
    if (index1 == host1 || index2 == host2) {
      // have to be handled separat
      break;
    }
    */

    ret = strnicmp(index1, index2, step);
    if (ret) {
      // both strings are NOT the same for 'step'-len
      //      debug ("strings NOT same, return %i", ret);
      return ret;
    }
    // both strings ARE the same for 'step'-len

    if (step1 < step2) {
      //      debug ("strings ARE same, 1 smaller");
      // index1 smaller than index2 -> index1 less than index2
      return -1;
    }  
    if (step2 < step1) {
      // index2 smaller than index1
      //      debug ("strings ARE same, 2 smaller");
      return 1;
    }
    // both strings are the same and the same length between '.' -- '.'

    // if not reached the beginning of the strings loop again
    if (index1 != host1)
      index1--;
    if (index2 != host2)
      index2--;
    //    step = 0;
  }

  // ok, here we are:
  // both strings have bee the same for all parts of the url.
  // Then the urls have to be the same
  //  debug ("strings are TOTALLY same");
  return 0;
}

int
KASizeList::compareItems( GCI item1, GCI item2 )
{
  return ((KAQueryFile *)item1)->getSize() - ((KAQueryFile *)item2)->getSize();
}

int
KADateList::compareItems( GCI item1, GCI item2 )
{
  if ( ((KAQueryFile *)item1)->getDateTime() < ((KAQueryFile *)item2)->getDateTime() )
    return 1;
  if ( ((KAQueryFile *)item1)->getDateTime() == ((KAQueryFile *)item2)->getDateTime() )
    return 0;

  return -1;
}

