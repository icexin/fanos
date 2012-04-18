#include <kernel.h>
#include <string.h>
#include <fs.h>

#define BLK_SIZE 1024
#define NR_DEV 10
#define NR_INODE 20
#define RAMDEV 0

char *ramfs_start, *ramfs_end;
char fsbuf[10*1024];
struct device dev_list[NR_DEV];
struct inode inode_tab[NR_INODE];



/**
 *获取一个块
 *
 *@param dev 
 *@param buf 存放块数据的缓冲区
 *@param n   块的编号，引导块为0
 */

void get_blk(int dev, char *buf, int n)
{
	assert(ramfs_start + n * BLK_SIZE < ramfs_end);
	memcpy(buf, ramfs_start + n * BLK_SIZE, BLK_SIZE);
}


struct super_blk *get_sblk(int dev)
{
	char buf[BLK_SIZE];
	get_blk(RAMDEV, buf, 1);
	memcpy((char*)&(dev_list[dev].sblk), buf, sizeof(struct super_blk));
	return &(dev_list[dev].sblk);
}

void read_imap()
{
}

void read_zmap()
{
}


struct inode *get_inode(int dev, int n)
{
	char buf[BLK_SIZE];
	assert(n!=0);
	
	struct super_blk *sblk = get_sblk(dev);

	int ioffset = 2 + sblk->s_imap_blocks \
		        + sblk->s_zmap_blocks \
			+ (n-1) * sizeof(struct inode) / 1024;
	
	int i;
	for(i=0; i<NR_INODE; i++){
		if(inode_tab[i].i_num == n){
			return inode_tab + i;
		}
	}

	for(i=0; i<NR_INODE; i++){
		if(!inode_tab[i].i_num)
			break;
	}
	get_blk(dev, buf, ioffset);
	memcpy((char *)(inode_tab + i), \
		buf + ((n -1) * INODE_SIZE % 1024), \
		INODE_SIZE);	
	inode_tab[i].i_num = n;

//	printk("get inode:%d,inode ref:%d\n", n, inode_tab[i].i_nlinks);
	return inode_tab + i;
}

void free_inode(struct inode *node)
{
	int i;
	for(i=0; i<NR_INODE; i++){
		if(inode_tab + i == node){
			inode_tab[i].i_num = 0;
		}
	}
}

	

void get_zone(int dev, char *blk, int n)
{
	get_blk(dev, blk, n);
}


int fs_read(struct inode *node, char *buf)
{
	int size = node->i_size;
	int nblk = (size + BLK_SIZE - 1) / BLK_SIZE ;
	int i;
	for(i=0; i<nblk; i++){
		get_zone(RAMDEV, fsbuf + i*BLK_SIZE, node->i_zone[i]);
	}
	memcpy(buf, fsbuf, size);
	return size;
}

int fs_open(char *name)
{
	struct inode *iroot;
	struct dir_entry *ent;
	char buf[BLK_SIZE];

	iroot = get_inode(0, 1);

	int size = iroot->i_size;
	int nblk = (size + BLK_SIZE - 1) / BLK_SIZE ;

	int i;
	for(i=0; i<nblk; i++){
		get_blk(RAMDEV, buf + i*BLK_SIZE, iroot->i_zone[i]);
		//printf("zone[%d]=%d\n", i, iroot->i_zone[i]);
	}
	
	ent = (struct dir_entry *)buf;

	for(i=0; i<size/sizeof(ent[0]); i++){
//		printk("name=%s, inode=%d\n", ent[i].name, ent[i].inode);
		if(!strcmp(ent[i].name, name)){
			return get_inode(RAMDEV, ent[i].inode);
		}
	}
}



