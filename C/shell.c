#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

//shell commands eg exit, cd, help

void cShell_exit(char **args){
	exit(0);
}


//note: remove nested if? switch statement?
void cShell_cd(char **args){
	if (args[1] == NULL) {
		fprintf(stderr, "cShell: cd: missing argument, run man cd for help with cd command\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("cShell :cd");
		}
	}
}

void cShell_prtc(char **args){
	char *ctext =
		"cShell was written by Cole Hall on 15/10/22. .\n"
		"🐚 - can you hear the ocean? .\n"
		"references used : .\n"
		"http://www.dmulholl.com/lets-build/a-command-line-shell.html .\n"
		"https://www.geeksforgeeks.org/making-linux-shell-c/ .\n"
		"https://brennan.io/2015/01/16/write-a-shell-in-c/ .\n"
		"https://github.com/brenns10/lsh .\n";
	printf("%s", ctext);
}
		

void cShell_help(char **args){
	char *thetext =
		"cShell - put your ear to it and you can hear the ocean.\n"
		"Built in commands:\n"
		" cd	- Change directory.\n"
		" help	- Provides a list of built in commands.\n"
		" prtc	- Prints information about this shell. \n"
		" exit	- Exits the shell. \n";
	printf("%s", thetext);
}

struct shellcmnd {
	char *name;
	void (*func)(char **args);
};

struct shellcmnd shellcmnds[] = {
	{"help", cShell_help},
	{"exit", cShell_exit},
	{"cd", cShell_cd},
	{"prtc", cShell_prtc},
};

int cShell_num_shellcmnds(){
	return sizeof(shellcmnds) / sizeof(struct shellcmnd);
}

//input parser and command launcher

void cShell_exec(char **args){
	//checks for built in shell commands before launching any external processes
	for (int i = 0; i < cShell_num_shellcmnds(); i++){
		if (strcmp(args[0], shellcmnds[i].name) == 0){
				shellcmnds[i].func(args);
				return;
		}
	}

	//forks the initial process giving us a child process thats the same as the current
	//then exectues commands in the child process to replace with program we want to run
	pid_t child_pid = fork();
	

	//if fork returns zero we need to execvp to search directories in $PATH for the first arg
	//second arg is NULL term array of string pointers with arguments for the exec
	// -------------
	//only returns on error, where it will print the error and exit child
	//--------------
	//if fork returns a value greater than zero we're in the parent, waits with waitpid
	//from syswait for child process to finish only exits loop when child has exited
	//or termintated
	//--------------
	//if a value lesser than zero an error is returned
	
	if (child_pid == 0){
		execvp(args[0], args);
		perror("cShell");
		exit(1);
	} else if (child_pid > 0) {
		int status;
		do {
			waitpid(child_pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	} else {
		perror ("cShell");
	}
}

char** cShell_split_line(char *line){
	int length = 0;
	int capacity = 16;
	char **comAndArgs = malloc(capacity * sizeof(char*));
	
	
	char *delims = " \t\r\n";
	//strtok from strin.h splits strings on whitespace
	char *comAndArg = strtok(line, delims);

	while (comAndArg != NULL){
		comAndArgs[length] = comAndArg;
		length++;

		if (length >= capacity) {
			capacity = (int) (capacity * 1.5);
			comAndArgs = realloc(comAndArgs, capacity * sizeof(char*));
		}

		comAndArg = strtok(NULL, delims);
	}

	comAndArgs[length] = NULL;
	return comAndArgs;
}

char* cShell_read_line(){
	char *line = NULL;
	size_t buflen = 0;
	//getline from stdio.h
	getline(&line, &buflen, stdin);
	return line;
}

//Entry point

//the main function that will run when the shell begins
int main(){
	//start the infinite loop, printing > on each iteration
	//exectues commands
	//frees up the memory before the next loop so we don't run out
	while (true) {
		printf("> ");
		//reads the input line and stores the pointer in line
		char *line = cShell_read_line();
		//reads the commands and arguments and runs the split line
		//function which splits the input string into an array of
		//individual commands with line as an input variable
		char **comAndArgs = cShell_split_line(line);
		//If there's something input, it will run the execture
		//command with the arguments and commands previously inputted
		if (comAndArgs[0] != NULL){
			cShell_exec(comAndArgs);
		}
		//frees up the memory afterward
		free(comAndArgs);
		free(line);
	}
}

