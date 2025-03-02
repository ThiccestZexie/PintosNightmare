#ifndef LIST_H
#define LIST_H

struct list_item {
	int value;
	struct list_item *next;
};

/* Puts x at the end of the list */
void append(struct list_item *first, int x);

/* Puts x at the start of the list */
void prepend(struct list_item *first, int x);

/* Find the first element in the list larger than x and
 * put x right before that element */
void input_sorted(struct list_item *first, int x);

/* Prints all elements in the list */
void print(struct list_item *first);

/* Free everything that is dynamically allocated */
void clear(struct list_item *first);

#endif
