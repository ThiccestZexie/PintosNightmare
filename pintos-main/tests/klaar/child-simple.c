/* Child process run by exec-multiple, exec-one, wait-simple, and
	wait-twice tests.
	Just prints a single message and terminates. */

#include "tests/lib.h"

#include <stdio.h>

const char* test_name = "child-simple";

int main(void)
{
	msg("run");
	return 81;
}
