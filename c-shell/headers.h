#include <stdio.h>    
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <errno.h>       // ERROR CHECK
#include <sys/types.h>   // opendir,
#include <sys/utsname.h> // UNAME,
#include <pwd.h>        
#include <unistd.h>      // getuid,getcwd(current working directory),chdir(change directory),fork(),execvp(),dup(),dup2()
#include <dirent.h>      // opendir,readdir
#include <sys/stat.h>    
// struct stat,st_size(Total size in bytes),stat.st_nlink(hard_link_count),stat.st_mode(File Type)
#include <time.h>        // struct tm,strftime,time_t(Total time in seconds),
#include <fcntl.h>       // open(),dup(),dup2()
#include <grp.h>         // getgrgid(),grp->gr_name
#include <sys/wait.h>    // wait(),

#include <termios.h>     
#include <ctype.h>       

#define MAX_STRING_LEN 1000
#define MAX_CMD_COUNT 1000
#define MAX_NO_DIR 100   // MAXIMUM NUMBER OF SUB DIRECTORIES
#define DELIM1 ";"
#define DELIM2 "&"
#define DELIM3 " \t"
#define DELIM4 "/"

typedef long long ll;
int fgPid;
char*** bgList;
int bgListSize;
// bgName --> bgList[0]
// bgPid  --> bgList[1]
// bgInd  --> bgList[2]


// NOT WORKING WITH SPEED jobs.c
