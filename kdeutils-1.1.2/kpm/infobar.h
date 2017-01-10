// infobar.h				emacs, this is written in -*-c++-*-
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef INFOBAR_H
#define INFOBAR_H

#include <qpainter.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qframe.h>
#include <qstring.h>
#include <qpixmap.h>

// the ClickLabel is just a QLabel that can be clicked (emits a signal)

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    ClickLabel(QWidget *parent) : QLabel(parent) {};

signals:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent *);
};

class BarGraph : public QFrame
{
    Q_OBJECT
public:
    BarGraph(QWidget *parent, const char *title, int n, bool nolegend = FALSE);

    void setColor(int index, QColor color) { colors[index] = color; };
    void setValue(int index, int value) { values[index] = value; };
    void setName(int index, const char *name) { names[index] = name; };
    void refresh();

    ~BarGraph();

signals:
    void clicked();

protected:
    virtual void drawContents(QPainter *p);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *);

private:
    void drawBar(QPainter *p, int x, int y, int w, int h);
    void drawLegend(QPainter *p, int x, int y, int w, int h, int index);
    void refreshBar(QPainter *p);

    int nbars;
    int *values;
    QString *names;
    QColor *colors;
    int bar_width;
    const char *caption;
    bool no_legend;

    static const int label_width;
    static const int title_width;
};

class LoadGraph : public QFrame
{
    Q_OBJECT
public:
    LoadGraph(QWidget *parent, int w, int h);
    ~LoadGraph();

    void add_point();		// adds a data point
    void configure();		// respond to configuration change

    QPixmap *make_icon(int w, int h);
    void draw_icon(QPixmap *);

signals:
    void clicked();

protected:
    void drawContents(QPainter *p);
    void mousePressEvent(QMouseEvent *);
    void draw_graph(QPainter *, int, int, int, int, bool = FALSE);

private:
    int max_points;		// history buffer size
    int npoints;		// nr of history points currently remembered
    static const int history_scale ;
    short *history;		// (circular) history buffer
    int h_index;		// next history index to use

    int current_scale;		// history_scale * load maximally shown
    QCOORD *points;
    QPointArray pts;		// cached for redrawing
    bool dirty;			// true if pts need recalculation
    QPixmap *icon;
};

class Infobar : public QWidget
{
    Q_OBJECT
public:
    Infobar(QWidget *parent = 0, const char *name = 0);

    void refresh(bool step_load);
    void update_load();
    QPixmap *load_icon(int w, int h) { return load_graph->make_icon(w, h); };
    void draw_icon(QPixmap *pm) {
      load_graph->draw_icon(pm);
    }
    void configure();

public slots:
    void toggle_load();
    void toggle_cpu();
    void toggle_mem();
    void toggle_swap();

signals:
    void config_change();

private:
    ClickLabel *load_lbl;
    LoadGraph *load_graph;
    BarGraph *mem_bar;
    ClickLabel *mem_lbl;
    QLabel *up_lbl;
    BarGraph *cpu_bar;
    ClickLabel *cpu_lbl;
    BarGraph *swap_bar;
    ClickLabel *swap_lbl;

    void mem_string(int kbytes, char *buf);
    void show_and_hide(bool p, QWidget *a, QWidget *b);
};

#endif	// INFOBAR_H
