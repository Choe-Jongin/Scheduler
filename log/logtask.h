#pragma once
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>


//시간정보의 구조체 기존의 int에서 정확도를 위해 double로 바꿈
#define TIMETYPE double

//
#define TO_SEC 1000000
typedef struct _time{
	TIMETYPE beg;
	TIMETYPE end;
	struct _time * prev;
	struct _time * next;
}Time;

Time * newTime(TIMETYPE beg, TIMETYPE end)
{
	Time * time = (Time*)malloc(sizeof(Time));
	time->beg = beg/TO_SEC;
	time->end = end/TO_SEC;
	time->prev = NULL;
	time->next = NULL;
	return time;
}

typedef struct Time_list{
	Time * head;
	Time * tail;
	int size;
}Timelist;

Timelist * newTimelist()
{
	Timelist * list = (Timelist*)malloc(sizeof(Timelist));
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	return list;
}
//큐에 삽입하고 몇 번째에 삽입 했는지 반환
void insertTime( Timelist * list, Time * time )
{
	//큐가 비었으면 헤드에 넣음
	if( list->head == NULL )
	{
		list->head = time;
		list->tail = time;
	}
	else
	{
		time->prev = list->tail;
		list->tail->next = time;
		list->tail = time;
	}

	list->size++;
}
void setLastEndTime(Timelist * list, TIMETYPE end)
{
	if( list->tail == NULL )
		return ;

	list->tail->end = end/TO_SEC;
}

//프로세서(태스크) 구조체
typedef struct Logask_struct{

	char name[128];			//프로세서의 이름
	TIMETYPE arrivaltime;	//도착시간
	TIMETYPE deadline;		//데드라인
	TIMETYPE realdeadline;	//실제 데드라인(스케줄러 시간으로 언제 끝나야 하는지)
	Timelist * timelist;	//
	int state;				//
}Logtask;

//process 생성 
Logtask * newLogtask(char * name, TIMETYPE arr, TIMETYPE dead)
{
	Logtask * task = (Logtask*)malloc(sizeof(Logtask));

	strcpy(task->name, name);
	task->arrivaltime 	= arr/TO_SEC;
	task->deadline		= dead/TO_SEC;
	task->realdeadline	= (arr + dead)/TO_SEC;
	task->timelist		= newTimelist();
	task->state			= 0;
	return task;
}

int isNowArrival( Logtask * task, double now, int col, int ganttunit)
{
	now = now + (double)col/(double)ganttunit;
	if( now <= task->arrivaltime && task->arrivaltime <= now+(double)1/(double)ganttunit )
		return 1;
	return 0;
}
int isNowDeadline( Logtask * task, double now, int col, int ganttunit)
{
	if( task->deadline == 0 )
		return -1;
	now = now + (double)col/(double)ganttunit;
	if( now <= task->realdeadline && task->realdeadline <= now+(double)1/(double)ganttunit )
		return 1;
	return 0;
}
int isNowProcessing( Logtask * task, double now, int col, int ganttunit)
{
	now = now + (double)col/(double)ganttunit;
	for( Time * time = task->timelist->head ; time != NULL ; time = time->next)
	{
		if( now < time->beg )
			break;
		if( time->end < now )
			continue;
		if( time->beg < now && now < time->end )
			return 1;

	}
	return 0;
}
void printLogtaskInfo( Logtask * task )
{
	printf("%-5s arr:%d dead:%d period count:%d\n", task->name, (int)(task->arrivaltime*1000), (int)(task->realdeadline*1000), task->timelist->size);
	for( Time * time = task->timelist->head ; time != NULL ; time = time->next)
	{
		printf("[%d~%d]",(int)(time->beg*1000), (int)(time->end*1000));
	}
	printf("\n");
}
void destroyLogtask( Logtask * task )
{
	Time * next;
	for( Time * time = task->timelist->head ; time != NULL ; time = next)
	{
		next = time->next;
		free(time);
	}
	free(task->timelist);
}
