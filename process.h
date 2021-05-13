#pragma once
#include <stdlib.h>
#include <string.h>

#define TIMETYPE double

typedef struct task_struct{

	char name[128];
	TIMETYPE arrivaltime;
	TIMETYPE bursttime;
	TIMETYPE deadline;
	TIMETYPE realdeadline;
	TIMETYPE remaintime;
	int state; // -1: not yet inserted the rq, 0: waiting, 1:running, 2:stoped, 3:finish

}Process;

//process 생성 
Process * newProcess(char * name, TIMETYPE arr, TIMETYPE burst, TIMETYPE dead)
{
	Process * proc = (Process*)malloc(sizeof(Process));

	strcpy(proc->name, name);
	proc->arrivaltime 	= arr;
	proc->bursttime 	= burst;
	proc->deadline		= dead;
	proc->realdeadline	= arr + dead;
	proc->remaintime	= burst;
	proc->state			= -1;

	return proc;
}

//process 도착 
void taskrun( Process * proc )
{
	proc->remaintime	= proc->bursttime;
	proc->state			= 0;
}

//process 처리, 소모한 시간을 돌려줌
TIMETYPE taskupdate( Process * proc, TIMETYPE delta )
{
	proc->state = 1;
	proc->remaintime -= delta;
	if( proc->remaintime <= 0 )
	{
		proc->state = 3;
		return delta + proc->remaintime;
	}
	//모든 시간을 다 씀
	return delta;
}
//process 중지
void taskstop( Process * proc )
{
	proc->state = 2;
}
