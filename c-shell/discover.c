#include "headers.h"
#include "discover.h"

static int file_not_found;

void recur(char* currDirPath,char* currDirName,char* findThis,char* Home_path,int is_f,int is_d,int is_found){
    DIR* dirStream=opendir(currDirPath); 
    struct dirent* dirEntry;
    struct stat fstat;
    
    char *dirListPath[1000],*dirListN[1000];
    for(int i=0;i<1000;i++){
        dirListPath[i]=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
        dirListN[i]=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
    }

    int check;
    int ind=0;
    if(strcmp(findThis,currDirName)==0) is_found=1;         // findThis is a FILE/DIR
    

    if(dirStream==NULL){                // FILE CHECK
        check=stat(currDirPath,&fstat);
        if(check==-1)                   // NO SUCH FILE
            perror("Program/discover.c");
        else{                           // FILE EXISTS
            if(is_f && is_found){
                if(currDirPath[0]!='.' && strncmp(currDirPath,Home_path,strlen(Home_path))!=0){
                    printf("./");
                }
                printf("%s\n",currDirPath);
            }
        }
        return;
    }
    
    if(is_d && is_found){           // IS DIR
        if(currDirPath[0]!='.' && strncmp(currDirPath,Home_path,strlen(Home_path))!=0){
            printf("./");
        }
        printf("%s\n",currDirPath);
    }

    file_not_found = is_found;

    dirEntry=readdir(dirStream);    
    while(dirEntry!=NULL){
        strcpy(dirListPath[ind],currDirPath);
        if(currDirPath[strlen(currDirPath)-1]!='/') strcat(dirListPath[ind],"/");
        strcat(dirListPath[ind],dirEntry->d_name);
        strcpy(dirListN[ind],dirEntry->d_name);
        dirEntry=readdir(dirStream);
        ind++;
    }
    check=closedir(dirStream);
    if(check==-1){
        perror("Program/discover.c/closedir:");
    }

    for(int i=0;i<ind;i++){         // RECURSIVELY CALL THE FILE/DIR IN THE CURR DIR
        if(strcmp(dirListN[i],".")==0 || strcmp(dirListN[i],"..")==0) continue;
        recur(dirListPath[i],dirListN[i],findThis,Home_path,is_f,is_d,is_found);
    }
}

int is_home(char* path){
    return (strcmp(path,".")==0 || strcmp(path,"./")==0 || strcmp(path,"~")==0 || strcmp(path,"~/")==0);
}
void Discover(char** cmd_txtArr,char* Home_path){
    file_not_found = 0;
    char currDirPath[10000]={},currDirName[1000]={},tempPath[300]={},findThis[300]={};

    int is_f,is_d,is_found;
    is_d=is_f=is_found=0;
    int i;
    for(i=1;strcmp(cmd_txtArr[i],"")!=0;i++){
        if(strcmp(cmd_txtArr[i],"-f")==0) is_f=1;
        else if(strcmp(cmd_txtArr[i],"-d")==0) is_d=1;
    }
    
    // QUOTES 
    int is_quotes=0;
    if(i>1 && cmd_txtArr[i-1][0]=='"' && cmd_txtArr[i-1][strlen(cmd_txtArr[i-1])-1]=='"'){
        is_quotes=1;
        cmd_txtArr[i-1]+=1;
        cmd_txtArr[i-1][strlen(cmd_txtArr[i-1])-1]='\0';
        strcpy(findThis,cmd_txtArr[i-1]);
    }
    // QUOTES 

    if(i==1 || strcmp(cmd_txtArr[1],"-f")==0 || strcmp(cmd_txtArr[1],"-d")==0 || (is_quotes && i==2)){
        strcpy(currDirPath,".");
        strcpy(currDirName,".");
    }
    else{
        if(cmd_txtArr[1][0]=='~'){
            strcpy(currDirPath,Home_path);
            cmd_txtArr[1]+=1;
            strcat(currDirPath,cmd_txtArr[1]);
            cmd_txtArr[1]-=1;
            strcpy(currDirName,currDirPath);
        }
        else{
            strcpy(currDirPath,cmd_txtArr[1]);
            strcpy(currDirName,cmd_txtArr[1]);
        }
    }

    if(is_f==0 && is_d==0)
        is_f=is_d=1;

    if(is_quotes){
        cmd_txtArr[i-1]-=1;
    }

    if(!is_quotes){
        is_found=1;
    }

    // printf("%d %d %s %s %s\n",is_f,is_d,findThis,currDirName,currDirPath);
    recur(currDirPath,currDirName,findThis,Home_path,is_f,is_d,is_found);
    if(file_not_found==0){
        printf("Program/discover.c/No such file\n");
    }
}