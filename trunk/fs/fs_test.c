#include "../include/fanos/fs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

const uint32_t boot_blk_num = 0;
const uint32_t super_blk_num = 1;
static char fsbuf[30*1024];

void* ramfs_start = NULL;
void* ramfs_end = NULL;
struct super_blk sblk;
struct inode iroot;

int inode_zone_start_num()
{
	return super_blk_num + sblk.s_imap_blocks + sblk.s_zmap_blocks;
}

int get_blk(uint32_t n, void* buf)
{
	assert((uint32_t)ramfs_start + n * BLK_SIZE < (uint32_t)ramfs_end);
	memcpy(buf, ramfs_start + n * BLK_SIZE, BLK_SIZE);
	return 0;
}

int get_inode_by_num(uint32_t i_num, struct inode* node)
{
	static char buf[BLK_SIZE];
	uint32_t iblk = inode_zone_start_num() + 
		(i_num * sizeof(struct fs_inode)  + BLK_SIZE - 1) / BLK_SIZE;
	uint32_t ioffset = (i_num - 1) * sizeof(struct fs_inode) % BLK_SIZE;
	
	get_blk(iblk, buf);
	memcpy((void*)node, buf + ioffset, sizeof(struct fs_inode));
	node->i_num = i_num;
	return 0;
}

int get_inode_second_zone(struct inode* node, size_t nblk, char* buff)
{
	char zone_buf[BLK_SIZE];
	uint16_t* zindex = (uint16_t*)zone_buf;
	get_blk(node->i_zone[SECOND_ZONE_INDEX], zone_buf);	
	
	int i = 0;
	for (i=0; i<nblk; i++) {
		get_blk(zindex[i], buff+i*BLK_SIZE);
	}
	return 0;
}

int get_inode_first_zone(struct inode* node, size_t nblk, char* buff)
{
	int i = 0;
	for (i=0; i<nblk; i++) {
		get_blk(node->i_zone[i], buff+i*BLK_SIZE);
	}
	return 0;
}

int get_inode_buff(struct inode* node, char* buff)
{
	uint32_t nblk = (node->i_size + BLK_SIZE - 1) / BLK_SIZE;

	if (nblk <= FIRST_ZONE_NUM) {
		get_inode_first_zone(node, nblk, buff);
	} else {
		get_inode_first_zone(node, FIRST_ZONE_NUM, buff);
		get_inode_second_zone(node, nblk-FIRST_ZONE_NUM, 
			buff+FIRST_ZONE_NUM*BLK_SIZE);
	}

	return node->i_size;
}

int get_inode_by_path(char* path, struct inode* node)
{
	if (path == NULL) {
		return 0;
	}
	if (strcmp(path, "/") == 0) {
		*node = iroot;
		return 0;
	}

	char* name = path + 1;
	struct dirent* ent = (struct dirent*)fsbuf;

	get_inode_buff(&iroot, fsbuf);
	int i = 0;
	for (i=0; i<iroot.i_size / sizeof(*ent); i++) {
		if (strcmp(ent[i].name, name) == 0) {
			get_inode_by_num(ent[i].inode, node);
			return 0;
		}
	}
	return -1;
}


int mount()
{
	char* file_name = "ramdisk";
	FILE* fp = NULL;
	int i = 0;
	ramfs_start = malloc(1024*1024*3);
	fp = fopen(file_name, "r");
	for (i=0; fread(ramfs_start + i, 1024, 1, fp) != 0; i+= 1024);
	ramfs_end = ramfs_start + 1024 * 1024 * 3;
	fclose(fp);
	
	char buf[BLK_SIZE];
	get_blk(super_blk_num, buf);
	memcpy((void*)&sblk, buf, sizeof(struct super_blk));

	get_inode_by_num(1, &iroot);
	return 0;
}

int unmount()
{
	free(ramfs_start);
	return 0;
}

int main()
{
	mount();
	struct inode node;
	if (get_inode_by_path("/tm", &node) == 0) {
		int len = get_inode_buff(&node, fsbuf);
		fwrite(fsbuf, len, 1, stdout);
	}
	unmount();
}
