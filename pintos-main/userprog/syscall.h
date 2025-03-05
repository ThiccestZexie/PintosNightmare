#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init(void);

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

#endif /* userprog/syscall.h */
