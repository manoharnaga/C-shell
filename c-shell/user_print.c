#include "headers.h"
#include "user_print.h"

void black_color() {
  printf("\033[1;30m");
}

void red_color() {
  printf("\033[1;31m");
}

void green_color() {
  printf("\033[1;32m");
}

void yellow_color() {
  printf("\033[1;33m");
}

void blue_color() {
  printf("\033[1;34m");
}

void purple_color() {
  printf("\033[1;35m");
}

void bold_color() {
  printf("\e[1m");
}

void reset_color() {
  printf("\033[0m");
}

user_info get_user_info(){
  user_info USER;
  USER.name=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
  USER.node_name=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
  uid_t uid;
  struct passwd* user_pw;  
  char* HostName=(char*)malloc(sizeof(char)*MAX_STRING_LEN);
  uid = geteuid();
  user_pw = getpwuid(uid);
  if(user_pw==NULL){
    perror("Program/user_print.c/user_pw:");
  }

  int uname_test = gethostname(HostName,MAX_STRING_LEN);
  if(uname_test==-1){
    perror("Program/user_print.c/user_uts:");
  }
  strcpy(USER.name,user_pw->pw_name);
  strcpy(USER.node_name,HostName);
  return USER;
}


void Print_User(user_info USER,char* curr_dir_path,char* fgTime){
  red_color();
  printf("<%s@%s",USER.name,USER.node_name);
  reset_color();
  printf(":");
  blue_color();
  printf("%s%s>",curr_dir_path,fgTime);
  reset_color();
  printf("$ ");
}

