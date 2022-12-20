#ifndef _USER_HOST_H
#define _USER_HOST_H

typedef struct User_Info{
    char* name;
    char* node_name;
} user_info;


user_info get_user_info();
void Print_User(user_info USER,char* curr_dir_path,char* fgTime);

// COLORS
void black_color();
void red_color();
void green_color();
void yellow_color();
void blue_color();
void purple_color();
void bold_color();
void reset_color();

#endif