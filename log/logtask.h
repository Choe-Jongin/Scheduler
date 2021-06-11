#pragma once
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>


//시간정보의 구조체 기존의 int에서 정확도를 위해 double로 바꿈
#define TIMETYPE double

typedef struct _time{
	TIMETYPE beg;
	TIMETYPE end;
	struct _time * prev;
	struct _time * next;
}Time;

Time * newTime(TIMETYPE beg, TIMETYPE end)
{
	Time * time = (Time*)malloc(sizeof(Time));
	time->beg = beg;
	time->end = end;
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
		list->head->next = list->tail;
		list->tail->prev = list->head;
	}
	else
	{
		time->prev = list->tail->next;
		list->tail->next = time;
	}

	list->size++;
}
void setLastEndTime(Timelist * list, TIMETYPE end)
{
	if( list -> size == 0 )
		return ;

	list->tail->end = end;
}

//프로세서(태스크) 구조체
typedef struct Logask_struct{

	char name[128];			//프로세서의 이름
	TIMETYPE arrivaltime;	//도착시간
	TIMETYPE deadline;		//데드라인
	TIMETYPE realdeadline;	//실제 데드라인(스케줄러 시간으로 언제 끝나야 하는지)
	Timelist * timelist;
}Logtask;

//process 생성 
Logtask * newLogtask(char * name, TIMETYPE arr, TIMETYPE dead)
{
	Logtask * task = (Logtask*)malloc(sizeof(Logtask));

	strcpy(task->name, name);
	task->arrivaltime 	= arr;
	task->deadline		= dead;
	task->realdeadline	= arr + dead;
	task->timelist = newTimelist();
	return task;
}

void destroyLogtask( Logtask * task)
{
	Time * next;
	for( Time * time = task->timelist->head ; time != NULL ; time = next)
	{
		next = time->next;
		free(time);
	}
	free(task->timelist);
}