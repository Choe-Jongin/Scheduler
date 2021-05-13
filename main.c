#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "queue.h"
#include "message.h"
#include "log.h"

#define READ_BUFF_SIZE 256
#define HEIGHT 30
#define WIDTH 160

#define FIFO	0
#define RM 		1

Queue * g_tasklist;	//모든 프로세서의 목록(아직 스케줄링 큐에 올라가진 않음)
TGUI * tgui;		//텍스트만으로 GUI처럼 화면에 표시하기 위한 구조체

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
		insertNewNode(g_tasklist, newProcess(name,(double)arr,(double)burst*50,(double)dead));
	}
	fclose(file);
}
// │ {str} blank │
char *matchformat(int w, char *str)
{
    int len = strlen(str);
	for( int i = 0 ; i < len ; i++ )
		if( str[i] == '\n' || str[i] == '\r' )
			str[i] = ' ';

    char *matchedstr = (char *)malloc(w * CHSIZE + 1);
    memset(matchedstr, 0, w * CHSIZE + 1);

    strcat(matchedstr, "│");
    if (len > w - 2)
        return str;

    strcat(matchedstr, str);
    for (int i = 0; i < w - len - 2; i++)
    {
        strcat(matchedstr, " ");
    }

    strcat(matchedstr, "│");
    return matchedstr;
}
char *strHorLine(int w)
{
    char *line = (char *)malloc(w * CHSIZE + 1);
    memset(line, 0, w * CHSIZE);

    for (int i = 0; i < w - 2; i++)
        strcat(line, "─");

    return line;
}

int main(int argc, char *argv[] )
{
	//Run Queue
	Queue * rq = newQueue(RM);
	tgui = newTGUI(HEIGHT,WIDTH);
	msg = newMSG();

	int isrunning = 1;			//메인루프 동작 여부 결정
	double schedulerTime = 0.0;	//스케줄러의 시간 0~끝날때까지 ms단위

	//시간 측정을 위한 변수들
	struct timeval elap, curr;
	double delta;
    gettimeofday(&elap, NULL);
	
	//태스트 목록 읽기 및 정보 확인
	fileread("process.txt");	
	printf("scheduler start\n");
	
	//결과 로그 파일 
	FILE * logfile = fopen("log.txt","w");

	cls();
	//main loop
	while(isrunning)
	{
		//흐른 시간 확인 
   	 	gettimeofday(&curr, NULL);
		delta = (curr.tv_sec + curr.tv_usec / 1000000.0 - elap.tv_sec - elap.tv_usec / 1000000.0)*1000000;
		schedulerTime += delta;
		invalidRect(tgui,0,0,WIDTH,HEIGHT);
		/***********************실직적인 메인************************/

		//실행 정보 표시
		sprintf(tgui->str,"scheduler excution time : %dms, tick %.3f", (int)schedulerTime/1000, delta);
		draw(tgui, 0, 0, tgui->str);
		sprintf(tgui->str,"task list %d,  Run Queue %d", g_tasklist->size, rq->size);
		draw(tgui, 0, 1, tgui->str);

		//task 도착확인 
		while(g_tasklist->head != g_tasklist->endnode)
		{
			//도착시 리스트에서 Pop하고 런큐에 삽입 
			if( g_tasklist->head->data->arrivaltime <= (int)schedulerTime )
			{
				Node * node = popQueue(g_tasklist);
				taskrun(node->data); // -> wating
				int index = insertNode(rq, node);
				sprintf(msg->str,"[%dms] %s arrive", (int)schedulerTime/1000, node->data->name);
				insertMSG();
				fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, node->data->name, (int)node->data->arrivaltime, LOG_ARRIV);

				//우선순위가 가장 높은게 추가 되었을 경우
				if( index == 0 )
				{
					//기존의 최우선 노드가 있고 그것이 처리중이였으면
					if( node->next != rq->endnode && node->next->data->state == TASK_RUN)
					{ 	
						//상태를 스톱으로 바꿈
						node->next->data->state = TASK_STOP;
						fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, node->next->data->name, (int)node->data->realdeadline, LOG_STOP);
						sprintf(msg->str,"[%dms] %s is stop", (int)schedulerTime/1000, node->next->data->name);
						insertMSG();
					}
				}
			}else break;
		}
		
		//실제 처리 중인 노드
		TIMETYPE remaindelta = delta;
		Node * currnode = NULL;
	
		while( remaindelta > 0.0 && rq->size > 0 )
		{
			currnode = popQueue(rq);
			Process * proc = currnode->data;
			//wait이였으면(즉 처음으로 처리단계에 들어왔으면)
			if( proc->state == TASK_WAIT )
			{

				sprintf(msg->str,"[%dms] %s start processing", (int)schedulerTime/1000, proc->name);
				insertMSG();
				fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, proc->name, (int)proc->realdeadline, LOG_START);
			}
			//STOP상태였으면
			else if( proc->state == TASK_STOP )
			{
				sprintf(msg->str,"[%dms] %s continue processing", (int)schedulerTime/1000, proc->name);
				insertMSG();
				fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, proc->name, (int)proc->realdeadline, LOG_CONT);
			}
			
			proc->state = 1;
			TIMETYPE tempremain = remaindelta;
			remaindelta = remaindelta - taskupdate(proc, remaindelta);
			if( proc->state != TASK_FINISH ){
				sprintf(tgui->str,"[%dms] %s spend %.3fus, remain Execution Time is %.3f", 
							(int)schedulerTime/1000, proc->name, tempremain, proc->remaintime);
				draw(tgui, 0, 2, tgui->str);
				insertNode(rq,currnode);
			}
			else{
				sprintf(msg->str,"[%dms] %s finish", (int)schedulerTime/1000, proc->name);
				insertMSG();
				fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, proc->name, (int)proc->realdeadline, LOG_FINISH);

				free(currnode->data);
				free(currnode);
			}
		}

		if( rq->size == 0 && g_tasklist->size == 0 )
		{
			insertStrMSG("All task is done\n");
			isrunning = 0;
		}

		//메세지 출력
		MSGUpdate(delta);	//MSG 최신화
		sprintf(tgui->str,"┌%s┐", strHorLine(MAX_MSG_WIDTH));
		draw(tgui,40,5,tgui->str);
		sprintf(tgui->str,"%s", matchformat(MAX_MSG_WIDTH,"<MESSAGE>"));
		draw(tgui,40,6,tgui->str);
		for( int i = 0 ; i < msg->size ; i++ )
		{
			sprintf(tgui->str,"%s ", matchformat(MAX_MSG_WIDTH,msg->msgstr[i]));
			draw(tgui,40,7+i,tgui->str);
		}
        sprintf(tgui->str,"└%s┘ ", strHorLine(MAX_MSG_WIDTH));
		draw(tgui,40,7+msg->size,tgui->str);
	
		
		/**************************메인 끝***************************/
		showBackBuff(tgui);
		elap = curr;
	}

	//실행 정보 표시
	sprintf(tgui->str,"scheduler excution time : %dms, tick %.3f", (int)schedulerTime/1000, delta);
	draw(tgui, 0, 0, tgui->str);
	sprintf(tgui->str,"task list %d,  Run Queue %d", g_tasklist->size, rq->size);
	draw(tgui, 0, 1, tgui->str);
	showBackBuff(tgui);

	//메모리 헤제
	fclose(logfile);
	destroyTgui(tgui);
	destroyQueue(g_tasklist);
	destroyQueue(rq);
	
	free(g_tasklist);
	free(rq);
}
