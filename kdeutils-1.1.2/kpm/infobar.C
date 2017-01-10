// infobar.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#include <stdio.h>
#include <qframe.h>
#include <qbrush.h>
#include <qdrawutl.h>
#include "infobar.h"
#include "proc.h"
#include "qps.h"

const int BarGraph::label_width = 50;
const int BarGraph::title_width = 28;
const int LoadGraph::history_scale = 100;

Infobar::Infobar(QWidget *parent, const char *name)
       : QWidget(parent, name)
{
    QFont fnt = font();
    fnt.setBold(FALSE);

    load_graph = new LoadGraph(this, 70, 50);
    load_graph->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    load_graph->move(2, 2);

    load_lbl = new ClickLabel(this);
    load_lbl->setFrameStyle(QFrame::Box | QFrame::Sunken);
    load_lbl->setGeometry(load_graph->geometry());
    load_lbl->setAlignment(AlignRight | AlignVCenter);
    load_lbl->setFont(fnt);

    connect(load_graph, SIGNAL(clicked()), SLOT(toggle_load()));
    connect(load_lbl, SIGNAL(clicked()), SLOT(toggle_load()));

    cpu_bar = new BarGraph(this, "cpu", 4);
    cpu_bar->setColor(0, black);
    cpu_bar->setName(0, "user");
    cpu_bar->setColor(1, darkGray);
    cpu_bar->setName(1, "nice");
    cpu_bar->setColor(2, lightGray);
    cpu_bar->setName(2, "sys");
    cpu_bar->setColor(3, white);
    cpu_bar->setName(3, "idle");
    cpu_bar->setGeometry(load_lbl->x() + load_lbl->width() + 2, load_lbl->y(),
			 200, load_lbl->height());
    cpu_bar->setFrameStyle(QFrame::Box | QFrame::Sunken);
    cpu_bar->setFont(fnt);

    cpu_lbl = new ClickLabel(this);
    cpu_lbl->setFrameStyle(QFrame::Box | QFrame::Sunken);
    cpu_lbl->setGeometry(cpu_bar->geometry());
    cpu_lbl->setAlignment(AlignLeft | AlignVCenter);
    cpu_lbl->setFont(fnt);

    connect(cpu_bar, SIGNAL(clicked()), SLOT(toggle_cpu()));
    connect(cpu_lbl, SIGNAL(clicked()), SLOT(toggle_cpu()));

    mem_bar = new BarGraph(this, "mem", 4);
    mem_bar->setColor(0, black);
    mem_bar->setName(0, "used");
    mem_bar->setColor(1, darkGray);
    mem_bar->setName(1, "buff");
    mem_bar->setColor(2, lightGray);
    mem_bar->setName(2, "cach");
    mem_bar->setColor(3, white);
    mem_bar->setName(3, "free");
    mem_bar->setGeometry(cpu_bar->x() + cpu_bar->width() + 2, cpu_bar->y(),
			 200, cpu_bar->height());
    mem_bar->setFrameStyle(QFrame::Box | QFrame::Sunken);
    mem_bar->setFont(fnt);

    mem_lbl = new ClickLabel(this);
    mem_lbl->setFrameStyle(QFrame::Box | QFrame::Sunken);
    mem_lbl->setGeometry(mem_bar->geometry());
    mem_lbl->setAlignment(AlignLeft | AlignVCenter);
    mem_lbl->setFont(fnt);

    connect(mem_bar, SIGNAL(clicked()), SLOT(toggle_mem()));
    connect(mem_lbl, SIGNAL(clicked()), SLOT(toggle_mem()));

    swap_bar = new BarGraph(this, "swap", 2, TRUE);
    swap_bar->setColor(0, black);
    swap_bar->setColor(1, white);
    swap_bar->setGeometry(mem_bar->x() + mem_bar->width() + 2, mem_bar->y(),
			  150, 24);
    swap_bar->setFrameStyle(QFrame::Box | QFrame::Sunken);
    swap_bar->setFont(fnt);

    swap_lbl = new ClickLabel(this);
    swap_lbl->setFrameStyle(QFrame::Box | QFrame::Sunken);
    swap_lbl->setGeometry(swap_bar->geometry());
    swap_lbl->setAlignment(AlignLeft | AlignVCenter);
    swap_lbl->setFont(fnt);

    connect(swap_bar, SIGNAL(clicked()), SLOT(toggle_swap()));
    connect(swap_lbl, SIGNAL(clicked()), SLOT(toggle_swap()));

    up_lbl = new QLabel(this);
    up_lbl->setFrameStyle(QFrame::Box | QFrame::Sunken);
    up_lbl->setGeometry(swap_lbl->x(), swap_lbl->y() + swap_lbl->height() + 2,
			swap_lbl->width(), 24);
    up_lbl->setAlignment(AlignLeft | AlignVCenter);
    up_lbl->setFont(fnt);
}
 
void Infobar::mem_string(int kbytes, char *buf)
{
    if(kbytes >= 10000)
	sprintf(buf, "%uM", kbytes / 1024);
    else
	sprintf(buf, "%uK", kbytes);
}

// refresh status bar; if step_load is true, add a point to the load graph
void Infobar::refresh(bool step_load)
{
    QString s(64);

    if(step_load)
	load_graph->add_point();
    if(Qps::show_load_graph) {
	load_graph->repaint();
    } else {
	s.sprintf("%1.02f\nload  %1.02f\n%1.02f", Procinfo::loadavg[0],
		  Procinfo::loadavg[1], Procinfo::loadavg[2]);
	load_lbl->setText(s);
    }

    int usedram = Procinfo::mem_total - Procinfo::mem_free 
                  - Procinfo::mem_buffers - Procinfo::mem_cached;
    if(Qps::show_mem_bar) {
	mem_bar->setValue(0, usedram);
	mem_bar->setValue(1, Procinfo::mem_buffers);
	mem_bar->setValue(2, Procinfo::mem_cached);
	mem_bar->setValue(3, Procinfo::mem_free);
	mem_bar->refresh();

    } else {
	char used[10], tot[10], buff[10], cache[10], free[10];
	mem_string(usedram, used);
	mem_string(Procinfo::mem_total, tot);
	mem_string(Procinfo::mem_buffers, buff);
	mem_string(Procinfo::mem_cached, cache);
	mem_string(Procinfo::mem_free, free);
	s.sprintf("mem %s avail, %s used\n%s buff, %s cache\n%s free",
		  tot, used, buff, cache, free);
	mem_lbl->setText(s);
    }

    if(Qps::show_swap_bar) {
	swap_bar->setValue(0, Procinfo::swap_total - Procinfo::swap_free);
	swap_bar->setValue(1, Procinfo::swap_free);
	swap_bar->refresh();

    } else {
	char used[10], tot[10];
	mem_string(Procinfo::swap_total - Procinfo::swap_free, used);
	mem_string(Procinfo::swap_total, tot);
	s.sprintf("swap used %s/%s", used, tot);
	swap_lbl->setText(s);
    }

    int u = Procinfo::uptime;
    int up_days = u / (3600 * 24);
    u %= (3600 * 24);
    int up_hrs = u / 3600;
    u %= 3600;
    int up_mins = u / 60;
    if(up_days == 0) {
	if(up_hrs == 0)
	    s.sprintf("up %d min", up_mins);
	else
	    s.sprintf("up %d:%02d", up_hrs, up_mins);
    } else
	s.sprintf("up %d day%s, %d:%02d",
		  up_days, (up_days == 1) ? "" : "s", up_hrs, up_mins);
    up_lbl->setText(s);

    int user = Procinfo::cpu_time[Procinfo::CPU_USER]
	       - Procinfo::old_cpu_time[Procinfo::CPU_USER];
    int nice = Procinfo::cpu_time[Procinfo::CPU_NICE]
	       - Procinfo::old_cpu_time[Procinfo::CPU_NICE];
    int system = Procinfo::cpu_time[Procinfo::CPU_SYSTEM]
	         - Procinfo::old_cpu_time[Procinfo::CPU_SYSTEM];
    int idle = Procinfo::cpu_time[Procinfo::CPU_IDLE]
	       - Procinfo::old_cpu_time[Procinfo::CPU_IDLE];
    if(Qps::show_cpu_bar) {
	cpu_bar->setValue(0, user);
	cpu_bar->setValue(1, nice);
	cpu_bar->setValue(2, system);
	cpu_bar->setValue(3, idle);
	cpu_bar->refresh();
    } else {
	float tot = (float)(user + nice + system + idle) / 100;
	s.sprintf("cpu %.1f%% user, %.1f%% nice\n%.1f%% system, %.1f%% idle",
		  user / tot, nice / tot, system / tot, idle / tot);
	cpu_lbl->setText(s);
    }
}

void Infobar::update_load()
{
    load_graph->add_point();
}

// if p, then show a and hide b; otherwise do the opposite
void Infobar::show_and_hide(bool p, QWidget *a, QWidget *b)
{
    if(!p) {
	QWidget *c = a;
	a = b;
	b = c;
    }
    if(!a->isVisible()) {
	b->hide();
	a->show();
    }
}

// show/hide child widgets according to settings
void Infobar::configure()
{
    show_and_hide(Qps::show_mem_bar, mem_bar, mem_lbl);
    show_and_hide(Qps::show_swap_bar, swap_bar, swap_lbl);
    show_and_hide(Qps::show_cpu_bar, cpu_bar, cpu_lbl);
    show_and_hide(Qps::show_load_graph, load_graph, load_lbl);
    load_graph->configure();
    refresh(FALSE);
}

void Infobar::toggle_load()
{
    Qps::show_load_graph = !Qps::show_load_graph;
    emit config_change();
    configure();
}

void Infobar::toggle_cpu()
{
    Qps::show_cpu_bar = !Qps::show_cpu_bar;
    emit config_change();
    configure();
}

void Infobar::toggle_mem()
{
    Qps::show_mem_bar = !Qps::show_mem_bar;
    emit config_change();
    configure();
}

void Infobar::toggle_swap()
{
    Qps::show_swap_bar = !Qps::show_swap_bar;
    emit config_change();
    configure();
}

BarGraph::BarGraph(QWidget *parent, const char *title, int n, bool nolegend)
         : QFrame(parent), nbars(n), caption(title), no_legend(nolegend)
{
    values = new int[n];
    for(int i = 0; i < n; i++)
	values[i] = 0;
    colors = new QColor[n];
    names = new QString[n];
}

BarGraph::~BarGraph()
{
    delete[] values;
    delete[] colors;
    delete[] names;
}

void BarGraph::resizeEvent(QResizeEvent *)
{
    bar_width = width() - 12 - title_width;
    repaint();
}

void BarGraph::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}

void BarGraph::refresh()
{
    QPainter p;
    p.begin(this);
    refreshBar(&p);
    p.end();
}

void BarGraph::refreshBar(QPainter *p)
{
    drawBar(p, 4 + title_width + 4, 4, bar_width, 16);
    
}

void BarGraph::drawContents(QPainter *p)
{
    p->drawText(4, 4, title_width, 16, AlignLeft | AlignVCenter, caption);
    refreshBar(p);
    if(!no_legend)
	for(int i = 0; i < nbars; i++)
	    drawLegend(p, 4 + label_width * i, 28,
		       label_width, 16, i);
}

void BarGraph::drawBar(QPainter *p, int x, int y, int w, int h)
{
    int sum = 0;
    for(int i = 0; i < nbars; i++)
	sum += values[i];
    if(sum > 1000000) {
	// possible overflow problem, downscale values
	sum = 0;
	for(int i = 0; i < nbars; i++) {
	    values[i] >>= 10;
	    sum += values[i];
	}
    }
    int dx = 0;

    QColorGroup g = colorGroup();
    QBrush fill(colors[nbars - 1]);
    qDrawShadePanel(p, x, y, w, h, g, TRUE, 1, &fill);
    w -= 2;
    h -= 2;
    x++;
    y++;

    if(sum > 0) {
	for(int i = 0; i < nbars - 1; i++) {
	    int iw = w * values[i] / sum;
	    p->fillRect(x + dx, y, iw, h, colors[i]);
	    dx += iw;
	}
    } else {
	p->drawText(x, y, w, h, AlignCenter, "(none)");
    }
}


void BarGraph::drawLegend(QPainter *p, int x, int y, int w, int h, int index)
{
    QColorGroup g = colorGroup();
    QBrush fill(colors[index]);
    qDrawShadePanel(p, x, y + 2, h - 4, h - 4, g, TRUE, 1, &fill);
    p->drawText(x + h, y, w - h, h, AlignLeft | AlignVCenter, names[index]);
}

LoadGraph::LoadGraph(QWidget *parent, int w, int h)
         : QFrame(parent), pts(w)
{
    max_points = w - 2;
    npoints = 0;
    history = new short[max_points];
    h_index = 0;

    // set x values
    points = new QCOORD[max_points * 2];
    for(int i = 0; i < max_points; i++)
	points[i * 2] = i + 1;
    current_scale = history_scale;
    dirty = TRUE;

    icon = 0;			// will be created as needed

    setBackgroundColor(Qps::phosphor_graph ? black
		                           : colorGroup().background());
    resize(w, h);
}

LoadGraph::~LoadGraph()
{
    delete[] history;
    delete[] points;
    if(icon)
	delete icon;
}

// add a point to the load history
void LoadGraph::add_point()
{
    // add new data point to history
    int scaled_data = (short)(Procinfo::loadavg[0] * history_scale);
    history[h_index++] = scaled_data;
    if(h_index >= max_points)
	h_index = 0;

    int max_y = height() - 2;
    
    // find peak
    int peak = history_scale;	// max Y is at least 1.00
    for(int i = 0; i < npoints; i++)
	if(history[i] > peak)
	    peak = history[i];

    npoints++;
    if(peak != current_scale) {
	// rescale point array
	if(npoints > max_points)
	    npoints = max_points;
	int j = h_index - npoints;
	if(j < 0) j += max_points;
	for(int i = 1; i < npoints * 2; i += 2) {
	    points[i] = max_y - history[j] * (max_y - 1) / peak;
	    j++;
	    if(j == max_points)
		j = 0;
	}
	current_scale = peak;
    } else {
	// same scale, just move existing points
	if(npoints > max_points) {
	    npoints = max_points;
	    for(int i = 1; i < npoints * 2 - 1; i += 2)
		points[i] = points[i + 2];
	}
	points[npoints * 2 - 1] = max_y - scaled_data * (max_y - 1)
	                                              / current_scale;
    }

    dirty = TRUE;
}

void LoadGraph::configure()
{
    setBackgroundColor(Qps::phosphor_graph ? black
		                           : colorGroup().background());
}

// return updated pixmap for use as an icon
QPixmap *LoadGraph::make_icon(int w, int h)
{
    if(!icon)
	icon = new QPixmap(w, h);
    // build point array
    int np = (npoints > w) ? w : npoints;
    pts.setPoints(np, points + (npoints - np) * 2);
    pts.translate(-(npoints - np + 1), -1);	// icon has no frame
    dirty = TRUE;
    
    QPainter p;
    icon->fill(backgroundColor());
    p.begin(icon);
    draw_graph(&p, 0, 0, w, h, Qps::solid_graph);
    p.end();
    return icon;
}

// return updated pixmap for use as an icon
void LoadGraph::draw_icon(QPixmap *pm)
{
  int h = pm->width();
  int w = pm->height();

    // build point array
    int np = (npoints > w) ? w : npoints;
    pts.setPoints(np, points + (npoints - np) * 2);
    pts.translate(-(npoints - np + 1), -1);	// icon has no frame
    dirty = TRUE;
    
    QPainter p;
    p.begin(pm);
    draw_graph(&p, 0, 0, w, h, Qps::solid_graph);
    p.end();
}

void LoadGraph::drawContents(QPainter *p)
{
    if(dirty) {
	pts.setPoints(npoints, points);
	dirty = FALSE;
    }

    draw_graph(p, 1, 1, max_points, height() - 2, Qps::solid_graph);
}

void LoadGraph::draw_graph(QPainter *p, int x, int y, int w, int h, bool fill)
{
    if(current_scale > history_scale) {
	// draw scale lines
	p->setPen(Qps::phosphor_graph ? darkGreen : colorGroup().mid());
	int scale_max = current_scale - (current_scale % history_scale);
	for(int z = scale_max; z > 0; z -= history_scale) {
	    int yy = h + y - 1 - z * h / current_scale;
	    p->drawLine(x, yy, x + w - 1, yy);
	}
    }
    p->setPen(Qps::phosphor_graph ? green : colorGroup().text());
    int np = (npoints > w) ? w : npoints;

    if(fill) {
      for(int i=0; i < np; i++)
	p->drawLine(i, h, i, pts[i].y());
    } else
      p->drawPolyline(pts, 0, np);
}

void LoadGraph::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}

void ClickLabel::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}
