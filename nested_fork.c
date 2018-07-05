#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int squared(int x);
int echo(int y);

int main() {
	// child1 pid
	pid_t child1_pid;
	// variables
    int x = 5, y = 5;
    // file Descriptors
    int fd[2], fd1[2];
    
    // print variables
    fprintf(stderr, "Variables:\nX: %d\nY: %d\n\n", x, y);
    
    // create pipes and leave them open
    pipe(fd);
    pipe(fd1);
    
    fprintf(stderr, "Parent\t\tPID\t%d\tParent PID\t%d\n", getpid(), getppid());
    
    if((child1_pid = fork())) {
		// child 1
		
		fprintf(stderr, "Child1\t%d\tPID\t%d\tParent PID\t%d\n", child1_pid, getpid(), getppid());
		// child2 pid
		pid_t child2_pid;
		if((child2_pid = fork())){
			// child 2
			
			fprintf(stderr, "Child2\t%d\tPID\t%d\tParent PID\t%d\n", child2_pid, getpid(), getppid());
			// close regions of pipes 
			close(fd[0]);
			close(fd[1]);
			close(fd1[0]);
			// update child2's y value
			y = echo(y);
			// write it with pipe
			write(fd1[1], &y, sizeof(y));
			// close used pipe region
			close(fd1[1]);
			// terminate successfully
			exit(EXIT_SUCCESS);
			
		} else {
			// child1 as parent for child2
			
			// child2's return status
			int returnStatus1;
			// wait for child2 to terminate with success
			waitpid(child2_pid, &returnStatus1, 0);
			// close regions of pipes
			close(fd1[0]);
			close(fd1[1]);
			close(fd[0]);
			// update child1's x value
			x = squared(x);
			// write it with pipe
			write(fd[1], &x, sizeof(x));
			// close used pipe region
			close(fd[1]);
			// terminate successfully
			exit(EXIT_SUCCESS);
		}
	} else {
		// parent
		
		// child1's return status
		int status;
		// wait for child1 to terminate with success
		waitpid(child1_pid, &status, 0);
		// close regions of pipes
		close(fd[1]);
		close(fd1[1]);
		// update local x with modified one by child1
		read(fd[0], &x, sizeof(x));
		// update local y with modified one by child2
		read(fd1[0], &y, sizeof(y));
		// close used pipe regions
		close(fd[0]);
		close(fd1[0]);
		// display result
		fprintf(stderr, "\n\nResult : %d\n", x+y);
		// exit successfully
		exit(EXIT_SUCCESS);
		         
	}
	// error situation
	fprintf(stderr, "Error in MAIN\n");
	return 1;
}

// return input squared
int squared(int x) {
	return x*x;
}

// return input
int echo(int y) {
	return y;
}
