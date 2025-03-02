/* Part of pfs.c suite.

	Write on the disk. Each time the buffer is filled with same
	character. Different character every time!
 */

#include "../lib.h"
#include "pfs.h"

#include <stdio.h>
#include <string.h>
#include <syscall.h>

static void fill_buffer(char* buf, int size, char c)
{
	for (int i = 0; i < size; i++) {
		buf[i] = c;
	}
}

char buffer[BIG];

int main(int argc, char* argv[])
{
	test_name = argv[0];
	quiet = true;

	if (argc != 3 || strlen(argv[1]) != 1 || strlen(argv[2]) != 1)
		return 1;

	char start = argv[1][0];
	char end = argv[2][0];

	char c = start;
	for (int i = 0; i < TIMES; ++i) {
		fill_buffer(buffer, BIG, c);

		int id = open("file.1");
		CHECK(id > 1, "open \"file.1\"");
		int bytes = write(id, buffer, BIG);
		CHECK(bytes == BIG, "write \"file.1\"");
		close(id);

		c = c + 1;

		if (c > end)
			c = start;
	}
	return 0;
}
