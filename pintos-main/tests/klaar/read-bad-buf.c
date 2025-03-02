/* Passes an semingly valid pointer to the read system call.
	The pointer will however span invalid pages.
	The process must be terminated with -1 exit code.
	(klaar@ida)
*/

#include "tests/lib.h"
#include "tests/main.h"

#include <syscall.h>

char global; /* allocated with process image */

void test_main(void)
{
	int handle;
	char local; /* allocated on process stack */

	CHECK((handle = open("sample.txt")) > 1, "open \"sample.txt\"");

	/* buffer will start and end at valid addresses ... */
	read(handle, (char*) &global, &local - &global + 1);
	fail("should not have survived read()");
}
