#include <schedule.h>
#include <mem.h>
#include <system.h>
#include <fs.h>
#include <elf.h>
#include <debug.h>
#include <kernel.h>



int find_empty_process()
{
	int i;
	for(i=0; i<NR_TASK; i++){
		if(!task[i])
			return i;
	}
	return -1;
}

int sys_fork(int ebx, int ecx, int edx, int ebp, int edi, int esi, \
	     int fs, int es, int ds, int eip, int cs, int eflags, int esp, int ss)
{
	int pid = 0;
	int ppid = 0;
	void* start_addr = 0;
	struct task_t *t;
	pid = find_empty_process();

	assert(pid!=-1);

	t = (struct task_t*)get_free_page();
	start_addr = alloc_process_mem();



	/* the return value of parent process is pid */
	current_task->tss.eax=pid;

	/* the parent process is the current process */
	ppid = current_task->pid;

	/* copy the task struct */
	*t = *current_task;

	t->ticks = t->priority = current_task->priority;

	t->start_addr = (int)start_addr;
	t->pid = pid;
	t->ppid = ppid;
	t->status = RUN;

	t->tss.esp0 = (int)t + PAGE_SIZE;
	t->tss.eax = 0;
	t->tss.ebx = ebx;
	t->tss.ecx = ecx;
	t->tss.edx = edx;
	t->tss.esi = esi;
	t->tss.edi = edi;
	t->tss.ebp = ebp;
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
	
	printk("new process pid=%d,start address=%x, ppid=%d\n", \
		pid, t->start_addr, ppid); 
		
	task[pid] = t;
}

char file_buf[30*1024];

int sys_exec(char *name)
{
	char tmp[128];
	struct inode *node;
	get_fs_str(tmp, name);
	node = fs_open(tmp);
	assert(node!=0);
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
			offset += prog_hdr->p_filesz;
		}
	}
	unsigned int *stack = (unsigned int*)&name;
	stack[9] = elf_hdr->e_entry;
	stack[12] = TASK_SIZE;

	return 0;
}
	

int sys_pid()
{
	return current_task->pid;
}
