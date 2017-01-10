/////////////////////////////////////////////////////////////////////////
//
// File: Trace.h
//
// Author: Kurt Granroth
//
// Description:
//    This implements the Trace class.  To use, #include this file and
//    link with Trace.o.  For each function you wish to trace, include
//    a TRACEINIT(function_name) at the beginning of the function.  The
//    macros TRACE and TRACEF will output a static string or a printf-
//    style string, respectively.
//
//    TRACEON(bool) turns tracing on or off.  TRACETAB(int) set the tab-
//    stop for the output.  Default is tracing = on and tabstop = 2
//
// Example:
//    void someFunction() {
//    	TRACEINIT("someFunction()");
//    	TRACE("Output");
//
//    	otherFunction();
//    }
//
//    void otherFunction() {
//    	TRACEINIT("otherFunction()");
//    	TRACEF("Some int = %d", 10);
//    }
//
// Output:
// 	Entering someFunction()
// 	  someFunction() : Output
// 	  Entering otherFunction()
// 	    otherFunction() : Some int = 10
// 	  Leaving otherFunction()
// 	Leaving someFunction()
//     
/////////////////////////////////////////////////////////////////////////
#ifndef TRACE_H
#define TRACE_H

#include <qstring.h>
#include <iostream.h>
#include <stdio.h>
#include <stdarg.h>

#define TRACEINIT Trace __trace
#define TRACE     __trace.output
#define TRACEF    __trace.outputf
#define TRACEON   Trace __trace
#define TRACETAB  __trace.setTabStop

class Trace
{
public:
	Trace(bool do_trace);
	Trace(const char* function_name);
	virtual ~Trace();

	static int traceIndent;
	static int tabStop;
	static bool traceOn;

	virtual void output(const QString& message);
	virtual void output(const QString& message, void * object);
	virtual void outputf(const QString& message ...);

	void setTraceOn(const bool trace_on = true);
	void setTabStop(const int tabstop);

protected:
	void m_indent();
	QString m_functionName;
};
#endif // TRACE_H
