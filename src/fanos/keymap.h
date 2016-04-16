#ifndef FANOS_KEYBOARD_H
#define FANOS_KEYBOARD_H


/* Shift key offset. */
#define SHIFT 0x0100

#define ESCAPE            27
#define DELETE           127

/* Function keys. */

#define F1              0x81
#define F2              0x82
#define F3              0x83
#define F4              0x84
#define F5              0x85
#define F6              0x86
#define F7              0x87
#define F8              0x88
#define F9              0x89
#define F10             0x8a
#define F11             0x8b
#define F12             0x8c

/* Numeric keypad. */

#define HOME            0x8d
#define END             0x8e
#define UP              0x8f
#define DOWN            0x90
#define LEFT            0x91
#define RIGHT           0x92
#define PGUP            0x93
#define PGDN            0x94
#define CENTER          0x95
#define INSERT          0x96

/* Extension keys. */

#define LSHIFT          0x97
#define RSHIFT          0x98
#define CTRL            0x99
#define ALT             0x9a

/* Lock keys. */

#define CAPS_LOCK       0x9b
#define NUM_LOCK        0x9c
#define SCR_LOCK        0x9d



#define SCAN_CODES_COUNT 128
#define KEYMAP_COLS       2

#endif
