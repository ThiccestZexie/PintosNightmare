
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
void retrive_args1(void *esp, int *argv[], unsigned argc);
// get file
struct file *get_file(int fd);

void halt(void);
void exit(int status);
pid_t exec(const char *cmd_line);
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
void validate_set_args(void *esp, int amount);

#define MAX_ARGS 3
#define MAX_FDS 130

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void validate_set_args(void *esp, int amount)
{
	// check all bytes of the arguments
	int validate = amount * 8;
	for (int i = 0; i < validate; i++)
	{
		validate_pointer(esp + i);
	}
}

static void syscall_handler(struct intr_frame *f)
{
	// retrieve system call number
	int argv[MAX_ARGS];

	for (int i = 0; i < 4; i++)
	{
		validate_pointer(f->esp + i);
	}

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
		retrive_args1(f->esp, argv, 1);
		f->eax = exec(argv[0]);
		break;
	case SYS_WAIT:
		retrive_args1(f->esp, argv, 1);
		f->eax = wait(argv[0]);
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
		exit(-1);
		break;
	}
	// sleep(1);
}

void halt(void)
{
	shutdown_power_off();
}

void exit(int status)
{
	struct thread *cur = thread_current();
	
	cur->parent_relation->exit_status = status;

	thread_exit(); // As long as USERPROG is defined, this will call process_exit
}

pid_t exec(const char *cmd_line)
{
	validate_string(cmd_line);
	return process_execute(cmd_line);
}

int wait(pid_t pid)
{
	return process_wait(pid);
}

bool create(const char *file, unsigned initial_size)
{
	validate_string(file);
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
// Have to be able to open the same file and assign a new file descriptor
int open(const char *file_name)
{

	validate_string(file_name);
	struct thread *cur = thread_current();
	if (cur->next_fd >= MAX_FDS)
		return -1;

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

	return file_write(f, buffer, size);
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

void retrive_args1(void *esp, int *argv[], unsigned argc)
{

	/**
	 * arg3
	 * arg2
	 * arg1
	 * Number for syscall  <- esp
	 * Theroetically speaking we could just instantly take argc
	 */

	// Validate all bytes of the arguments on the stack.
	validate_set_args(esp + 4, argc);
	int *arg_ptr = (int *)esp;
	for (unsigned i = 0; i < argc; i++)
	{
		// Validate each pointer to the argument.
		validate_pointer((void *)(arg_ptr));
		arg_ptr += 1;
		argv[i] = *arg_ptr;
	}
}

void validate_pointer(void *ptr)
{
	void *page = pagedir_get_page(thread_current()->pagedir, ptr);
	if (ptr == NULL)
	{
		exit(-1);
	}
	if (is_kernel_vaddr(ptr))
	{
		exit(-1);
	}
	if (page == NULL)
	{
		exit(-1);
	}
	if (ptr < (void *)0x08048000)
	{
		exit(-1);
	}
}

void validate_buffer(void *buffer, unsigned size)
{
	// if we valdiate the start and end of the buffer the entire buff should be safe.
	const char *buf = (const char *)buffer;
	// validate_pointer(buf);
	// validate_pointer(buf + size);
	for (unsigned i = 0; i < size; i++)
	{
		validate_pointer((void *)(buf + i));
	}
}

void validate_string(const char *str)
{
	if (str == NULL)
		exit(-1);
	// Validate first pointer and then every subsequent byte until the terminator.
	validate_pointer(str);
	while (*str != '\0')
	{
		str++;
		validate_pointer(str);
	}
}