#include "headers.h"
#include "jobs.h"

static char path[1137],readStr[1137];
static char Info_Array[113][MAX_STRING_LEN];


int cmpfunc2(const void* a, const void* b){
  return strcasecmp((*(const char***)a)[0],(*(const char***)b)[0]);
}

void jP(){
    for(int i=0;i<bgListSize;i++){
        printf("%s %s %s\n",bgList[i][0],bgList[i][1],bgList[i][2]);
    }
}

void Jobs(char* input){
    qsort(bgList,bgListSize,sizeof(char**),cmpfunc2);
    int fboth=0;
    if(strcmp(input,"-s")!=0 && strcmp(input,"-r")!=0) fboth=1;

    for(int i=0;i<bgListSize;i++){
        sprintf(path,"/proc/%s/stat",bgList[i][1]); // pid = bgList[i][1];
        FILE* fptr=fopen(path,"r");

        if(fptr==NULL){
            continue;   // process exited...
        }

        int ind=0;
        while(fscanf(fptr,"%s",readStr)!=EOF){
            strcpy(Info_Array[ind++],readStr);
        }
        if(fclose(fptr)==EOF){
            perror("jobs.c/fclose");
            return;
        }
        // Info_Array[0] --> The process's PID.
        // Info_Array[1] --> "The FILE_NAME of the Executable."
        // Info_Array[3] --> The PID of the parent of this process.
        
        char ProcStat[113];
        int is_r=0,is_s=0;
        if(Info_Array[2][0]!='T') {strcpy(ProcStat,"Running");is_r=1;}
        else                      {strcpy(ProcStat,"Stopped");is_s=1;}
        if(strcmp(input,"-r")==0 && is_r || strcmp(input,"-s")==0 && is_s || fboth){
            printf("[%s] %s %s [%s]\n",bgList[i][2],ProcStat,bgList[i][0],bgList[i][1]);
        }
    }   
}