#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <assert.h>

#include "svec.h"
#include "tokens.h"


// Recursively executes the given command.
void
execute(char **cmd)
{
    int cpid;
	int logical_op = 0;
	int background = 0;
	int saved_stdout = dup(1);
	int argc = 0;

	if (cmd[0] != 0 && strcmp(cmd[0], "exit") == 0)
		exit(0);

	for (int i = 0; cmd[i] != 0; i++) {
		argc++;
	} 

	char **next_cmd;

	for (int j = 1; cmd[j] != 0; j++) {

		char *token = cmd[j];
		assert(token != NULL);

		if (strcmp(token, ";") == 0) {

			next_cmd = cmd + j + 1;
			next_cmd[argc - j] = 0;
			cmd[j] = 0;

			execute(cmd);
			execute(next_cmd);	
			return;
		}	
	}

	for (int j = 1; cmd[j] != 0; j++) {

		char *token = cmd[j];
		assert(token != NULL);

		if (strcmp(token, "&&") == 0) {
			logical_op = 1;

			cmd[j] = 0;
			next_cmd = cmd + j + 1;
			next_cmd[argc - j] = 0;
			break;
		}
		else if (strcmp(token, "||") == 0) {
			logical_op = 2;
			
			cmd[j] = 0;
			next_cmd = cmd + j + 1;
			next_cmd[argc - j] = 0;
			break;
		}
		else if (strcmp(token, "<") == 0) {
			assert(cmd[j+1] != NULL);
			int fd = open(cmd[j+1], O_RDONLY);
			close(0);
			dup(fd);
			close(fd);
			cmd[j] = 0;
		}
		else if (strcmp(token, ">") == 0) { // output redir
			close(1); // close standard output
			assert(cmd[j+1] != NULL);
			open(cmd[j+1], O_WRONLY | O_CREAT | O_TRUNC, 0644); 
			cmd[j] = 0;
		}
		else if (strcmp(token, "|") == 0) {
			int pipe_fds[2];
			assert(pipe(pipe_fds) == 0);

			cmd[j] = 0;
			next_cmd = cmd + j + 1;
			next_cmd[argc - j] = 0;

			close(1);
			dup2(pipe_fds[1], 1);
			close(pipe_fds[1]);
			execute(cmd);

			close(1);
			dup2(saved_stdout, 1);
			close(saved_stdout);

			close(0);
			dup2(pipe_fds[0], 0);
			close(pipe_fds[0]);
			execute(next_cmd);
			return;
		}
		else if (strcmp(token, "&") == 0) {
			background = 1;
			cmd[j] = 0;
			next_cmd = cmd + j + 1;
			next_cmd[argc - j] = 0;
			break;

		}
	

	}

    if ((cpid = fork())) {
		int status;
		if (background != 1) {
			waitpid(cpid, &status, 0);

			if (WIFEXITED(status)) {
				dup2(saved_stdout, 1);
				close(saved_stdout);
				
				int exit_value = WEXITSTATUS(status);
				if (logical_op == 1 && exit_value == 0)
					execute(next_cmd);
				else if (logical_op == 2 && exit_value != 0)
					execute(next_cmd);		
			}
		}
    }
    else {
	
		if (cmd[0] != 0 && cmd[1] != 0 && strcmp(cmd[0], "cd") == 0)
			chdir(cmd[1]);
        else if (cmd[0] != 0) {
			execvp(cmd[0], cmd);
		}
    }

}

// Allows the user to execute commands in a shell.
int
main(int argc, char* argv[])
{
    char cmd[256];

	svec* vec = make_svec();

    if (argc == 1) { 
		while (1) {
			printf("nush$ ");
		    fflush(stdout);

			if (fgets(cmd, 256, stdin) == NULL) {
				free_svec(vec);
				puts("");
				exit(0);
			}
			
			parse(cmd, vec);
			vec->data[vec->size] = 0;
			execute(vec->data);
			vec->size = 0;
		}
    }
	else {
		FILE *file = fopen(argv[1], "r");
		
		while (1) {
		    fflush(stdout);

			if (fgets(cmd, 256, file) == NULL) {
				free_svec(vec);
				exit(0);
				fclose(file);
			}

			parse(cmd, vec);
			vec->data[vec->size] = 0;
			execute(vec->data);
			vec->size = 0;
		}
	}
	
}
