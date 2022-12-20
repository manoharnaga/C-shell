#include "headers.h"
#include "cd.h"

char Prev_cd_path[MAX_STRING_LEN];

void Cd(char** cmd_Arr,char* curr_dir_path,char* Home_path){
    char Args[MAX_STRING_LEN],temp_Args[MAX_STRING_LEN],temp_half_path[MAX_STRING_LEN]={};
    char cd_path[MAX_STRING_LEN]={};
    char* temp_curr_path=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
    char* temp_Prev_cd_path=(char*)malloc(sizeof(char)*MAX_STRING_LEN);

    if(strcmp(cmd_Arr[1],"-")!=0){
        getcwd(Prev_cd_path,MAX_STRING_LEN);     // PATH BEFORE  cd command
    }
    
    if(strcmp(cmd_Arr[1],"")==0){ // cd
        //HOME
        chdir(Home_path);
        strcpy(curr_dir_path,"~");
        return;
    }
    strcpy(Args,cmd_Arr[1]);
    strcpy(temp_Args,Args);

    if(strcmp(cmd_Arr[2],"")!=0){
        printf("Cd :\tTOO MANY ARGUMENTS\n");
        return;
    }
    int dir_check=0;
    if(strcmp(Args,"-")==0){
        getcwd(temp_Prev_cd_path,MAX_STRING_LEN);
        printf("%s\n",Prev_cd_path);
        dir_check=chdir(Prev_cd_path);              //Empty Initially
        strcpy(Prev_cd_path,temp_Prev_cd_path);     // PATH BEFORE  "cd -" command
    }
    else if(strcmp(Args,"~")==0){
        dir_check=chdir(Home_path);
    }
    else{
        getcwd(cd_path,MAX_STRING_LEN);
        strcpy(temp_curr_path,cd_path);
        
        char* token_argNew = strtok(Args,DELIM4);
        if(strcmp(token_argNew,"~")==0){
            token_argNew=strtok(NULL,DELIM4);
            strcpy(cd_path,Home_path);
            while(token_argNew!=NULL){
                strcat(cd_path,"/");
                strcat(cd_path,token_argNew);
                token_argNew=strtok(NULL,DELIM4);
            }
        }
        else{
            strcpy(cd_path,temp_Args);
        }
        dir_check=chdir(cd_path);
        
    }
    if(dir_check==-1){
        perror("Program/cd.c/chdir:");
        return;
    }

    getcwd(curr_dir_path,MAX_STRING_LEN);   

    // CHANGE HOME_PATH TO ~ ==> (if possible)
    getcwd(temp_curr_path,MAX_STRING_LEN);  // ABSOLUTE PATH
    if(strncmp(curr_dir_path,Home_path,strlen(Home_path))==0){
        strcpy(curr_dir_path,"~"); 
        temp_curr_path+=strlen(Home_path);
        strcat(curr_dir_path,temp_curr_path);
        temp_curr_path-=strlen(Home_path);
    }

}