#include "headers.h"
#include "history.h"

static char H_list[21][MAX_STRING_LEN+100];
static char lineStr[MAX_STRING_LEN+100];
static char HfileStr[(MAX_STRING_LEN*21+31)];
static char H_fPath[MAX_STRING_LEN];
static FILE* fptr;

void History(char* Hcmd,char* Home_path){
    if(strcmp(Hcmd,"")==0) return;              // DON'T SAVE ON PRESSING ENTER!!
    int ind=0,check=0; 
    int indP=-1;                    // Previous Element with LEAST index PRIORITY - indP
    strcpy(H_list[0],"-1");         // indP INTILIASING
    strcpy(H_fPath,Home_path);
    strcat(H_fPath,"/.History.txt");

    // READ
    fptr=fopen(H_fPath, "r");
    if(fptr==NULL){
        perror("Program/history.c/File Reading");
        return;
    }
    while(fscanf(fptr, "%[^\n]s", H_list[ind++])!=EOF){
        if(fgetc(fptr)==EOF)     break;
    }
    if(fclose(fptr)==EOF){
        perror("Program/history.c/fclose");
        return;
    }
    // READ

    // ind --> TOTAL NO OF COMMANDS IN .History.txt
    if(strcmp(H_list[0],"-1")!=0)           
        indP = atoi(H_list[0]);
    else                                    // EMPTY FILE
        indP = 0;

    int is_return=0;
    if(strcmp(H_list[indP],Hcmd)==0)       // COMMAND SAME AS JUST PREVIOUS COMMAND
        is_return=1;
    else{
        if(indP==20)
            indP=1;
        else
            indP+=1;
        strcpy(H_list[indP],Hcmd);
    }
    if(strcmp(Hcmd,"history")==0){
        // PRINT LATEST 10 COMMANDS
        int cnt=0;
        if(ind<10){
            for(int i=1;i<=indP;i++){
                printf("%s\n",H_list[i]);
            }
        }
        else if((indP-10+1)>=1){
            for(int i=indP-10+1;i<=indP;i++){
                printf("%s\n",H_list[i]);
            }
        }
        else{
            int remains=10-indP-1;
            for(int i=20-remains;i<=20;i++){
                printf("%s\n",H_list[i]);
            }
            for(int i=1;i<=indP;i++){
                printf("%s\n",H_list[i]);
            }
        }
    }
    if(is_return) return;
    
    // printf("INDEX: %d\n",indP);
    sprintf(H_list[0],"%d",indP);

    // WRITE
    fptr=fopen(H_fPath, "w");
    if(fptr==NULL){
        perror("Program/history.c/File Reading");
        return;
    }
    for(int i=0;i<ind && i<=20;i++){
        fprintf(fptr,"%s\n",H_list[i]);
    }
    if(fclose(fptr)==EOF){
        perror("Program/history.c/fclose");
        return;
    }
    // WRITE
}