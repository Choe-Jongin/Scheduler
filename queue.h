#pragma once
#include "process.h"

//알고리즘 번호
#define FIFO	0				
#define SJF		1
#define PRIO	2
#define RM		3
#define EDF		4

//Node, 연결리스트 방식의 노드임
typedef struct element
{
	struct element * prev;
	struct element * next;
	Process * data;
	int priority;			//노드의 우선순위, 오름차순 이기에 수치가 낮을 수록 우선순위가 높음
}Node;

//알고리즘 별 우선순위 값을 찾아줌
int getPriority( Process * proc, int pp )
{
	if( proc == NULL )
			return 0;

 	switch( pp )
	{
	case FIFO:	//도착시간 순 (FIFO)
		return proc->arrivaltime;
	case SJF:	//짧은 작업  순(SJF)
		return (int)proc->bursttime;
	case PRIO:	//우선순위 순(PRIO)
		return (int)proc->deadline;
	case RM:	//데드라인 적은 순(RM)
		return (int)proc->deadline;
	case EDF:	//현재 시점 마감 임박 순(EDF)
		return (int)proc->deadline;
	}

	return 0;
}

//새 노드
Node * newNode(Process * proc, int pp)
{
	Node * newnode		= (Node*)malloc(sizeof(Node));
	newnode->prev 		= NULL;
	newnode->next 		= NULL;
	newnode->data 		= proc;
	newnode->priority	= getPriority(proc, pp);	
	return newnode;
}


//Queue, 연결 리스트 방식의 큐임(실제 리눅스에선 RB트리를 사용한다고 함)
typedef struct runqueue
{
	int size;
	Node * head;
	Node * tail;
	int pp; // priority policy 우선순위 정책임
	Node * endnode; //더미노드(항상 맨 뒤에 있음)
}Queue;

//새 큐
Queue * newDefaultQueue()
{
	Queue * newq = (Queue*)malloc(sizeof(Queue));
	newq->endnode = newNode(NULL, 0); //더미노드 초기화 
	
	newq->size = 0;
	newq->head = newq->endnode;		//아무것도 없을 땡 더미노드가 헤드임
	newq->tail = newq->endnode;		//더미노드는 항상 꼬리임
	newq->pp = FIFO;				//기본적으론 선입선출(FIFO)임 

	return newq;
}
//우선 순위를 가지고 새 큐를 만들고 싶을 때
Queue * newQueue(int pp)
{
	Queue * newq = newDefaultQueue();
	newq->pp = pp;

	return newq;
}

//해당 큐가 선점형인지 비 선점형인지 반환 0:비선점, 1:선점
int isPreemptive(Queue * queue)
{
	if( queue->pp == RM || queue->pp == EDF)
		return 1;
	return 0;
}

//큐에 삽입하고 몇 번째에 삽입 했는지 반환
int insertNode( Queue * queue, Node * node )
{
	//큐 알고리즘의 맞게 노드의 우선순위를 다시 설정함
	node->priority = getPriority(node->data, queue->pp);

	//몇번째 노드인지 새는 변수
	int index = 0;
	
	//큐가 비었으면 헤드에 넣음
	if( queue->head == queue->endnode )
	{
		queue->head = node;
		queue->head->next = queue->tail;
		queue->tail->prev = queue->head;
		index == 0;
	}
	//새로 들어온 노드가 제일 우선 순위가 높으면 헤드에 넣음(선점형이거나 헤처리도중이 아닐때만, 즉 비선점형이면 헤드가 RUN이 아닐때만)
	else if( node->priority <= queue->head->priority && ( isPreemptive(queue) == 1 || queue->head->data->state != TASK_RUN ) ) 
	{
		node->next = queue->head;
		queue->head->prev = node;
		queue->head = node;
		index == 0;
	}
	//큐가 비어있지 않으면
	else
	{
		//순차 탐색
		Node * it = queue->head;
		for( it = queue->head ; it != queue->endnode ; it = it->next )
		{
			index++;
			//순차 탐색 중 새 노드보다 우선순위가 낮은 기존의 노드 발견시(선점형이거나 태스크가 처리중이 아닐때만)
			if( node->priority < it->priority && ( isPreemptive(queue) == 1 || it->data->state != TASK_RUN ) )
			{
				//기존 노드 앞에 삽입 함
				node -> prev = it->prev;
				node -> next = it;
				it -> prev->next = node;
				it -> prev = node;
				break;
			}
		}
		//우선순위가 더 낮은 기존 노드를 찾지 못했으면 맨뒤(더미노드 앞)에 삽입
		if( it == queue->endnode )
		{
			it = it->prev;
			it->next = node;
			node->prev = it;
			node->next = queue->tail;
			queue->tail->prev = node;
			index++;
		}
			
	}

	queue->size++;
	return index;
}
//Process만으로 노드를 새로 만들어서 큐에 삽입
int insertNewNode( Queue * queue, Process * proc )
{
	return insertNode(queue, newNode(proc, queue->pp));
}

//큐에서 맨 앞 노드 추출(삭제)
Node * popQueue( Queue * queue)
{
	//큐가 비었으면 
	if( queue->head <= queue->endnode )
			return NULL;

	Node * tempnode = queue->head;
	queue->head = queue->head->next;
	queue->head->prev = NULL;
	tempnode->prev = NULL;
	tempnode->next = NULL;

	queue->size--;

	return tempnode;
}

//소멸자
void destroyQueue(Queue * queue)
{	
	while( queue->head != queue->endnode )
	{
		Node * node = popQueue(queue);
		free(node->data);
		free(node);
	}
}



//개발 단계 디버깅용 이젠 쓸 순 없지만 혹시 몰라서 남겨 둠
//모든 노드 출력 
void printQueue( Queue * queue )
{
	printf("size:%d\n", queue->size);
	printf("head:%p, tail:%p, end:%p\n", queue->head, queue->tail, queue->endnode);
	int index = 0;
	for( Node * it = queue->head ; it != queue->endnode ; it = it->next )
	{
		printf("%03d. prev:%p, this:%p, next:%p\n", index++,it->prev, it, it->next);
	}
}
//모든 노드 내용 출력
void printNodeData( Queue * queue )
{
	Process * data;
	for( Node * it = queue->head ; it != queue->endnode ; it = it->next )
	{
		data = it -> data;
		printf("[%s]arr:%d, burst:%d, deadline:%d, remain:%d, state:%d\n",
			data->name, (int)data->arrivaltime, (int)data->bursttime,  (int)data->deadline,  (int)data->remaintime, data->state);
	}
}

