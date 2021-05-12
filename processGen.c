#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
	int size = 100;
	int dist = 0; // 0:uniform, 1:exponetial, 2:Poisson
	int expCPU = 100;

	srand((unsigned)time(NULL));

	//실행 설정(인자 확인) 
	for( int i = 1 ; i < argc ; i++)
	{
		char * str[64];
		char * prop;
		char * value = NULL;

		strcpy(str, argv[i]);
		prop = strtok(str,":");
		if( str != NULL )
			value = strtok(NULL, ":");
			
		if( value == NULL )
		{
			printf("can't understaned the argument : %s\n",argv[i]);
			continue;
		}

		//프로세서 개수 설정
		if( strcmp(prop,"size") == 0)
			size = atoi(value);
		
		//CPU이용률 설정(정확한 값이 아닌 그 부근에서 결정)
		if( strcmp(prop,"CPU") == 0)
			expCPU = atoi(value);

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


	int totalWCET = 0;
	int totaldead = 0;
	float totalCPUutil = 0;
	switch( dist )
	{
	case 0:
		for( int i = 0 ; i < size ; i++ )
		{
			char processname[64];
			int arrivtime 	= 0 + rand()%(10*1000);
			int deadline	= size + rand()%(9*size);
			int bursttime 	= 1 + (deadline*(rand()%((expCPU-1)/5)))/(size*10);
			sprintf(processname,"%c%c%c%c%04d", (65+rand()%26), (65+rand()%26), (65+rand()%26), (65+rand()%26), rand()%10000);
			fprintf(file,"%s %d %d %d\n",processname, arrivtime, bursttime, deadline);
			totalWCET += bursttime;
			totaldead += deadline;
			totalCPUutil += (float)bursttime/deadline;
		}
		printf("[total] WCET : %d, deadline : %d, CPU uitilization %d%\n", totalWCET, totaldead, (int)(totalCPUutil*100));
		break;

	}

	fclose(file);

	return 0;
}
