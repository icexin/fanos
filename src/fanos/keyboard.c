#include <fanos/tty.h>
#include <fanos/keymap.h>
#include <fanos/schedule.h>

#define KEY_BUF_LEN 128

extern unsigned short keymap[];

void do_keyboard()
{
	char code;
	unsigned char key;
	static char shift_key;

	code = in_byte(0x60);
	if(code & 0x80){
		code &= 0x7F;
		key = keymap[code*KEYMAP_COLS];
		if(key == LSHIFT || key == RSHIFT){
			shift_key = 0;
		}
		return;
	}else{
		if(shift_key){
			key = keymap[code*KEYMAP_COLS+1];
		}else{
			key = keymap[code*KEYMAP_COLS];
		}
		if(key == LSHIFT || key == RSHIFT){
			shift_key = 1;
			return;
		}
	}

	eflags_t eflag;
	disable_hwint(eflag);

	/* echo the key */
	buffq_push(&tty.in_buf, key);
	// wake the tty task when keyboard is pressed
	wake_up(tty.ttytask);

	restore_hwint(eflag);
}
