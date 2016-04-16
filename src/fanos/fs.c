#include <fanos/fs.h>
#include <fanos/string.h>
#include <fanos/fmt.h>
#include <fanos/system.h>

const uint32 boot_blk_num = 0;
const uint32 super_blk_num = 1;
static char fsbuf[30*1024];

void* ramfs_start = NULL;
void* ramfs_end = NULL;
struct super_blk sblk;
struct inode iroot;

int inode_zone_start_num()
{
	return super_blk_num + sblk.s_imap_blocks + sblk.s_zmap_blocks;
}

int get_blk(uint32 n, void* buf)
{
	assert((uint32)ramfs_start + n * BLK_SIZE < (uint32)ramfs_end);
	memcpy(buf, ramfs_start + n * BLK_SIZE, BLK_SIZE);
	return 0;
}

int get_inode_by_num(uint32 i_num, struct inode* node)
{
	static char buf[BLK_SIZE];
	uint32 iblk = inode_zone_start_num() +
		(i_num * sizeof(struct fs_inode)  + BLK_SIZE - 1) / BLK_SIZE;
	uint32 ioffset = (i_num - 1) * sizeof(struct fs_inode) % BLK_SIZE;

	get_blk(iblk, buf);
	memcpy((void*)node, buf + ioffset, sizeof(struct fs_inode));
	node->i_num = i_num;
	return 0;
}

int get_inode_second_zone(struct inode* node, uint32 nblk, char* buff)
{
	char zone_buf[BLK_SIZE];
	uint16* zindex = (uint16*)zone_buf;
	get_blk(node->i_zone[SECOND_ZONE_INDEX], zone_buf);

	int i = 0;
	for (i=0; i<nblk; i++) {
		get_blk(zindex[i], buff+i*BLK_SIZE);
	}
	return 0;
}

int get_inode_first_zone(struct inode* node, uint32 nblk, char* buff)
{
	int i = 0;
	for (i=0; i<nblk; i++) {
		get_blk(node->i_zone[i], buff+i*BLK_SIZE);
	}
	return 0;
}

int get_inode_buff(struct inode* node, char* buff)
{
	uint32 nblk = (node->i_size + BLK_SIZE - 1) / BLK_SIZE;

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
	if (path == NULL || *path == 0 || *path != '/') {
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
	char buf[BLK_SIZE];
	get_blk(super_blk_num, buf);
	memcpy((void*)&sblk, buf, sizeof(struct super_blk));

	get_inode_by_num(1, &iroot);
	return 0;
}

int unmount()
{
	return 0;
}

int sys_open(char* name, char* buff)
{
	char file_name[32] = "";
	struct inode node;
	get_fs_str(file_name, name);
	if (get_inode_by_path(file_name, &node) != 0) {
		return -1;
	}
	int len = get_inode_buff(&node, fsbuf);
	put_fs_buff(buff, fsbuf, len);
	return len;
}
