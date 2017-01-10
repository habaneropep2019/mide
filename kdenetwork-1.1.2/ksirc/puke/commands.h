
// 0 is special "invalid character"
// Value > 0 indicates from dsirc -> ksirc
// Value < 0 indicates from ksirc -> dsirc

// Desc: INVALID command
// iWinId: not defined
// iArg: not defined
// cArg: not defined
#define PUKE_INVALID 0

// Desc: associantes server name with fd.
// iWinId: not defined, but pass unchanged
// iArg: not defined
// cArg: name of the server
#define PUKE_SETUP 1

// Desc: replies to make sure association was valid
// iWinId: not defined, but pass unchanged
// iArg: size of Message
// cArg: not defined
#define PUKE_SETUP_ACK -1


// Desc: sends the ack back.  Used for doing actions after returning to select.
// iWinId: window id
// iArg: not defined
// cArg: not define
#define PUKE_ECHO 5

// Desc: sends the ack back.  Used for doing actions after returning to select.
// iWinId: window id
// iArg: not defined
// cArg: not define
#define PUKE_ECHO_ACK -5


// From ksirc to sirc, event command unkown.
// There should be somewhere better for this!!!

#define PUKE_EVENT_UNKOWN -999

// Dumps object tree
// iWinId: puke controller
// iArg: not defined
// cArg: undef
#define PUKE_DUMPTREE 997

// Desc: dump object tree ack
// iWinId: undef
// iArg: undef
// cArg: undef
#define PUKE_DUMPTREE_ACK -997

// Release a widget without delete'ing it, used after fetching a widget
// iWinId: window id
// iArg: undef
// cArg: undef
#define PUKE_RELEASEWIDGET 996

// Desc: release ack
// iWinId: window id
// iArg: undef
// cArg: undef
#define PUKE_RELEASEWIDGET_ACK -996


// Fetch widgets
// Desc: get widget from parent
// iWinId: not defined
// iArg: not defined
// cArg: 2 feilds, tab sperated, must be returned unchanged. 1. random string.  3. Object Name.
#define PUKE_FETCHWIDGET 998

// Desc: Fetch widget ack
// iWinId: new widget id
// iArg: not defined
// cArg: 2 feilds, tab sperated, must be returned unchanged. 1. random string. 3. Object Name.
#define PUKE_FETCHWIDGET_ACK -998


// Widget commands starts at 1000 and end at 10000

// Desc: create new widget
// iWinId: parent widget id
// iArg: widget type as defined by PWIDGET_*
// cArg: Must be return unchanged
#define PUKE_WIDGET_CREATE 1000

// Desc: ack for newly created new widget
// iWinId: new widget Id, 0 if failed
// iArg: widget type as defined by PWIDGET_*
// cArg: Returned unchanged
#define PUKE_WIDGET_CREATE_ACK -1000


// Desc: shows requested widget
// iWinId: widget to show
// iArg: not defined
// cArg: not define
#define PUKE_WIDGET_DELETE 1001

// Desc: ack for show requested widget
// iWinId: widget to show
// iArg: not defined
// cArg: not define
#define PUKE_WIDGET_DELETE_ACK -1001

// Desc: shows requested widget
// iWinId: widget to show
// iArg: not defined
// cArg: not define
#define PUKE_WIDGET_SHOW 1002

// Desc: ack for show requested widget
// iWinId: widget to show
// iArg: not defined
// cArg: not define
#define PUKE_WIDGET_SHOW_ACK -1002

// Desc: hidess requested widget
// iWinId: widget to hide
// iArg: not defined
// cArg: not define
#define PUKE_WIDGET_HIDE 1003

// Desc: ack for hide requested widget
// iWinId: widget to hide
// iArg: not defined
// cArg: not define
#define PUKE_WIDGET_HIDE_ACK -1003

// Desc: repaint widget
// iWinId: widget to repaint
// iArg: erase, 0 for false, 1 for true
// cArg: no defines
#define PUKE_WIDGET_REPAINT 1005

// Desc: repaint widget ack
// iWinId: widget to repaint
// iArg: not defined
// cArg: not defined
#define PUKE_WIDGET_REPAINT_ACK -1005

// Desc: update widget on next event loop
// iWinId: widget to repaint
// iArg: erase, 0 for false, 1 for true
// cArg: no defines
#define PUKE_WIDGET_UPDATE 1010

// Desc: repaint widget ack
// iWinId: widget to repaint
// iArg: not defined
// cArg: not defined
#define PUKE_WIDGET_UPDATE_ACK -1010

// Desc: resize the widget
// iWinId: widget to repaint
// iArg: width in the low 16 bit, height in next 16 bits
// cArg: not defined
#define PUKE_WIDGET_RESIZE 1015

// Desc: repaint widget ack
// iWinId: widget to repaint
// iArg: new widget in lower 16 bit, new height in lower 16 bits
// cArg: not defined
#define PUKE_WIDGET_RESIZE_ACK -1015

// -1020 to -1040 defines QEvent types
// All arguments are 0 unless otherwise stated
#define PUKE_WIDGET_EVENT_NONE -1020
// iArg: timerId
#define PUKE_WIDGET_EVENT_TIMER -1021
// All EVENT_MOUSE are:
// cArg: cast to int[25], int[0] = x(), int[1] = y(), int[2] = button()
//       int[3] = state()
#define PUKE_WIDGET_EVENT_MOUSEBUTTONPRESS -1022
#define PUKE_WIDGET_EVENT_MOUSEBUTTONRELEASE -1023
#define PUKE_WIDGET_EVENT_MOUSEDBLCLICK -1024
#define PUKE_WIDGET_EVENT_MOUSEMOVE -1025
// All EVENT_KEY
// cArg: cast to int[25], int[0] = key(), int[1] = ascii(), int[2] = state()
#define PUKE_WIDGET_EVENT_KEYPRESS -1026
#define PUKE_WIDGET_EVENT_KEYRELEASE -1027
// All EVENT_FOCUS
// cArg[0] = gotFocus()
// cArg[1] = lostFocus()
#define PUKE_WIDGET_EVENT_FOCUSIN -1028
#define PUKE_WIDGET_EVENT_FOCUSOUT -1029
#define PUKE_WIDGET_EVENT_ENTER -1030
#define PUKE_WIDGET_EVENT_LEAVE -1031
// Paint event
#define PUKE_WIDGET_EVENT_PAINT -1032
// cArg: int[0] = pos()->x() int[1] = pos->y()
//       int[2] = oldPos->x() int[3] = oldPos->y()
#define PUKE_WIDGET_EVENT_MOVE -1033
// cArg: int[0] = size()->width() int[1] = size()->height()
//       int[2] = oldSize()->width() int[3] = oldSize()->height()
#define PUKE_WIDGET_EVENT_RESIZE -1034
#define PUKE_WIDGET_EVENT_CREATE -1035 
#define PUKE_WIDGET_EVENT_DESTROY -1036
#define PUKE_WIDGET_EVENT_SHOW -1037   
#define PUKE_WIDGET_EVENT_HIDE -1038   
#define PUKE_WIDGET_EVENT_CLOSE -1039  

// Desc: move widget
// iWinId: widget id
// iArg: new location, lower short is x, upper short is y
// cArg: not define
#define PUKE_WIDGET_MOVE 1050

// Desc: move widget
// iWinId: widget id
// iArg: new location, lower short is x, upper short is y
// cArg: not define
#define PUKE_WIDGET_MOVE_ACK -1050

// Desc: open and load library file
// iWinid: not defined
// iArg: type of widget
// cArg: file name
#define PUKE_WIDGET_LOAD 1055

// Desc: ack the open library file
// iWinid: not defined
// iArg: widget number
// cArg: not defined
#define PUKE_WIDGET_LOAD_ACK -1055

// Desc: unload library file
// iWinid: not define
// iArg: type of widget to unload
// cArg: no define
#define PUKE_WIDGET_UNLOAD 1060

// Desc: ack open and load library file
// iWinid: not defined
// iArg: not defined
// cArg: no defined
#define PUKE_WIDGET_UNLOAD_ACK -1060

// Desc: set's a a minimum size
// iWinid: window id
// iArg: 2 ints, lower is width , upper is height
// cArg: not define
#define PUKE_WIDGET_SETMINSIZE 1065

// Desc: ack for set size
// iWinid: window id
// iArg: 2 ints, lower is new width, upper is new height
// cArg: not defined
#define PUKE_WIDGET_SETMINSIZE_ACK -1065

// Desc: set's a a minimum size
// iWinid: window id
// iArg: 2 ints, lower is width , upper is height
// cArg: not define
#define PUKE_WIDGET_SETMAXSIZE 1070

// Desc: ack for set size
// iWinid: window id
// iArg: 2 ints, lower is new width, upper is new height
// cArg: not defined
#define PUKE_WIDGET_SETMAXSIZE_ACK -1070

// Desc: set widget caption
// iWinid: window id
// iArg: not defined
// cArg: widget caption
#define PUKE_WIDGET_SETCAPTION 1075

// Desc: ack for set caption
// iWinid: window id
// iArg: not define
// cArg: new caption
#define PUKE_WIDGET_SETCAPTION_ACK -1075

// Desc: get background colour
// iWinid: window id
// iArg: not defined
// cArg: not defined
#define PUKE_WIDGET_GET_BACKGROUND_COLOUR 1080

// Desc: get background colour ack
// iWinid: window id
// iArg: not defined
// cArg: 3 ints packed in rbg combo
#define PUKE_WIDGET_GET_BACKGROUND_COLOUR_ACK -1080

// Desc: set background colour
// iWinid: window id
// iArg: not defined
// cArg: 3 ints packed in rbg combo
#define PUKE_WIDGET_SET_BACKGROUND_COLOUR 1085

// Desc: set background colour ack
// iWinid: window id
// iArg: not defined
// cArg: not defined
#define PUKE_WIDGET_SET_BACKGROUND_COLOUR_ACK -1085

// Desc: set background pixmap
// iWinid: window id
// iArg: not defined
// cArg: path name to pixmap
#define PUKE_WIDGET_SET_BACKGROUND_PIXMAP 1086

// Desc: set background pixmap ack
// iWinid: window id
// iArg: not defined
// cArg: not defined
#define PUKE_WIDGET_SET_BACKGROUND_PIXMAP_ACK -1086

// Desc: set background mode
// iWinid: window id
// iArg: mode
// cArg: not defined
#define PUKE_WIDGET_SET_BACKGROUND_MODE 1087

// Desc: set background mode ack
// iWinid: window id
// iArg: new mode
// cArg: not defined
#define PUKE_WIDGET_SET_BACKGROUND_MODE_ACK -1087

// Desc: set widget enabled or disabled
// iWinid: window id
// iArg: 0 for disabled, 1 for enabled
// cArg: not defined
#define PUKE_WIDGET_SET_ENABLED 1090

// Desc: ack for enable/disable
// iWinid: window id
// iArg: not defined
// cArg: not defined
#define PUKE_WIDGET_SET_ENABLED_ACK -1090

// Desc: recreate widget with new parent, etc
// IwinId: window id
// iArg: window ID for new parent, 0x0 for no parent, toplevel
// cArg: 3 packed ints, 0 = x, 1 = y, 2 = showit
#define PUKE_WIDGET_RECREATE 1091

// Desc: ACK recreate widget with new parent, etc
// IwinId: window id
// iArg: 0 (for all ok, other for error)
// cArg: 0
#define PUKE_WIDGET_RECREATE_ACK -1091

// QFrame gets 1100

// 1100 defines QFrame
// Desc: set Frame style
// iWinId: widget to chanse
// iArg: frame style to set.
// cArg: no define
#define PUKE_QFRAME_SET_FRAME 1100

// Desc: get/ack Frame style
// iWinId: widget to changed
// iArg: frame style.
// cArg: no define
#define PUKE_QFRAME_SET_FRAME_ACK -1100

// Desc: set Frame line width
// iWinId: widget to chanse
// iArg: newline width.
// cArg: no define
#define PUKE_QFRAME_SET_LINEWIDTH 1105

// Desc: get/ack Frame line width
// iWinId: widget to changed
// iArg: line width.
// cArg: no define
#define PUKE_QFRAME_SET_LINEWIDTH_ACK -1105

//----------------------------------------------------
// 1200 is a QLineEdit

// Desc: set max line length
// iWinId: widget to change
// iArg: max line length
// cArg: not defined
#define PUKE_LINED_SET_MAXLENGTH 1200

// Desc: set max line length
// iWinId: widget to change
// iArg: new max line length
// cArg: not defined
#define PUKE_LINED_SET_MAXLENGTH_ACK -1200

// Desc: set echo mode, normal password, no echo, etc
// iWinId: widget to change
// iArg: echo mode
// cArg: not defined
#define PUKE_LINED_SET_ECHOMODE 1205

// Desc: ack for set
// iWinId: widget to change
// iArg: current echo mode
// cArg: not define
#define PUKE_LINED_SET_ECHOMODE_ACK -1205

// Desc: set text contents of widget
// iWinId: widget to change
// iArg: not defined
// cArg: text to set widget too
#define PUKE_LINED_SET_TEXT 1210

// Desc: ack for set
// iWinId: widget to change
// iArg: not define
// cArg: current test
#define PUKE_LINED_SET_TEXT_ACK -1210

// Desc: get current text
// iWinId: widget to change
// iArg:not define
// cArg: not defined
#define PUKE_LINED_GET_TEXT 1215

// Desc: returned info for get text
// iWinId: widget to change
// iArg: not defined
// cArg: text in widget
#define PUKE_LINED_GET_TEXT_ACK -1215

// Desc: return was pressed, action not defined
// iWinId: 
// iArg:
// cArg:
#define PUKE_LINED_RETURN_PRESSED 1220

// Desc: return was pressed, return information
// iWinId: widget pressed in
// iArg: not defined
// cArg: not defined
#define PUKE_LINED_RETURN_PRESSED_ACK -1220

//------------------------------------------------

// Desc: set's button's current text
// iWinId: window id
// iArg: not defined
// cArg: text
#define PUKE_BUTTON_SET_TEXT 1300

// Desc: ack for set text
// iWinId: widget id
// iArg: not defined
// cArg: text
#define PUKE_BUTTON_SET_TEXT_ACK -1300

// Desc: set button to pixmap
// iWinId: window id
// iArg: not defined
// cArg: path to pixmap
#define PUKE_BUTTON_SET_PIXMAP 1305

// Desc: ack for set pixmap
// iWinId: widget id
// iArg: pixmap()->isNull()
// cArg: not defined
#define PUKE_BUTTON_SET_PIXMAP_ACK -1305

// Desc: set if button resizes with content changes
// iWinId: window id
// iArg: reisze, 0 false, 1 true
// cArg: not defined
#define PUKE_BUTTON_SET_AUTORESIZE 1310

// Desc: ack for autreisze
// iWinId: widget id
// iArg: autoresize()
// cArg: not defined
#define PUKE_BUTTON_SET_AUTORESIZE_ACK -1310

// Desc: button was pressed, not used
// iWinId: not defined
// iArg: not defined
// cArg: not define
#define PUKE_BUTTON_PRESSED 1350

// Desc: signal button was pressed
// iWinId: winid
// iArg: not define
// cArg: not defined
#define PUKE_BUTTON_PRESSED_ACK -1350

// Desc: button was relased, not used
// iWinId: not defined
// iArg: not defined
// cArg: not define
#define PUKE_BUTTON_RELEASED 1351

// Desc: signal button was released
// iWinId: winid
// iArg: not define
// cArg: not defined
#define PUKE_BUTTON_RELEASED_ACK -1351

// Desc: button was clicked, not used
// iWinId: not defined
// iArg: not defined
// cArg: not define
#define PUKE_BUTTON_CLICKED 1352

// Desc: signal button was clicked
// iWinId: winid
// iArg: not defined
// cArg: not defined
#define PUKE_BUTTON_CLICKED_ACK -1352

// Desc: button was toggled, not used
// iWinId: not defined
// iArg: not defined
// cArg: not define
#define PUKE_BUTTON_TOGGLED 1353

// Desc: signal button was toggled
// iWinId: winid
// iArg: not define
// cArg: not defined
#define PUKE_BUTTON_TOGGLED_ACK -1353


//------------------------------------------------

// Desc: set's the range of the control
// iWinId: window id
// iArg: two packed short ints.  Lower is lower, upper is upper
// cArg: not define
#define PUKE_KSPROGRESS_SET_RANGE 1400

// Desc: ack set's the range of the control
// iWinId: window id
// iArg: not define
// cArg: not defined
#define PUKE_KSPROGRESS_SET_RANGE_ACK -1400

// Desc: set the top text line
// iWinId: window id
// iArg: not define
// cArg: top line
#define PUKE_KSPROGRESS_SET_TOPTEXT 1410

// Desc: ack set the top text line
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_KSPROGRESS_SET_TOPTEXT_ACK -1410

// Desc: set the bottom text line
// iWinId: window id
// iArg: not define
// cArg: bottom line
#define PUKE_KSPROGRESS_SET_BOTTEXT 1415

// Desc: ack set the top text line
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_KSPROGRESS_SET_BOTTEXT_ACK -1415

// Desc: set status value
// iWinId: window id
// iArg: not define
// cArg: current value
#define PUKE_KSPROGRESS_SET_VALUE 1420

// Desc: ack set the top text line
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_KSPROGRESS_SET_VALUE_ACK -1420

                                                
// Desc: cancel pressed, typically only ack sent.
// iWinId: window id
// iArg: not define
// cArg: current value
#define PUKE_KSPROGRESS_CANCEL 1425

// Desc: send message saying cancel button was pressed
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_KSPROGRESS_CANCEL_ACK -1425

// QTable View at 1500

// none yet

// QListBox at 1600

// Desc: insert string item
// iWinId: window id
// iArg: number of items now in list
// cArg: undef
#define PUKE_LISTBOX_INSERT 1600

// Desc: insert string item ack
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LISTBOX_INSERT_ACK -1600


// Desc: insert string item in sorted order
// iWinId: window id
// iArg: number of items in list
// cArg: undef
#define PUKE_LISTBOX_INSERT_SORT 1605

// Desc: ack insert string item in sorted order
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LISTBOX_INSERT_SORT_ACK -1605

// Desc: insert pixmap 
// iWinId: window id
// iArg: not defined
// cArg: file name
#define PUKE_LISTBOX_INSERT_PIXMAP 1610

// Desc: ack insert pixmap
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LISTBOX_INSERT_PIXMAP_ACK -1610

// Desc: highligth number
// iWinId: window id
// iArg: item index to highlight
// cArg: undef
#define PUKE_LISTBOX_HIGHLIGHT 1615

// Desc: ack
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LISTBOX_HIGHLIGHT_ACK -1615

// Desc: remove number
// iWinId: window id
// iArg: item index to remove
// cArg: undef
#define PUKE_LISTBOX_REMOVE 1620

// Desc: ack
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LISTBOX_REMOVE_ACK -1620

// Desc: Get text from item number
// iWinId: window id
// iArg: list item
// cArg: not defined
#define PUKE_LISTBOX_GETTEXT 1625

// Desc: ack for get text with cArg set to text
// iWinId: window id
// iArg: 1 for sucsess, 0 for failure
// cArg: text
#define PUKE_LISTBOX_GETTEXT_ACK -1625 

// Desc: Clear list box
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LISTBOX_CLEAR 1630

// Desc: ack for clear list box
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LISTBOX_CLEAR_ACK -1630

// Desc: set scroll bar
// iWinId: window id
// iArg: bool, enable/disable
// cArg: undef
#define PUKE_LISTBOX_SET_SCROLLBAR 1635

// Desc: set scroll bar ack
// iWinId: window id
// iArg: undef
// cArg: undef
#define PUKE_LISTBOX_SET_SCROLLBAR_ACK -1635

// Desc: set auto scroll bar
// iWinId: window id
// iArg: bool, enable/disable
// cArg: undef
#define PUKE_LISTBOX_SET_AUTO_SCROLLBAR 1636

// Desc: set auto scroll bar ack
// iWinId: window id
// iArg: undef
// cArg: undef
#define PUKE_LISTBOX_SET_AUTO_SCROLLBAR_ACK -1636

// Desc: item highlighted, not used signal to dsirc
// iWinId: undef
// iArg: undef
// cArg: undef
#define PUKE_LISTBOX_HIGHLIGHTED 1690

// Desc: item highlighted
// iWinId: window id
// iArg: index
// cArg: contents
#define PUKE_LISTBOX_HIGHLIGHTED_ACK -1690

// Desc: item selected
// iWinId: window id
// iArg: index
// cArg: contents
#define PUKE_LISTBOX_SELECTED 1690

// Desc: send message saying cancel button was pressed
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LISTBOX_SELECTED_ACK -1690

// *** 1700 goes to the Plabel

// Desc: Set label to text in carg
// iWinId: window id
// iArg: not defined
// cArg: text to be set to
#define PUKE_LABEL_SETTEXT 1700

// Desc: Set label to text in carg, ack
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LABEL_SETTEXT_ACK -1700

// Desc: Set label pixmap specified by filane name
// iWinId: window id
// iArg: not defined
// cArg: filena,me for pixmap
#define PUKE_LABEL_SETPIXMAP 1705

// Desc: Set label pixmap specified by filane name ACK
// iWinId: window id
// iArg: not defined
// cArg: not defined
#define PUKE_LABEL_SETPIXMAP_ACK -1705

// Desc: Set label to movie specified by filename
// iWinId: window id
// iArg: not defined
// cArg: movie's filename
#define PUKE_LABEL_SETMOVIE 1710

// Desc: Set label to movie specified by filename, ACK
// iWinId: window id
// iArg: not defined
// cArg: not deefine
#define PUKE_LABEL_SETMOVIE_ACK -1710

// Desc: Set label's allignment
// iWinId: window id
// iArg: alignment
// cArg: not defined
#define PUKE_LABEL_SETALIGNMENT 1715

// Desc: Set label's allignment, ACK
// iWinId: window id
// iArg: alignment
// cArg: not defined
#define PUKE_LABEL_SETALIGNMENT_ACK -1715

// -----------------------------------------------------------------------
// PMenuData

// Desc: Inserts newtext menu item
// iWinId: window id
// iArg: keyboard accelerator (-1 for none)
// cArg: Text
#define PUKE_MENUDATA_INSERT_TEXT 1800

// Desc: ack for insert
// iWinId: window id
// iArg: object id
// cArg: undef
#define PUKE_MENUDATA_INSERT_TEXT_ACK -1800

// Desc: Inserts new pixmap into menu
// iWinId: window id
// iArg: keyboard accelerator (-1 for none)
// cArg: Text
#define PUKE_MENUDATA_INSERT_PIXMAP 1801

// Desc: ack for insert
// iWinId: window id
// iArg: object id
// cArg: undef
#define PUKE_MENUDATA_INSERT_PIXMAP_ACK -1801


// Desc: Item got activated, not used, only ack
// iWinId: undef
// iArg: undef
// cArg: uidef
#define PUKE_POPUPMENU_ACTIVATED 1805

// Desc: Item got activate
// iWinId: window id
// iArg: item id
// cArg: undef
#define PUKE_POPUPMENU_ACTIVATED_ACK -1805

// Desc: Popup to current cursor position
// iWinId: menu id
// iArg: undef
// cArg: uidef
#define PUKE_POPUPMENU_POPUP_CURRENT 1810

// Desc: Ack for popup
// iWinId: window id
// iArg: 1 for success
// cArg: undef
#define PUKE_POPUPMENU_POPUP_CURRENT_ACK -1810

// Desc: Remvoes an item from a popup menu
// iWinId: menu id
// iArg: undef
// cArg: uidef
#define PUKE_MENUDATA_REMOVE_ITEM 1815

// Desc: Ack for remove item
// iWinId: window id
// iArg: 1 for success
// cArg: undef
#define PUKE_MENUDATA_REMOVE_ITEM_ACK -1815

// -----------------------------------------------------------------------
// PAMenuData

// Desc: is the item in the top of the list?
// iWinId: window id
// iArg: index
// cArg: not defined
#define PUKE_ALISTBOX_ISTOP 1900

// Desc: ack for istop
// iWinId: window id
// iArg: 1 for top, 0 for bottom
// cArg: not defined
#define PUKE_ALISTBOX_ISTOP_ACK -1900

// Desc: set green highligh (voice)
// iWinId: window id
// iArg: highligh, 1 for set, 0 for not
// cArg: string
#define PUKE_ALISTBOX_SMALL_HIGHLIGHT 1905

// Desc: ack for set green highligh
// iWinId: window id
// iArg: 0 for success, 1 for failure
// cArg: not defined
#define PUKE_ALISTBOX_SMALL_HIGHLIGHT_ACK -1905

// Desc: set red highligh and raise to the top (op)
// iWinId: window id
// iArg: highlight, 1 for set, 0 for remove
// cArg: string
#define PUKE_ALISTBOX_BIG_HIGHLIGHT 1910

// Desc: ack for set red and raise
// iWinId: window id
// iArg: 0 for success, 1 for failure
// cArg: not defined
#define PUKE_ALISTBOX_BIG_HIGHLIGHT_ACK -1910

// Desc: finds nick index
// iWinId: window id
// iArg: not defined
// cArg: string
#define PUKE_ALISTBOX_FIND_NICK 1915

// Desc: ack for find nick
// iWinId: window id
// iArg: index
// cArg: not defined
#define PUKE_ALISTBOX_FIND_NICK_ACK -1915

// -----------------------------------------------------------------------
// PTabDialog

// Desc: add Widget defined by iArg's widget id
// iWinId: window id
// iArg: widget to add, widget id, in current fd
// cArg: not defined
#define PUKE_TABDIALOG_ADDTAB 2001

// Desc: ack for istop
// iWinId: window id
// iArg: 1 for failure, 0 for success
// cArg: not defined
#define PUKE_TABDIALOG_ADDTAB_ACK -2001

// -----------------------------------------------------------------------
// PKSircListBox

// Desc: is the item in the top of the list?
// iWinId: window id
// iArg: index
// cArg: not defined
#define PUKE_KSIRCLISTBOX_TOBOTTOM 2100

// Desc: ack for istop
// iWinId: window id
// iArg: 1 for top, 0 for bottom
// cArg: not defined
#define PUKE_KSIRCLISTBOX_TOBOTTOM_ACK -2100

// 2200 for PKFileDialog
// 2300 for PObjFinder


// ***********************************************************************
// ***********************************************************************

// 
// Base commands are done, next describes Widget's
//

// OBJECT base class
#define PWIDGET_OBJECT 1

// WIDGET defines a base QWidget class
#define PWIDGET_WIDGET 2

// FRAME defines a base class
#define PWIDGET_FRAME 3

// LINED defines the simple SLE
#define PWIDGET_LINED 4

// BUTTON defines the abstract class QButton
#define PWIDGET_BUTTON 5

// PUSH defined the push button
#define PWIDGET_PUSHBT 6

// Progress window for dcc and stuff
#define PWIDGET_KSPROGRESS 7

// Table view, not really used for much, base class for lots
#define PWIDGET_TABLEVW 8

// List box
#define PWIDGET_LISTBOX 9

// Label
#define PWIDGET_LABEL 10

// BoxLayout
#define POBJECT_LAYOUT 11

// MenuData
#define PWIDGET_MENUDATA 12

// PopupMenu

#define PWIDGET_POPMENU 13

// PAListBox

#define PWIDGET_ALISTBOX 14

// PKSircListBox

#define PWIDGET_KSIRCLISTBOX 15

// PTabDialog

#define PWIDGET_TABDIALOG 16

// PKFileDialog

#define PWIDGET_KFILEDIALOG 17

// PObjFinder

#define PWIDGET_OBJFINDER 18


// Group layout commands exist between 10000 and 11000

// Desc: create new box layout
// iWinId: PWidget parent
// iArg: 2 shorts, short[0] direction, short[1] border
// cArg: random character string
#define PUKE_LAYOUT_NEW 11000
// Desc: ack for box layout
// iWinId: Layout ID.
// iArg: not define
// cArg: same random m character string as PUKE_LAYOUT_NEW
#define PUKE_LAYOUT_NEW_ACK -11000

#define PUKE_LAYOUT_ADDLAYOUT 11005
#define PUKE_LAYOUT_ADDLAYOUT_ACK -11005

// Desc: add widget into layout manager
// iWinId: Layout Manager to add widget too
// iArg: Widget Id to be added
// cArg: 2 characters, char[0] strech, char[1] alignement
#define PUKE_LAYOUT_ADDWIDGET 11010
// Desc: ack for add widget
// iWinId: Layout manager 
// iArg: not defined
// cArg: not define
#define PUKE_LAYOUT_ADDWIDGET_ACK -11010

// Desc: adds a strut for the current box, ie can'be be bigger then int
// iWinId: Layout Manager to add strut
// iArg: strut size
// cArg: not define
#define PUKE_LAYOUT_ADDSTRUT 11015

// Desc: ack for strut add widget
// iWinId: Layout manager 
// iArg: not defined
// cArg: not define
#define PUKE_LAYOUT_ADDSTRUT_ACK -11015

// Desc: activates layout management, like show() for widget
// iWinId: Layout Manager to activate
// iArg: undef
// cArg: undef
#define PUKE_LAYOUT_ACTIVATE 11020

// Desc: ack for strut add widget
// iWinId: Layout manager 
// iArg: 1 on failure, 0 on success
// cArg: not define
#define PUKE_LAYOUT_ACTIVATE_ACK -11020

/*
 * ----------------------------------------------------------------------
 * persistant objects
 */

#define PUKE_CONTROLLER 1
