#ifndef FILESYS_FILE_H
#define FILESYS_FILE_H

#include "filesys/off_t.h"
#include "filesys/inode.h"


/* An open file. */
struct file
{
	struct inode *inode; /* File's inode. */
	off_t pos;			 /* Current position. */
};

/* Opening and closing files. */
struct file* file_open(struct inode*);
struct file* file_reopen(struct file*);
void file_close(struct file*);
struct inode* file_get_inode(struct file*);

/* Reading and writing. */
off_t file_read(struct file*, void*, off_t);
off_t file_read_at(struct file*, void*, off_t size, off_t start);
off_t file_write(struct file*, const void*, off_t);
off_t file_write_at(struct file*, const void*, off_t size, off_t start);

/* File position. */
void file_seek(struct file*, off_t);
off_t file_tell(struct file*);
off_t file_length(struct file*);

#endif /* filesys/file.h */
