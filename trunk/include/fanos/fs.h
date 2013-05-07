#ifndef _FS_H
#define _FS_H
#include <stdint.h>

#define BLK_SIZE 1024
#define FIRST_ZONE_NUM 7
#define SECOND_ZONE_INDEX 7
#define THIRD_ZONE_INDEX 8


struct super_blk{
	uint16_t s_ninodes;
	uint16_t s_nzones;
	uint16_t s_imap_blocks;
	uint16_t s_zmap_blocks;
	uint16_t s_firstdatazone;
	uint16_t s_log_zone_size;
	uint32_t s_max_size;
	uint16_t s_magic;
}__attribute__((packed));

struct fs_inode{
	uint16_t i_mode;
	uint16_t i_uid;
	uint32_t i_size;
	uint32_t i_mtime;
	uint8_t  i_gid;
	uint8_t  i_nlinks;
	uint16_t i_zone[9];
}__attribute__((packed));

struct inode{
	uint16_t i_mode;
	uint16_t i_uid;
	uint32_t i_size;
	uint32_t i_mtime;
	uint8_t  i_gid;
	uint8_t  i_nlinks;
	uint16_t i_zone[9];

	uint32_t i_num;
}__attribute__((packed));

struct dirent {
	uint16_t inode;
	char name[30];
}__attribute__((packed));


extern void* ramfs_start;
extern void* ramfs_end;

int get_inode_buff(struct inode* node, char* buff);
int get_inode_by_path(char* path, struct inode* node);
int mount();

#endif
