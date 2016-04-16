#ifndef FANOS_FS_H
#define FANOS_FS_H
#include <fanos/type.h>

#define BLK_SIZE 1024
#define FIRST_ZONE_NUM 7
#define SECOND_ZONE_INDEX 7
#define THIRD_ZONE_INDEX 8


struct super_blk{
	uint16 s_ninodes;
	uint16 s_nzones;
	uint16 s_imap_blocks;
	uint16 s_zmap_blocks;
	uint16 s_firstdatazone;
	uint16 s_log_zone_size;
	uint32 s_max_size;
	uint16 s_magic;
}__attribute__((packed));

struct fs_inode{
	uint16 i_mode;
	uint16 i_uid;
	uint32 i_size;
	uint32 i_mtime;
	uint8  i_gid;
	uint8  i_nlinks;
	uint16 i_zone[9];
}__attribute__((packed));

struct inode{
	uint16 i_mode;
	uint16 i_uid;
	uint32 i_size;
	uint32 i_mtime;
	uint8  i_gid;
	uint8  i_nlinks;
	uint16 i_zone[9];

	uint32 i_num;
}__attribute__((packed));

struct dirent {
	uint16 inode;
	char name[30];
}__attribute__((packed));


extern void* ramfs_start;
extern void* ramfs_end;

int get_inode_buff(struct inode* node, char* buff);
int get_inode_by_path(char* path, struct inode* node);
int mount();

#endif
