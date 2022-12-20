#include "headers.h"

// USER DEFINED HEADERS
#include "user_print.h"
#include "echo.h"
#include "PwD.h"
#include "cd.h"
#include "ls.h"
#include "fg.h"
#include "bg.h"
#include "pinfo.h"
#include "discover.h"
#include "history.h"
#include "jobs.h"

extern char Prev_cd_path[MAX_STRING_LEN];   // PREVIOUS PATH USED FOR (cd -)

int fgStatus = 0;
char* curr_dir_path;
user_info USER;
char fgTime[MAX_STRING_LEN];

// FOR AUTOC
void die(const char *s) {
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int cmpfunc1(const void* a, const void* b){
    return strcmp(* (char * const *)a, * (char * const *)b);
}


char* AutoC(char* input,int end,char* saveStr){
    DIR* dirStream=opendir("."); 
    struct dirent* dirEntry;
    struct stat fstat;
    if(dirStream==NULL){
        perror("Program/autoc.c");
        exit(0);
    }

    int ind=0;
    char *dirListN[1000];
    for(int i=0;i<1000;i++){
        dirListN[i]=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
    }
    dirEntry=readdir(dirStream);

    while(dirEntry!=NULL){
        if(strncmp(dirEntry->d_name,input,end)==0){
            strcpy(dirListN[ind],dirEntry->d_name);
            stat(dirEntry->d_name,&fstat);    
            if(S_ISDIR(fstat.st_mode)) strcat(dirListN[ind],"/");
            ind++;
        }
        dirEntry=readdir(dirStream);
    }
    if(closedir(dirStream)==-1){
        perror("Program/autoc.c/closedir:");
        exit(0);
    }
    char* tempname;
    if(ind==0){
        for(int i=0;i<8;i++){           // TABS --> 8 spaces
            printf(" ");
            strcat(saveStr," ");
        }
        return NULL;
    }
    if(ind==1){
        int k=strlen(saveStr);
        for(int i=end;i<strlen(dirListN[0]);i++){
            printf("%c",dirListN[0][i]);
            saveStr[k++]=dirListN[0][i];
        }
        return NULL;
    }
    else if(ind>1){
        qsort(dirListN,ind,sizeof(char*),cmpfunc1);
        printf("\n");
        for(int i=0;i<ind;i++){
            printf("%s\n",dirListN[i]);
        }

        Print_User(USER,curr_dir_path,fgTime);

        int f=1,j=0;
        while(f){
            for(int i=1;i<ind;i++){
                if(dirListN[i][j]!=dirListN[i-1][j]){
                    f=0;
                    break;
                }
            }
            if(!f) break;
            j++;
        }
        tempname = dirListN[0];
        tempname[j] = '\0';
        return tempname;
    }
    return NULL;
}

// FOR AUTOC

void take_input(char* input){
    setbuf(stdout, NULL);
    enableRawMode();
    
    int ind=0;
    char ch;
    char nextStr[1005];
    while(read(STDIN_FILENO, &ch, 1)==1){
        if(iscntrl(ch) && ch==4){           // ctrl+D
            printf("\nLogging Out\n");
            exit(0);
        }
        if(ch=='\t'){
            int end=strlen(input)-1,start,tempstart;
            start=end;
            if(input[end]==' '){
                continue;
            }

            while(input[start]!=' ' && start>=0){
                start--;
            }
            start++;
            tempstart=start;

            char temp[1100]={};
            int k=0;
            while(start<=end){
                temp[k++]=input[start];
                start++;
            }
            char* tempStr=AutoC(temp,strlen(temp),input);
            if(tempStr!=NULL){
                input[tempstart]='\0';
                sprintf(nextStr,"%s%s",input,tempStr);
                strcpy(input,nextStr);
                printf("%s",input);
            }
            ind = strlen(input);
        }
        else if(ch=='\n') break;
        else if (ch==127){             // Back Space
            if(ind>0){
                if(input[ind-1]==9){
                    for(int i=0;i<7;i++){
                        printf("\b");
                    }
                }
                input[--ind] = '\0';
                printf("\b \b");
            }
        }
        else{
            printf("%c",ch);
            input[ind++]=ch;
        }
        // printf("\n\t%sX\tinput\n",input);
    }
    disableRawMode();
    printf("\n");
    // printf("\n%sx\n",input);
}


// Sig/Fg/Bg HANDLER
char path[1137],readStr[1137];
char Info_Array[113][MAX_STRING_LEN];

void SigFgBg(int jobNo,int signo,char* input){
    int found=0,pid=-1;
    for(int i=0;i<bgListSize;i++){
        if(atoi(bgList[i][2])==jobNo){
            sprintf(path,"/proc/%s/stat",bgList[i][1]); // pid = bgList[i][1];
            FILE* fptr=fopen(path,"r");

            if(fptr==NULL){
                continue;   // process exited...
            }

            int ind=0;
            while(fscanf(fptr,"%s",readStr)!=EOF){
                strcpy(Info_Array[ind++],readStr);
            }
            if(fclose(fptr)==EOF){          perror("jobs.c/fclose");return;}
            if(strcmp(input,"sig")==0){
                if(Info_Array[2][0]!='T'){      // Running
                    found=1;
                    pid = atoi(bgList[i][1]);
                     if(kill(pid,signo)<0){
                        perror("sig/kill");
                    }
                }
            }
            else if(strcmp(input,"fg")==0){
                found = 1;
                // Bg to Fg
                setpgid(atoi(Info_Array[0]), getpgid(0));
                signal(SIGTTOU, SIG_IGN);
                signal(SIGTTIN, SIG_IGN);
                tcsetpgrp(STDIN_FILENO, atoi(Info_Array[0]));   // make Bg Process to Fg on stdin
                if(kill(atoi(Info_Array[0]),SIGCONT)<0){        perror("Can't run Process");exit(0);}
                int WaitStatus;
                waitpid(atoi(Info_Array[0]),&WaitStatus,WUNTRACED);
                tcsetpgrp(STDIN_FILENO, getpgid(0));               // again make the Calling process back to Fg
                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL);
            }
            else if(strcmp(input,"bg")==0){
                found = 1;
                if(Info_Array[2][0]=='R')   return;     // Already Running
                //  SIGCONT      P1990      Cont    Continue if stopped - man7
                if(kill(atoi(Info_Array[0]),SIGCONT)<0){        perror("Can't run Process");exit(0);}
            }
            if(found) break;
        }
    }

    if(!found){
        printf("ERROR!! NO job with the given job number is Running\n");
        return;
    }
}
// Sig/Fg/Bg HANDLER


void ctrlCcmd(){
    if(fgStatus==1){                        
        fgStatus = 0;
        if(kill(fgPid,SIGKILL)<0)   { perror("Can't kill Process");}
        strcpy(fgTime,"");
        printf("\n");
    }
    else{   
        printf("^C\n"); 
        Print_User(USER,curr_dir_path,fgTime);
        strcpy(fgTime,"");
    } 
}

void ctrlZcmd(){
    if(fgStatus==1)
    {
        fgStatus = 0;
        printf("\n");
    }
    else{
        printf("^Z\n"); 
        Print_User(USER,curr_dir_path,fgTime);
    }
}


ll token_cmdlist(char* input,char*** cmd_list,int* sep_flag,int* redirFlag)
{
    char** tok_list1 = (char**)malloc(sizeof(char*)*MAX_CMD_COUNT);
    for(int i=0;i<MAX_CMD_COUNT;i++){
      tok_list1[i]=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
    }

    // Number of Commands --> ; or & or |
    ll cmd_ind=0;
    for(ll i=0;i<strlen(input);i++){
        if(input[i]==';'){
            sep_flag[cmd_ind++]=0;
        }
        else if(input[i]=='&'){
            sep_flag[cmd_ind++]=1;
        }
        else if(input[i]=='|'){
            sep_flag[cmd_ind++]=2;
        }
    }
    if(input[strlen(input)-1]=='&'){    // FOR COMMANDS LIKE -->    echo hello& 
        sep_flag[cmd_ind-1]=1;
    }
    // printf("%lld\n",cmd_ind);
    // Number of Commands --> ; or & or |


    ll tok1_ind=0;
    char* token1 = strtok(input,";&|");            // split by ; or & or |
    while(token1!=NULL){
        strcpy(tok_list1[tok1_ind],token1);
        token1 = strtok(NULL,";&|");
        tok1_ind++;
    }

    ll cmd_indj=-1;
    for(ll i=0;i<tok1_ind;i++){                     // split by SPACE/TAB
        char* token4 = strtok(tok_list1[i]," \t");
        cmd_indj=0;
        while(token4!=NULL){
            strcpy(cmd_list[i][cmd_indj],token4);   // cmd_indi == i
            token4 = strtok(NULL," \t");
            cmd_indj++;
        }
    }
    return tok1_ind;//No. of Commands
}

void Print_List(char*** list,int* sep_flag,int* redirFlag,ll list_size)
{   // EXTRA FUNCTION TO PRINT
    for(ll i=0;i<list_size;i++){
        for(ll j=0;strcmp(list[i][j],"")!=0;j++){
            printf("%s|  ",list[i][j]);
        }
        printf("%d|  %d|\n",sep_flag[i],redirFlag[i]);
    }
}

void make_cmd_list_empty(char*** cmd_list,ll cmd_list_size)
{    
    for(ll i=0;i<cmd_list_size;i++){
        for(ll j=0;strcmp(cmd_list[i][j],"")!=0;j++){
            strcpy(cmd_list[i][j],"");
        }
    }
}


int main(){
    char* Home_path=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
    USER    =   get_user_info();

    int check=0;
    char*** cmd_list;
    ll cmd_list_size=-1;
    curr_dir_path=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
    char* input=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
    char* Htemp_input=(char*)malloc(sizeof(char)*MAX_STRING_LEN); // FOR HISTORY STORAGE
    memset(fgTime,'\0',1000);

    //  ALLOCATE MEMORY FOR COMMAND LIST
    cmd_list=(char***)malloc(sizeof(char**)*MAX_CMD_COUNT);
    for(ll i=0;i<MAX_CMD_COUNT;i++){
      cmd_list[i]=(char**)malloc(sizeof(char*)*MAX_STRING_LEN);
      for(ll j=0;j<MAX_STRING_LEN;j++){
        cmd_list[i][j]=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
      }
    }
    int* sep_flag = (int*)malloc(sizeof(int)*MAX_CMD_COUNT);   // TO HANDLE & AND ;


    // BACK GROUND
    bgList    = (char***)malloc(sizeof(char**)*1000);          // 1000 PROCESSES MAX
    for(int i=0;i<1000;i++){
        bgList[i] = (char**)malloc(sizeof(char*)*3);
        for(int j=0;j<3;j++){
            bgList[i][j] = (char*)malloc(sizeof(char)*500);
        }
    }
    
    bgListSize = 0;
    // BACK GROUND


    // INPUT/OUTPUT REDIRECTION
    int* redirFlag=(int*)malloc(sizeof(int)*MAX_CMD_COUNT);

    int bgInd=1;
    time_t startTime,endTime;

    getcwd(Home_path,MAX_STRING_LEN);
    strcpy(curr_dir_path,"~");   // START WITH HOME DIRECTORY
    strcpy(Prev_cd_path,Home_path);


    // CREATE .History.txt 
    char H_fPath[MAX_STRING_LEN];
    strcpy(H_fPath,Home_path);
    strcat(H_fPath,"/.History.txt");
    FILE* fptr=fopen(H_fPath, "a+");
    if(fptr==NULL){
        perror("Program/main.c/File Reading");
        return 0;
    }
    if(fclose(fptr)==EOF){
        perror("Program/main.c/fclose");
        return 0;
    }
    // CREATE .History.txt
    
    signal(SIGCHLD,checkForBgProcess);
    signal(SIGINT,ctrlCcmd);
    // signal(SIGTSTP,ctrlZcmd);

    fgPid = getpid(); // current process pid
    while(1){
        make_cmd_list_empty(cmd_list,cmd_list_size);
        for(ll i=0;i<MAX_CMD_COUNT;i++)     sep_flag[i]=0;         // Assume all Processes to be foreground.
        for(ll i=0;i<MAX_CMD_COUNT;i++)     redirFlag[i]=0;        // NO I/0 REDIRECT intially
        Print_User(USER,curr_dir_path,fgTime);
        memset(input,'\0',1000);
        take_input(input);
        // continue;
        // scanf("%[^\n]s",input);
        // while(getchar()!='\n');          // input buffer may have something in it after i/o redirection

        time(&startTime);
        strcpy(Htemp_input,input);       // USED FOR STORING IN HISTORY
        History(Htemp_input,Home_path);
        cmd_list_size=token_cmdlist(input,cmd_list,sep_flag,redirFlag);
        // Print_List(cmd_list,sep_flag,redirFlag,cmd_list_size);
        int Pf = 0;
        int firstPipe = 0,lastPipe = 0;
        int isPipe = 0;

        if(sep_flag[0]==2) isPipe = 1;

        for(ll i=0;i<cmd_list_size;i++){
            char Cmd_name[MAX_STRING_LEN];
            strcpy(Cmd_name,cmd_list[i][0]);


            // pipe
            int stdoutFd,stdinFd,INfd,OUTfd;
            int IOstdoutFd,IOstdinFd;

            int file_des[2][2];
            int iofd[2];
            int fd1,fd2,fd3;
            fd1=fd2=fd3=0;
            iofd[0]=0;
            iofd[1]=0;

            lastPipe = (sep_flag[i]!=2 && i>0 && sep_flag[i-1]==2);
            
            if((sep_flag[i]==2 || lastPipe) && isPipe){
                if(!firstPipe) firstPipe=1;
                else           firstPipe=-1;

                if(!lastPipe){
                    if(pipe(file_des[i%2])<0){
                        perror("Error!!");
                        return 0;
                    }
                    iofd[1] = file_des[i%2][1];
                    stdoutFd = dup(STDOUT_FILENO);
                    if(dup2(iofd[1],1)<0)  {           perror("main.c/dup2");exit(0);}
                }
                if(firstPipe==-1){
                    iofd[0] = file_des[1-i%2][0];
                    stdinFd  = dup(STDIN_FILENO);
                    if(dup2(iofd[0],0)<0)  {           perror("main.c/dup2");exit(0);}
                }
            }
            // pipe


            // I/0 Redirection  
            int cnt=0;                          // if(cnt>0) then I/O redirection
            int j=0;
            IOstdinFd  = dup(STDIN_FILENO);
            IOstdoutFd = dup(STDOUT_FILENO);
            while(strcmp(cmd_list[i][j],"")!=0){
                int flag_io=0;
                if(!strcmp(cmd_list[i][j],"<")){
                    fd3 = open(cmd_list[i][j+1], O_RDONLY);
                    if(fd3==-1){                   perror("main.c/IOredirect/open");exit(0);}
                    if(dup2(fd3,0)==-1){           perror("main.c/IOredirect/dup2");exit(0);}
                    flag_io=1;
                }
                else if(!strcmp(cmd_list[i][j],">")){
                    fd2 = open(cmd_list[i][j+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if(fd2==-1){                   perror("main.c/IOredirect/open");exit(0);}
                    if(dup2(fd2,1)==-1){           perror("main.c/IOredirect/dup2");exit(0);}
                    flag_io=1;
                }
                else if(!strcmp(cmd_list[i][j],">>")){
                    fd1 = open(cmd_list[i][j+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if(fd1==-1){                   perror("main.c/IOredirect/open");exit(0);}
                    if(dup2(fd1,1)==-1){           perror("main.c/IOredirect/dup2");exit(0);}
                    flag_io=1;
                }

                if(flag_io){
                    cnt++;
                    strcpy(cmd_list[i][j],"");
                    strcpy(cmd_list[i][j+1],"");
                    j++;
                }
                if(cnt>2){
                    fprintf(stderr,"More than 2 Files are given as input\n");
                    return 0;
                }
                j++;
            }
            // I/0 Redirection

            if(j==0){
                continue;
            }
            
            if(strcmp(Cmd_name,"echo")==0){
                Echo(cmd_list[i]);
            }
            else if(strcmp(Cmd_name,"pwd")==0){
                Pwd();
            }
            else if(strcmp(Cmd_name,"cd")==0){
                Cd(cmd_list[i],curr_dir_path,Home_path);
            }
            else if(strcmp(Cmd_name,"ls")==0){
                Ls(cmd_list[i],curr_dir_path,Home_path);
            }
            else if(strcmp(Cmd_name,"history")==0){
                // PASS --> ALREADY EXECUTED
            }
            else if(strcmp(Cmd_name,"pinfo")==0){
                Pinfo(cmd_list[i][1],Home_path);
            }
            else if(strcmp(Cmd_name,"discover")==0){
                Discover(cmd_list[i],Home_path);
            }
            else if(strcmp(Cmd_name,"exit")==0){
                exit(0);
            }
            else if(strcmp(Cmd_name,"jobs")==0){
                Jobs(cmd_list[i][1]);
            }
            else if(!strcmp(Cmd_name,"sig") || !strcmp(Cmd_name,"fg") || !strcmp(Cmd_name,"bg")){
                SigFgBg(atoi(cmd_list[i][1]),atoi(cmd_list[i][2]),Cmd_name);
            }
            else{
                fgStatus = 1;
                int is_bg=0;
                if(sep_flag[i]==1){
                    check=bg(cmd_list[i],bgInd);
                    is_bg=1;
                    fgStatus = 0;
                }
                else{
                    fg(cmd_list[i]);
                }

                if(check==-1)
                    printf("%s: COMMAND NOT FOUND\n",Cmd_name);
                else{
                    if(is_bg)
                        bgInd++;
                }
            }
            
            if(dup2(IOstdinFd,STDIN_FILENO)<0)  {           perror("main.c/IOredirect/dup2");exit(0);}
            if(dup2(IOstdoutFd,STDOUT_FILENO)<0){           perror("main.c/IOredirect/dup2");exit(0);}
            if(close(IOstdinFd)<0){            perror("main.c/IOredirect/close");exit(0);}
            if(close(IOstdoutFd)<0){           perror("main.c/IOredirect/close");exit(0);}
        
            if(iofd[0]){
                if(dup2(stdinFd,STDIN_FILENO)<0)  {           perror("main.c/dup2");exit(0);}
                if(close(stdinFd)<0){            perror("main.c/close");exit(0);}
            }

            if(iofd[1]){
                if(dup2(stdoutFd,STDOUT_FILENO)<0){           perror("main.c/dup2");exit(0);}
                if(close(stdoutFd)<0){           perror("main.c/close");exit(0);}
            }
            
            if(!lastPipe && isPipe){      
                if(close(file_des[i%2][1])<0){           perror("main.c/close");exit(0);}
            }

            if(firstPipe==-1 && isPipe){
                if(close(file_des[1-i%2][0])<0){           perror("main.c/close");exit(0);}
            }
        }
        time(&endTime);
        double timeTaken = difftime(endTime,startTime);
        if(timeTaken>=1){
            sprintf(fgTime,"took %ds",(int)timeTaken);
        }
        else    strcpy(fgTime,"");
    }
    return 0;   
}
