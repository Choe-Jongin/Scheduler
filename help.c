#include <stdio.h>

void showRunForm()
{
	printf("./schedeuler {algorithm} {filename}\n");
	printf("default : ./schedeuler RM (appropriate file)\n");
	printf("algorithm : FIFO(FCFS), SJF, SRF, RR, PRIO, RM, EDF\n");
}

void showAlgorithmList()
{
	printf("Surpport Algorithm\n");
	printf("-------------------------------------------------------\n");
	printf("   Name    Type  Preemptive          Explain\n");
	printf("FIFO(FCFS) UNIV     Non        First In First Out\n");
	printf("   SJF     UNIV     Non        Shortest Job First\n");
	printf("   SRF     UNIV     Yes      Shortest Remaing First\n");
	printf("   RR      UNIV   Yes/Non          Round Robin\n");
	printf("   PRIO    PRIO     Non             Priority\n");
	printf("   RM       RT      Yes          Rate Monotonic\n");
	printf("   EDF      RT      Yes      Earliest Deadline First\n");
	printf("\n");
}
void showFileList()
{
	printf("filename list(space serparated)\n");
	printf("-------------------------------------------------------\n");
	printf("name arrival burst          -> processGen/uniprocess.txt\n");
	printf("name arrival burst priority -> processGen/prioprocess.txt\n");
	printf("name arrival burst deadline -> processGen/rtprocess.txt\n");
	printf("\n");
}
void showControlCommand()
{

	printf("Control command\n");
	printf("-------------------------------------------------------\n");
	printf("q : Quit scheduler\n");
	printf("s : Stop/continue scheduler\n");
	printf("i : zoom In gantt chart\n");
	printf("o : zomm Out gnatt chart\n");

}
void showProcessGenInfo()
{
	printf("cd processGen\n");
	printf("./processGen {size:N} {type:type} {CPU:%%} {time:sec}\n");
	printf("default : ./processGen size:30 type:UNIV CPU:100 time:10\n");
	printf("size : Number of tasks\n");
	printf("type : UNIV(uni,univ), PRIO(priority), RT\n");
	printf("CPU  : CPU utilization(But Approximate value)\n");
	printf("time : scheduling time(But Approximate value)\n");
	printf("\n");
}
int main()
{
	char in;

	showRunForm();

	while(in != 'q')
	{
		printf("\nMORE INFO\nRun_Form: r, Algorithm_Info:a, Filelist:f, ProcessGen:p, Command:c, Quit:q\n>>");
		in = getchar();
		getchar();
		printf("\n");
		switch(in)
		{
			case 'r':  showRunForm(); break;
			case 'a':  showAlgorithmList(); break;
			case 'f':  showFileList(); break;
			case 'p':  showProcessGenInfo(); break;
			case 'c':  showControlCommand(); break;
		}

	}
	


	return 0;
}

