#ifndef POBJFINDER_CMD_H
#define POBJFINDER_CMD_H

/*
 * We get 2300
 */

// Desc: fetchs all object names that kSirc knows about
// iWinId: widget
// iArg: not defined
// cArg: path
#define PUKE_OBJFINDER_ALLOBJECTS 2300

// Desc: ack for the fetch
// iWinId: widget
// iArg: not defined
// cArg: new line seperated list of widgets className::name
#define PUKE_OBJFINDER_ALLOBJECTS_ACK -2300

// Desc: signal for creation of a new object, not used, only ack
// iWinId: not defined
// iArg: not defined
// cArg: not defined
#define PUKE_OBJFINDER_NEWOBJECT 2301

// Desc: signal for new widget created
// iWinId: widget
// iArg: not defined
// cArg: className::name of new object
#define PUKE_OBJFINDER_NEWOBJECT_ACK -2301

#endif