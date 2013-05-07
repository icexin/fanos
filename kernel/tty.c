#include <fanos/tty.h>
#include <fanos/serial.h>
#include <fanos/kernel.h>
#include <fanos/system.h>
#include <fanos/task.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>

#define IO_BUFF_LEN 512

char* const videoram = (char *const)0xB8000;
static int tty_task();
struct tty_t tty;

int sys_write(int fd, char *buf, int len)
{
	char tmp[IO_BUFF_LEN];
	get_fs_buff(tmp, buf, IO_BUFF_LEN);

	if (len > IO_BUFF_LEN) {
		len = IO_BUFF_LEN;
	}

	if(fd == 2)
		return rs_write(tmp, len);
	else
		return tty_write(tmp, len);
}

int sys_read(int fd, char *buf, int len)
{
	char tmp[IO_BUFF_LEN];
	if(len > 512){
		printk("sys_read overflow\n");
		len = 512;
	}
	int cnt = tty_read(tmp, len);
	put_fs_buff(buf, tmp, cnt + 1);
	return cnt;
}

int tty_write(char *str, int len)
{
	int done = 0;
	
	while (buffq_full(&tty.out_buf)) {
		log("tty out full\n");
		ksema_down(&tty.out_sem);
	}

	kmutex_lock(&tty.out_mutex);
	done = buffq_write(&tty.out_buf, str, len);
	wake_up(tty.ttytask);
	kmutex_unlock(&tty.out_mutex);

	return done;
}

int tty_read(char *str, int len)
{	
	int done = 0;
	while (buffq_empty(&tty.cooked_inbuf)) {
		ksema_down(&tty.cooked_sem);
	}
	kmutex_lock(&tty.cooked_mutex);
	done = buffq_read(&tty.cooked_inbuf, str, len);
	str[done] = '\0';
	kmutex_unlock(&tty.cooked_mutex);
	return done;
}

void init_tty()
{
	out_byte(0x3D4, 0xC);
	out_byte(0x3D5, 0);

	out_byte(0x3D4, 0xD);
	out_byte(0x3D5, 0);

	tty.row = 0;
	tty.col = 0;

	
	buffq_init(&tty.in_buf);
	buffq_init(&tty.out_buf);
	buffq_init(&tty.cooked_inbuf);
	tty.ttytask = create_task(tty_task);

	ksema_init(&tty.cooked_sem, 0);
	kmutex_init(&tty.cooked_mutex);
	ksema_init(&tty.out_sem, 0);
	kmutex_init(&tty.out_mutex);

	/* keyboard interrupt */
	pic_enable(IRQ_KEYBOARD);
	tty_clear();
}


void tty_clear()
{
   int i;
   for (i=0;i<80*25;i++) {
       videoram[i*2] = ' ';
   }
}

void tty_update_cursor()
{
	int pos = tty.row * MAX_COL + tty.col;
	out_byte(0x3D4, 0xE);
	out_byte(0x3D5, (pos >> 8) & 0xFF);

	out_byte(0x3D4, 0xF);
	out_byte(0x3D5, pos & 0xFF);
}

void tty_write_char(char ch)
{
	int j,k;

	/*换行*/
	if(ch == '\n' || tty.col == MAX_COL){
		tty.row++;
		tty.col = 0;
	}
	
	if(ch == '\b'){
		tty.col--;
	}
	
	/*返回到上一行*/
	if(tty.col < 0 && tty.row > 0){
		tty.col = MAX_COL - 1;
		tty.row--;
	}

	/*如果要写的行超出最大行，则所有行依次上移*/
	if(tty.row >= MAX_ROW){
		for(j=0; j<MAX_ROW-1; j++){
			for(k=0; k<MAX_COL; k++){
				screen(j,k) = screen(j+1,k);
			}
		}

		/*清最后一行*/
		for(j=0; j<MAX_COL; j++){
			screen(MAX_ROW-1, j) = ' ';
		}
		tty.row = MAX_ROW - 1;
	}

	if(ch == '\b'){
		screen(tty.row, tty.col) = ' ';
	}else if(ch != '\n'){
		screen(tty.row, tty.col++) = ch;
	}
	tty_update_cursor();
}

/* tty task 把键盘的输入字符按行规则处理,放入cooked_buf和outbuf
 * 并把outbuf中的字符输出到屏幕
 * 在没有任务做的时候进行睡眠,会被键盘和write调用唤醒
 */
int tty_task()
{
	log("tty task start pid=%d\n", tty.ttytask->pid);

	struct buffq *inbuf, *cooked_inbuf, *outbuf;

	inbuf = &tty.in_buf;
	cooked_inbuf = &tty.cooked_inbuf;
	outbuf = &tty.out_buf;

	int need_sleep = 1; //如果inbuf有字符则不会睡眠
	while (1) {
		need_sleep = 1;
		kmutex_lock(&tty.out_mutex);
		/* write the tty out buffer to screen */
		while (!buffq_empty(outbuf)) {
			char ch = buffq_shift(outbuf);
			int i = 0;
			if (ch == '\t') {
				for (i=0; i<TAB_BLANK_NUM; i++) {
					tty_write_char(' ');
				}
			} else {
				tty_write_char(ch);
			}
		}
		ksema_up(&tty.out_sem);
		kmutex_unlock(&tty.out_mutex);
		
		while (!buffq_empty(inbuf)) {
			char ch = 0;
			ch = buffq_shift(inbuf);

			kmutex_lock(&tty.cooked_mutex);
			if (ch == '\b') {
				if (!buffq_empty(cooked_inbuf)) {
					buffq_pop(cooked_inbuf);
					//log("cooked_inbuf.len=%d\n", cooked_inbuf->cnt);
					tty_write_char('\b');
				}
				kmutex_unlock(&tty.cooked_mutex);
				continue;
			}
			need_sleep = 0;
			buffq_push(cooked_inbuf, ch);
			buffq_push(outbuf, ch);

			if (ch == '\n') {
				if (waitq_isempty(&tty.cooked_sem.waitq)) {
					log("no process wait input, drop\n");
					buffq_init(cooked_inbuf);
				}
				ksema_up(&tty.cooked_sem);
			}
			kmutex_unlock(&tty.cooked_mutex);
		}
		if (need_sleep) 
			sleep_on(tty.ttytask);
	}
	return 0;
}

int putline(int row, int col, const char* fmt, ...)
{
	static char tmp[128] = {0};
	va_list ap = NULL;
	char* p = videoram + (row * MAX_COL + col) * 2;
	char* q = tmp;

	va_start(ap, fmt);
	vsnprintf(tmp, 128, fmt, ap);
	va_end(ap);

	while (*q) {
		*p = *q;
		p += 2;
		q += 1;
	}
	return 0;
}
	
