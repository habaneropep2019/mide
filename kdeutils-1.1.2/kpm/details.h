// details.h				emacs, this is written in -*-c++-*-
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef DETAILS_H
#define DETAILS_H

#include <qdialog.h>
#include <qintdict.h>

#include "htable.h"
#include "proc.h"

// list node for LRU cache of hostnames
class Hostnode {
public:
    Hostnode();
    Hostnode(unsigned addr);

    // these must be called on the head of the list
    void moveToFront(Hostnode *node);
    void deleteLast();
    void insertFirst(Hostnode *node);
    Hostnode *last() { return prev; };

    QString name;
    unsigned ipaddr;
    Hostnode *next, *prev;
};

class Qps;

class Details : public HeadedTable
{
    Q_OBJECT
public:
    Details(QWidget *parent, Procinfo *pi, Qps *qps);
    ~Details();

    void refresh();
    void process_gone();
    void refresh_window();
    bool refresh_sockets();
    const char *servname(unsigned port);
    void set_procinfo(Procinfo *pi);
    QString ipAddr(unsigned addr);
    QString hostname(unsigned addr);
    void config_change();

signals:
    void closed(Details *);

public slots:
    void dismiss();

protected:
    virtual QString title(int col);
    virtual QString text(int row, int col);
    virtual int colWidth(int col);
    virtual int alignment(int col);
    virtual int leftGap(int col);
    virtual QString tipText(int col);

    virtual void closeEvent(QCloseEvent *);

private:
    Procinfo *procinfo;

    bool doing_lookup;		// if table painted with host lookup

    static bool have_services;	// true if we have tried reading services
    static QIntDict<char> servdict;
    static QIntDict<Hostnode> hostdict;
    static Hostnode hostlru;

    static const int hostname_cache_size; // don't cache more than this
};

#endif	// DETAILS_H
