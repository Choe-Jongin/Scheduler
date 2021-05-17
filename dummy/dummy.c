#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
	char name[64];
	int deadline;
	int priority;

	if( argc > 1)	strcpy(name, argv[1]);
	if( argc > 2)	deadline = atoi(argv[2]);
	if( argc > 3)	deadline = atoi(argv[3]);

	while (1)
	;

	return 0;
}
