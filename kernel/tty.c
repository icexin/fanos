#include <tty.h>
#include <string.h>
#include <unistd.h>
#include <system.h>

#define MAX_ROW 25
#define MAX_COL 80

#define screen(row,col) (*(videoram+((row)*MAX_COL+(col))*2))

struct tty_t tty;
char tty_in_lock, tty_out_lock;

char* const videoram = (char *const)0xB8000;


static void tty_task();

extern int rs_write(char *buf, int len);

int sys_write(int fd, char *buf, int len)
{
	static char tmp[512];
	get_fs_str(tmp, buf);

	if(fd == 4)
		return rs_write(tmp, len);
	else
		return tty_write(tmp, len);
	
}

int sys_read(int fd, char *buf, int len)
{
	static char tmp[512];
	if(len > 512){
		printk("sys_read overflow\n");
		len = 512;
	}
	tty_read(tmp, len);
	put_fs_str(buf, tmp, len);
}



int tty_write(char *str, int len)
{
	char *p;
	int i;
	for(p=str; p<str+len; p++){
		while(!buf_push(&tty.out_buf, *p)){
			schedule();
		}
	
	}
}

int tty_read(char *str, int len)
{
	int cnt = 0;
	char ch;
	while(cnt < len){
		while(buf_empty(&tty.cooked_inbuf)){
			schedule();
		}
		ch = buf_shift(&tty.cooked_inbuf);
		if(ch != -1){
			str[cnt++] = ch;
		}
	}
}





void init_tty()
{
	out_byte(0x3D4, 0xC);
	out_byte(0x3D5, 0);

	out_byte(0x3D4, 0xD);
	out_byte(0x3D5, 0);

	tty.row = 0;
	tty.col = 0;


	buf_init(&tty.in_buf);
	buf_init(&tty.out_buf);
	buf_init(&tty.cooked_inbuf);
	create_task(tty_task);

	/* keyboard interrupt */
	out_byte(0x21, in_byte(0x21)&~0x02);
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




void tty_task()
{
	printk("tty task start\n");

	int i;
	char ch;
	struct tty_buf *inbuf, *cooked_inbuf, *outbuf;

	inbuf = &tty.in_buf;
	cooked_inbuf = &tty.cooked_inbuf;
	outbuf = &tty.out_buf;

	while(1){
	/* we simply go to sleep when all the buffer is not in state */
		while(buf_empty(inbuf) && buf_empty(outbuf)){

			schedule();
		}

		/* write the tty out buffer to screen */
		if(!buf_empty(outbuf)){
			tty_write_char(buf_shift(outbuf));
		}
		
		/* read in buffer to cooked buf
		 * if the cooked buffe is full we just
		 * drop the char
		 */


		while(!buf_empty(inbuf)){
			ch = buf_shift(inbuf);
			if(ch == '\b' && !buf_empty(cooked_inbuf)){
				buf_tail_back(cooked_inbuf);
				continue;
			}
			buf_push(cooked_inbuf, ch);
		}



	}
}

int buf_empty(struct tty_buf *buf)
{
	return buf->cnt <= 0;
}
			
int buf_full(struct tty_buf *buf)
{
	return buf->cnt >= TTY_BUF_CNT;
}

void buf_init(struct tty_buf *buf)
{
	buf->head = buf->tail = 0;
	buf->cnt = 0;
}

char buf_shift(struct tty_buf *buf)
{
	int i = buf->head;
	if(buf->cnt < 0){
		return -1;
	}
	buf->head = (i + 1) % TTY_BUF_CNT;	
	buf->cnt--;

	return buf->data[i];
}

int buf_push(struct tty_buf *buf, char ch)
{
	int i = buf->tail; 
	
	if(buf->cnt >= TTY_BUF_CNT)
		return 0;	

	buf->data[i] = ch;
	buf->tail = (i + 1) % TTY_BUF_CNT;
	buf->cnt++;
	return 1;
}

int buf_tail_back(struct tty_buf *buf)
{
	if(buf->cnt <= 0){
		return 0;
	}else{
		buf->tail--;
		buf->cnt--;
	}
	return 1;
}
		
	
	
