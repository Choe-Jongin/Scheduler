#pragma once

#include <string.h>
#include "tgui.h"

#define MAX_MSG_SIZE 20
#define MAX_MSG_WIDTH 40
#define SHOW_MSG_TIME 1*1000*1000 //메세지를 보여줄 기간

typedef struct message
{
	char msgstr[MAX_MSG_SIZE][MAX_MSG_WIDTH*CHSIZE];
	TIMETYPE duration[MAX_MSG_SIZE];
	int size;
	char str[MAX_MSG_WIDTH*CHSIZE];
}MSG;

MSG * msg = NULL;

MSG * newMSG()
{
	MSG * msg = (MSG*)malloc(sizeof(MSG));
	for( int i = 0 ; i < MAX_MSG_SIZE ; i++ )
		msg->duration[i] = SHOW_MSG_TIME;
	msg->size = 0;

	return msg;
}

//새 메시지 추가
void insertStrMSG(char * str)
{
	for( int i = msg->size-1; i > 0 ; i-- )
	{
		strcpy(msg->msgstr[i], msg->msgstr[i-1]);
		msg->duration[i] = msg->duration[i-1];
	}
	strcpy(msg->msgstr[0], str);
	msg->duration[0] = SHOW_MSG_TIME;
	if( ++msg->size> MAX_MSG_SIZE )
		msg->size = MAX_MSG_SIZE;
}
void insertMSG()
{
	insertStrMSG(msg->str);
}
//가장 오래된 메세지 삭제
void popMSG()
{
	if( msg->size <= 0 )
		return ;
	strcpy(msg->msgstr[msg->size-1],"");
	msg->duration[msg->size-1] = 0.0;
	if( --msg->size < 0)
			msg->size = 0;
	
}
void MSGUpdate( TIMETYPE delta )
{
	for( int i = 0 ; i < msg->size ; i++ )
		msg->duration[i] -= delta;

	for( int i = msg->size-1; i > 0 ; i-- )
		if( msg->duration[i] <= 0.0)
				popMSG();
}
