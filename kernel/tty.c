#include <tty.h>
#include <string.h>
#include <unistd.h>
#include <system.h>

#define MAX_ROW 25
#define MAX_COL 80

#define screen(row,col) (*(videoram+((row)*MAX_COL+(col))*2))
char* const videoram = (char *const)0xB8000;
static unsigned char pos_row = 0;
static unsigned char pos_col = 0;
extern int rs_write(char *buf, int len);

int sys_write(int fd, char *buf, int len)
{
	static char tmp[512];
	get_fs_str(tmp, buf);

	if(fd == 1)
		return rs_write(tmp, len);
	else
		return tty_write(tmp, len);
	
}


int tty_write(char *str, int len)
{
	int i, j, k;
	for(i=0; i<len; i++){

		if(str[i] == '\n' || pos_col == MAX_COL){
			if(pos_col == MAX_COL){
				i--;
			}
			pos_row++;
			pos_col = 0;
			continue;
		}

		/*如果要写的行超出最大行，则所有行依次上移*/
		if(pos_row >= MAX_ROW){
			for(j=0; j<MAX_ROW-1; j++){
				for(k=0; k<MAX_COL; k++){
					screen(j,k) = screen(j+1,k);
				}
			}

			/*清最后一行*/
			for(j=0; j<MAX_COL; j++){
				screen(MAX_ROW-1, j) = ' ';
			}

			pos_row = MAX_ROW - 1;
			pos_col = 0;
		}

		screen(pos_row, pos_col++) = str[i];
	}

	cli();
	out_byte(0x3D4, 0x0E);
	out_byte(0x3D5, (pos_row * 80 + pos_col) >> 8);

	out_byte(0x3D4, 0x0F);
	out_byte(0x3D5, (pos_row * 80 + pos_col) & 0xFF);

	sti();

}



void init_tty()
{
	out_byte(0x3D4, 0xC);
	out_byte(0x3D5, 0);

	out_byte(0x3D4, 0xD);
	out_byte(0x3D5, 0);
}


void tty_clear()
{
   int i;
   for (i=0;i<80*25;i++) {
       videoram[i*2] = ' ';
   }
}
