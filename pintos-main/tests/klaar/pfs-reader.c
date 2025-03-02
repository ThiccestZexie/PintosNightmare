/* Part of pfs.c suite.

	Reads from the file and checks consistency.
	The buffer should all contain the same character!!
 */

#include "../lib.h"
#include "pfs.h"

#include <stdio.h>
#include <syscall.h>

static bool check_consistency(char* buf, int size)
{
	for (int i = 1; i < size; ++i) {
		if (buf[0] != buf[i]) {
			/* Ooops, inconsistency */
			return false;
		}
	}
	return true;
}

char buffer[BIG];

int main(int argc UNUSED, char* argv[])
{
	test_name = argv[0];
	quiet = true;

	for (int i = 0; i < TIMES; ++i) {
		int id = open("file.1");
		CHECK(id > 1, "open \"file.1\"");
		int bytes = read(id, buffer, BIG);
		CHECK(bytes == BIG, "read \"file.1\"");
		close(id);

		CHECK(check_consistency(buffer, BIG), "inconsistency");
	}
	return 0;
}
