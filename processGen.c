#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
	int size = 100;
	int dist = 0; // 0:uniform, 1:exponetial, 2:Poisson

	srand((unsigned)time(NULL));

	//실행 설정(인자 확인) 
	for( int i = 1 ; i < argc ; i++)
	{
		char * str[64];
		char * prop;
		char * value;

		strcpy(str, argv[i]);
		prop = strtok(str,":");
		if( prop != NULL )
			value = strtok(NULL, ":");
			
		if( prop == NULL )
		{
			printf("can't undersanted the argument : %s\n",argv[i]);
			continue;
		}

		//프로세서 개수 설정
		if( strcmp(prop,"size") == 0)
			size = atoi(value);

		//분포 설정 
		if( strcmp(prop,"dist") == 0 || strcmp(prop,"distribution") == 0)
		{
			if( strcmp(value,"uni") == 0 || strcmp(value,"uniform") == 0 )
			{
				printf("uniform distribution : %s\n",argv[i]);
				dist = 0;
			}
			else
			{
				printf("%s is can't support \n", value);
			}
		}
	}

	FILE * file = fopen("process.txt","w");


	switch( dist )
	{
	case 0:
		for( int i = 0 ; i < size ; i++ )
		{
			char processname[64];
			int arrivtime 	= 0 + rand()%(10*1000);
			int deadline	= 25 + rand()%225;
			int bursttime 	= (deadline*(rand()%60))/100;
			sprintf(processname,"%c%c%c%c%d", (65+rand()%26), (65+rand()%26), (65+rand()%26), (65+rand()%26), rand()%10000);
			fprintf(file,"%s, %d, %d, %d\n",processname, arrivtime, bursttime, deadline);
		}
		break;

	}

	fclose(file);

	return 0;
}
