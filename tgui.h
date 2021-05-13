#pragma once
#include <string.h>

#define CHSIZE 3 //특수문자 바이트 수

//TGUI 정보를 담는 구조체
typedef struct TGUI_{
	int height;
	int width;
	char **texel;

	char * str;
}TGUI;

TGUI * newTGUI(int height, int width)
{
	TGUI * newtgui = (TGUI*)malloc(sizeof(TGUI));
	newtgui->height = height;
	newtgui->width = width;

	newtgui->texel = (char**)malloc(sizeof(char*)*height);
	for( int i = 0 ; i < height ; i++ ){		
		newtgui->texel[i] = (char*)malloc(sizeof(char)*(width+1));
		newtgui->texel[i][width] = '\0';
	}
	//newtgui->texel[height-1][width] = '\0';
	newtgui->str = (char*)malloc(sizeof(char)*(width+1));

	return newtgui;
}

void invalidRect(TGUI * tgui, int l, int t, int r, int b)
{
	if( t < 0 ) t = 0;
	if( l < 0 ) l = 0;
	if( b > tgui->height ) b = tgui->height;
	if( r > tgui->width ) r = tgui->width;
	for( int row = t ; row < b ; row++)			
		for( int col = l ; col < r; col++)
				tgui->texel[row][col] = ' ';
}
void cls()
{
	printf ("%c[2J", 27);
}
void gotoxy (short row, short col)
{
	printf ("%c[%d;%dH", 27, row, col);
}

void draw(TGUI * tgui, int x, int y, char * str)
{
	int len = strlen(str);
	int index = 0;
	for( int i = x ; i < len+x ; i++ )
			tgui->texel[y+i/tgui->width][i%tgui->width] = str[i-x];

}
void showBackBuff(TGUI * tgui)
{
//	cls();
	gotoxy(0,0);
	for( int r = 0 ; r < tgui->height ; r++)
		printf("%s\n",tgui->texel[r]);
}

void destroyTgui(TGUI * tgui)
{
	for( int i = 0 ; i < tgui->height ; i++ ){		
		free(tgui->texel[i]);
	}
	free(tgui->texel);

}

