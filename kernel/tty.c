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
	tty_read(tmp, len);
	put_fs_str(buf, tmp, len);
}



int tty_write(char *str, int len)
{
	char *p;
	int i;
	for(p=str; p<str+len; p++){
		i = tty.out_buf.tail; 
		tty.out_buf.data[i] = *p;
		tty.out_buf.tail = (i + 1) % TTY_BUF_CNT;
		tty.out_buf.cnt++;
		while(tty.out_buf.cnt >= TTY_BUF_CNT){
			schedule();
		}
	
	}
}

int tty_read(char *str, int len)
{
	int cnt = 0;
	char ch;
	while(cnt < len){
		while(tty.in_buf.cnt <= 0){
			schedule();
		}
		int i = tty.in_buf.head;
		ch = tty.in_buf.data[i];
		if(ch == '\b' && cnt > 0){
			--cnt;
		}else{
			str[cnt++] = ch;
		}
		tty.in_buf.head = (i + 1) % TTY_BUF_CNT;
		tty.in_buf.cnt--;
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

	tty.in_buf.head = tty.in_buf.tail = 0;
	tty.in_buf.cnt = 0;

	tty.out_buf.head = tty.out_buf.tail = 0;
	tty.out_buf.cnt = 0;

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
	static unsigned char i = 0;

	while(1){
		while(tty.out_buf.cnt <= 0){
			schedule();
		}
		int i = tty.out_buf.head;
		char ch = tty.out_buf.data[i];
		tty.out_buf.head = (i + 1) % TTY_BUF_CNT;
		tty.out_buf.cnt--;
		tty_write_char(ch);
	}
}
			
