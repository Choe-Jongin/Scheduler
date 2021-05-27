#include "main.h"

int main(int argc, char *argv[] )
{
	//스케줄링 알고리즘
	int pp = RM;
	int pptype = PP_RT;
	if( argc > 1 )
	{
		
		if( strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0 )
		{	
			printf("./schedeuler {algorithm} {filename}\n");
			printf("default : ./schedeuler RM (appropriate file)\n");
			printf("algorithm : FIFO(FCFS), SJF, PRIO, RM, EDF\n");
			printf("\n");
			printf("filename list \n");
			printf("name arrival burst          -> processGen/uniprocess.txt\n");
			printf("name arrival burst priority -> processGen/prioprocess.txt\n");
			printf("name arrival burst deadline -> processGen/rtprocess.txt\n");

			return 0;
		}
		else if( strcmp(argv[1], "FIFO") == 0 || strcmp(argv[1], "FCFS") == 0 )
		{
			pp = FIFO;
			pptype = PP_UNIV;
		}
		else if( strcmp(argv[1], "SJF") == 0 )
		{
			pp = SJF;
			pptype = PP_UNIV;
		}
		else if( strcmp(argv[1], "PRIO") == 0 || strcmp(argv[1], "priority") == 0)
		{
			pp = PRIO;
			pptype = PP_PRIO;
		}
		else if( strcmp(argv[1], "RM") == 0 )
		{
			pp = RM;
			pptype = PP_RT;
		}
		else if( strcmp(argv[1], "EDF") == 0 )
		{	
			pp = EDF;
			pptype = PP_RT;
		}
	}
//	pptype = getPolicyType(pp);

	//Run Queue 생성(RM 알고리즘)
	Queue * rq = newQueue(pp);
	//Text기반 GUI관리 구조체 생성
	tgui = newTGUI(HEIGHT,WIDTH);
	//메세지 관리자 생성
	msg = newMSG();
	
	int isrunning = 1;							//메인루프 동작 여부 결정
	double schedulerTime = 0.0;					//스케줄러의 시간 0~끝날때까지 us단위
	double termicountdown = 1*1000.0*1000.0;	//처리 후 메인루프가 끝나기 까지
	double totwaiting = 0.0f;					//총 대기시간(평균대기시간 = 총대기/처리시작태스크수)
	double avgwaiting = 0.0f;					//평균대기시간 = 총대기/처리시작태스크수
	double maxwaiting = 0.0f;					//가장 오래 기다린 태스크의 대기시간
	int size = 0;								//불러온 처리해야 할 태스크의 개수
	int deadlinemiss = 0;						//데드라인 미스 카운터

	//시간 측정을 위한 변수들
	struct timeval elap, curr;
	double delta;				//지난 프레임에서 몇 us가 더 지낫는지
	gettimeofday(&elap, NULL);
	
	//태스트 목록 읽기 및 정보 확인
	//명시했을 경우
	if( argc > 2 && (strcmp(argv[2], "processGen/uniprocess.txt") == 0
				 ||  strcmp(argv[2], "processGen/prioprocess.txt") == 0
			     || strcmp(argv[2], "processGen/rtprocess.txt") == 0 ))
	{
		if(strcmp(argv[2], "processGen/uniprocess.txt") == 0)
			pptype = PP_UNIV;
		if(strcmp(argv[2], "processGen/prioprocess.txt") == 0)
			pptype = PP_PRIO;
		if(strcmp(argv[2], "processGen/rtprocess.txt") == 0)
			pptype = PP_RT;
		fileread(argv[2], pptype);
	}
	//따로 파일 이름을 명시하지 않았을 경우(혹은 잘못된 경로)
	else
	{
		if( pptype == PP_UNIV )
			fileread("processGen/uniprocess.txt", pptype);	
		else if( pptype == PP_PRIO )
			fileread("processGen/prioprocess.txt", pptype);	
		else if( pptype == PP_RT )
			fileread("processGen/rtprocess.txt", pptype);	
		else
			fileread("processGen/uniprocess.txt", PP_UNIV);	
	}

	size = g_tasklist->size;
	
	//수평선, 타이틀명 상수 문자열화(한번만 만들고 재활용)
	char * horline = strHorLine(WIDTH);
	char title[64] = "TEAM 8 SCHEDULER";	
	char titlesp[WIDTH] = " ";	//타이틀 가운데 정렬 용 공백 문자열
	switch(pp)					//타이틀 맨 뒤에 알고리즘 종류 표기
	{
		case FIFO:	strcat(title,"(FIFO)"); break;
		case SJF:	strcat(title,"(SJF)"); break;
		case PRIO:	strcat(title,"(PRIO)"); break;
		case RM:	strcat(title,"(RM)"); break;
		case EDF:	strcat(title,"(EDF)"); break;
	}
	for( int i = 0 ; i < (WIDTH-strlen(title))/2 -2 ; i++ )
		strcat(titlesp, " ");
	strcat(titlesp, title);
	strcpy(title, matchformat(WIDTH,titlesp));

	//시작 전 화면을 지우고 백버퍼도 초기화 함
	insertStrMSG("scheduler start");
	cls();
	invalidRect(tgui,0,0,WIDTH,HEIGHT);

	//가장 마지막 추가된 노드
	Node * newtasknode = NULL;
	
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
	
		//키보드 입력
		if( kbhit() )
		{
			char in = getchar();
			if( in == 'q')
			{
				sprintf(msg->str,"[%dms] Quit", (int)schedulerTime/1000);
				insertMSG();
	            isrunning = 0;
				termicountdown = 0.0;
			}
		}
		/***********************실직적인 메인************************/

		//실행 정보 표시
		sprintf(tgui->str,"<SCEHDULER STATUS>");
		draw(tgui, 5, 3, tgui->str);
		sprintf(tgui->str,"scheduler excution time : %dms, tick %.3fus", (int)schedulerTime/1000, delta);
		draw(tgui, 5, 4, tgui->str);
		
		switch(pptype)
		{
		case PP_UNIV:
			sprintf(tgui->str,"average watting : %dms, max watting : %dms", (int)avgwaiting/1000, (int)maxwaiting/1000);
			break;
		case PP_PRIO:
			sprintf(tgui->str,"average watting : %dms, max watting : %dms", (int)avgwaiting/1000, (int)maxwaiting/1000);
			break;
		case PP_RT:
			sprintf(tgui->str,"deadline miss : %d", deadlinemiss);
			break;
		}
		draw(tgui, 5, 5, tgui->str);
		
		//task의 도착확인, g_tasklist의 가장 앞 노드의 도착 시간이 실행시간보다 적으면 도착한것임
		while(g_tasklist->head != g_tasklist->endnode && g_tasklist->head->data->arrivaltime <= (int)schedulerTime )
		{
			Node * node = popQueue(g_tasklist);
			newtasknode = node;		//가장 마지막에 추가된 노드 주소 저장
			taskrun(node->data); // state load -> wating
			int index = insertNode(rq, node);	//해당 노드가 몇번 째 위치에 삽입 되었는지 저장
			sprintf(msg->str,"[%dms] %s arrive", (int)schedulerTime/1000, node->data->name);
			insertMSG();
							
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
				raise(SIGSTOP);
				char * argv[] = {"dummy/dummytask", node->data->name, NULL};
				char * env[] = {NULL};
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
					sprintf(msg->str,"[%dms] %s is stop", (int)schedulerTime/1000, node->next->data->name);
					insertMSG();
				}
			}
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

				currnode->waiting = schedulerTime - proc->arrivaltime;
				if( currnode->waiting > maxwaiting )
					maxwaiting = currnode->waiting;
				totwaiting += currnode->waiting;
				avgwaiting = totwaiting/(size-g_tasklist->size);
			}
			//STOP상태였으면
			else if( proc->state == TASK_STOP )
			{
				//선점당해 중단된 처리를 재개한다고 메세지와 로그를 남김
				sprintf(msg->str,"[%dms] %s continue processing", (int)schedulerTime/1000, proc->name);
				insertMSG();
			}
			
			remaindelta = remaindelta - taskupdate(proc, remaindelta);	// 남은 시간 = 남은시간 - task를 처리하는데 소모한 시간

			//태스크가 끝나지 않았을 경우
			if( proc->state != TASK_FINISH )
			{
				//방금 처리한 노드의 정보를 출력함
				//real time 스케줄러는 데드라인 미스 또한 체크해 봐야 함 
				if( pptype == PP_RT )
				{
					//아직 데드라인 미스는 아님
					if (proc->realdeadline >= schedulerTime )
					{
						sprintf(tgui->str,"[%dms] RUN : %s, remain %.0fus, watting time : %dms real deadline %dms", 
							(int)schedulerTime/1000, proc->name, proc->remaintime, (int)currnode->waiting/1000, (int)proc->realdeadline/1000);
					}
					//데드라인 미스
					else
					{
						sprintf(tgui->str,"[%dms] RUN : %s, remain %.0fus, watting time : %dms real deadline %dms, DEADLINE MISS", 
							(int)schedulerTime/1000, proc->name, proc->remaintime, (int)currnode->waiting/1000, (int)proc->realdeadline/1000);
						
						//여기는 미스당 딱 한번만 들어와 함
						if( currnode->isdeadmiss == 0 )
						{
							currnode->isdeadmiss = 1;
							deadlinemiss++;
							sprintf(msg->str,"[%dms] %s deadline miss", (int)schedulerTime/1000, proc->name);
			                insertMSG();
						}
					}

				}
				//Real Time 스케줄러가 아니면 간단하게 현재 정보 만 출력
				else
				{
					sprintf(tgui->str,"[%dms] RUN : %s, remain %.0fus, watting time : %dms", 
						(int)schedulerTime/1000, proc->name, proc->remaintime, (int)currnode->waiting/1000);
				}
				draw(tgui, 5, 6, tgui->str);

				insertNode(rq,currnode);	//다시 런큐에 삽입 함
			}
			//끝났으면
			else
			{
				kill(proc->pid,SIGKILL);
				sprintf(msg->str,"[%dms] %s finish", (int)schedulerTime/1000, proc->name);
				insertMSG();
				//추출한 노드를 삽입하지 않고 메모리 해제
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
			switch(pptype)
			{
			case PP_PRIO:
				sprintf(tgui->str,"%s arrival : %d, burst : %d, priority : %d",
						it->data->name, (int)it->data->arrivaltime, (int)it->data->bursttime, (int)it->data->priority);
				break;
			case PP_RT:
				sprintf(tgui->str,"%s arrival : %d, burst : %d, deadline : %d",
						it->data->name, (int)it->data->arrivaltime, (int)it->data->bursttime, (int)it->data->deadline);
				break;
			default:
				sprintf(tgui->str,"%s arrival : %d, burst : %d",
						it->data->name, (int)it->data->arrivaltime, (int)it->data->bursttime);
			}
			if( it == newtasknode )
			{
				draw(tgui,MESSAGE_X-6, LIST_Y+rqi, "NEW!!");
			}
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
	free(horline);
	
	destroyTgui(tgui);
	destroyQueue(g_tasklist);
	destroyQueue(rq);
	
	free(msg);
	free(tgui);
	free(g_tasklist);
	free(rq);
}

//파일 읽기
void fileread(char * filename, int pptype)
{
	//도착시간순으로 정렬
	g_tasklist = newQueue(FIFO);
	
	sprintf(msg->str,"filename : %s", filename);
	insertMSG();

	//file read
	FILE * file = fopen(filename,"r");
	while (feof(file) == 0)
	{
        char str[READ_BUFF_SIZE] = "";
    	char name[64];
		int arr, burst, priority = 0, dead = 0;

		fscanf(file,"%s",name);
		if( feof(file) )
			break;
		fscanf(file,"%d",&arr);
		fscanf(file,"%d",&burst);

		if( pptype == PP_PRIO )
			fscanf(file,"%d", &priority);
		if( pptype == PP_RT)
			fscanf(file,"%d", &dead);
		
		insertNewNode(g_tasklist, newProcess(name,(double)arr,(double)burst*EX_BURST, dead, priority));
	}

	fclose(file);
}

//이 아래는 볼 필요 없음

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

//키보드 버퍼가 비였는데 확인
int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}



//도입 고려중인 방식임(현재 실 사용 없음)
int getPolicyType(int pp)
{
	switch(pp)
	{
	case FIFO:
	case SJF:
		return PP_UNIV;
	case PRIO:
		return PP_PRIO;
	case RM:
	case EDF:
		return PP_RT;
	}

	return PP_UNIV;
}
