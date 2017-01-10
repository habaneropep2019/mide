
    /*

    fract.C   0.1.2

    Copyright (C) 1997 Uwe Thiem   
                       uwe@uwix.alt.na

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */


#include <kapp.h>
#include <stdio.h>

#include "fract.h"



Fract::Fract()
  {
  }


void Fract::init( double center_x, double center_y, 
                  double dx, double dy,
                  int x_max, int y_max,
                  int iter_max, double bail_out,
                  double param_1, double param_2 )
  {
  int i;
  double nonsense;

  nonsense = param_1;
  nonsense += param_2;

  max_x = x_max + 1;
  max_y = y_max + 1;
  max_iter = iter_max;
  bail = bail_out;
  
  for ( i = 0; i < max_x; i++ )
    {
    x_values[i] = center_x - dx * x_max / 2.0 + dx * i;
    }
  for ( i = 0; i < y_max; i++ )
    {
    y_values[i] = center_y - dy * y_max / 2.0 + dy * i;
    }
  }


int Fract::calcPoint( int x, int y )
  {
  int a, b;
  a = b = 0;
  a += x;
  b += y;

  fprintf( stderr, i18n("Fract::calcPoint() is an abstract method. You MUST\n\
re-write it!\n"));

  return 0;
  }


double Fract::defaultCenterX()
  {
  return 0.0;
  }


double Fract::defaultCenterY()
  {
  return 0.0;
  }


double Fract::defaultWidth()
  {
  return 0.0;
  }


double Fract::defaultBailout()
  {
  return 0.0;
  }


double Fract::defaultExtraX()
  {
  return 0.0;
  }


double Fract::defaultExtraY()
  {
  return 0.0;
  }
