#include <tty.h>
#include <keymap.h>

#define KEY_BUF_LEN 128

extern unsigned short keymap[];
extern struct tty_t tty;

void do_keyboard()
{
	char code;
	unsigned char key;
	static char shift_key;
	static char keybuf[KEY_BUF_LEN];
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
	
	/* echo the key */
	buf_push(&tty.out_buf, key);

	/* we flush the keyboard buffer when it's full and
	 * the current key is not '\n'
	 */

	if(cnt == KEY_BUF_LEN  && key != '\n'){
		printk("keyboard buffer full\n");
		key = '\n';
	}
		
	
	keybuf[cnt++] = key;

	if(key == '\n'){
		char *p = keybuf;
		for(; p<keybuf+cnt; p++){
			buf_push(&tty.in_buf, *p);
		}
		cnt = 0;
	}
}


