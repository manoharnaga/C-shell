#include "headers.h"
#include "echo.h"


void Echo(char** cmd_txtArr){
    char OutputString[MAX_STRING_LEN]={};
    ll ind=1;
    strcat(OutputString,cmd_txtArr[ind]);
    ind++;
    while(strcmp(cmd_txtArr[ind],"")!=0){
        strcat(OutputString," ");
        strcat(OutputString,cmd_txtArr[ind]);
        ind++;
    }
    printf("%s\n",OutputString);
}
