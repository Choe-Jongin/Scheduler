#include <stdio.h>

int main(int argc, char * argv[])
{
	int size = 100;
	int dist = 0; // 0:uniform, 1:exponetial, 2:Poisson
	
	//실행 설정(인자 확인) 
	for( int i = 1 ; i < argc ; i++)
	{
		char * str[64];
		char * prop;
		char * value;

		strcpy(str, argv[i]);
		prop = strtok(str,':');
		if( prop != NULL )
			value = strtok(NULL, ':');
			
		if( prop == NULL )
		{
			printf("can't undersanted the argument : %s\n",argv[i]);
			continue;
		}

		//프로세서 개수 설정
		if( strcmp(prop,"size") == 0)
			size = atoi(value);

		//분포 설정 
		if( strcmp(prop,"dist") == 0 || strcmp(prop,"distribution"))
		{
			if( strcmp(value,"uni") == 0 || strcmp(value,"uniform") == 0 )
			{
				printf("uniform distribution : %s\n",argv[i]);
				dist = 0;
			}		
			printf("%s is can't support \n", value);
		}
	}



}
