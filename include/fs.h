struct super_blk{
	short s_ninodes;
	short s_nzones;
	short s_imap_blocks;
	short s_zmap_blocks;
	short s_firstdatazone;
	short s_log_zone_size;
	long s_max_size;
	short s_magic;
}__attribute__((packed));

struct inode{
	short i_mode;
	short i_uid;
	long i_size;
	long i_mtime;
	char  i_gid;
	char  i_nlinks;
	short i_zone[9];

	unsigned int i_num;
}__attribute__((packed));

#define INODE_SIZE 32

struct dir_entry{
	unsigned short inode;
	char name[30];
}__attribute__((packed));

struct device{
	struct super_blk sblk;
};

struct device dev_list[];
char *ramfs_start, *ramfs_end;

void read_sblk(int dev);
struct inode  *get_inode(int dev, int n);
void list_root_dir(struct inode *node);
int fs_open(char *name);
