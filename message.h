#pragma once

#include <string.h>
#include "tgui.h"

#define MAX_MSG_SIZE 	20			 	//최대 몇개의 메세지를 보여줄 것인가(main.c의 LIST_SIZE보다 커선 안됨)
#define MAX_MSG_WIDTH	40				//메세지 하나의 최대 문자열 길이(바이트 단위임)
#define SHOW_MSG_TIME 	3*1000*1000 	//메세지를 보여줄 기간(us단위)

//메세지 관리자 구조체(하나한의 메세지 구조체가 아님을 주의!!)
typedef struct message
{
	char msgstr[MAX_MSG_SIZE][MAX_MSG_WIDTH*CHSIZE]; 	//메세지 문자열 들
	TIMETYPE duration[MAX_MSG_SIZE];					//화면 출력 남은 시간
	int size;											//현재 메세지가 몇 개 있는지
	char str[MAX_MSG_WIDTH*CHSIZE];						//다용도 임시 문자열
}MSG;

MSG * msg = NULL;								//메세지 관리자 구조체 전역번수, g_는 그냥 안 붙임 ㅋㅋ 

//메세지 관리자 구조체 초기화
MSG * newMSG()
{
	MSG * msg = (MSG*)malloc(sizeof(MSG));		//동적할당
	for( int i = 0 ; i < MAX_MSG_SIZE ; i++ )
		msg->duration[i] = SHOW_MSG_TIME;		//화면 표시 시간 설정
	msg->size = 0;

	return msg;
}

//새 메시지 추가(문자열을 직접 입력)
void insertStrMSG(char * str)
{
	//이미 최대 사이즈 초과 하게 되면
	if( msg->size >= MAX_MSG_SIZE )
	{
		//하나씩 앞으로 땡김
		for( int i = 0; i < msg->size -1 ; i++ )
		{
			strcpy(msg->msgstr[i], msg->msgstr[i+1]);
			msg->duration[i] = msg->duration[i+1];
		}
		msg->size--;
	}
	
	strcpy(msg->msgstr[msg->size], str);
	msg->duration[msg->size] = SHOW_MSG_TIME;

	msg->size++;

}
//새 메시지 추가(임시 문자열 내용을 그대로)
void insertMSG()
{
	insertStrMSG(msg->str);
}
//가장 오래된 메세지 삭제
void popMSG()
{
	if( msg->size <= 0 )
		return ;
	
	for( int i = 0; i < msg->size -1 ; i++ )
	{
		strcpy(msg->msgstr[i], msg->msgstr[i+1]);
		msg->duration[i] = msg->duration[i+1];
	}
	strcpy(msg->msgstr[msg->size-1],"");
	msg->duration[msg->size-1] = 0.0;

	if( --msg->size < 0)
			msg->size = 0;
	
}
//메세지의 기간 최신화(흐른 시간 만큼 줄이고 시간이 안 남았으면 팝)
void MSGUpdate( TIMETYPE delta )
{
	for( int i = 0 ; i < msg->size ; i++ )
		msg->duration[i] -= delta;

	for( int i = 0 ; i < msg->size ; i++ )
		if( msg->duration[i] <= 0.0)
		{
				popMSG();
				i--;
		}
}
