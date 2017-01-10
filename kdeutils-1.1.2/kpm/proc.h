// proc.h				emacs, this is written in -*-c++-*-
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef PROC_H
#define PROC_H

#include <unistd.h>
#include <sys/time.h>
#include <qstring.h>
#include <qwindefs.h>
#include <qintdict.h>
#include "svec.h"

enum fields {
    F_PID, F_PPID, F_PGID, F_SID, F_TTY, F_TPGID,
    F_USER,
    F_PRI, F_NICE,
    F_PLCY, F_RPRI,
    F_MAJFLT, F_MINFLT, F_TRS, F_DRS, F_SIZE, F_SWAP, F_RSS, F_SHARE, F_DT,
    F_STAT,
    F_FLAGS,
    F_WCHAN,
    F_UID,
    F_WCPU, F_CPU, F_MEM,
    F_START, F_TIME,
    F_COMM, F_CMDLINE,
    F_END = -1 };

class Details;

class Sockinfo
{
public:
    enum proto_t { TCP, UDP };
    proto_t proto;
    unsigned char st;
    unsigned char tr;
    unsigned local_addr;
    unsigned rem_addr;
    unsigned short local_port;
    unsigned short rem_port;
    unsigned tx_queue;
    unsigned rx_queue;
    unsigned tm_when;
    unsigned rexmits;
    int uid;
    int timeout;
    int inode;
};

class Procinfo
{
public:
    Procinfo(int pid);
    ~Procinfo();

    int readproc(int pid);
    static void read_common();
    static void read_loadavg();
    static int read_file(char *name, char *buf, int max);
    bool read_socket_inodes();
    static bool read_socket_list(Sockinfo::proto_t proto, char *pseudofile);
    static void read_sockets();
    static void invalidate_sockets();

    // accessors for lazy evaluation
    int get_policy();
    int get_rtprio();

    int pid;

    // from /proc/XX/status
    int uid, euid, suid, fsuid;
    int gid, egid, sgid, fsgid;

    // from /proc/XX/cmdline
    QString cmdline;

    // from /proc/XX/stat
    QString comm;
    char state;
    int ppid;
    int pgrp;
    int session;
    int tty;
    int tpgid;
    unsigned flags;
    unsigned minflt;
    unsigned cminflt;
    unsigned majflt;
    unsigned cmajflt;
    int utime;
    int stime;
    int cutime;
    int cstime;
    int priority;
    int nice;
    unsigned timeout;
    unsigned itrealvalue;
    int starttime;
    unsigned vsize;
    unsigned rss;
    unsigned rlim;
    unsigned startcode;
    unsigned endcode;
    unsigned startstack;
    unsigned kstkesp;
    unsigned kstkeip;
    int signal;
    int blocked;
    int sigignore;
    int sigcatch;
    unsigned wchan;

    // from /proc/XX/statm
    unsigned size;	// total pages of memory
    unsigned resident;	// number of resident set (non-swapped) pages
    unsigned share;	// number of shared memory pages (mmaped)
    unsigned trs;      	// text resident set size
    unsigned lrs;	// shared-lib resident set size
    unsigned drs;	// data resident set size
    unsigned dt;	// dirty pages

    // from gettimeofday(2)
    struct timeval tv;

    // from sched_getscheduler(2)
    int policy;		// -1 = uninitialized
    // from sched_getparam(2)
    int rtprio;		// 0-99, higher can pre-empt lower (-1 = uninitialized)

    // computed %cpu and %mem since last update
    float wcpu;
    float pcpu;
    float pmem;

    // from /proc/loadavg
    static float loadavg[3];

    // from /proc/meminfo (these are in kB)
    static int mem_total, mem_free, mem_shared, mem_buffers;
    static int mem_cached;
    static int swap_total, swap_free;

    // from /proc/stat
    enum { CPU_USER, CPU_NICE, CPU_SYSTEM, CPU_IDLE };
    static const int CPUTIMES;

  // this little hack prevents a compiler warning under egcs
#define CPUTIMES (CPU_IDLE+1)
    static unsigned cpu_time[CPUTIMES];
    static unsigned old_cpu_time[CPUTIMES];
#undef CPUTIMES
    static unsigned boot_time;

    // from /proc/uptime
    static int uptime;	// seconds of uptime

    // from /proc/net/{tcp,udp}
    static QIntDict<Sockinfo> socks;
    static bool socks_current;	// true if the socks list is current

    // from /proc/XX/fd
    Svec<int> *sock_inodes;	// socket inodes or NULL if not read

    Details *details;		// details window or NULL (backlink)

    bool selected;	// true if selected in current view
    bool viewed;	// true if part of current process view

    static const int MAX_CMD_LEN;
    static int pagesize;
};

class Category
{
public:
    Category(char *heading, char *explain) : name(heading), help(explain) {};

    virtual int alignment() = 0;
    virtual QString string(Procinfo *p) = 0;
    virtual int width() = 0;
    virtual int compare(Procinfo *a, Procinfo *b);
    virtual int gap() { return 0; };

    char *name;
    char *help;
    int index;
};

class Cat_int : public Category
{
public:    
    Cat_int(char *heading, char *explain, int w, int Procinfo::*member);
    virtual int alignment() { return AlignRight; };
    virtual QString string(Procinfo *p);
    virtual int width() { return field_width; };
    virtual int compare(Procinfo *a, Procinfo *b);

protected:
    int Procinfo::*int_member;
    int field_width;
};

class Cat_uint : public Category
{
public:
    Cat_uint(char *heading, char *explain, int w, unsigned Procinfo::*member);
    virtual int alignment() { return AlignRight; };
    virtual QString string(Procinfo *p);
    virtual int width() { return field_width; };
    virtual int compare(Procinfo *a, Procinfo *b);

protected:
    unsigned Procinfo::*uint_member;
    int field_width;
};

class Cat_hex : public Cat_uint
{
public:
    Cat_hex(char *heading, char *explain, int w, unsigned Procinfo::*member);
    virtual QString string(Procinfo *p);
};

class Cat_swap : public Category
{
public:
    Cat_swap(char *heading, char *explain);
    virtual int alignment() { return AlignRight; };
    virtual QString string(Procinfo *p);
    virtual int width() { return 56; };
    virtual int compare(Procinfo *a, Procinfo *b);
};

class Cat_string : public Category
{
public:
    Cat_string(char *heading, char *explain, QString Procinfo::*member = 0);
    virtual int alignment() { return AlignLeft; };
    virtual QString string(Procinfo *p);
    virtual int width() { return -1; };
    virtual int gap() { return 8; };

protected:
    QString Procinfo::*str_member;
};

class Cat_user : public Cat_string
{
public:
    Cat_user(char *heading, char *explain);
    virtual QString string(Procinfo *p);
};

class Cat_wchan : public Cat_string
{
public:
    Cat_wchan(char *heading, char *explain);
    virtual QString string(Procinfo *p);
};

class Cat_cmdline : public Cat_string
{
public:
    Cat_cmdline(char *heading, char *explain);
    virtual QString string(Procinfo *p);
};

class Cat_state : public Category
{
public:
    Cat_state(char *heading, char *explain);
    virtual int alignment() { return AlignLeft; };
    virtual QString string(Procinfo *p);
    virtual int width() { return 44; };
    virtual int gap() { return 8; };
};

class Cat_policy : public Category
{
public:
    Cat_policy(char *heading, char *explain);
    virtual int alignment() { return AlignLeft; };
    virtual QString string(Procinfo *p);
    virtual int width() { return 48; };
    virtual int gap() { return 8; };
    virtual int compare(Procinfo *a, Procinfo *b);
};

class Cat_rtprio : public Category
{
public:
    Cat_rtprio(char *heading, char *explain);
    virtual int alignment() { return AlignRight; };
    virtual QString string(Procinfo *p);
    virtual int width() { return 32; };
    virtual int compare(Procinfo *a, Procinfo *b);
};

class Cat_time : public Category
{
public:
    Cat_time(char *heading, char *explain);
    virtual int alignment() { return AlignRight; };
    virtual QString string(Procinfo *p);
    virtual int width() { return 48; };
    virtual int compare(Procinfo *a, Procinfo *b);
};

class Cat_start : public Category
{
public:
    Cat_start(char *heading, char *explain);
    virtual int alignment() { return AlignRight; };
    virtual QString string(Procinfo *p);
    virtual int width() { return 56; };
    virtual int compare(Procinfo *a, Procinfo *b);
};

class Cat_percent : public Category
{
public:
    Cat_percent(char *heading, char *explain, int w, float Procinfo::*member);
    virtual int alignment() { return AlignRight; };
    virtual QString string(Procinfo *p);
    virtual int width() { return field_width; };
    virtual int compare(Procinfo *a, Procinfo *b);

protected:
    float Procinfo::*float_member;
    int field_width;
};

class Cat_tty : public Category
{
public:
    Cat_tty(char *heading, char *explain);
    virtual int alignment() { return AlignLeft; };
    virtual QString string(Procinfo *p);
    virtual int width() { return 36; };
    virtual int gap() { return 8; };
};

class Proc
{
public:
    Proc();
    void refresh();

    Svec<Procinfo *> procs;
    Svec<Category *> cats;
};

class Procview
{
public:
    Procview(Proc *p);
    static int compare(Procinfo **a, Procinfo **b);
    static int compare_backwards(Procinfo **a, Procinfo **b);
    void refresh();
    void rebuild();
    void set_fields();
    void set_fields_list(int fields[]);
    void add_cat(Category *c);
    void remove_cat(int index);
    int findCol(int field);
    
    Svec<Procinfo *> procs;
    Svec<Procinfo *> old_procs;
    Svec<Category *> cats;
    Category *sortcat;
    bool reversed;		// true if sorted backwards

    enum procstates {ALL, OWNED, NROOT, RUNNING} viewproc;
    enum fieldstates {USER = RUNNING + 1, JOBS, MEM, CUSTOM} viewfields;

    // lists of fields to be used for different views, terminated by -1:
    static int user_fields[];
    static int jobs_fields[];
    static int mem_fields[];

    static float avg_factor;		// exponential factor for averaging

    static const int cpu_avg_time;	// averaging time for WCPU (ms)

private:
    static Category *static_sortcat;	// kludge: to be used by compare

    Proc *proc;
};

#endif	// PROC_H
