#include "headers.h"
#include "ls.h"
#include "user_print.h"

int is_not_flag(char* cmd){
    return !(strcmp(cmd,"-a")==0 || strcmp(cmd,"-l")==0 || strcmp(cmd,"-al")==0 || strcmp(cmd,"-la")==0);
}

int cmpfunc(const void* a, const void* b){
  return strcasecmp((*(const char***)a)[9],(*(const char***)b)[9]);
}

static char temp_path1[MAX_STRING_LEN],temp_path2[MAX_STRING_LEN*3];
int check;  // check for -1/ILLEGAL CASES


char get_file_type(struct stat fstat){
    char ftype;
    if(S_ISREG(fstat.st_mode))           ftype='-';
    else if(S_ISDIR(fstat.st_mode))      ftype='d';
    else if(S_ISCHR(fstat.st_mode))      ftype='c';
    else if(S_ISBLK(fstat.st_mode))      ftype='b';
    else if(S_ISFIFO(fstat.st_mode))     ftype='p';
    else if(S_ISLNK(fstat.st_mode))      ftype='l';
    else if(S_ISSOCK(fstat.st_mode))     ftype='s';
    return ftype;
}

void get_permissions(struct stat fstat,char* Perm){
    strcpy(Perm,"---------");
    // ONLY & BECAUSE THEY ARE MASKS NOT FLAGS
    if(fstat.st_mode & S_IRUSR) Perm[0]='r';    
    if(fstat.st_mode & S_IWUSR) Perm[1]='w';
    if(fstat.st_mode & S_IXUSR) Perm[2]='x';

    if(fstat.st_mode & S_IRGRP) Perm[3]='r';
    if(fstat.st_mode & S_IWGRP) Perm[4]='w';
    if(fstat.st_mode & S_IXGRP) Perm[5]='x';

    if(fstat.st_mode & S_IROTH) Perm[6]='r';
    if(fstat.st_mode & S_IWOTH) Perm[7]='w';
    if(fstat.st_mode & S_IXOTH) Perm[8]='x';    
}

void process_Out(char*** Buff,ll col,ll ind,ll len_required){  // col is from 2 to 8
    char temp_str[MAX_STRING_LEN];
    for(ll i=0;i<ind;i++){
        strcpy(temp_str,"");
        ll lenStr = ((ll)strlen(Buff[i][col]));
        while((len_required-lenStr)>0){
            strcat(temp_str," ");
            lenStr++;
        }
        strcat(temp_str,Buff[i][col]);
        strcpy(Buff[i][col],temp_str);
    }
}

void make_Output(char ftype,char* P,ll flinks,char* pwName,char* gpwName,ll fsize,char* b_month,char* b_date,char* b_time,char* dName,char** Buff){
    sprintf(Buff[0],"%c",ftype);
    sprintf(Buff[1],"%s",P);
    sprintf(Buff[2],"%lld",flinks);
    sprintf(Buff[3],"%s",pwName);
    sprintf(Buff[4],"%s",gpwName);
    sprintf(Buff[5],"%lld",fsize);
    sprintf(Buff[6],"%s",b_month);
    sprintf(Buff[7],"%s",b_date);
    sprintf(Buff[8],"%s",b_time);
    sprintf(Buff[9],"%s",dName);
}

void handle_file(char* fileName,int is_l,int is_a,int is_gt1){
    struct passwd *UserPwd;
    struct group *GroupPwd;
    struct tm* tmptr = (struct tm *) malloc(sizeof(struct tm));
    struct stat file_stat;                  // STATUS OF FILE/DIRECTORY 
    char b_month[100],b_date[100],b_time[100],Permissions[10];

    char tempPath[500],tempfilePath[1000];
    getcwd(tempPath,500);
    sprintf(tempfilePath,"%s/%s",tempPath,fileName);
    stat(tempfilePath,&file_stat);        
    UserPwd = getpwuid(file_stat.st_uid);
    GroupPwd = getgrgid(file_stat.st_gid);

    char ftype = get_file_type(file_stat);
    
    get_permissions(file_stat,Permissions);
    tmptr = localtime(&(file_stat.st_mtime));   // LAST MODIFIED
    strftime(b_month,MAX_STRING_LEN,"%b",tmptr);
    strftime(b_date,MAX_STRING_LEN,"%d",tmptr);
    strftime(b_time,MAX_STRING_LEN,"%H:%M",tmptr);
    // make_Output(ftype,Permissions,(ll)file_stat.st_nlink,UserPwd->pw_name,GroupPwd->gr_name,(ll)file_stat.st_size,b_month,b_date,b_time,fileName,storeStr);

    if(!is_a && fileName[0]=='.') return;
    if(is_gt1)
        printf("%s:\n",fileName);
    if(is_l){
        printf("%c%s %lld %s %s %lld %s %s %s %s\n",ftype,Permissions,(ll)file_stat.st_nlink,UserPwd->pw_name,GroupPwd->gr_name,(ll)file_stat.st_size,b_month,b_date,b_time,fileName);
    }
    else{
        printf("%s\n",fileName);
    }
}

void Ls(char** cmd_txtArr,char* curr_dir_path,char* Home_path){
    struct dirent* dir_entry;               // ENTRY'S IN THE DIRECTORY STREAM OF OPENDIR()
    DIR* dir_stream;                        // DIRECTORY STREAM
    struct passwd *UserPwd;
    struct group *GroupPwd;
    struct tm* tmptr = (struct tm *) malloc(sizeof(struct tm));
    struct stat file_stat;                  // STATUS OF FILE/DIRECTORY 
    char b_month[100],b_date[100],b_time[100],Permissions[10];

    int is_l=0,is_a=0,is_gt1=0;             // is_greater_than1 args
    ll cmd_cnt=0;
    for(ll i=1;strcmp(cmd_txtArr[i],"")!=0;i++){
        if(strcmp(cmd_txtArr[i],"-a")==0) is_a=1;
        else if(strcmp(cmd_txtArr[i],"-l")==0) is_l=1;
        else if(strcmp(cmd_txtArr[i],"-al")==0)  is_a=is_l=1;
        else if(strcmp(cmd_txtArr[i],"-la")==0)  is_a=is_l=1;
        else    cmd_cnt++;
    }
    
    char*** Output;
    Output=(char***)malloc(sizeof(char**)*MAX_CMD_COUNT);
    for(ll i=0;i<MAX_CMD_COUNT;i++){
      Output[i]=(char**)malloc(sizeof(char*)*11);
      for(ll j=0;j<10;j++){                     // ONLY 10 Elements in each line
        Output[i][j]=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
      }
    }

    if(cmd_cnt>1){
        is_gt1 = 1;
    }
    for(ll i=1;strcmp(cmd_txtArr[i],"")!=0 || !cmd_cnt;i++){           // i==0 is "ls"
        ll ind=0,blk_total=0;
        if(is_not_flag(cmd_txtArr[i]) || !cmd_cnt){
            if(!cmd_cnt){
                dir_stream=opendir(".");            // NO DIRECTORY INPUT
                getcwd(temp_path1,MAX_STRING_LEN);
                cmd_cnt=1;                          // CONSIDER ONLY ONCE
            }
            else{
                if(strncmp(cmd_txtArr[i],"~",1)==0){
                    strcpy(temp_path1,Home_path);
                    cmd_txtArr[i]+=1;
                    strcat(temp_path1,cmd_txtArr[i]);
                    dir_stream=opendir(temp_path1);
                    strcpy(cmd_txtArr[i],temp_path1);
                    cmd_txtArr[i]-=1;

                }
                else{
                    dir_stream=opendir(cmd_txtArr[i]);  
                    strcpy(temp_path1,cmd_txtArr[i]);
                }
            }

            if(dir_stream==NULL){
                check=stat(temp_path1,&file_stat);
                if(check==-1)
                    perror("Program/ls.c");
                else
                    handle_file(cmd_txtArr[i],is_l,is_a,is_gt1);
                // printf("%s\n",cmd_txtArr[i]);
                continue;
            }

            dir_entry=readdir(dir_stream);
            
            while(dir_entry!=NULL){
                sprintf(temp_path2,"%s/%s",temp_path1,dir_entry->d_name);

                stat(temp_path2,&file_stat);        
                UserPwd = getpwuid(file_stat.st_uid);
                GroupPwd = getgrgid(file_stat.st_gid);

                char ftype = get_file_type(file_stat);
                
                get_permissions(file_stat,Permissions);
                tmptr = localtime(&(file_stat.st_mtime));   // LAST MODIFIED
                strftime(b_month,MAX_STRING_LEN,"%b",tmptr);
                strftime(b_date,MAX_STRING_LEN,"%d",tmptr);
                strftime(b_time,MAX_STRING_LEN,"%H:%M",tmptr);

                if((dir_entry->d_name)[0]!='.'){
                    blk_total += file_stat.st_blocks;
                    make_Output(ftype,Permissions,(ll)file_stat.st_nlink,UserPwd->pw_name,GroupPwd->gr_name,(ll)file_stat.st_size,b_month,b_date,b_time,dir_entry->d_name,Output[ind++]);
                }
                if(is_a && (dir_entry->d_name)[0]=='.'){    // hidden files
                    blk_total += file_stat.st_blocks;
                    make_Output(ftype,Permissions,(ll)file_stat.st_nlink,UserPwd->pw_name,GroupPwd->gr_name,(ll)file_stat.st_size,b_month,b_date,b_time,dir_entry->d_name,Output[ind++]);
                }
                dir_entry=readdir(dir_stream);
            }
            check=closedir(dir_stream);

            if(check==-1){
                perror("Program/ls.c/closedir:");
            }
            qsort(Output,ind,sizeof(char**),cmpfunc);

            // ADD SPACING
            if(is_l){
                for(ll j=2;j<9;j++){
                    if(j==6) continue;
                    ll max_length=-1;
                    for(ll i=0;i<ind;i++){
                        ll lenStr = ((ll)strlen(Output[i][j]));
                        if(max_length<lenStr){
                            max_length=lenStr;
                        }
                    }
                    process_Out(Output,j,ind,max_length);
                }
            }
            // ADD SPACING

            if(is_gt1){
                printf("%s:\n",cmd_txtArr[i]);
            }
            if(is_l)
                printf("total %lld\n",blk_total/2);
            for(ll i=0;i<ind;i++){
                if(is_l){
                    printf("%s",Output[i][0]);
                    for(ll j=1;j<9;j++){
                        printf("%s ",Output[i][j]);
                    }
                }
                if(Output[i][0][0]=='d'){
                    bold_color();blue_color();
                    printf("%s\n",Output[i][9]);
                    reset_color();
                }
                else if(Output[i][1][2]=='x'){
                    bold_color();green_color();
                    printf("%s\n",Output[i][9]);
                    reset_color();
                }
                else{
                    bold_color();
                    printf("%s\n",Output[i][9]);
                    reset_color();
                }
            }
            
        }
    }
    
}