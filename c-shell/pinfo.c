#include "headers.h"
#include "pinfo.h"

static char file_path[MAX_STRING_LEN];
static char Info_Array[21][MAX_STRING_LEN];

void Pinfo(char* cmd_pid,char* Home_path){
    char* readStr=(char*)malloc(sizeof(char)*MAX_STRING_LEN*3);
    char* ExePath=(char*)malloc(sizeof(char)*MAX_STRING_LEN*2);
    char* temp_curr_path=(char*)malloc(sizeof(char)*MAX_STRING_LEN*2);
    int Pid;
    struct stat Fstat;
    Pid = getpid();
    strcpy(file_path,"/proc/");

    if(strcmp(cmd_pid,"")==0){
        sprintf(cmd_pid,"%d",Pid);
    }
    strcat(file_path,cmd_pid);
    strcat(file_path,"/stat");
    printf("%s\n",file_path);
    
    FILE* fptr = fopen(file_path,"r");
    if(fptr==NULL){
        perror("Program/pinfo.c/fptr: ");
        return;
    }
    int ind=0; 
    while(fscanf(fptr,"%s",readStr)!=EOF){
        strcpy(Info_Array[ind++],readStr);
    }
    if(fclose(fptr)==EOF){
        perror("bg.c/fclose");
        return;
    }
    // Executable path --> 1 %s
    // Process status --> 2 %c
    // Virtual Memory Size --> (23-1) vsize  %lu
    // pgrp ID --> equals the process ID of the first member of the process group, called the process group leader
    // tpgid --> The ID of the foreground process group of the controlling terminal of the process.

    char is_fg=' ';
    if(strcmp(Info_Array[4],Info_Array[7])==0)  // pgrp == tpgid --> foreground group leader == current process --> running in foreground
        is_fg='+';

    // printf("%s %s %c",Info_Array[4],Info_Array[7],is_fg);

    strcpy(file_path,"/proc/");
    strcat(file_path,cmd_pid);
    strcat(file_path,"/exe");


    readlink(file_path,ExePath,MAX_STRING_LEN*2);
    strcpy(temp_curr_path,ExePath);

    printf("pid : %s\n",cmd_pid);
    printf("process status : %s%c\n",Info_Array[2],is_fg);
    
    printf("memory : %s {Virtual Memory}\n",Info_Array[22]);

    // CHANGE HOME_PATH TO ~ ==> (if possible)
    if(strncmp(ExePath,Home_path,strlen(Home_path))==0){
        strcpy(ExePath,"~"); 
        temp_curr_path+=strlen(Home_path);
        strcat(ExePath,temp_curr_path);
        temp_curr_path-=strlen(Home_path);
    }
    printf("executable Path : %s\n",ExePath);
}