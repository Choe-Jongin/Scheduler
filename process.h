#pragma once
#include <stdlib.h>
#include <string.h>

typedef struct task{

	char name[128];
	int arrivaltime;
	int bursttime;
	int deadline;
	int remaintime;
	int state; // -1: not yet inserted th run queue, 0: waiting, 1:running, 2:finished

}Process;

//process 생성 
Process * newProcess(char * name, int arr, int burst, int dead)
{
	Process * proc = (Process*)malloc(sizeof(Process));

	strcpy(proc->name, name);
	proc->arrivaltime 	= arr;
	proc->bursttime 	= burst;
	proc->deadline		= dead;
	proc->remaintime	= burst;
	proc->state			= -1;

	return proc;
}

//process 도착 
void run( Process * proc )
{
	proc->remaintime	= proc->bursttime;
	proc->state			= 0;
}

//process 처리 return 0: remain, return 1: finish
int update( Process * proc, int elapse )
{
	proc->state = 1;
	proc->remaintime -= elapse;
	if( proc->remaintime <= 0 )
	{
		proc->state = 2;
	}
}
