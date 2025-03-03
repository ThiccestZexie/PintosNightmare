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

//get file
struct file* get_file(int fd);

void halt(void);
void exit(int status);
bool create(const char *file, unsigned initial_size);
int open(const char *file_name);
void close(int fd);
int write(int fd, const void *buffer, unsigned size);
int read(int fd, void *buffer, unsigned size);
bool remove(const char *file);
int filesize(int fd);
void sleep(int millis);

static void syscall_handler(struct intr_frame *f UNUSED)
{
	// retrieve system call number
	int *argv[MAX_ARGS];

	int syscall_num = *(int *)f->esp;
	printf("syscall number: %d\n", syscall_num);
	retrieve_args(f->esp, argv); // this gets argc from stack

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
		break;
	case SYS_OPEN:
		open(argv[0]);
		break;
	case SYS_FILESIZE:
		break;
	case SYS_READ:
		break;
	case SYS_WRITE:
		break;
	case SYS_SEEK:
		break;
	case SYS_TELL:
		break;
	case SYS_CLOSE:
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
 * Returns the
 */
struct file* get_file(int fd)
{
	if (fd < 2 || fd >= MAX_FDS)
		return NULL;

	return thread_current()->fds[fd];
}

int open(const char *file_name)
{
	struct file *f = filesys_open(file_name);
	if (f == NULL)
	{
		return -1;
	}

	struct thread *this_thread = thread_current();
	for (int i = 2; i < MAX_FDS; i++)
	{
		if (this_thread->fds[i] != NULL)
			continue; // ALREADY OCCUPIED

		this_thread->fds[i] = f;
		return i; // found a free file descriptor
	}

	return -1; // unable to find a free file descriptor
}

void close(int fd)
{
	struct thread *this_thread = thread_current();
	if (this_thread->fds[fd] == NULL)
		return; // already closed

	file_close(this_thread->fds[fd]);
	this_thread->fds[fd] = NULL;
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
		// unopened file
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
		return -1; // unopened file

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
		return -1; // unopened file

	return file_length(f);
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