#ifndef FILESYS_INODE_H
#define FILESYS_INODE_H

#include "devices/block.h"
#include "filesys/off_t.h"
#include <stdbool.h>

#include <debug.h>
#include <list.h>
#include <round.h>
#include <string.h>
#include "threads/synch.h"

/* On-disk inode.
	Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk {
	block_sector_t start; /* First data sector. */
	off_t length;			 /* File size in bytes. */
	unsigned magic;		 /* Magic number. */
	uint32_t unused[125]; /* Not used. */
};

/* In-memory inode. */
struct inode {
	struct list_elem elem;	/* Element in inode list. */
	block_sector_t sector;	/* Sector number of disk location. */
	int open_cnt;				/* Number of openers. */
	bool removed;				/* True if deleted, false otherwise. */
	struct inode_disk data; /* Inode content. */
	
	unsigned current_readers; // :) the actual read count of how many are reading the given file currentyly concurrently actually given tfile
	struct semaphore writers_lock; 
	struct semaphore readers_lock;
};

struct bitmap;

void inode_init(void);
bool inode_create(block_sector_t, off_t);
struct inode* inode_open(block_sector_t);
struct inode* inode_reopen(struct inode*);
block_sector_t inode_get_inumber(const struct inode*);
void inode_close(struct inode*);
void inode_remove(struct inode*);
off_t inode_read_at(struct inode*, void*, off_t size, off_t offset);
off_t inode_write_at(struct inode*, const void*, off_t size, off_t offset);
off_t inode_length(const struct inode*);

#endif /* filesys/inode.h */
