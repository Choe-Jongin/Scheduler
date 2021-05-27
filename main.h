#pragma once

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>

#include "queue.h"
#include "message.h"
#include <termios.h>


//주기적으로 도착하는 태스크를 상정하고 태스크 제네레터를 만들어서 burst시간이 짧음
//그래서 파일을 읽을때 몇 배를 크게 읽어드리기 위해 존재하는 상수
#define EX_BURST 1				//processGen 수전 완료 후 1로 수정

#define READ_BUFF_SIZE 256		//파일 읽기 시 한 줄에 몇 글자를 읽는지
#define HEIGHT	30				//최대 높이		LIST_Y + LIST_SIZE + 1 보단 커야 함
#define WIDTH	145				//최대 넓이

#define LIST_Y		8			//큐, 메세지 목록이 시작될 y위치
#define LIST_SIZE	20			//큐, 메세지 등의 최대 표시 갯수
#define TASK_LIST_X	5			//g_tasklist의 노드 표시 x위치
#define RUNQ_X		33			//rq의 노드 표시 x위치
#define MESSAGE_X	105			//메세지의 표시 x위치

#define PP_UNIV		0			//범용 스케줄링 알고리즘
#define PP_PRIO		1			//우선순위 스케줄링(태스크에 우선순위가 부여되어있음)
#define PP_RT		2			//실시간 스케줄링 알고리즘

Queue * g_tasklist;	//모든 프로세서의 목록(아직 스케줄링 큐에 올라가진 않음)
TGUI * tgui;		//텍스트만으로 GUI처럼 화면에 표시하기 위한 구조체

void fileread(char * filename, int pptype);     //파일 읽기
char *matchformat(int w, char *str);// │ {str} blank │ 형태로 문자열 양쪽 끝에 수직선을 하나 넣어줌(중요하지 않음)
char *strHorLine(int w);            //w-2만큼 수평선을 만들어서 반환(중요하지 않음)
int kbhit(void);                    //키보드 버퍼가 비였는데 확인
int getPolicyType(int pp);
