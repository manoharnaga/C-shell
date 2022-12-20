#include "headers.h"
#include "bg.h"

static char tempPName[MAX_STRING_LEN],Pid[MAX_STRING_LEN];
static FILE* fptr;
static char Info_Array[1000][MAX_STRING_LEN];
static int childStatus;


void checkForBgProcess(){
    char* readStr=(char*)malloc(sizeof(char)*MAX_STRING_LEN*3);
    int pidFromWait=waitpid(-1,&childStatus,WNOHANG);
    while(1){
        if(pidFromWait<=0) break;
        for(int i=0;i<bgListSize;i++){
            if(atoi(bgList[i][1])==pidFromWait){
                strcpy(tempPName,bgList[i][1]);
                break;
            }
        }
        // REMOVE PROCESS FROM THE LIST
        // int i;
        // for(i=0;i<bgListSize;i++){
        //     if(atoi(bgList[1][i])==pidFromWait){
        //         break;
        //     }
        // }
        // for(int j=i;j<bgListSize-1;j++){
        //     strcpy(bgList[0][j],bgList[0][j+1]);
        //     strcpy(bgList[1][j],bgList[1][j+1]);
        //     strcpy(bgList[2][j],bgList[2][j+1]);
        // }
        // bgListSize--;
        // REMOVE PROCESS FROM THE LIST

        if(WIFEXITED(childStatus))  fprintf(stderr,"\n%s with pid %d has exited normally\n",tempPName,pidFromWait);
        else                        fprintf(stderr,"\n%s with pid %d has exited abnormally\n",tempPName,pidFromWait);

        pidFromWait=waitpid(-1,&childStatus,WNOHANG);
    }
}

int bg(char** cmd_txtArr,int bgInd){
	int pid = fork();
	if(pid==-1){
        return -1;
    }
	if(pid==0){
		setpgid(0,0);
		// Child process
		int i=0;
		while(strcmp(cmd_txtArr[i++],"")!=0 && i<(MAX_STRING_LEN-1));
		cmd_txtArr[i-1]=NULL;

		if(execvp(cmd_txtArr[0],cmd_txtArr)==-1)
			perror("fg/execvp");
        exit(EXIT_FAILURE);
	}
	else{
        int i=0;
        strcpy(bgList[bgListSize][0],"");
		while(strcmp(cmd_txtArr[i],"")!=0){
            strcat(bgList[bgListSize][0],cmd_txtArr[i]);
            strcat(bgList[bgListSize][0]," ");
            i++;
        }
        sprintf(bgList[bgListSize][1],"%d",pid);
        sprintf(bgList[bgListSize][2],"%d",bgInd);
        
        bgListSize++;
        
        // printf("%s %d %d %dXX\n",cmd_txtArr[0],pid,bgInd,bgListSize);
        printf("[%d] %d\n",bgInd,pid);
	}
	return 1;
}
