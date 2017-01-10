#ifndef COMMANDS_H
#define COMMANDS_H

// Define generic command codes
#define ID_OPEN 100
#define ID_NEW 101
#define ID_SAVE 102
#define ID_SAVE_AS 103
#define ID_CLOSE 104
#define ID_NEW_WINDOW 105
#define ID_CLOSE_WINDOW 106
#define ID_COPY 107
#define ID_CUT 108
#define ID_PASTE 109
#define ID_OPTIONS 110
#define ID_EXIT 111
#define ID_HELP_CONTENTS 112
#define ID_ABOUT 113 
#define ID_OPEN_URL 114
#define ID_SAVE_URL 115
#define ID_SHOW_TOOLBAR 116
#define ID_SHOW_MENUBAR 117
#define ID_SHOW_STATUSBAR 118
#define ID_SAVE_OPTIONS 119

// Define app specific command codes
#define ID_FORMAT 200
#define ID_PASTE_IMAGE 201
#define ID_ZOOM_IN 202
#define ID_ZOOM_OUT 203
#define ID_MASK 204
#define ID_INFO 205
#define ID_PALETTE 206
#define ID_DEPTH 207
#define ID_RELEASENOTES 208
#define ID_SHOW_TOOLS_TOOLBAR 209
#define ID_SHOW_COMMANDS_TOOLBAR 210
#define ID_RESIZE_IMAGE 211

// Tags for statusbar items
#define ID_FILE_SIZE 300
#define ID_ZOOM_FACTOR 301
#define ID_FILE_NAME 302
#define ID_COLOR_DEPTH 303
#define ID_FILE_TYPE 304
#define ID_TOOL_NAME 305

// Tags for toolbars
#define ID_COMMANDS_TOOLBAR 0
#define ID_TOOLS_TOOLBAR 1

#endif // COMMANDS_H
