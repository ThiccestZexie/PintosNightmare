/* dagjo@ida (convert to built-in pintos test)
 */

#include "../lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

const char* test_name = "recursor_ng";

int main(int argc, char* argv[])
{
	quiet = true;

	int retval = 0;

	/* Print args. */
	// printf ("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
	// printf("+");
	/* Execute child and wait for it to finish if requested. */

	msg("begin");
	retval = wait(exec("recursor-c pintosmaster 6 1"));
	if (retval)
		fail("You have failed");

	msg("end");
	printf("recursor_ng done\n");

	return retval;
}
