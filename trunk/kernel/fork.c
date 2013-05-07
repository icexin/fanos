#include <fanos/task.h>
#include <fanos/mem.h>
#include <fanos/system.h>
#include <fanos/fs.h>
#include <fanos/debug.h>
#include <fanos/kernel.h>
#include <elf.h>
#include <string.h>

static uint32_t copy_process_args(struct task_t* t, char* args[]);

int sys_fork(int ebx, int ecx, int edx, int ebp, int edi, int esi, \
	     int gs, int fs, int es, int ds, int eip, int cs, int eflags, int esp, int ss)
{
	pid_t pid = 0;
	pid_t ppid = 0;
	void* start_addr = NULL;
	struct task_t *t = NULL;

	pid = find_empty_process();
	assert(pid!=-1);

	t = (struct task_t*)get_free_page();
	memset((void*)t, 0, PAGE_SIZE);
	start_addr = alloc_process_mem();

	/* the parent process is the current process */
	ppid = current_task->pid;

	/* copy the task struct */
	*t = *current_task;

	t->ticks = t->priority = current_task->priority;

	t->start_addr = start_addr;
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
	t->tss.gs = gs & 0xFFFF;
	t->tss.cs = cs & 0xFFFF;
	t->tss.ss = ss & 0xFFFF;
	t->tss.eflags = eflags;
	t->tss.esp = esp;
	t->tss.eip = eip;
	t->tss.ldt = _LDT(pid);

	memcpy(t->start_addr, current_task->start_addr, TASK_SIZE);
	
	eflags_t eflag;
	disable_hwint(eflag);
	create_gdt_desc(GDT_TSS(pid), (unsigned int)&t->tss, sizeof(*t)-1, DA_386TSS);

	create_ldt_desc(t->ldt, 0, 0, 0);
	create_ldt_desc(t->ldt+1, (uint32_t)t->start_addr, PROCESS_MEM_LEN, DA_DPL3|DA_32|DA_CR|DA_LIMIT_4K);
	create_ldt_desc(t->ldt+2, (uint32_t)t->start_addr, PROCESS_MEM_LEN, DA_DPL3|DA_32|DA_DRW|DA_LIMIT_4K);
	create_gdt_desc(GDT_LDT(pid), (uint32_t)t->ldt, sizeof(struct desc)*3-1, DA_LDT);
	
	log("new proc pid=%d,ppid=%d,saddr=%x, taddr=%x\n", \
		pid, ppid, t->start_addr, (int)t); 
		
	task[pid] = t;
	/* the return value of parent process is pid */
	restore_hwint(eflag);
	return pid;
}

char file_buf[30*1024];

#define EIP 10
#define ESP 13
int sys_exec(char *name, char* args[])
{
	char tmp[128];
	struct inode node;
	get_fs_str(tmp, name);
	int ret = get_inode_by_path(tmp, &node);
	if (ret != 0) {
		return -1;
	}

	get_inode_buff(&node, file_buf);
	
	Elf32_Ehdr *elf_hdr = (Elf32_Ehdr*)file_buf;

	char *offset = current_task->start_addr + elf_hdr->e_entry;

	int i=0;
	for (i=0; i<elf_hdr->e_phnum; i++) {
		Elf32_Phdr *prog_hdr = (Elf32_Phdr*)(file_buf + \
				elf_hdr->e_phoff + i * elf_hdr->e_phentsize);
		if (prog_hdr->p_type == PT_LOAD) {
			memcpy(offset, file_buf+prog_hdr->p_offset,\
				prog_hdr->p_filesz);
			offset += prog_hdr->p_filesz;
		}
	}

	uint32_t new_esp = copy_process_args(current_task, args);

	uint32_t* call_stack = (unsigned int*)&name;
	call_stack[EIP] = elf_hdr->e_entry;
	call_stack[ESP] = new_esp;
	return 0;
}

int sys_pid()
{
	return current_task->pid;
}

uint32_t copy_process_args(struct task_t* t, char* args[])
{
	uint32_t argc = 0;
	char** user_argv = NULL;
	char* user_arg = NULL;
	user_argv = get_user_addr(t, (void*)args);
	while (*user_argv++) argc++;

	uint32_t new_argv_user = TASK_SIZE - (argc + 1) * sizeof(uint32_t*);
	uint32_t new_arg_user = new_argv_user;

	uint32_t* new_argv_kern = get_user_addr(t, (void*)new_argv_user);
	uint32_t* new_arg_kern = NULL;


	new_argv_kern[argc] = 0;

	user_argv = get_user_addr(t, (void*)args);
	int i = 0;
	for (i=argc-1; i>=0; i--) {
		user_arg = get_user_addr(t, (void*)user_argv[i]);
		int arg_len = strlen(user_arg);
		new_arg_user -= (arg_len + 1);
		new_arg_kern = get_user_addr(t, (void*)new_arg_user);

		new_argv_kern[i] = new_arg_user;
		strcpy((char*)new_arg_kern, user_arg);
	}

	uint32_t new_esp_user = new_arg_user - 2 * sizeof(uint32_t*) - 128;
	uint32_t* new_esp_kern = get_user_addr(t, (void*)new_esp_user);
	new_esp_kern[0] = argc;
	new_esp_kern[1] = new_argv_user;

	return new_esp_user;
}
