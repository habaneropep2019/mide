/**************************************************************************

    randomlist.cpp  - Some "random functions" :-)
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

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

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include "randomlist.h"
#include <stdlib.h>
#include <stdio.h>

#define RAND_UNIFORM (double)rand()/(double)RAND_MAX

int random_discrete(double *distrib,int n)
{
    int i=0;
    double g=0.0;
    double z=0.0;
    while ((z==0.0)||(z==1.0)) z=RAND_UNIFORM;
    while ((g<z)&&(i<n))
    {
        g+=distrib[i];
        i++;
    };
    return i-1;
};

double *generate_discrete_uniform_distrib(int n)
{
    double *distrib=new double[n];

    for (int i=0;i<n;i++)
        distrib[i]=1.0/(double)n;

    return distrib;
};

void remove_lmn_from_discrete_distrib(int k,double *distrib,int n,int used=0)
{
    int i;
    if (used==0)  //guess how many values of the v.a. are strictly positive
    {
        for (i=0;i<n;i++) if (distrib[i]>0) used++;
    };
    used--; // k will no longer be used :-)
    if (used==0) return;
    double piece=distrib[k]/(double)used;
    distrib[k]=0.0;
    for (i=0;i<n;i++) if (distrib[i]>0) distrib[i]+=piece;
};

void show_distrib(double *distrib,int n)
{
    printf("(");
    for (int j=0;j<n;j++) printf("%g,",distrib[j]);
    printf(")\n");
};

int *generate_random_list(int n)
{
    if (n==0) return NULL;
    int *list=new int[n];
    double *distrib=generate_discrete_uniform_distrib(n);
    int used=n;
    int x;
    int i=1;
    while (used>0)
    {
        x=random_discrete(distrib,n);
        list[x]=i;
        i++;
        remove_lmn_from_discrete_distrib(x,distrib,n,used);
        used--;
    };
    delete distrib;
    
    return list;
};

int *generate_list(int n)
{
    int *list=new int[n];
    for (int i=0;i<n;i++)
        list[i]=i+1;

    return list;
};
