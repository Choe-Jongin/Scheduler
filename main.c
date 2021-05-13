#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "queue.h"
#include "message.h"
#include "log.h"

#define READ_BUFF_SIZE 256
#define HEIGHT 30
#define WIDTH 160

#define LIST_Y 8
#define LIST_SIZE 20
#define TASK_LIST_X 5
#define RUNQ_X 40
#define MESSAGE_X 115

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
	double termicountdown = 3*1000.0*1000.0;	//처리 후 메인루프가 끝나기 까지

	//시간 측정을 위한 변수들
	struct timeval elap, curr;
	double delta;
    gettimeofday(&elap, NULL);
	
	//태스트 목록 읽기 및 정보 확인
	fileread("process.txt");	
	printf("scheduler start\n");
	
	//결과 로그 파일 
	FILE * logfile = fopen("log.txt","w");

	char * horline = strHorLine(WIDTH); 
	char * title = matchformat(WIDTH,"  TEAM 8 SCHEDULER"); 

	cls();
	invalidRect(tgui,0,0,WIDTH,HEIGHT);		
	//main loop
	while(termicountdown > 0.0)
	{
		//흐른 시간 확인 
   	 	gettimeofday(&curr, NULL);
		delta = (curr.tv_sec + curr.tv_usec / 1000000.0 - elap.tv_sec - elap.tv_usec / 1000000.0)*1000000;
		schedulerTime += delta;
		
		//백 버퍼 초기화
		invalidRect(tgui,3,3,WIDTH,HEIGHT-1);		
		sprintf(tgui->str,"┌%s┐", horline);
		draw(tgui,0,0,tgui->str);
		sprintf(tgui->str,"%s", title);
		draw(tgui,0,1,tgui->str);
		sprintf(tgui->str,"├%s┤", horline);
		draw(tgui,0,2,tgui->str);
		for( int i = 3 ; i < 6 ; i++ )
			draw(tgui, 0, i, matchformat(WIDTH," "));
		sprintf(tgui->str,"├%s┤", horline);
		draw(tgui,0,6,tgui->str);
		for( int i = 7 ; i < HEIGHT-1 ; i++ )
		{
	//		draw(tgui, 0, i, matchformat(WIDTH," "));
			draw(tgui, 0, i, "│");
			draw(tgui, RUNQ_X, i, "│");
			draw(tgui, MESSAGE_X, i, "│");
			draw(tgui, WIDTH, i, "     │");
		}
		sprintf(tgui->str,"└%s┘", horline);
		draw(tgui,0,HEIGHT-1,tgui->str);

		/***********************실직적인 메인************************/

		//실행 정보 표시
		sprintf(tgui->str,"<SCEHDULER STATUS>");
		draw(tgui, 5, 3, tgui->str);
		sprintf(tgui->str,"scheduler excution time : %dms, tick %.3f", (int)schedulerTime/1000, delta);
		draw(tgui, 5, 4, tgui->str);
//		sprintf(tgui->str,"task list %d,  Run Queue %d", g_tasklist->size, rq->size);
//		draw(tgui, 5, 5, tgui->str);
		
		
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
				draw(tgui, 5, 5, tgui->str);
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

		if( rq->size == 0 && g_tasklist->size == 0 && isrunning == 1)
		{
			sprintf(msg->str,"[%dms] All tasks completed", (int)schedulerTime/1000);
			insertMSG();
			isrunning = 0;
		}
		
		//Task list 출력
		sprintf(tgui->str,"TASK LIST : %d", g_tasklist->size);
		draw(tgui,TASK_LIST_X, LIST_Y-1, tgui->str);
		int taski = 0;
		for( Node * it = g_tasklist->head ; it != g_tasklist->endnode && taski < LIST_SIZE; it = it->next )		
		{
			sprintf(tgui->str,"%s arrival:%d", it->data->name, (int)it->data->arrivaltime);
			draw(tgui,TASK_LIST_X, LIST_Y+taski, tgui->str);
			taski++;
		}
		if( g_tasklist->size > LIST_SIZE )
			draw(tgui,TASK_LIST_X, LIST_Y+taski, "MORE...");
		
		//Run Queue 출력
		sprintf(tgui->str,"RUN QUEUE : %d", rq->size);
		draw(tgui,RUNQ_X + 5, LIST_Y-1, tgui->str);
		int rqi = 0;
		for( Node * it = rq->head ; it != rq->endnode && rqi < LIST_SIZE; it = it->next )		
		{
			sprintf(tgui->str,"%s arrival : %d, burst : %d , deadline : %d",
							it->data->name, (int)it->data->arrivaltime, (int)it->data->bursttime, (int)it->data->deadline);
			draw(tgui,RUNQ_X+5, LIST_Y+rqi, tgui->str);
			rqi++;
		}
		if( rq->size > LIST_SIZE )
			draw(tgui,RUNQ_X+5, LIST_Y+rqi, "MORE...");

		//메세지 출력
		MSGUpdate(delta);	//MSG 최신화
		draw(tgui,MESSAGE_X+5, LIST_Y-1, "MESSAGE:");
		for( int i = 0 ; i < msg->size ; i++ )
			draw(tgui,MESSAGE_X+5, LIST_Y+i, msg->msgstr[i]);


		//끝났으면 카운트 다운 시작
		if( isrunning == 0 )
			termicountdown -= delta;
		
		/**************************메인 끝***************************/
		showBackBuff(tgui);
		elap = curr;
	}

	//메모리 헤제
	free(horline);
	free(title);
	
	fclose(logfile);
	destroyTgui(tgui);
	destroyQueue(g_tasklist);
	destroyQueue(rq);
	
	free(g_tasklist);
	free(rq);
}
