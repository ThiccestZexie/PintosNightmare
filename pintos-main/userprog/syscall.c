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
void retrive_args1(void *esp, int *argv[], unsigned argc);
// get file
struct file *get_file(int fd);

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
void seek(int fd, unsigned position);
unsigned tell(int fd);
void validate_pointer(void *ptr);
void validate_buffer(void *buffer, unsigned size);
void validate_string(const char *str);

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
	// retrieve_args(f->esp, argv); // this gets argc from stack hypothetically speaking that  is

	switch (syscall_num)
	{
	case SYS_HALT:
		halt();
		break;
	case SYS_EXIT:
		retrive_args1(f->esp, argv, 1);
		exit(argv[0]);
		break;
	case SYS_EXEC:
		printf("exec\n");
		break;
	case SYS_WAIT:
		printf("wait\n");
		break;
	case SYS_CREATE:
		retrive_args1(f->esp, argv, 2);
		f->eax = create(argv[0], argv[1]);
		break;
	case SYS_REMOVE:
		retrive_args1(f->esp, argv, 1);
		f->eax = remove(argv[0]);
		break;
	case SYS_OPEN:
		retrive_args1(f->esp, argv, 1);
		f->eax = open(argv[0]);
		break;
	case SYS_FILESIZE:
		retrive_args1(f->esp, argv, 1);
		f->eax = filesize(argv[0]);
		break;
	case SYS_READ:
		retrive_args1(f->esp, argv, 3);
		f->eax = read(argv[0], argv[1], argv[2]);
		break;
	case SYS_WRITE:
		retrive_args1(f->esp, argv, 3);
		f->eax = write(argv[0], argv[1], argv[2]);
		break;
	case SYS_SEEK:
		retrive_args1(f->esp, argv, 2);
		seek(argv[0], argv[1]);
		break;
	case SYS_TELL:
		retrive_args1(f->esp, argv, 1);
		f->eax = tell(argv[0]);
		break;
	case SYS_CLOSE:
		retrive_args1(f->esp, argv, 1);
		close(argv[0]);
		break;
	case SYS_SLEEP:
		retrive_args1(f->esp, argv, 1);
		sleep(argv[0]);
		break;
	default:
		printf("system call not implemented\n");
		exit(-1);
		break;
	}
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
	struct thread *cur = thread_current();
	if (cur->next_fd >= MAX_FDS)
		return -1;

	validate_string(file_name);

	struct file *f = filesys_open(file_name);
	if (f == NULL)
		return -1;

	struct file_descriptor *fd_entry = malloc(sizeof(struct file_descriptor));
	fd_entry->fd = cur->next_fd;
	cur->next_fd++;
	fd_entry->file = f;

	list_push_back(&cur->file_descriptors, &fd_entry->elem);

	return fd_entry->fd;
}

void close(int fd)
{
	struct thread *this_thread = thread_current();
	if (fd < 2 || fd >= MAX_FDS)
		return;

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
	validate_buffer(buffer, size);
	if (fd == 1)
	{
		putbuf(buffer, size);
		return size;
	}
	struct file *f = get_file(fd);

	if (f == NULL)
		return -1;

	return (int *)file_write(f, buffer, size);
}

int read(int fd, void *buffer, unsigned size)
{
	validate_buffer(buffer, size);
	if (fd == 0)
	{
		for (unsigned i = 0; i < size; i++)
		{
			*(int *)(buffer + i) = input_getc();
			// Echo it back :D
			putbuf(buffer + i, 1);
		}
		return sizeof(char) * size;
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
	// File_seek already does this...

	// TODO: SHOULD IT FAIL SILENTLY?
	// if (f == NULL)
	// 	return;

	ASSERT(f != NULL);

	if (position > file_length(f))
		return;
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

//
void retrieve_args(void *esp, int *argv[])
{
	// assuming esp is pointing at the bottom of the stack as of pushing the args on top of it.
	unsigned argc = *(unsigned *)(esp + 4);
	printf("argc: %d\n", argc);
	for (int i = 1; i <= argc; i++)
	{
		argv[i] = *(int *)(esp + 4 * i);
		printf("argv[%d]: %d\n", i, argv[i]);
	}
}

void retrive_args1(void *esp, int *argv[], unsigned argc)
{

	/**
	 * arg3
	 * arg2
	 * arg1
	 * Number for syscall  <- esp
	 * Theroetically speaking we could just instantly take argc
	 */
	int *arg_ptr = esp;
	for (int i = 0; i < argc; i++)
	{
		arg_ptr += 1;
		validate_pointer(arg_ptr);
		argv[i] = *arg_ptr;
	}
}

void validate_pointer(void *ptr)
{
	if (ptr == NULL || !is_user_vaddr(ptr) || pagedir_get_page(thread_current()->pagedir, ptr) == NULL)
	{
		exit(-1);
	}
}

void validate_buffer(void *buffer, unsigned size)
{
	// if we valdiate the start and end of the buffer the entire buff should be safe.
	const char *buf = (const char *)buffer;
	validate_pointer(buf);
	validate_pointer(buf + size - 1);
}

void validate_string(const char *str)
{
	if (str == NULL)
		exit(-1);

	validate_pointer(str);
	while (*str != '\0')
	{
		str++;
		validate_pointer(str);
	}
}