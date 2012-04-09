#include <kernel.h>
#include <string.h>
#include <fs.h>

#define BLK_SIZE 1024
#define NR_DEV 10
#define NR_INODE 20

char *ramfs_start, *ramfs_end;
char fsbuf[4*1024];
struct device dev_list[NR_DEV];
struct inode_desc inode_tab[NR_INODE];



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
	//memcpy(buf, ramfs_start + n * BLK_SIZE, BLK_SIZE);
	printf("position:%x\n", n * BLK_SIZE);
	hex_dump(buf, 32);
}


void read_sblk(int dev)
{
	/*skip loader block */
	get_blk(0, fsbuf, 1);
	memcpy((char*)&(dev_list[dev].s_blk), fsbuf, sizeof(struct super_blk));
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
	struct device *pdev = dev_list + dev;
	int ioffset = 2 + pdev->s_blk.s_imap_blocks \
		        + pdev->s_blk.s_zmap_blocks \
			+ (n-1) * sizeof(struct inode) / 1024;
	
	printf("offset=%x\n", ioffset);

	int i;
	for(i=0; i<NR_INODE; i++){
		if(inode_tab[i].n_inode == n){
			printf("Got:tab[%d],n=%d\n", i, n);
			return &(inode_tab[i].inode);
		}
	}

	for(i=0; i<NR_INODE; i++){
		if(!inode_tab[i].n_inode)
			break;
	}
	get_blk(0, buf, ioffset);
	memcpy((char *)&(inode_tab[i].inode), \
		buf + ((n -1) * sizeof(struct inode) % 1024), \
		sizeof(struct inode));	
	inode_tab[i].n_inode = n;

	printf("get inode:%d,inode ref:%d\n", n, inode_tab[i].inode.i_nlinks);
	return &(inode_tab[i].inode);
}

void free_inode(struct inode *node)
{
	int i;
	for(i=0; i<NR_INODE; i++){
		if(&(inode_tab[i].inode) == node){
			inode_tab[i].n_inode = 0;
		}
	}
}

	

void get_zone(char *blk, int n)
{
	get_blk(0, blk, n);
}


int fs_read(struct inode *node, char *buf)
{
	int size = node->i_size;
	int nblk = (size + BLK_SIZE - 1) / BLK_SIZE ;
	int i;
	for(i=0; i<nblk; i++){
		get_zone(buf + i*BLK_SIZE, node->i_zone[i]);
	}
}

int fs_open(char *name)
{
	struct inode *iroot;
	struct dir_entry ent[10];

	iroot = get_inode(0, 1);

	int size = iroot->i_size;
	int nblk = (size + BLK_SIZE - 1) / BLK_SIZE ;

	int i;
	for(i=0; i<nblk; i++){
		get_blk(0, (char*)ent + i*BLK_SIZE, iroot->i_zone[i]);
		printf("zone[%d]=%d\n", i, iroot->i_zone[i]);
	}
/*
	hex_dump((char*)ent, 32);
	for(i=0; i<size/sizeof(ent[0]); i++){

	//	printf("inode:%d, name:%s\n", ent[i].inode, ent[i].name);
		struct inode *tmp;
//		tmp = get_inode(0, ent[i].inode);
	}
	*/
}



