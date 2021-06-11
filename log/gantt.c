#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

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

	if (ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

//커서를 특정 포인트로 이동( 0,0으로 이동을 주로 사용)
void gotoxy(short row, short col)
{
	printf("%c[%d;%dH", 27, row, col);
}
//화면 지우기 함수
void cls()
{
	printf ("%c[2J", 27);
}
void setcolor(int c)
{
	if (c < 0)
		c = 0;
	if (c > 8)
		c = 8;
	switch (c)
	{
	case 0:
		printf("\033[0m");
		break;
	case 1:
		printf("\033[40m");
		break;
	case 2:
		printf("\033[41m");
		break;
	case 3:
		printf("\033[42m");
		break;
	case 4:
		printf("\033[43m");
		break;
	case 5:
		printf("\033[44m");
		break;
	case 6:
		printf("\033[45m");
		break;
	case 7:
		printf("\033[46m");
		break;
	case 8:
		printf("\033[47m");
		break;
	}
}

int main(int argc, char *argv[])
{
	FILE *file = fopen("log.txt", "r");


	//간트 카트에 필요한 변수들
	int ganttW = 140; //간트 차트 그리는 영역의 넓이
	int ganttH = 30; //간트 차트 그리는 영역의 넓이
	int ganttunit = 10; //간트차트 시간 그래프 한칸 단위의 역수 1:1초, 5: 1/5초 10: 1/10초, 100: 1/100초
	int isRunning = 1;
	
	//Schduling info
	double schedulerTime = 20000000.0f;
	double gantttime = ganttW/ganttunit;
	int size = 50;

	int index = 0;

	cls();


	//main loop
	while (isRunning)
	{
		gotoxy(0, 0);
		int timeoffset = (int)(gantttime * ganttunit) - ganttW;
		if (timeoffset < 0)
			timeoffset = 0;

		printf("Task ");
		for (int i = timeoffset; i < ganttW + timeoffset; i++)
		{
			if (i % ganttunit == 1 && i / ganttunit >= 10)
			{
			}
			else if (i == ganttW + timeoffset - 1)
			{
				printf("-");
			}
			else if (i % ganttunit == 0)
			{
				printf("%d", i / ganttunit);
			}
			else
				printf("-");
		}
		printf(" \n");

		for (int i = 0; i < size; i++)
		{
			if( i < index || i > index + ganttH)
				continue;
			

			setcolor(0);
			for (int j = 0; j < ganttW; j++)
				printf(" ");
			printf("\r");
			printf("P%03d ", i);
			setcolor(3+i%6);
			for (int j = 0; j < ganttW; j++)
			{
				if ((j + timeoffset) % ganttunit == 0)
					printf("│");
				else
					printf(" ");
			}
			setcolor(0);
			printf("\n");
		}

		char in;
		while (!kbhit())
		{
			//wait
		}


		//input
		in = getchar();
		switch (in)
		{
		case 'q':
			isRunning = 0;
			break;
		case 'i':
			if (++ganttunit > 100)
				ganttunit = 100;
			break;
		case 'o':
			if (--ganttunit < ganttW / (schedulerTime/1000000))
				ganttunit = ganttW / (schedulerTime/1000000);
			break;

		case 65:
			if (--index < 0)
				index = 0;
			break;
		case 66:
			if (++index > size - ganttH - 1)
				index = size - ganttH - 1;
			break;
		case 68:
			gantttime -= 1 / (double)ganttunit;
			if (gantttime < ganttW / ganttunit)
				gantttime = ganttW / ganttunit;
			break;
		case 67:
			gantttime += 1 / (double)ganttunit;
			if (gantttime > schedulerTime / 1000000)
				gantttime = schedulerTime / 1000000;
			break;

		//extend
		case -32:
			in = getchar();
			switch (in)
			{
			case 72:
				if( --index < 0 )
					index = 0;
				break;
			case 80:
				if( ++index > size - ganttH -1 )
					index = size - ganttH -1;
				break;
			case 75:
			gantttime -= 1/(double)ganttunit;
			if (gantttime < ganttW/ganttunit)
				gantttime = ganttW/ganttunit;
				break;
			case 77:
			gantttime += 1/(double)ganttunit;
			if (gantttime > schedulerTime / 1000000)
				gantttime = schedulerTime / 1000000;
				break;
			
			}
			break;
		}
	}
	setcolor(0);

	fclose(file);
	return 0;
}