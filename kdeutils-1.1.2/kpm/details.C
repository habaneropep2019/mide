// details.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#include <features.h>
#if (defined(__GLIBC__) && (__GLIBC__ > 1))
// glibc 2 (aka linux libc 6) doesn't use linux kernel headers
#define _GNU_SOURCE
#include <netinet/tcp.h>
#else
extern "C" {
  #include <linux/tcp.h>
}
#endif

#include <netinet/in.h>
#include <netdb.h>

#include <qlabel.h>
#include <qpainter.h>
#include <qaccel.h>

#include <stdio.h>

#include "details.h"
#include "qps.h"
#include "svec.h"

// declaration of static members
bool Details::have_services = FALSE;
const int Details::hostname_cache_size = 400; // don't cache more than this
QIntDict<char> Details::servdict(101);
QIntDict<Hostnode> Details::hostdict(hostname_cache_size + 1);
Hostnode Details::hostlru;

struct SockField
{
    char *name;
    int width;
    int gap;
    int align;
    char *tooltip;
};

enum sockf_t {
    PROTO, RECVQ, SENDQ, LOCALADDR, LOCALPORT, REMOTEADDR, REMOTEPORT, STATE,
    SOCKFIELDS
};

static SockField fields[SOCKFIELDS] = {
    {"Proto", 44, 8, AlignLeft, "Protocol (TCP or UDP)"},
    {"Recv-Q", 60, 8, AlignRight, "Bytes in receive queue"},
    {"Send-Q", 60, 8, AlignRight, "Bytes in send queue"},
    {"Local Addr", -1, 8, AlignLeft, "Local IP address"},
    {"Port", 48, 8, AlignLeft, "Local port"},
    {"Remote Addr", -1, 8, AlignLeft, "Remote IP address"},
    {"Port", 48, 8, AlignLeft, "Remote port"},
    {"State", 128, 8, AlignLeft, "Connection state"}
};

Details::Details(QWidget *parent, Procinfo *pi, Qps *qps)
         : HeadedTable(parent, HTBL_HEADING_TOOLTIPS),
	   procinfo(pi)
{
    setAppearance(Qps::old_style_tables ? macOS7 : macOS8);
    setNumCols(SOCKFIELDS);
    QString cap(80);
    cap.sprintf("Sockets used by process %d (%s)",
		procinfo->pid, (const char*)procinfo->comm);
    setCaption(cap);
    procinfo->details = this;

    // The usual accelerators should work here as well
    QAccel *acc = new QAccel(this);
    acc->connectItem(acc->insertItem(ALT + Key_W),
		     this, SLOT(dismiss()));
    acc->connectItem(acc->insertItem(ALT + Key_Q),
		     qps, SLOT(save_quit()));
    acc->connectItem(acc->insertItem(Key_Space),
		     qps, SLOT(forced_update()));
    acc->connectItem(acc->insertItem(Key_Return),
		     qps, SLOT(forced_update()));

    doing_lookup = Qps::hostname_lookup;
    refresh();
    // compute total width = window width
    int totw = 0;
    for(int i = 0; i < SOCKFIELDS; i++)
	totw += actualColWidth(i);
    setGeometry(0, 23, totw, 200);
}

Details::~Details()
{
    if(procinfo)
	procinfo->details = 0;
}

QString Details::title(int col)
{
    return fields[col].name;
}

QString Details::text(int row, int col)
{
    if(!procinfo->sock_inodes)
	refresh_sockets();
    int inode = (*procinfo->sock_inodes)[row];
    Sockinfo *si = Procinfo::socks[inode];
    if(!si) fatal("sockinfo = 0!\n");

    QString s(20);
    switch(col) {
    case PROTO:
        s = (si->proto == Sockinfo::TCP) ? "tcp" : "udp";
	break;

    case RECVQ:
	s.setNum(si->rx_queue);
	break;

    case SENDQ:
	s.setNum(si->tx_queue);
	break;

    case LOCALADDR:
	s = ipAddr(si->local_addr);
	break;

    case LOCALPORT:
	{
	    const char *serv = servname(si->local_port);
	    if(serv)
		s = serv;
	    else
		s.setNum(si->local_port);
	}
	break;

    case REMOTEADDR:
	s = ipAddr(si->rem_addr);
	break;

    case REMOTEPORT:
	{
	    const char *serv = servname(si->rem_port);
	    if(serv)
		s = serv;
	    else
		s.setNum(si->rem_port);
	}
	break;

    case STATE:
	switch(si->st) {
	case TCP_ESTABLISHED:
	    s = "ESTABLISHED"; break;
	case TCP_SYN_SENT:
	    s = "SYN_SENT"; break;
	case TCP_SYN_RECV:
	    s = "SYN_RECV"; break;
	case TCP_FIN_WAIT1:
	    s = "FIN_WAIT1"; break;
	case TCP_FIN_WAIT2:
	    s = "FIN_WAIT2"; break;
	case TCP_TIME_WAIT:
	    s = "TIME_WAIT"; break;
	case TCP_CLOSE:
	    s = "CLOSE"; break;
	case TCP_CLOSE_WAIT:
	    s = "CLOSE_WAIT"; break;
	case TCP_LAST_ACK:
	    s = "LAST_ACK"; break;
	case TCP_LISTEN:
	    s = "LISTEN"; break;
	case TCP_CLOSING:
	    s = "CLOSING"; break;
	default:
	    s = "UNKNOWN"; break;
	}
	break;
    }
    return s;
}

int Details::colWidth(int col)
{
    return fields[col].width;
}

int Details::alignment(int col)
{
    return fields[col].align;
}

int Details::leftGap(int col)
{
    return fields[col].gap;
}

QString Details::tipText(int col)
{
    return fields[col].tooltip;
}

QString Details::ipAddr(unsigned addr)
{
    if(doing_lookup)
	return hostname(addr);
    else {
	QString s;
	unsigned a = htonl(addr);
	if(a == 0)
	    s = "*";
	else
	    s.sprintf("%d.%d.%d.%d",
		      (a >> 24) & 0xff,
		      (a >> 16) & 0xff,
		      (a >> 8) & 0xff,
		      a & 0xff);
	return s;
    }
}

void Details::refresh_window()
{
    if(!procinfo) return;
    int rows = procinfo->sock_inodes->size();
    setNumRows(rows);
    setNumCols(SOCKFIELDS);
    repaintAll();
}

void Details::refresh()
{
    if(refresh_sockets())
	refresh_window();
}

// return true if sockets could be read successfully, false otherwise
bool Details::refresh_sockets()
{
    Procinfo::read_sockets();
    return procinfo->read_socket_inodes();
}

void Details::process_gone()
{
    procinfo = 0;
    // for now, we just close the window. Another possibility would be
    // to leave it with a "process terminated" note.
    dismiss();
}

void Details::set_procinfo(Procinfo *p)
{
    procinfo = p;
}

void Details::dismiss()
{
    emit closed(this);
}

void Details::closeEvent(QCloseEvent *)
{
    dismiss();
}

// react to changes in preferences
void Details::config_change()
{
    setAppearance(Qps::old_style_tables ?
		  HeadedTable::macOS7
		  : HeadedTable::macOS8);
    if(doing_lookup != Qps::hostname_lookup) {
	doing_lookup = Qps::hostname_lookup;
	setNumCols(SOCKFIELDS);
	repaintAll();
    }
}

const char *Details::servname(unsigned port)
{
    if(!have_services) {
	have_services = TRUE;
	// fill servdict from /etc/services (just try once)
	setservent(1);
	struct servent *s;
	while((s = getservent()) != 0) {
	    unsigned short hport = ntohs((unsigned short)s->s_port);
	    if(!servdict[hport])
		servdict.insert(hport, strdup(s->s_name));
	}
	endservent();
    }
    return servdict[port];
}

QString Details::hostname(unsigned addr)
{
    // first look in our cache
    Hostnode *hn = hostdict[addr];
    if(hn) {
	hostlru.moveToFront(hn);
    } else {
	hn = new Hostnode(addr);
	if(hostdict.count() >= (uint)hostname_cache_size) {
	    // remove least recently used item
	    hostdict.remove(hostlru.last()->ipaddr);
	    hostlru.deleteLast();
	}
	hostlru.insertFirst(hn);
	hostdict.insert(addr, hn);
    }
    return hn->name;
}

Hostnode::Hostnode()
        : next(this), prev(this)
{}

Hostnode::Hostnode(unsigned addr)
        : ipaddr(addr), next(0), prev(0)
{
    QString s;
    unsigned a = htonl(addr);
    if(a == 0)
	s = "*";
    else {
	struct hostent *h = gethostbyaddr((char *)&addr, sizeof(addr),
					  AF_INET);
	if(h) {
	    s = h->h_name;
	} else {
	    // host name resolution failed, use numerical address instead
	    s.sprintf("%d.%d.%d.%d",
		      (a >> 24) & 0xff,
		      (a >> 16) & 0xff,
		      (a >> 8) & 0xff,
		      a & 0xff);
	}
    }
    name = s;
}

void Hostnode::moveToFront(Hostnode *node)
{
    if(next != node) {
	Hostnode *p = node->prev, *n = node->next;
	p->next = n;
	n->prev = p;
	node->next = next;
	node->prev = this;
	next->prev = node;
	next = node;
    }
}

void Hostnode::deleteLast()
{
    Hostnode *nuke = prev;
    prev = nuke->prev;
    prev->next = this;
    delete nuke;
}

void Hostnode::insertFirst(Hostnode *node)
{
    node->prev = this;
    node->next = next;
    next->prev = node;
    next = node;
}

