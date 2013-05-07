#include <fanos/system.h>
#include <fanos/kernel.h>

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

#define CMD_READ 0x20
#define CMD_WRITE 0x30

#define CTL_DISINT 0x02
#define CTL_RESET  0x04

#define STATUS_BUSY 0x80
#define STATUS_ERR  0x01

#define hd_wait() do{while(in_byte(HD_STATUS) & STATUS_BUSY);}while(0)
#define hd_error() (in_byte(HD_STATUS) & STATUS_ERR)

int hd_read(int blk_num, char *buf)
{
	cli();
	hd_wait();
	log("before read\n");	
	
	out_byte(HD_NSECTOR, 1);
	out_byte(HD_LBA_LOW, blk_num && 0xFF);
	out_byte(HD_LBA_MID, (blk_num >> 8) & 0xFF);
	out_byte(HD_LBA_HIGH, (blk_num >> 16) & 0xFF);
	out_byte(HD_CURRENT, 0xE0 | ((blk_num >> 24) & 0x0F));
	out_byte(HD_CMD, CMD_READ);

	hd_wait();

	log("after read\n");	
	if(hd_error()){
		log("hd read error\n");
		return 0;
	}

	short *tmp = (short *)buf;

	int i;
	for(i=0; i<256; i++){
		tmp[i] = in_word(HD_DATA);
	}
	sti();
	return 1;
}

void hd_reset()
{
	out_byte(HD_CTL, CTL_RESET);mdelay(2);
	int n = 4;
	while(n--){
		log("status:%x\n", in_byte(HD_STATUS) & STATUS_BUSY);
	}
}

void hd_init()
{
	cli();
	hd_reset();
	out_byte(HD_CTL, CTL_DISINT); //禁止中断，允许复位
	sti();
}
	
