#ifndef PKFILEDIALOG_CMD_H
#define PKFILEDIALOG_CMD_H

/*
 * We get 2200
 */

// Desc: sets the current directory
// iWinId: widget
// iArg: not defined
// cArg: path
#define PUKE_KBFD_SET_PATH 2200

// Desc: ack for the set
// iWinId: widget
// iArg: not defined
// cArg: actual path
#define PUKE_BFD_SET_PATH_ACK -2200

// Desc: sets the current filter
// iWinId: widget
// iArg: not defined
// cArg: filter, as defined in KFileBaseDialog::setFilter
#define PUKE_KBFD_SET_FILTER 2201

// Desc: ack for the set
// iWinId: widget
// iArg: not defined
// cArg: not define
#define PUKE_KBFD_SET_FILTER_ACK -2201

// Desc: sets the current file
// iWinId: widget
// iArg: not defined
// cArg: filename (does it need the full path?)
#define PUKE_KBFD_SET_SELECTION 2202

// Desc: ack for the set
// iWinId: widget
// iArg: not defined
// cArg: not define
#define PUKE_KBFD_SET_SELECTION_ACK -2202

// Desc: ack for current file select
// iWinId: not defined
// iArg: not defined
// cArg: not defined
#define PUKE_KBFD_FILE_SELECTED 2203

// Desc: ack for current file select
// iWinId: widget
// iArg: not defined
// cArg: filename
#define PUKE_KBFD_FILE_SELECTED_ACK -2203

#endif