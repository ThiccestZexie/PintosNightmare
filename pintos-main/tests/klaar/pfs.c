/* klaar@ida (convert to built-in pintos test)

	Tests that read and write are properly synchronized. Also stresses
	the filesystem somewhat.

	pfs_reader and pfs_writer are needed child processes.
*/

#include "pfs.h"

#include "../lib.h"

#include <random.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

static void swap(char* a, char* b)
{
	char c = *a;
	*a = *b;
	*b = c;
}

static void start_rw(int rn, int wn)
{
	int rpid = -1;
	int wpid = -1;

	if (rn > 0) {
		rpid = exec("pfs-reader");
		msg("exec(\"pfs-reader\"): %d", rpid);
	}

	if (wn > 0) {
		int range = '~' - '!' + 1;	  // ascii 32 - 126
		char start = '!' + random_ulong() % range;
		char end = '!' + random_ulong() % range;

		if (end < start)
			swap(&start, &end);

		const int CMDSIZE = 64;
		char cmd[CMDSIZE];
		snprintf(cmd, CMDSIZE, "pfs-writer %c %c", start, end);

		wpid = exec(cmd);
		msg("exec(\"%s\"): %d", cmd, wpid);
	}

	if (rn > 0 || wn > 0)
		start_rw(rn - 1, wn - 1);

	if (rpid != -1)
		CHECK(wait(rpid) != -1, "wait pfs-reader %d", rpid);

	if (wpid != -1)
		CHECK(wait(wpid) != -1, "wait pfs-writer %d", wpid);
}

int main(int argc, char* argv[])
{
	if (argc != 3)
		fail("usage: pfs START END");

	quiet = true;
	test_name = argv[0];
	int num_reader = atoi(argv[1]);
	int num_writer = atoi(argv[2]);

	msg("begin");

	CHECK(create("file.1", BIG), "create \"file.1\"");
	CHECK(create("messages", TIMES), "create \"messages\"");

	random_init(0);

	start_rw(num_reader, num_writer);

	msg("end");

	// Tell the system we're entirely done. Otherwise the test accepts
	// that this process is killed for some reason (e.g. invalid syscall).
	printf("pfs done\n");
	return 0;
}
