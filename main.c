/* Anthony Kim
   CSC345-02
   2/22/2019
   Project 1
*/

#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define MAX_ARGS 50
#define MAX_CMD_LEN 100


void cd(char* dest);
int fork_p(char** args1, char** args2 );
void splitCmd(char* cmd, char** args);
int exeCmd(char** args, int back, int in, int out, char* ifile);

int main(void){
	char *args[MAX_ARGS];
	char *args2[MAX_ARGS];
	char cmd[MAX_CMD_LEN];
	char rec[MAX_CMD_LEN] = "";
	char *end;
	char *token;
	const char del[] = " | ";
	while (1){
		int pipe = 0;
		int in = 0;
		int out = 0;
		int back = 0;		
		printf("osh>");
		//get input from user put into cmd
		if(fgets(cmd, sizeof(cmd), stdin) == NULL) break;
		
		if(cmd[strlen(cmd)-1] == '\n'){
			cmd[strlen(cmd)-1] = '\0';
		}
		//split command by space
		//check if command was "!!"	
		//uses pointers to seperate commands then calls function to create arguments
		int repeat = 0;
		if((strcmp(cmd, "!!") == 0) && strlen(rec) == 0){
			printf("No commands in history\n");
			continue;
		}
		//piping last command
		else if(strcmp(cmd, "!!") == 0 && strchr(rec, '|') != NULL){
			token = strchr(rec, '|');
			end = strchr(rec, '|');
			end+=2;
			*token = '\0';
			rec[strlen(rec) - 1] = '\0';
			splitCmd(rec, args);
			splitCmd(end, args2);
			repeat = 1;
			pipe = 1;
		}
		//last command
		else if(strcmp(cmd, "!!") == 0){
			splitCmd(rec, args);
			repeat = 1;
		}
		//Save command
		else{
			for(int i = 0; i < strlen(cmd); ++i){
				rec[i] = cmd[i];
			}
		}
	    
		//Split command for piping
		if(strchr(cmd, '|') != NULL && repeat == 0){
			token = strchr(cmd, '|');
			end = strchr(cmd, '|');
			end+=2;
			*token = '\0';
			cmd[strlen(cmd) - 1] = '\0';
			splitCmd(cmd, args);
			splitCmd(end, args2);
			pipe = 1;
		}
		//put normal command into array of pointers
		else if (repeat == 0){
			splitCmd(cmd, args);
		}
		//check for input and output
		int ifile = 0;
		int last = 0;
		for(; last < MAX_ARGS; ++last){
			if(args[last] == NULL) break;
			else if(strcmp(args[last], "<") == 0){
				in = 1;
				ifile = last;
			}
			else if(strcmp(args[last], ">") == 0){
				out = 1;
				ifile = last;
			}
			
		}
		//Find index in arguments with file name and save it as a string
		char filename[50] = "";
		if(in || out){
			args[ifile] = '\0';
			ifile++;
			strcpy(filename, args[ifile]);
		}
		//exit
		if(strcmp(args[0], "exit") == 0) break;
		//check for & at the end
		if(strcmp(args[last-1], "&") == 0){
			back = 1;
			args[last-1] = '\0';
		}
		//normal fork vs piping fork vs cd
		if(pipe) fork_p(args, args2);
		else if(strcmp(args[0], "cd") == 0) cd(args[1]);
		else exeCmd(args, back, in, out, filename);
	}
	return 0;
}
//splits command into arguments
void splitCmd(char* cmd, char** args){
	for(int i = 0;i < MAX_ARGS; ++i){
		args[i] = strsep(&cmd, " ");
		if(args[i] == NULL) break;
	}
}

void cd(char* dest){
	char dt[MAX_CMD_LEN];
	strcpy(dt, dest);

	char cwd[MAX_CMD_LEN];
	//create destination directory and change directory
	if(dest[0] != '/'){
		getcwd(cwd, sizeof(cwd));
		strcat(cwd, "/");
		strcat(cwd, dt);
		chdir(cwd);
	}
	//destination directory is given ready
	else{
		chdir(dest);
	}
}

//Normal forking for executing background processes, input/output and simple commands
int exeCmd(char** args, int back, int in, int out, char* ifile){
	int fd;
	pid_t pid = fork();
	if(pid == -1){
		//Error while forking
		printf("fork error\n");
		return 1;
	}
	//child process
	else if(pid == 0){
		//background process
		if(back == 1) setpgid(0, 0);
		//setup to use file as input
		if(in){
			int fd = open(ifile, O_RDONLY, 0);
			dup2(fd, STDIN_FILENO);
			close(fd);
			in = 0;
		}
		//setup to use file as output
		if(out){
			int fdo = creat(ifile, 0644);
			dup2(fdo, STDOUT_FILENO);
			close(fdo);
			out = 0;
		}
		//execute command
		execvp(args[0], args);
		if(back == 1) return 1;
		return 0;
	}
	//don't wait for child process to end while it runs in background
	else if(back == 1){
		return 1;
	}
	//parent process
	else{
		wait(NULL);
		return 1;
	}
}

//Pipe forking for executing pipe commands of two commands
int fork_p(char** args1, char** args2 ){
	pid_t pid1, pid2;
	int stat1, stat2, fd[2];
	pipe(fd);
	pid1 = fork();
	//child process 1
	if(pid1 == 0){
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		execvp(args1[0], args1);
		return 0;
	}
	pid2 = fork();
	//child process 2
	if(pid2 == 0){
		dup2(fd[0], STDIN_FILENO);
		close(fd[1]);
		execvp(args2[0], args2);
		return 0;
	}
	//parent process
	close(fd[0]);
	close(fd[1]);
	waitpid(pid1, &stat1, 0);
	waitpid(pid2, &stat2, 0);
	
	return 1;
}
