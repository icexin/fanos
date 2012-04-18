#include <schedule.h>
#include <mem.h>
#include <system.h>
#include <fs.h>
#include <elf.h>



int find_empty_process()
{
	int i;
	for(i=0; i<NR_TASK; i++){
		if(!task[i])
			return i;
	}
}

int sys_fork(int ebx, int ecx, int edx, int ebp, int edi, int esi, \
	     int fs, int es, int ds, int eip, int cs, int eflags, int esp, int ss)
{
	int pid;
	struct task_t *t;
	pid = find_empty_process();
	t = get_free_page();
	*t = *current_task;


	current_task->tss.eax=pid;

	t->ticks = t->priority = current_task->priority;

	t->start_addr = (int)alloc_process_mem();
	t->pid = pid;
	t->tss.esp0 = (int)t + PAGE_SIZE;
	t->tss.eax = 0;
	t->tss.ebx = ebx;
	t->tss.ecx = ecx;
	t->tss.edx = edx;
	t->tss.esi = esi;
	t->tss.edi = edi;
	t->tss.fs = fs & 0xFFFF;
	t->tss.es = es & 0xFFFF;
	t->tss.ds = ds & 0xFFFF;
	t->tss.cs = cs & 0xFFFF;
	t->tss.ss = ss & 0xFFFF;
	t->tss.eflags = eflags;
	t->tss.esp = esp;
	t->tss.eip = eip;
	t->tss.ldt = _LDT(pid);

	memcpy(t->start_addr, current_task->start_addr, TASK_SIZE);
	
	create_gdt_desc(gdt+FIRST_TSS+pid*2, (unsigned int)&t->tss, sizeof(*t)-1, DA_386TSS);

	create_ldt_desc(t->ldt+1, t->start_addr, 0x1FF, DA_DPL3|DA_32|DA_CR|DA_LIMIT_4K);
	create_ldt_desc(t->ldt+2, t->start_addr, 0x1FF, DA_DPL3|DA_32|DA_DRW|DA_LIMIT_4K);
	create_gdt_desc(gdt+FIRST_LDT+pid*2, (unsigned int)t->ldt, sizeof(struct desc)*3-1, DA_LDT);

	task[pid] = t;
}

char file_buf[10*PAGE_SIZE];

int sys_exec(char *name)
{
	char tmp[128];
	struct inode *node;

	get_fs_str(tmp, name);
	node = fs_open(tmp);
	int len = fs_read(node, file_buf);

	int i=0;
	Elf32_Ehdr *elf_hdr = (Elf32_Ehdr*)file_buf;


	char *offset = current_task->start_addr + elf_hdr->e_entry;

	for(i=0; i<elf_hdr->e_phnum; i++){
		Elf32_Phdr *prog_hdr = (Elf32_Phdr*)(file_buf + \
				elf_hdr->e_phoff + i * elf_hdr->e_phentsize);
		if(prog_hdr->p_type == PT_LOAD){
			memcpy(offset, file_buf+prog_hdr->p_offset,\
				prog_hdr->p_filesz);
			printk("size:%d\n", prog_hdr->p_filesz);
			offset += prog_hdr->p_filesz;
		}
	}
	unsigned int *stack = (unsigned int*)&name;
	stack[9] = elf_hdr->e_entry;
	stack[12] = TASK_SIZE;

	return 0;
}

void sys_exit(int status)
{
	free_page(current_task->tss.esp0);
	free_process_mem(current_task->start_addr);
	task[current_task->pid] = 0;
	schedule();
	return 0;
}
	

int sys_pid()
{
	return current_task->pid;
}
