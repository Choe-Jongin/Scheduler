#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "queue.h"

#define READ_BUFF_SIZE 256

#define FIFO	0
#define RM 		1

Queue * g_tasklist;	//모든 프로세서의 목록(아직 스케줄링 큐에 올라가진 않음)

//파일 읽기
void fileread(char * filename)
{
	//도착시간순으로 정렬
	g_tasklist = newQueue(FIFO);

	//file read
	FILE * file = fopen(filename,"r");
    while (feof(file) == 0) {
        char str[READ_BUFF_SIZE];
        fgets(str, READ_BUFF_SIZE, file);
    	char name[64];
		int arr, burst, dead;
		fscanf(file, "%s %d %d %d",name, &arr, &burst, &dead);
		insertNewNode(g_tasklist, newProcess(name,(double)arr,(double)burst,(double)dead));
	}
	fclose(file);
}

int main(int argc, char *argv[] )
{
	//Run Queue
	Queue * rq = newQueue(RM);

	int isrunning = 1;			//메인루프 동작 여부 결정
	double schedulerTime = 0.0;	//스케줄러의 시간 0~끝날때까지 ms단위

	//시간 측정을 위한 변수들
	struct timeval elap, curr;
	double delta;
    gettimeofday(&elap, NULL);
	
	//태스트 목록 읽기 및 정보 확인
	fileread("process.txt");	
	printQueue(g_tasklist); 		//노드 주소 확인
	printNodeData(g_tasklist);	//노드들의 데이터 확인 

	
	printf("scheduler start\n");
	
	//main loop
	while(isrunning)
	{
		//흐른 시간 확인 
   	 	gettimeofday(&curr, NULL);
		delta = (curr.tv_sec + curr.tv_usec / 1000000.0 - elap.tv_sec - elap.tv_usec / 1000000.0)*1000000;
		schedulerTime += delta;
//		printf("scheduler time : %d\n",(int)schedulerTime);
		/***********************실직적인 메인************************/
	
		//task 도착확인 
		while(g_tasklist->head != g_tasklist->endnode)
		{
			//도착시 리스트에서 Pop하고 런큐에 삽입 
			if( g_tasklist->head->data->arrivaltime <= (int)schedulerTime )
			{
				Node * node = popQueue(g_tasklist);
				taskrun(node->data); // -> wating
				insertNode(rq, node);
				printf("[%s] arrival, running:%d, left:%d\n", node->data->name, rq->size, g_tasklist->size);
			}else break;
		}
		
		
		//실제 처리 중인 노드
		TIMETYPE remaindelta = delta;
		Node * currnode = NULL;
	
		while( remaindelta > 0.0 && rq->size > 0 )
		{
			currnode = popQueue(rq);
			Process * proc = currnode->data;
			TIMETYPE tempremain = remaindelta;
			remaindelta = remaindelta - taskupdate(proc, remaindelta);
			if( proc->state != 3 ){
				printf("[%s] spend %.3fus, remain Execution Time is %.3f\n", proc->name, tempremain, proc->remaintime);
				insertNode(rq,currnode);
			}
			else{
				printf("[%s] finish, remain %.3fus this phase \n", proc->name, remaindelta);
				free(currnode->data);
				free(currnode);
			}
		}

		if( rq->size == 0 && g_tasklist->size == 0 )
		{
			printf("All task is done\n");
			isrunning = 0;
		}

		/**************************메인 끝***************************/
		elap = curr;
	}

}
