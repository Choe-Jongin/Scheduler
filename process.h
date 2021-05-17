#pragma once
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

//시간정보의 구조체 기존의 int에서 정확도를 위해 double로 바꿈
#define TIMETYPE double

//프로세서의 상태를 나타넴
#define TASK_LOAD	-1	//미도착(즉 파일을 읽은 후 태스크 정보만 가지고 있음
#define TASK_WAIT	0	//도착시간이 되어서 런큐로 들어갔지만 먼저 있던 작업 진행중
#define TASK_RUN	1	//실제 처리중 한번에 1개의 프로세스만 이 상태일 수 있음
#define TASK_STOP	2	//처리 도중 더 우선순위가 높은 프로세스가 와서 선점 당함
#define TASK_FINISH	3	//처리가 완료 됨

//프로세서(태스크) 구조체
typedef struct task_struct{

	char name[128];			//프로세서의 이름
	pid_t	 pid;			//PID 프로세서 아이디, 개체 생성시엔 정해져 있지 않음
	TIMETYPE arrivaltime;	//도착시간
	TIMETYPE bursttime;		//처리시간(WCET)
	TIMETYPE deadline;		//데드라인
	TIMETYPE realdeadline;	//실제 데드라인(스케줄러 시간으로 언제 끝나야 하는지)
	TIMETYPE remaintime;	//남아있는 처리 시간
	int state; 				// 프로세서 상태  -1: not yet inserted the rq, 0: waiting, 1:running, 2:stoped, 3:finish

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
	proc->state			= TASK_LOAD;

	return proc;
}

//process 도착시간이 되서 런큐에 넣기 위한 작업
void taskrun( Process * proc)
{
	proc->remaintime	= proc->bursttime;
	proc->state			= TASK_WAIT;
}

//process 처리함, 소모한 시간을 돌려줌
TIMETYPE taskupdate( Process * proc, TIMETYPE delta )
{
	kill(proc->pid, SIGCONT);
	proc->state = TASK_RUN;
	proc->remaintime -= delta;
	
	//태스크가 작업을 완전히 완료 함
	if( proc->remaintime <= 0 )
	{
		proc->state = TASK_FINISH;
		return delta + proc->remaintime;	// delta는 양수 남은시간은 음수여서 결국 delta보다 작은 시간 반환 
	}
	//모든 시간을 다 씀(처리 할게 남아있음)
	return delta;
}
//process 중지
void taskstop( Process * proc )
{
	proc->state = TASK_STOP;
	kill(proc->pid,SIGSTOP);
}
