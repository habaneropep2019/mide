/*
 *  Qt support for MICO
 *  Copyright (C) 1997 Lars Doelle
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Send comments and/or bug reports to:
 *                 mico@informatik.uni-frankfurt.de
 */

#include <CORBA.h>
#include <mico/template_impl.h>
#include "kmicoapp.h"
#include <algorithm>


// KMicoApp /////////////////////////////////////////////////////////////

KMicoApp::KMicoApp (int argc, char *argv[])
    : KApplication (argc,argv), state (Initial)
{
    orb = CORBA::ORB_init (argc, argv, "mico-local-orb");
    boa = orb->BOA_init (argc, argv, "mico-local-boa");
    qtd = new QTDispatcher (this);
    orb->dispatcher (qtd);
}

KMicoApp::~KMicoApp()
{
    // this will delete the QtDispatcher
    orb->dispatcher (0);
    // orb and boa are automatically released by the _var types
}

void KMicoApp::doquit()
{
    switch (state) {
    case ImplReady:
	deactivate_impl (CORBA::ImplementationDef::_nil());
	shutdown (TRUE);
	break;
    case ObjReady:
	deactivate_obj (CORBA::Object::_nil());
	shutdown (TRUE);
	break;
    }
}

void
KMicoApp::impl_is_ready (CORBA::ImplementationDef_ptr impl)
{
    state = ImplReady;
    boa->impl_is_ready (impl);
}

void
KMicoApp::deactivate_impl (CORBA::ImplementationDef_ptr impl)
{
    state = Initial;
    boa->deactivate_impl (impl);
}

void
KMicoApp::obj_is_ready (CORBA::Object_ptr obj, CORBA::ImplementationDef_ptr impl)
{
    state = ObjReady;
    boa->obj_is_ready (obj, impl);
}

void
KMicoApp::deactivate_obj (CORBA::Object_ptr obj)
{
    state = Initial;
    boa->deactivate_obj (obj);
}

void
KMicoApp::run ()
{
    orb->run ();
}

void
KMicoApp::shutdown (CORBA::Boolean wait)
{
    orb->shutdown (wait);
}

void KMicoApp::processNextEvent (bool b)
{
    KApplication::processNextEvent (b);
    // XXX hack
    if (KApplication::quit_now) {
	KApplication::quit_now = false;
	doquit();
    }
}
  
// FileEvent /////////////////////////////////////////////////////////////////

FileEvent::~FileEvent()
{
    delete xid;
}

FileEvent::FileEvent()
{
}

FileEvent::FileEvent (QTDispatcher *_disp,
		      CORBA::DispatcherCallback *_cb, int fd, 
		      QSocketNotifier::Type _evq,
		      CORBA::Dispatcher::Event _evc)
    : disp (_disp), xid (new QSocketNotifier (fd,_evq)), cb (_cb), ev (_evc)
{
    QObject::connect (xid, SIGNAL (activated(int)),
		      this, SLOT (activated(int)));
}

void FileEvent::activated(int /*fd*/) 
{
    cb->callback (disp, ev);
}

// TimerEvent //////////////////////////////////////////////////////////

TimerEvent::~TimerEvent()
{
    delete xid;
}

TimerEvent::TimerEvent()
{
}

TimerEvent::TimerEvent (QTDispatcher* _disp,
			CORBA::DispatcherCallback *_cb,
			long tmout)
    : xid (new QTimer()), cb (_cb), disp (_disp)
{
    QObject::connect (xid, SIGNAL (timeout()), this, SLOT (timeout()));
    xid->start (tmout, TRUE);
}

void TimerEvent::timeout()
{
    disp->remove (this);
    cb->callback (disp, CORBA::Dispatcher::Timer);
    delete this;
}

// create & destroy ////////////////////////////////////////////////////

QTDispatcher::QTDispatcher (KMicoApp* _ctx)
    : ctx (_ctx)
{
}

QTDispatcher::~QTDispatcher ()
{
    list<FileEvent*>::iterator i;
    for (i = fevents.begin(); i != fevents.end(); ++i) {
	(*i)->cb->callback(this, Remove);
	delete *i;
    }

    list<TimerEvent*>::iterator j;
    for (j = tevents.begin(); j != tevents.end(); ++j) {
	(*j)->cb->callback(this, Remove);
	delete *j;
    }
}

// adding events ///////////////////////////////////////////////////////

void QTDispatcher::rd_event (CORBA::DispatcherCallback *cb, CORBA::Long fd)
{
    fevents.push_back(new FileEvent(this, cb, fd, QSocketNotifier::Read,
				    CORBA::Dispatcher::Read));
}

void QTDispatcher::wr_event (CORBA::DispatcherCallback *cb, CORBA::Long fd)
{
    fevents.push_back(new FileEvent(this, cb, fd, QSocketNotifier::Write,
				    CORBA::Dispatcher::Write));
}

void QTDispatcher::ex_event (CORBA::DispatcherCallback *cb, CORBA::Long fd)
{
    fevents.push_back(new FileEvent(this, cb, fd, QSocketNotifier::Exception,
				    CORBA::Dispatcher::Except));
}

void QTDispatcher::tm_event (CORBA::DispatcherCallback *cb, CORBA::ULong tmout)
{
    // both Qt and MICO timeouts are millisecs
    tevents.push_back(new TimerEvent(this, cb, tmout));
}

void QTDispatcher::remove (CORBA::DispatcherCallback *cb,
			   CORBA::Dispatcher::Event e)
{
    if (e == All || e == Timer) {
	list<TimerEvent*>::iterator i, next;
	for (i = tevents.begin(); i != tevents.end(); i = next) {
	    next = i;
	    ++next;
	    if ((*i)->cb == cb) {
		delete *i;
		tevents.erase (i);
	    }
	}
    }
    if (e == All || e == Read || e == Write || e == Except) {
	list<FileEvent*>::iterator i, next;
	for (i = fevents.begin(); i != fevents.end(); i = next) {
	    next = i;
	    ++next;
	    if ((*i)->cb == cb && (e == All || e == (*i)->ev)) {
		delete *i;
		fevents.erase(i);
	    }
	}
    }
}

void QTDispatcher::remove (TimerEvent *t)
{
    list<TimerEvent*>::iterator i
	= find (tevents.begin(), tevents.end(), t);
    assert (i != tevents.end());
    tevents.erase (i);
}

// misc ///////////////////////////////////////////////////////////////

void QTDispatcher::run (CORBA::Boolean infinite)
{
    do {
	ctx->processNextEvent (TRUE);
    } while (infinite);
}

void QTDispatcher::move (CORBA::Dispatcher *)
{
    assert (0);
}

CORBA::Boolean QTDispatcher::idle () const
{
    return fevents.size() + tevents.size() == 0;
}
