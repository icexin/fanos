#include <tty.h>
#include <keymap.h>

extern unsigned short keymap[];
extern struct tty_t tty;

void do_keyboard()
{
	char code;
	unsigned char key;
	static char shift_key;
	static char keybuf[128];
	static int cnt = 0;
	int i;

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
	
	i = tty.out_buf.tail; 
	tty.out_buf.data[i] = key;
	tty.out_buf.tail = (i + 1) % TTY_BUF_CNT;
	tty.out_buf.cnt++;

	keybuf[cnt++] = key;
	if(key == '\n'){
		char *p = keybuf;
		for(; p<keybuf+cnt; p++){
			i = tty.in_buf.tail;
			tty.in_buf.data[i] = *p;
			tty.in_buf.tail = (i + 1) % TTY_BUF_CNT;
			tty.in_buf.cnt++;
		}
		cnt = 0;
	}
}


