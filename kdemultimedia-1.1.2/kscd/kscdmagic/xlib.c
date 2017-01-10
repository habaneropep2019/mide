/* 
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright (C) 1996,1997 by
 *
 *   $ Id: $
 *
 *      Jan Hubicka          (hubicka@paru.cas.cz)
 *      Thomas Marsh         (tmarsh@austin.ibm.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#if defined(__linux__) || defined(__svr4__)


#define X11_DRIVER

#ifdef X11_DRIVER
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "xlib.h"

#ifdef AMIGA
#define XFlush(x) while(0)
#endif


#undef PIXMAP

#define chkalloc(n) if (!n) fprintf(stderr, "out of memory\n"), exit(3)

int xupdate_size(xdisplay * d)
{
    int tmp;
    Window wtmp;
    int width = d->width, height = d->height;
    XGetGeometry(d->display, d->window, &wtmp, &tmp, &tmp, 
		 &d->width, &d->height, (unsigned int *) &tmp, (unsigned int *) &tmp);

    if ((int)d->width != width || (int)d->height != height)
	return 1;
    return 0;
}

void xflip_buffers(xdisplay * d)
{
    d->back = d->vbuffs[d->current];
    d->current ^= 1;
    d->vbuff = d->vbuffs[d->current];
}

void draw_screen(xdisplay * d)
{

    switch (d->image[0]->bits_per_pixel) {

    case 16:{
	    unsigned short *de;
	    unsigned char *s;
	    unsigned char *e;
	    for (s = (unsigned char *) d->vbuffs[d->current],
		 e = (unsigned char *) d->vbuffs[d->current] + (d->linewidth * d->height),
		 de = (unsigned short *) d->data[d->current]; s < e; s += 8, de += 8)
		*de = d->pixels[*s],
		    *(de + 1) = d->pixels[*(s + 1)],
		    *(de + 2) = d->pixels[*(s + 2)],
		    *(de + 3) = d->pixels[*(s + 3)],
		    *(de + 4) = d->pixels[*(s + 4)],
		    *(de + 5) = d->pixels[*(s + 5)],
		    *(de + 6) = d->pixels[*(s + 6)],
		    *(de + 7) = d->pixels[*(s + 7)];
	    s -= 8;
	    de -= 8;
	    for (; s < e; s++, de++)
		*de = d->pixels[*s];
	    break;
	}

    case 32:{
	    unsigned long *de;
	    unsigned char *s;
	    unsigned char *e;
	    for (s = (unsigned char *) d->vbuffs[d->current],
		 e = (unsigned char *) d->vbuffs[d->current] + 
		   (d->linewidth * d->height),
		 de = (unsigned long *) d->data[d->current]; s < e; s += 8, de += 8)
		*de = d->pixels[*s],
		    *(de + 1) = d->pixels[*(s + 1)],
		    *(de + 2) = d->pixels[*(s + 2)],
		    *(de + 3) = d->pixels[*(s + 3)],
		    *(de + 4) = d->pixels[*(s + 4)],
		    *(de + 5) = d->pixels[*(s + 5)],
		    *(de + 6) = d->pixels[*(s + 6)],
		    *(de + 7) = d->pixels[*(s + 7)];
	    s -= 8;
	    de -= 8;
	    for (; s < e; s++, de++)
		*de = d->pixels[*s];
	    break;
	}
    }

#ifdef MITSHM

    if (d->SharedMemFlag) {

	XShmPutImage(d->display, d->window, d->gc, d->image[d->current], 0, 0, 0,
		     0, d->width, d->height, True);

	XFlush(d->display);

    } else

#endif


    {
	XPutImage(d->display, d->window, d->gc, d->image[d->current], 
		  0, 0, 0, 0, d->width, d->height);
	XFlush(d->display);
    }

    d->screen_changed = 0;
}

#ifdef MITSHM
int alloc_shm_image(xdisplay * new)
{
    register char *ptr;
    int temp, size = 0, i;
    ptr = DisplayString(new->display);
    if (!ptr || (*ptr == ':') || !strncmp(ptr, "localhost:", 10) ||
	!strncmp(ptr, "unix:", 5) || !strncmp(ptr, "local:", 6)) {
	new->SharedMemOption = XQueryExtension(new->display, "MIT-SHM", &temp, &temp, &temp);
    } else {
	new->SharedMemOption = False;
	return 0;
    }
    new->SharedMemFlag = False;
#if 0
    new->SharedMemOption = True;
    new->SharedMemFlag = False;
#endif

    if (new->SharedMemFlag) {
	XShmDetach(new->display, &new->xshminfo[0]);
	XShmDetach(new->display, &new->xshminfo[1]);
	new->image[0]->data = (char *) NULL;
	new->image[1]->data = (char *) NULL;
	shmdt(new->xshminfo[0].shmaddr);
	shmdt(new->xshminfo[1].shmaddr);
    }
    for (i = 0; i < 2; i++) {
	if (new->SharedMemOption) {
	    int mul;
	    if (new->depth == 8)
		mul = 1;
	    else if (new->depth <= 24)
		mul = 2;
	    else
		mul = 4;
	    new->SharedMemFlag = False;
	    new->image[i] = XShmCreateImage(new->display, new->visual, new->depth, ZPixmap,
		 NULL, &new->xshminfo[i], new->width, new->height * mul);
	    if (new->image[i]) {
		temp = new->image[i]->bytes_per_line * new->image[i]->height;
		new->linewidth = new->image[i]->bytes_per_line * 8 / new->image[i]->bits_per_pixel;
		if (temp > size)
		    size = temp;
		new->xshminfo[i].shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0777);
		if (new->xshminfo[i].shmid != -1) {
		    new->xshminfo[i].shmaddr = (char *) shmat(new->xshminfo[i].shmid, 0, 0);
		    if (new->xshminfo[i].shmaddr != (char *) -1) {
			new->image[i]->data = new->xshminfo[i].shmaddr;
			new->data[i] = new->vbuffs[i] = (char *) new->image[i]->data;
			new->xshminfo[i].readOnly = True;

			new->SharedMemFlag = XShmAttach(new->display, &new->xshminfo[i]);
			XSync(new->display, False);
			if (!new->SharedMemFlag) {
			    XDestroyImage(new->image[i]);
			    new->image[i] = (XImage *) NULL;
			    new->SharedMemFlag = 0;
			    return 0;
			}
		    }
		    /* Always Destroy Shared Memory Ident */
		    shmctl(new->xshminfo[i].shmid, IPC_RMID, 0);
		}
		if (!new->SharedMemFlag) {
		    XDestroyImage(new->image[i]);
		    new->image[i] = (XImage *) NULL;
		    new->SharedMemFlag = 0;
		    return 0;
		}
	    } else {
		new->SharedMemFlag = 0;
		return 0;
	    }
	} else {
	    new->SharedMemFlag = 0;
	    return 0;
	}
    }
    new->current = 0;
    xflip_buffers(new);
    return 1;
}

void free_shm_image(xdisplay * d)
{
    if (d->SharedMemFlag) {
	XDestroyImage(d->image[0]);
	XDestroyImage(d->image[1]);
	XShmDetach(d->display, &d->xshminfo[0]);
	XShmDetach(d->display, &d->xshminfo[1]);
	shmdt(d->xshminfo[0].shmaddr);
	shmdt(d->xshminfo[1].shmaddr);
    }
}

#endif

int alloc_image(xdisplay * d)
{
    int i;
#ifdef MITSHM
    if (!d->params->nomitshm && alloc_shm_image(d)) {
	if (d->depth != 8) {
	    for (i = 0; i < 2; i++)
		d->vbuffs[i] = malloc(d->linewidth * d->height);
	}
	return 1;
    }
#endif
    for (i = 0; i < 2; i++) {
	d->image[i] = XCreateImage(d->display, d->visual, d->depth, ZPixmap, 0,
				   NULL, d->width, d->height, 8, 0);
	if (d->image[i] == NULL) {
	    printf("Out of memory for image..exiting\n");
	    exit(3);
	}
	d->image[i]->data = malloc(d->image[i]->bytes_per_line * d->height);
	if (d->image[i]->data == NULL) {
	    printf("Out of memory for image buffers..exiting\n");
	    exit(3);
	}
	d->data[i] = d->vbuffs[i] = (char *) d->image[i]->data;
	d->linewidth = d->image[i]->bytes_per_line * 8 / d->image[i]->bits_per_pixel;
    }
    if (d->depth != 8) {
	for (i = 0; i < 2; i++) {
	    d->vbuffs[i] = malloc(d->linewidth * d->height);
	    if (d->vbuffs[i] == NULL) {
		printf("Out of memory for image buffers2..exiting\n");
		exit(3);
	    }
	}
    }
    xflip_buffers(d);
    return 1;
}

void free_image(xdisplay * d)
{
    if (d->depth != 8)
	free(d->vbuffs[0]), free(d->vbuffs[1]);
#ifdef MITSHM
    if (d->SharedMemFlag) {
	free_shm_image(d);
	return;
    }
#endif
    XDestroyImage(d->image[0]);
    XDestroyImage(d->image[1]);
}
#define MAX(x,y) ((x)>(y)?(x):(y))


xdisplay *xalloc_display(char *s, int xHint, int yHint, int x, int y, xlibparam * params)
{
    xdisplay *newd;
    Visual *defaultvisual;
    XVisualInfo vis;


    newd = (xdisplay *) calloc(sizeof(xdisplay), 1);
    chkalloc(newd);
    newd->display = XOpenDisplay((char *) NULL);
    if (!newd->display) {
	free((void *) newd);
	return NULL;
    }
    newd->screen = DefaultScreen(newd->display);
    newd->attributes = (XSetWindowAttributes *)
	malloc(sizeof(XSetWindowAttributes));
    chkalloc(newd->attributes);
    newd->attributes->background_pixel = BlackPixel(newd->display,
						   newd->screen);
    newd->attributes->border_pixel = BlackPixel(newd->display, newd->screen);
    newd->attributes->event_mask = ButtonPressMask | StructureNotifyMask | ButtonReleaseMask | ButtonMotionMask | KeyPressMask | ExposureMask | KeyReleaseMask;
    newd->attributes->override_redirect = False;
    newd->attr_mask = CWBackPixel | CWBorderPixel | CWEventMask;
    newd->classX = InputOutput;
    newd->xcolor.n = 0;
    newd->parent_window = RootWindow(newd->display, newd->screen);
    defaultvisual = DefaultVisual(newd->display, newd->screen);
    newd->params = params;
    if (!params->usedefault) {
	if (defaultvisual->class != PseudoColor || 
	    (!XMatchVisualInfo(newd->display,
			       newd->screen, 8, PseudoColor, &vis) 
	     && vis.colormap_size > 128)) {
	    newd->fixedcolormap = 1;
	    if (!XMatchVisualInfo(newd->display, newd->screen, 15, TrueColor, &vis)) {
		if (!XMatchVisualInfo(newd->display, newd->screen, 16, TrueColor, &vis)) {
		    if (!XMatchVisualInfo(newd->display, newd->screen, 32, TrueColor, &vis) &&
			!XMatchVisualInfo(newd->display, newd->screen, 24, TrueColor, &vis)) {
			if (!XMatchVisualInfo(newd->display, newd->screen, 8, PseudoColor, &vis) &&
			    !XMatchVisualInfo(newd->display, newd->screen, 7, PseudoColor, &vis)) {
			    if (!XMatchVisualInfo(newd->display, newd->screen, 8, TrueColor, &vis) &&
				!XMatchVisualInfo(newd->display, newd->screen, 8, StaticColor, &vis) &&
				!XMatchVisualInfo(newd->display, newd->screen, 8, StaticGray, &vis)) {
				printf("Display does not support PseudoColor depth 7,8,StaticColor depth 8, StaticGray depth 8, Truecolor depth 8,15,16,24 nor 32! try -usedefault\n");
				return NULL;
			    } else
				newd->truecolor = 1;
			} else
			    newd->fixedcolormap = 0, newd->truecolor = 0;
		    } else
			newd->truecolor = 1;
		} else
		    newd->truecolor = 1;
	    } else
		newd->truecolor = 1;
	} else {
	    newd->truecolor = 0;
	}
	newd->depth = vis.depth;
	newd->visual = vis.visual;
    } else {			/*usedefault */
	vis.depth = newd->depth = DefaultDepth(newd->display, newd->screen);
	newd->visual = defaultvisual;
	switch (defaultvisual->class) {
	case PseudoColor:
	    if (newd->depth <= 8) {
		newd->depth = 8;
		newd->truecolor = 0;
		newd->fixedcolormap = 0;
	    } else {
		printf("Pseudocolor visual on unsuported depth try autodetection of visuals\n");
		return NULL;
	    }
	    break;
	case TrueColor:
	case StaticColor:
	case StaticGray:
	    newd->truecolor = 1;
	    newd->fixedcolormap = 1;
	    if (newd->depth <= 8)
		newd->depth = 8;
	    else if (newd->depth <= 16)
		newd->depth = 16;
	    else if (newd->depth <= 32)
		newd->depth = 32;
	    else {
		printf("Truecolor visual on unsuported depth try autodetection of visuals\n");
		return NULL;
	    }
	    break;
	default:
	    printf("Unusuported visual\n");
	    break;
	}
    }
    /*newd->visual->map_entries = 256; */
    newd->colormap = newd->defaultcolormap = DefaultColormap(newd->display, newd->screen);

    newd->window_name = s;
    newd->height = y;
    newd->width = x;
    newd->border_width = 2;
    newd->lastx = 0;
    newd->lasty = 0;
    newd->font_struct = (XFontStruct *) NULL;

    newd->window = XCreateWindow(newd->display, newd->parent_window, xHint, yHint,
			      newd->width, newd->height, newd->border_width,
				vis.depth, newd->classX, newd->visual,
				newd->attr_mask, newd->attributes);
    if (!newd->fixedcolormap && params->privatecolormap) {
	unsigned long pixels[256];
	int i;
	newd->colormap = XCreateColormap(newd->display, newd->window, newd->visual, AllocNone);
	XAllocColorCells(newd->display, newd->colormap, 1, 0, 0, pixels, MAX(newd->visual->map_entries, 256));
	for (i = 0; i < 16; i++) {
	    newd->xcolor.c[i].pixel = pixels[i];
	}
	XQueryColors(newd->display, newd->defaultcolormap, newd->xcolor.c, 16);
	XStoreColors(newd->display, newd->colormap, newd->xcolor.c, 16);
	newd->privatecolormap = 1;
    }
    if (!newd->fixedcolormap)
	XSetWindowColormap(newd->display, newd->window, newd->colormap);
    newd->gc = XCreateGC(newd->display, newd->window, 0L, &(newd->xgcvalues));
    XSetBackground(newd->display, newd->gc,
		   BlackPixel(newd->display, newd->screen));
    XSetForeground(newd->display, newd->gc,
		   WhitePixel(newd->display, newd->screen));
    XStoreName(newd->display, newd->window, newd->window_name);
    XMapWindow(newd->display, newd->window);
#if 1
    XSelectInput(newd->display, newd->window, 
                 /* ExposureMask | KeyPress |
		   KeyRelease | 
                   ConfigureRequest | 
                   FocusChangeMask | */
		 StructureNotifyMask |
                 ButtonPressMask | ButtonReleaseMask);
#endif
#ifdef PIXAMP
    newd->pixmap = XCreatePixmap(newd->display, newd->window, newd->width,
				newd->height, newd->depth);
#endif
    return (newd);
}

void xsetcolor(xdisplay * d, int col)
{
    switch (col) {
    case 0:
	XSetForeground(d->display, d->gc,
		       BlackPixel(d->display, d->screen));
	break;
    case 1:
	XSetForeground(d->display, d->gc,
		       WhitePixel(d->display, d->screen));
	break;
    default:
	if ((col - 2) > d->xcolor.n) {
	    fprintf(stderr, "color error\n");
	    exit(3);
	}
	XSetForeground(d->display, d->gc,
		       d->xcolor.c[col - 2].pixel);
	break;
    }
}
void xrotate_palette(xdisplay * d, int direction, unsigned char co[3][256], int ncolors)
{
    int i, p;

    if (d->privatecolormap) {
	for (i = 0; i < d->xcolor.n; i++) {
	    p = d->xcolor.c[i].pixel;
	    d->xcolor.c[i].red = (int) co[0][p] * 256;
	    d->xcolor.c[i].green = (int) co[1][p] * 256;
	    d->xcolor.c[i].blue = (int) co[2][p] * 256;
	}
	XStoreColors(d->display, d->colormap, d->xcolor.c, d->xcolor.n);
    }
    if (d->truecolor) {
	unsigned long oldpixels[256];
	memcpy(oldpixels, d->pixels, sizeof(oldpixels));
	p = (ncolors - 1 + direction) % (ncolors - 1) + 1;
	for (i = 1; i < ncolors; i++) {		/*this is ugly..I know */
	    d->pixels[i] = oldpixels[p];
	    p++;
	    if (p >= ncolors)
		p = 1;
	}
	draw_screen(d);
    }
}
int xalloc_color(xdisplay * d, int r, int g, int b, int readwrite)
{
    d->xcolor.n++;
    d->xcolor.c[d->xcolor.n - 1].flags = DoRed | DoGreen | DoBlue;
    d->xcolor.c[d->xcolor.n - 1].red = r;
    d->xcolor.c[d->xcolor.n - 1].green = g;
    d->xcolor.c[d->xcolor.n - 1].blue = b;
    d->xcolor.c[d->xcolor.n - 1].pixel = d->xcolor.n - 1;
    if ((readwrite && !d->fixedcolormap) || d->privatecolormap) {
	unsigned long cell;
	if (d->privatecolormap) {
	    cell = d->xcolor.c[d->xcolor.n - 1].pixel += 16;
	    if (d->xcolor.c[d->xcolor.n - 1].pixel >= d->visual->map_entries) {
		d->xcolor.n--;
		return (-1);
	    }
	} else {
	    if (!XAllocColorCells(d->display, d->colormap, 0, 0, 0, &cell, 1)) {
		d->xcolor.n--;
		if (d->xcolor.n <= 32)
		    printf("Colormap is too full! close some colorfull aplications or use -private\n");
		return (-1);
	    }
	    d->xcolor.c[d->xcolor.n - 1].pixel = cell;
	}
	XStoreColor(d->display, d->colormap, &(d->xcolor.c[d->xcolor.n - 1]));
	return (cell);
    }
    if (!XAllocColor(d->display, d->colormap, &(d->xcolor.c[d->xcolor.n - 1]))) {
	d->xcolor.n--;
	if (d->xcolor.n <= 32)
	    printf("Colormap is too full! close some colorfull aplications or use -private\n");
	return (-1);
    }
    d->pixels[d->xcolor.n - 1] = d->xcolor.c[d->xcolor.n - 1].pixel;
    return (d->depth != 8 ? d->xcolor.n - 1 : d->xcolor.c[d->xcolor.n - 1].pixel);
}

void xfree_colors(xdisplay * d)
{
    unsigned long pixels[256];
    int i;
    for (i = 0; i < d->xcolor.n; i++)
	pixels[i] = d->xcolor.c[i].pixel;
    if (!d->privatecolormap)
	XFreeColors(d->display, d->colormap, pixels, d->xcolor.n, 0);
    d->xcolor.n = 0;
}

void xfree_display(xdisplay * d)
{
    XSync(d->display, 0);
    if (d->font_struct != (XFontStruct *) NULL) {
	XFreeFont(d->display, d->font_struct);
    }
    XUnmapWindow(d->display, d->window);
#ifdef PIXMAP
    XFreePixmap(d->display, d->pixmap);
#endif
    XDestroyWindow(d->display, d->window);
    XCloseDisplay(d->display);
    free((void *) d->attributes);
    free((void *) d);
}

#ifdef PIXMAP
void xline(xdisplay * d, int x1, int y1, int x2, int y2)
{
    XDrawLine(d->display, d->pixmap, d->gc, x1, y1, x2, y2);
    d->lastx = x2, d->lasty = y2;
    d->screen_changed = 1;
} void xlineto(xdisplay * d, int x, int y)
{

    XDrawLine(d->display, d->pixmap, d->gc, d->lastx, d->lasty, x, y);
    d->lastx = x, d->lasty = y;
    d->screen_changed = 1;
} void xrect(xdisplay * d, int x1, int y1, int x2, int y2)
{

    XDrawRectangle(d->display, d->pixmap, d->gc, x1, y1,
		   (x2 - x1), (y2 - y1));
    d->lastx = x2, d->lasty = y2;
    d->screen_changed = 1;
} void xfillrect(xdisplay * d, int x1, int y1, int x2, int y2)
{

    XFillRectangle(d->display, d->pixmap, d->gc, x1, y1,
		   (x2 - x1), (y2 - y1));
    d->lastx = x2, d->lasty = y2;
    d->screen_changed = 1;
} void xpoint(xdisplay * d, int x, int y)
{

    XDrawPoint(d->display, d->pixmap, d->gc, x, y);
    d->lastx = x, d->lasty = y;
    d->screen_changed = 1;
} void xflush(xdisplay * d)
{

    draw_screen(d);
    XFlush(d->display);
}

void xclear_screen(xdisplay * d)
{
    xfillrect(d, 0, 0, d->width, d->height);
    d->screen_changed = 1;
}

#endif
void xmoveto(xdisplay * d, int x, int y)
{
    d->lastx = x, d->lasty = y;
} int xsetfont(xdisplay * d, char *font_name)
{

    if (d->font_struct != (XFontStruct *) NULL) {
	XFreeFont(d->display, d->font_struct);
    }
    d->font_struct = XLoadQueryFont(d->display, font_name);
    if (!d->font_struct) {
	fprintf(stderr, "could not load font: %s\n", font_name);
	exit(3);
    }
    return (d->font_struct->max_bounds.ascent + d->font_struct->max_bounds.descent);
}

void xouttext(xdisplay * d, char *string)
{
    int sz;

    sz = strlen(string);
    XDrawImageString(d->display, d->window, d->gc, d->lastx, d->lasty,
		     string, sz);
#if 0
    d->lastx += XTextWidth(d->font_struct, string, sz);
    d->screen_changed = 1;
#endif
} void xresize(xdisplay * d, XEvent * ev)
{

#ifdef PIXMAP
    XFreePixmap(d->display, d->pixmap);
#endif
    d->width = ev->xconfigure.width;
    d->height = ev->xconfigure.height;
#ifdef PIXMAP
    d->pixmap = XCreatePixmap(d->display, d->window, d->width,
			      d->height, d->depth);
#endif
}

#ifdef PIXMAP
void xarc(xdisplay * d, int x, int y, unsigned int w,
	  unsigned int h, int a1, int a2)
{
    XDrawArc(d->display, d->pixmap, d->gc, x, y, w, h, a1, a2);
} void xfillarc(xdisplay * d, int x, int y, unsigned int w,
		unsigned int h, int a1, int a2)
{
    XFillArc(d->display, d->pixmap, d->gc, x, y, w, h, a1, a2);
} 
#endif

void xsize_set(xdisplay *d, int width, int height)
{
  XResizeWindow(d->display, d->window, width, height);
}

int xmouse_x(xdisplay * d)
{

    return d->mouse_x;
}

int xmouse_y(xdisplay * d)
{
    return d->mouse_y;
}

void xmouse_update(xdisplay * d)
{
    Window rootreturn, childreturn;
    int rootx = 0, rooty = 0, buttons = 0;

    XEvent event;
  
    if (XCheckMaskEvent(d->display,ButtonPressMask | ButtonReleaseMask, &event)) {
      if (event.type == ButtonPress)
        d->mouse_buttons |= 1 << ((XButtonEvent*)(&event))->button;
      else
        d->mouse_buttons &= ~( 1 << ((XButtonEvent*)(&event))->button );
    }

    XQueryPointer(d->display, d->window, &rootreturn, &childreturn,
		  &rootx, &rooty, &(d->mouse_x), &(d->mouse_y),
		  &buttons); 
}

int xsize_update(xdisplay *d,int *width,int *height) {
    XEvent event;
  
    if (XCheckMaskEvent(d->display,StructureNotifyMask, &event)) {
      if (event.type == ConfigureNotify) {
        xupdate_size(d);
        alloc_image(d);
        *width = d->linewidth;
        *height = d->height;
        return 1;
      }
    }
    return 0;
}

unsigned int xmouse_buttons(xdisplay * d)
{
    return d->mouse_buttons;
}
#endif


#endif /* linux*/
