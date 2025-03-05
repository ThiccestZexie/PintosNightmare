#include "userprog/syscall.h"
#include "filesys/filesys.h"
#include "filesys/file.h"

#include "devices/input.h"
#include "devices/timer.h"
#include "devices/shutdown.h"

#include "threads/synch.h"
#include "threads/vaddr.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <list.h>
#include <stdio.h>
#include <syscall-nr.h>

static void syscall_handler(struct intr_frame *);
void retrieve_args(void *esp, int *argv[]);

#define MAX_ARGS 3
#define MAX_FDS 130

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void syscall_handler(struct intr_frame *f UNUSED)
{
	// retrieve system call number
	int *argv[MAX_ARGS];

	int syscall_num = *(int *)f->esp;
	printf("syscall number: %d\n", syscall_num);
	retrieve_args(f->esp, argv); // this gets argc from stack hypothetically speaking that  is

	switch (syscall_num)
	{
	case SYS_HALT:
		halt();
		break;
	case SYS_EXIT:
		exit(argv[0]);
		break;
	case SYS_EXEC:
		break;
	case SYS_WAIT:
		break;
	case SYS_CREATE:
		create(argv[0], argv[1]);
		break;
	case SYS_REMOVE:
		remove(argv[0]);
		break;
	case SYS_OPEN:
		open(argv[0]);
		break;
	case SYS_FILESIZE:
		filesize(argv[0]);
		break;
	case SYS_READ:
		read(argv[0], argv[1], argv[2]);
		break;
	case SYS_WRITE:
		write(argv[0], argv[1], argv[2]);
		break;
	case SYS_SEEK:
		seek(argv[0], argv[1]);
		break;
	case SYS_TELL:
		tell(argv[0]);
		break;
	case SYS_CLOSE:
		close(argv[0]);
		break;
	case SYS_SLEEP:
		sleep(argv[0]);
		break;
	default:
		break;
	}

	printf("system call!\n");
	thread_exit();
}

void halt(void)
{
	shutdown_power_off();
}

void exit(int status)
{
	thread_exit();
}

bool create(const char *file, unsigned initial_size)
{
	return filesys_create(file, initial_size);
}

/**
 * Returns the file associated with the given file descriptor.
 */
struct file *get_file(int fd)
{
	struct thread *cur = thread_current();
	if (fd < 2 || fd >= MAX_FDS)
		return NULL;

	struct list_elem *e;

	for (e = list_begin(&cur->file_descriptors);
		 e != list_end(&cur->file_descriptors);
		 e = list_next(e))
	{
		struct file_descriptor *fd_entry = list_entry(e, struct file_descriptor, elem);
		if (fd_entry->fd == fd)
			return fd_entry->file;
	}
	return NULL;
}

int open(const char *file_name)
{
	struct file *f = filesys_open(file_name);
	if (f == NULL)
		return -1;

	struct thread *cur = thread_current();
	struct file_descriptor *fd_entry = malloc(sizeof(struct file_descriptor));
	fd_entry->fd = cur->next_fd++;
	fd_entry->file = f;

	list_push_back(&cur->file_descriptors, &fd_entry->elem);

	return fd_entry->fd;
}

void close(int fd)
{
	struct thread *this_thread = thread_current();
	if (fd < 2 || fd >= MAX_FDS)
		return -1;

	struct list_elem *e;

	for (e = list_begin(&this_thread->file_descriptors);
		 e != list_end(&this_thread->file_descriptors);
		 e = list_next(e))
	{
		struct file_descriptor *fd_entry = list_entry(e, struct file_descriptor, elem);
		if (fd_entry->fd == fd)
		{
			file_close(fd_entry->file);
			list_remove(e);
			free(fd_entry);
			return;
		}
	}
}

int write(int fd, const void *buffer, unsigned size)
{

	if (fd == 1)
	{
		putbuf(buffer, size);
		return size;
	}
	struct file *f = get_file(fd);

	if (f == NULL)

		return -1;

	int bytes_written;

	bytes_written = file_write(f, buffer, size);

	return bytes_written;
}

int read(int fd, void *buffer, unsigned size)
{
	if (fd == 0)
	{
		for (int i = 0; i < size; i++)
		{
			((char *)buffer)[i] = input_getc();
		}
		return size;
	}

	struct file *f = get_file(fd);

	if (f == NULL)
		return -1;

	return file_read(f, buffer, size);
}

bool remove(const char *file)
{

	return filesys_remove(file);
}

int filesize(int fd)
{
	if (fd < 2 || fd >= MAX_FDS)
		return -1;

	struct file *f = get_file(fd);

	if (f == NULL)
		return -1;

	return file_length(f);
}

void seek(int fd, unsigned position)
{
	struct file *f = get_file(fd);

	if (f == NULL)
		return -1;

	file_seek(f, position);
}

unsigned tell(int fd)
{
	struct file *f = get_file(fd);

	if (f == NULL)
		return -1;

	return file_tell(f);
}

void sleep(int millis)
{
	timer_msleep(millis);
}

void retrieve_args(void *esp, int *argv[])
{
	unsigned argc = *(unsigned *)(esp + 4);
	for (int i = 1; i < argc; i++)
	{
		argv[i] = *(int *)(esp + 4 * i);
	}
}