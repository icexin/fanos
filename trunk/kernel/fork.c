#include <schedule.h>



int find_empty_process()
{
	int i;
	for(i=0; i<NR_TASK; i++){
		if(!task[i])
			return i;
	}
}

int do_fork()
{
}

