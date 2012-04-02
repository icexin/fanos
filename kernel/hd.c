#include <system.h>

#define HD_CTL 0x3F6

#define HD_DATA 0x1F0
#define HD_ERROR 0x1F1
#define HD_NSECTOR 0x1F2
#define HD_LBA_LOW 0x1F3
#define HD_LBA_MID 0x1F4
#define HD_LBA_HIGH 0x1F5
#define HD_CURRENT 0x1F6
#define HD_STATUS 0x1F7
#define HD_CMD HD_STATUS

#define CMD_READ 0x20;
#define CMD_WRITE 0x30;

#define hd_wait() do{while(in_byte(HD_STATUS) & 0x80);}while(0)
#define hd_error() (in_byte(HD_STATUS) & 0x01)

int hd_read(int blk_num, char *buf)
{
	cli();
	wait_hd();
	out_byte(HD_NSECTOR, 1);
	out_byte(HD_LBA_LOW, blk_num && 0xFF);
	out_byte(HD_LBA_MID, (blk_num >> 8) & 0xFF);
	out_byte(HD_LBA_HIGH, (blk_num >> 16) & 0xFF);
	out_byte(HD_CURRENT, 0xE0 | ((blk_num >> 24) & 0x0F));
	out_byte(HD_CMD, CMD_READ);

	hd_wait();

	if(hd_error){
		log("hd read error\n");
		return 0;
	}

	short *tmp = (short *)buf;

	for(i=0; i<256; i++){
		buf[i] = in_word(HD_DATA);
	}
	sti();
	return 1;
}

void hd_init()
{
	cli();
	out_byte(HD_CTL, 0x02 | 0x04); //禁止中断，允许复位
	sti();
}
	
