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
		newtgui->texel[i] = (char*)malloc(sizeof(char)*(width*CHSIZE+1));
		newtgui->texel[i][width*CHSIZE] = '\0';
	}
//	newtgui->texel[height-1][width*CHSIZE] = '\0';
	newtgui->str = (char*)malloc(sizeof(char)*(width*CHSIZE+1));

	return newtgui;
}

void invalidRect(TGUI * tgui, int l, int t, int r, int b)
{
	if( t < 0 ) t = 0;
	if( l < 0 ) l = 0;
	if( b > tgui->height*CHSIZE ) b = tgui->height*CHSIZE;
	if( r > tgui->width*CHSIZE ) r = tgui->width*CHSIZE;
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
	for( int i = x ; i < len+x ; i++ )
		if( str[i-x] != '\0')
			tgui->texel[y][i] = str[i-x];
//			tgui->texel[y+i/(tgui->width*CHSIZE)][i%(tgui->width*CHSIZE)] = str[i-x];

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

