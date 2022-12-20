#include "headers.h"
#include "fg.h"


void fg(char** cmd_txtArr){
	int Pid;
	Pid = fork();
	if(Pid==-1){
		perror("fg/fork(): ");
		return;
	}
	else if(Pid == 0){
		// Child process
		int i=0;
		while(strcmp(cmd_txtArr[i++],"")!=0 && i<(MAX_STRING_LEN-1));
		// char* tempPtr=cmd_txtArr[i-1];
		
		cmd_txtArr[i-1]=NULL;
		if(execvp(cmd_txtArr[0],cmd_txtArr)==-1)
			perror("fg/execvp");
		exit(EXIT_FAILURE);
	}
	else{
		// Parent process
		fgPid = Pid;
		if(waitpid(Pid,NULL,0)==-1){
			perror("fg/wait");
		}
		// if(wait(NULL)==-1)
	}
}