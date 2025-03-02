#include "list.h"

#include <stdlib.h>

int main()
{
    /*  This is called a sentinel node. It's not really a part of the list,
     *  it's just used to hold on to the list to be able to avoid certain
     *  special cases in the list implementation, and to make the solution
     *  slightly eaiser.
     */
	struct list_item root;
	root.value = -1;
	root.next = NULL;

	/* Write your test cases here */
	return 0;
}
