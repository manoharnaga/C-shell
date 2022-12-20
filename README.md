### Operating Systems and Networks - Course Assignment
### ***This is an Interactive Shell program in C language with the functionalities written in c-shell1.pdf,c-shell2.pdf***

        typedef long long ll         -     is used in all 3 questions.
        MAX COMMAND COUNT IN 1 LINE  -     100
        MAX STRING LENGTH            -     1000
        CHANGED DIRECTORY PATH       -     cd_path
        Home_path                    - Path where Shell Starts(cwd)

### **Compilation Format:**

    -   make main
    -   ./a.out


## Description of Files and there functionalities:

- user_print.c
	- Print Terminal Name like -    ***<manohar@manohar-Victus:~>$***
	- COLOR CODE FUNCTIONS
- main.c
	- It has all other file functionalities accessible.
        Command tokenizing is implemented in it.
- echo.c
	- echo functionality accounting for SPACES and TABS
- PwD.c
	- Print current Working Directory PATH
- cd.c 
	- Change Directory
- ls.c
	- Color coding:
		- Executables - Green		
		- DIR			- Blue
		- Others      - White
	- Here the ROWS are sorted using the FILE/DIR name with ***strcasecmp***

- fg.c
	- Execute the Process in foreground using execvp excluding the commands that are implemented.
	- TIME TAKEN : Prints the sum of time taken by all the processes
- bg.c 
	- Execute the Process in background and inform USER about EXIT STATUS by causing an interrupt 		to 	the current foreground process

- pinfo.c
	- Print pinfo for current process/given pid

- discover.c
	- prints the FILE/DIR FULL PATH if found, And lists all SUB-DIRS/FILES inside it.
	- Hidden files are NOT handled.
	- MAX NUMBER OF FILES/DIRS IN A FOLDER 1000

- history.c
	- Print Last 10 commands executed by the USER 
	- history command is given ALONE
	- STORE Last 20 commands in a HIDDEN file - .History.txt in Home_path
- I/O - REDIRECTION
	- I Assume that there is spacing between i/o redirection symbols (<,>,>>) 
- Auto complete 
	- I have used the sample main.c code given for Assignment2 for Autosuggestions, for taking input.
- bg.c - **Update**
	- the BACKGROUND numbering increases continuously as we keep getting new background jobs.
	- MAX NUMBER OF JOBS ALLOWED == 1000 
- jobs.c
	- sorted with qsort --> strcasecmp 









# c_shell
