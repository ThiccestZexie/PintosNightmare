#include <stdio.h>
#include <syscall.h>

/* Compile it, and run it from userprog/ with the following command
pintos -v -k -T 240 --filesys-size=2 -p ../examples/lab4test1 -a lab4 -- -f -q run lab4

    If lab 4 is correctly implemented this program should call itself in an infinite loop
    and spawn itself as a child process indefinetly.
    The PID of each child process created should be printed to the screen.
    In order to see if your implementation works check if the PID of each new  child
    process is incremented as expected.
*/

int main(void)
{
	int pid = exec("lab4");
	printf("Child process ID: %d\n", pid);

	return EXIT_SUCCESS;
}
