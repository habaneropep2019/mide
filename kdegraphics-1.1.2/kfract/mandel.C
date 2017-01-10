
    /*

    mandel.C  version 0.1.2

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


#include <stdio.h>

#include "mandel.h"


#define DEFAULT_X -0.5
#define DEFAULT_Y  0.0
#define DEFAULT_WIDTH 4.0
#define DEFAULT_BAILOUT 4.0


Mandel::Mandel()
  {
  }


void Mandel::init( double center_x, double center_y, 
                  double dx, double dy,
                  int x_max, int y_max,
                  int iter_max, double bail_out )
  {
  Fract::init( center_x, center_y, dx, dy,
               x_max, y_max, iter_max, bail_out );
  }


int Mandel::calcPoint( int x, int y )
  {
  double r = x_values[x], i = y_values[y]; 
  double r_out = r, i_out = i; 
  double tmp, r2 = r * r, i2 = i * i;
  register int iter = 0;

  while ( ( r2 + i2 < bail ) && ( iter < max_iter ) )
    {
    tmp = 2.0 * r_out * i_out + i;
// This is sort of sneaky because we actually compare the values of the
// last iteration to bailout. But the results are very close to those of
// the correct way and it's significantly faster.
    r_out = ( r2 = r_out * r_out ) - ( i2 = i_out * i_out ) + r;
    i_out = tmp;
    iter++;
    }
  return iter;
  }



double Mandel::defaultCenterX()
  {
  return DEFAULT_X;
  }


double Mandel::defaultCenterY()
  {
  return DEFAULT_Y;
  }


double Mandel::defaultWidth()
  {
  return DEFAULT_WIDTH; 
  }


double Mandel::defaultBailout()
  {
  return DEFAULT_BAILOUT;
  }
