#include "../lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

#define MAX_SPAWN 100

int main(int argc, char* argv[])
{
	if (argc != 4) {
		return 1;
	}

	test_name = argv[0];
	quiet = true;

	char buffer[128];
	pid_t pid[MAX_SPAWN];
	int retval = 0;
	int i;
	int ret = 0;

	/* Print args. */
	// printf ("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
	// printf("+");
	/* Execute child and wait for it to finish if requested. */
	int spawn = atoi(argv[2]);

	if (spawn != 0) {
		for (i = 0; i < spawn; i++) {
			snprintf(
				 buffer,
				 sizeof buffer,
				 "recursor-c %s %d %s",
				 argv[1],
				 atoi(argv[2]) - 1,
				 argv[3]);
			pid[i] = exec(buffer);
		}
		if (atoi(argv[3])) {
			for (i = 0; i < spawn; i++) {
				retval = wait(pid[i]);
				if (retval < 0)
					ret = 1;
			}
		}

		/* Done. */
		if (ret)
			fail("YOU HAVE FAILED\n");
	}

	return ret;
}
