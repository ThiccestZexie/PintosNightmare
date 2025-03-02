#include <stdio.h>
#include <syscall.h>

/* NOTE: Will not work until the WRITE syscall is implemented. */

/* Compile it, and run it from userprog/ with the following command
pintos -v -k -T 240 --filesys-size=2 -p ../examples/lab1test -a lab1 -- -f -q run 'lab1 arg1 arg2 arg3'

If lab 1 is correctly implemented the arguments should be printed to the
console.
Try it wih different arguments and different number of arguments.
*/

int main(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		printf("Parameter: %d: %s \n", i, argv[i]);
	}

	return EXIT_SUCCESS;
}
