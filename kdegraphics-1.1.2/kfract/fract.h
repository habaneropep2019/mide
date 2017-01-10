
    /*

    fract.h   0.1.2

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

#ifndef FRACT_H
# define FRACT_H


class Fract
  {
  public:
    Fract();
    virtual void init( double center_x, double center_y, 
                       double dx, double dy,
                       int x_max, int y_max, 
                       int iter_max, double bail_out, 
                       double param_1 = 0.0, double param_2 = 0.0 );
    virtual int calcPoint( int x, int y );
    virtual double defaultCenterX();
    virtual double defaultCenterY();
    virtual double defaultWidth();
    virtual double defaultBailout();
    virtual double defaultExtraX();
    virtual double defaultExtraY();
  protected:
    double x_values[2000];
    double y_values[2000];
    int max_x, max_y;
    int max_iter;
    double bail;
  private:
  };


#endif   // FRACT_H
