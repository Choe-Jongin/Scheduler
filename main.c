#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "queue.h"
#include "message.h"
#include "Gantt/log.h"

//주기적으로 도착하는 태스크를 상정하고 태스크 제네레터를 만들어서 burst시간이 짧음
//그래서 파일을 읽을때 몇 배를 크게 읽어드리기 위해 존재하는 상수
#define EX_BURST 50

#define READ_BUFF_SIZE 256		//파일 읽기 시 한 줄에 몇 글자를 읽는지
#define HEIGHT	30				//최대 높이		LIST_Y + LIST_SIZE + 1 보단 커야 함
#define WIDTH	145				//최대 넓이

#define LIST_Y 8				//큐, 메세지 목록이 시작될 y위치
#define LIST_SIZE 20			//큐, 메세지 등의 최대 표시 갯수
#define TASK_LIST_X 5			//g_tasklist의 노드 표시 x위치
#define RUNQ_X 33				//rq의 노드 표시 x위치
#define MESSAGE_X 105			//메세지의 표시 x위치

//알고리즘 번호
#define FIFO	0				
#define RM		1

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
		insertNewNode(g_tasklist, newProcess(name,(double)arr,(double)burst*EX_BURST,(double)dead));
	}
	fclose(file);
}
// │ {str} blank │ 형태로 문자열 양쪽 끝에 수직선을 하나 넣어줌(중요하지 않음)
char *matchformat(int w, char *str)
{
	int len = strlen(str);
    if (len > w - 2)
        return str;
	
	for( int i = 0 ; i < len ; i++ )
		if( str[i] == '\n' || str[i] == '\r' )
			str[i] = ' ';

    char *matchedstr = (char *)malloc(w * CHSIZE + 1);
    memset(matchedstr, 0, w * CHSIZE + 1);

    strcat(matchedstr, "│");
    strcat(matchedstr, str);

    for (int i = 0; i < w - len - 2; i++)
    {
        strcat(matchedstr, " ");
    }

    strcat(matchedstr, "│");
    return matchedstr;
}
//w-2만큼 수평선을 만들어서 반환(중요하지 않음)
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
	//Run Queue 생성(RM 알고리즘)
	Queue * rq = newQueue(RM);
	//Text기반 GUI관리 구조체 생성
	tgui = newTGUI(HEIGHT,WIDTH);
	//메세지 관리자 생성
	msg = newMSG();

	int isrunning = 1;							//메인루프 동작 여부 결정
	double schedulerTime = 0.0;					//스케줄러의 시간 0~끝날때까지 us단위
	double termicountdown = 3*1000.0*1000.0;	//처리 후 메인루프가 끝나기 까지

	//시간 측정을 위한 변수들
	struct timeval elap, curr;
	double delta;				//지난 프레임에서 몇 us가 더 지낫는지
	gettimeofday(&elap, NULL);
	
	//태스트 목록 읽기 및 정보 확인
	fileread("processGen/process.txt");	
	
	//결과 로그 파일 
	FILE * logfile = fopen("Gantt/log.txt","w");

	//수평선, 타이틀명 상수 문자열화(한번만 만들고 재활용)
	char * horline = strHorLine(WIDTH);
	char * title = "TEAM 8 SCHEDULER";	
	char titlesp[WIDTH] = " ";	// 타이틀 가운데 정렬 용 공백 문자열
	for( int i = 0 ; i < (WIDTH-strlen(title))/2 -2 ; i++ )
		strcat(titlesp, " ");
	strcat(titlesp, title);
	title = matchformat(WIDTH,titlesp);

	//시작 전 화면을 지우고 백버퍼도 초기화 함
	printf("scheduler start\n");
	cls();
	invalidRect(tgui,0,0,WIDTH,HEIGHT);

	//main loop
	while(termicountdown > 0.0)
	{
		//흐른 시간 확인 
   	 	gettimeofday(&curr, NULL);																			//현재 시간 - 과거 프레임의 시간
		delta = (curr.tv_sec + curr.tv_usec / 1000000.0 - elap.tv_sec - elap.tv_usec / 1000000.0)*1000000;	//지난 프레임과 비교해서 지난 시간
		schedulerTime += delta;	//흐른 시간을 스케줄로 실행시간에 더해줌
		
		//백 버퍼 초기화 및 사각 테두리를 백버퍼에 등록(자세하게 안 봐도 됨)
		invalidRect(tgui,3,3,WIDTH,HEIGHT-1);		
		sprintf(tgui->str,"┌%s┐", horline);
		draw(tgui,0,0,tgui->str);
		sprintf(tgui->str,"%s", title);
		draw(tgui,0,1,tgui->str);
		sprintf(tgui->str,"├%s┤", horline);
		draw(tgui,0,2,tgui->str);
		for( int i = 3 ; i < LIST_Y-2 ; i++ )
			draw(tgui, 0, i, matchformat(WIDTH," "));
		sprintf(tgui->str,"├%s┤", horline);
		draw(tgui,0,LIST_Y-2,tgui->str);
		for( int i = LIST_Y-1 ; i < LIST_Y+LIST_SIZE+1 ; i++ )
		{
			draw(tgui, 0, i, "│");
			draw(tgui, RUNQ_X, i, "│");
			draw(tgui, MESSAGE_X, i, "│");
			draw(tgui, WIDTH, i, "     │");
		}
		sprintf(tgui->str,"└%s┘", horline);
		draw(tgui,0,LIST_Y+LIST_SIZE+1,tgui->str);

		/***********************실직적인 메인************************/

		//실행 정보 표시
		sprintf(tgui->str,"<SCEHDULER STATUS>");
		draw(tgui, 5, 3, tgui->str);
		sprintf(tgui->str,"scheduler excution time : %dms, tick %.3fus", (int)schedulerTime/1000, delta);
		draw(tgui, 5, 4, tgui->str);
		
		//task의 도착확인, g_tasklist의 가장 앞 노드의 도착 시간이 실행시간보다 적으면 도착한것임
		while(g_tasklist->head != g_tasklist->endnode)
		{
			//도착시 g_tasklist에서 Pop하고 런큐에 삽입 
			if( g_tasklist->head->data->arrivaltime <= (int)schedulerTime )
			{	
				Node * node = popQueue(g_tasklist);
				taskrun(node->data); // state load -> wating
				int index = insertNode(rq, node);	//해당 노드가 몇번 째 위치에 삽입 되었는지 저장
				sprintf(msg->str,"[%dms] %s arrive", (int)schedulerTime/1000, node->data->name);
				insertMSG();
				fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, node->data->name, (int)node->data->arrivaltime, LOG_ARRIV);
								
				pid_t pid = fork();//자식 프로세스 분기
				if( pid < 0 )
				{
					sprintf(msg->str,"[%dms] fork faild PID : %d", (int)schedulerTime/1000, pid);
					insertMSG();
				}
				else if(pid == 0)
				{
					//자식 프로세스 생성
					//자식 프로세스가 생성되었어도 실행은 되면 안되기에 즉시 정지
					char * argv[] = {"dummy/dummytask", node->data->name, NULL};
					char * env[] = {NULL};
					raise(SIGSTOP);
					execve("dummy/dummytask", argv, env);

					sprintf(msg->str,"[%dms] child task is not excuted", (int)schedulerTime/1000);
					insertMSG();
					break;
				}
				
				node->data->pid = pid;
				//우선순위가 가장 높은게 도착한 것이였을 경우(맨 앞에 저장되었을 경우)
				if( index == 0 )
				{
					//기존의 노드 처리중이였으면
					if( node->next != rq->endnode && node->next->data->state == TASK_RUN)
					{ 	
						//그것의 상태를 스톱으로 바꿈(preemptive)
						taskstop(node->next->data);
						node->next->data->state = TASK_STOP;
						fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, node->next->data->name, (int)node->data->realdeadline, LOG_STOP);
						sprintf(msg->str,"[%dms] %s is stop", (int)schedulerTime/1000, node->next->data->name);
						insertMSG();
					}
				}
			}else break;
		}
		
		//진짜 실행해야 하는 단 하나의 노드를 처리 하는 부분
		TIMETYPE remaindelta = delta;	//소모하고 남아있는데 델타값(흐른시간 = 이번 프레임에 처리 할 수 있는 양)
		Node * currnode = NULL;			
		
		//이번 프레임에서 주어진 처리 시간이 남아있는 동안은 계속 처리함
		while( remaindelta > 0.0 && rq->size > 0 )
		{
			currnode = popQueue(rq);	//런큐 가장 앞에 노드를 뽑아 냄	
			Process * proc = currnode->data;
			//wait이였으면(즉 처음으로 처리단계에 들어왔으면)
			if( proc->state == TASK_WAIT )
			{
				//처음으로 처리를 시작했다고 메세지와 로그를 남김
				sprintf(msg->str,"[%dms] %s start processing", (int)schedulerTime/1000, proc->name);
				insertMSG();
				fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, proc->name, (int)proc->realdeadline, LOG_START);
			}
			//STOP상태였으면
			else if( proc->state == TASK_STOP )
			{
				//선점당해 중단된 처리를 재개한다고 메세지와 로그를 남김
				sprintf(msg->str,"[%dms] %s continue processing", (int)schedulerTime/1000, proc->name);
				insertMSG();
				fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, proc->name, (int)proc->realdeadline, LOG_CONT);
			}
			
			TIMETYPE tempremain = remaindelta;				//처리 전 남은 델타값을 임시 저장
			remaindelta = remaindelta - taskupdate(proc, remaindelta);	// 남은 시간 = 남은시간 - task를 처리하는데 소모한 시간

			//태스크가 끝나지 않았을 경우
			if( proc->state != TASK_FINISH )
			{
				sprintf(tgui->str,"[%dms] %s spend %.3fus, remain Execution Time is %.3f", 
							(int)schedulerTime/1000, proc->name, tempremain, proc->remaintime);
				draw(tgui, 5, 5, tgui->str);
				insertNode(rq,currnode);	//다시 런큐에 삽입 함
			}
			//끝났으면
			else
			{
				kill(proc->pid,SIGKILL);
				sprintf(msg->str,"[%dms] %s finish", (int)schedulerTime/1000, proc->name);
				insertMSG();
				fprintf(logfile,"%09d %s %d %d\n", (int)schedulerTime, proc->name, (int)proc->realdeadline, LOG_FINISH);
				//추출한 노를 삽입하지 않고 메모리 해제
				free(currnode->data);
				free(currnode);
			}
		}


		/***********************각종 리스트 출력*************************/
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


		//런큐와 태스크리스트가 비었으면 스케줄러가 꺼져야 함. 한번만 들어와야 하므로 isrunnig변수 사용
		if( rq->size == 0 && g_tasklist->size == 0 && isrunning == 1)
		{
			sprintf(msg->str,"[%dms] All tasks completed", (int)schedulerTime/1000);
			insertMSG();
			isrunning = 0;
		}

		//끝났으면 카운트 다운 시작
		if( isrunning == 0 )
			termicountdown -= delta;
		
		/**************************메인 끝***************************/
		showBackBuff(tgui);
		elap = curr;	// 다음 프레임의 지난 프레임 시간 = 현재 프레임의 시간
	}

	//메모리 헤제
	fclose(logfile);
	
	free(horline);
	free(title);
	
	destroyTgui(tgui);
	destroyQueue(g_tasklist);
	destroyQueue(rq);
	
	free(msg);
	free(tgui);
	free(g_tasklist);
	free(rq);
}
