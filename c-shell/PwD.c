#include "headers.h"
#include "PwD.h"

void Pwd(){
    char curr_dir[MAX_STRING_LEN];
    void* check=getcwd(curr_dir,MAX_STRING_LEN);
    if(check==NULL){
        printf("Program/pwd.c:\tNULL return!!!\n");
    }
    else{
        printf("%s\n",curr_dir);
    }
}
