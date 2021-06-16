#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSEC	*1000
#define SEC		*1000*1000

int main(int argc, char * argv[])
{
	int size = 30;
	int dist = 0;	// 0:uniform, 1:exponetial, 2:Poisson
	int expCPU = 100;
	int type = 0;	// 0:UNIV, 1:RT, 2:Priority
	int exptime = 10 SEC;

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
	
		//기대 실행 시간(초단위)
		if( strcmp(prop,"time") == 0)
			exptime = atoi(value) SEC;

		if( strcmp(prop,"type") == 0 || strcmp(prop,"TYPE") == 0)
		{
			if( strcmp(value,"UNI") == 0 || strcmp(value,"uni") == 0 || strcmp(value,"UNIV") == 0 )
			{
				printf("for universar scheduler \n");
				type = 0;
			}
			else if( strcmp(value,"RT") == 0 || strcmp(value,"real") == 0 )
			{
				printf("for real time scheduler \n");
				type = 1;
			}
			else if( strcmp(value,"PRIO") == 0 || strcmp(value,"prio") == 0 )
			{
				printf("for Priority scheduler \n");
				type = 2;
			}
			else
			{
				printf("%s is can't support \n", value);
			}

		}
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

	FILE * file;
	
	switch( type )
	{
		case 0: //universal
			file = fopen("uniprocess.txt","w"); break;
		case 1: //real time
			file = fopen("rtprocess.txt","w"); break;
		case 2: //priority
			file = fopen("prioprocess.txt","w"); break;
	}
	
	exptime = (exptime/expCPU)*100;
	int totalWCET = 0;
	int totaldead = 0;
	float totalCPUutil = 0;

	printf("%d\n",exptime);
	switch( dist )
	{
	case 0:
		for( int i = 0 ; i < size ; i++ )
		{
			char processname[64];
			int arrivtime 	= (float)(rand()%(size))/size*exptime + (rand()%300) MSEC + rand()%1000;
			int bursttime 	= (float)(exptime/size)*(float)(rand()%2000)/1000*(float)expCPU/100.0f;
			int deadline	= (int)(bursttime*( 1.3f + (float)(rand()%100)/100.0f ));
			int priority	= rand()%(size);
			
			sprintf(processname,"p%d",i);

			switch( type )
			{
			case 0: //universal
				fprintf(file,"%s %d %d\n",processname, arrivtime, bursttime);
				break;
			case 1: //real time
				fprintf(file,"%s %d %d %d\n",processname, arrivtime, bursttime, deadline);
				break;
			case 2: //priority
				fprintf(file,"%s %d %d %d\n",processname, arrivtime, bursttime, priority);
				break;
			}

			totalWCET += bursttime;
			totaldead += deadline;
			totalCPUutil += (float)bursttime/deadline;
		}
		//printf("total WCET : %d, average deadline : %d, CPU uitilization %d%\n", totalWCET, totaldead/size, (int)(totalCPUutil*100));
		printf("size : %d, total WCET : %d\n", size, totalWCET );
		break;
	}

	fclose(file);

	return 0;
}
