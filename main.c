#include <stdio.h>
#include <time.h>

#include "queue.h"

#define READ_BUFF_SIZE 256

Queue * g_processlist;	//모든 프로세서의 목록(아직 스케줄링 큐에 올라가진 않음)
int main(int argc, char *argv[] )
{
	g_processlist = newQueue();
	
	//file read
	FILE * file = fopen("process.txt","r");
	int i = 0 ;
    while (feof(file) == 0) {
        char str[READ_BUFF_SIZE];
        fgets(str, READ_BUFF_SIZE, file);
    	char name[64];
		int arr, burst, dead;
		fscanf(file, "%s %d %d %d",name, &arr, &burst, &dead);
		insertQueue(g_processlist, newNode(newProcess(name,arr,burst,dead)));
	}

	printQueue(g_processlist); 		//노드 주소 확인
	printNodeData(g_processlist);	//노드들의 데이터 확인 
	
	

}

void Run()
{



}
