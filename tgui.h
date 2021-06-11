#pragma once
#include <string.h>

#define CHSIZE 3 //특수문자 바이트 수

//TGUI 정보를 담는 구조체
typedef struct TGUI_{
	int height;		//화면의 높이
	int width;		//화면이 넓이
	char **texel;	//백버퍼(여기에 화면에 표시할 텍스트 들을 모아놓고 한번에 출력함

	char * str;		//임시 문자열 저장소
}TGUI;

//초기화
TGUI * newTGUI(int height, int width)
{
	TGUI * newtgui = (TGUI*)malloc(sizeof(TGUI));
	newtgui->height = height;
	newtgui->width = width;

	//2차원 문자열 동적할당
	newtgui->texel = (char**)malloc(sizeof(char*)*height);
	for( int i = 0 ; i < height ; i++ ){		
		//가로 배열의 크기를 넓이 * CHSIZE를 하는 이유는 유니코드의 경우 한글자가 3바이트까지 먹기 때문
		newtgui->texel[i] = (char*)malloc(sizeof(char)*(width*CHSIZE+1));
		for( int j = 0 ; j < width*CHSIZE ; j++)
			newtgui->texel[i][j] = '\0';
		newtgui->texel[i][width*CHSIZE] = '\0';				//마지막 글자는 종료문자				
	}

	//임시 문자열 동적 할당
	newtgui->str = (char*)malloc(sizeof(char)*(width*CHSIZE+1));

	return newtgui;
}

//백버퍼 초기화
void invalidRect(TGUI * tgui, int l, int t, int r, int b)
{
	if( t < 0 ) t = 0;
	if( l < 0 ) l = 0;
	if( b > tgui->height ) b = tgui->height;
	if( r > tgui->width*CHSIZE ) r = tgui->width*CHSIZE;
	for( int row = t ; row < b ; row++)
	{
		for( int col = l ; col < r; col++)
				tgui->texel[row][col] = ' ';
	}

}

//화면 지우기 함수
void cls()
{
	printf ("%c[2J", 27);
}

//커서를 특정 포인트로 이동( 0,0으로 이동을 주로 사용)
void gotoxy (short row, short col)
{
	printf ("%c[%d;%dH", 27, row, col);
}
void setcolor(int c)
{
    if( c < 0 ) 
        c = 0;
    if( c > 8 ) 
        c = 8;
    switch(c)
    {
        case 0: printf("\033[0m"); break;
        case 1: printf("\033[40m"); break;
        case 2: printf("\033[41m"); break;
        case 3: printf("\033[42m"); break;
        case 4: printf("\033[43m"); break;
        case 5: printf("\033[44m"); break;
        case 6: printf("\033[45m"); break;
        case 7: printf("\033[46m"); break;
        case 8: printf("\033[47m"); break;
    }   
}

//백버퍼에 문자열 삽입 주석처리 해둔 코드는 문자열이 넓이를 넘어갔을 경우 자동개행되게 하는 방법임
void draw(TGUI * tgui, int x, int y, char * str)
{
	int len = strlen(str);
	for( int i = x ; i < len+x ; i++ )
		if( str[i-x] != '\0')
			tgui->texel[y][i] = str[i-x];
//			tgui->texel[y+i/(tgui->width*CHSIZE)][i%(tgui->width*CHSIZE)] = str[i-x];

}

//백버퍼를 한번에 화면에 표시, 메 프레임마다 1번씩 해 줘야함
void showBackBuff(TGUI * tgui)
{
//	cls();
	gotoxy(0,0);
	for( int r = 0 ; r < tgui->height ; r++)
		printf("%s  \n",tgui->texel[r]);

}

//소멸자
void destroyTgui(TGUI * tgui)
{
	for( int i = 0 ; i < tgui->height ; i++ ){		
		free(tgui->texel[i]);
	}
	free(tgui->texel);
}

