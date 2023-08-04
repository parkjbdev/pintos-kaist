#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/init.h"
#include "threads/palloc.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "threads/flags.h"
#include "intrinsic.h"
#include "userprog/process.h"
#include "utils.h"

void syscall_entry(void);
void syscall_handler(struct intr_frame *);

/* System call.
 *
 * Previously system call services was handled by the interrupt handler
 * (e.g. int 0x80 in linux). However, in x86-64, the manufacturer supplies
 * efficient path for requesting the system call, the `syscall` instruction.
 *
 * The syscall instruction works by reading the values from the the Model
 * Specific Register (MSR). For the details, see the manual. */

#define MSR_STAR 0xc0000081         /* Segment selector msr */
#define MSR_LSTAR 0xc0000082        /* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* Mask for the eflags */

void syscall_init(void) {
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48 | ((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t)syscall_entry);

	/* The interrupt service rountine should not serve any interrupts
	 * until the syscall_entry swaps the userland stack to the kernel
	 * mode stack. Therefore, we masked the FLAG_FL. */
	write_msr(MSR_SYSCALL_MASK, FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
}

/* The main system call interface */
void syscall_handler(struct intr_frame *f) {
	switch (f->R.rax) {
	/* Projects 2 and later. */
	case SYS_HALT:
		/* Halt the operating system. */
		halt();
		break;
	case SYS_EXIT:
		/* Terminate this process. */
		exit(f->R.rdi);
		break;
	case SYS_FORK:
		/* Clone current process. */
		f->R.rax = process_fork((const char *)f->R.rdi, f);
		break;
	case SYS_EXEC:
		/* Switch current process. */
		if ((f->R.rax = exec((const char *)f->R.rdi) == -1))
			exit(-1);
		break;
	case SYS_WAIT:
		/* Wait for a child process to die. */
		f->R.rax = process_wait(f->R.rdi);
		break;
	case SYS_CREATE:
		/* Create a file. */
		break;
	case SYS_REMOVE:
		/* Delete a file. */
		break;
	case SYS_OPEN:
		/* Open a file. */
		break;
	case SYS_FILESIZE:
		/* Obtain a file's size. */
		break;
	case SYS_READ:
		/* Read from a file. */
		break;
	case SYS_WRITE:
		/* Write to a file. */
		f->R.rax = write(f->R.rdi, (void *)f->R.rsi, f->R.rdx);
		break;
	case SYS_SEEK:
		/* Change position in a file. */
		break;
	case SYS_TELL:
		/* Report current position in a file. */
		break;
	case SYS_CLOSE:
		/* Close a file. */
		break;

	/* Project 3 and optionally project 4. */
	case SYS_MMAP:
		/* Map a file into memory. */
		break;
	case SYS_MUNMAP:
		/* Remove a memory mapping. */
		break;

	/* Project 4 only. */
	case SYS_CHDIR:
		/* Change the current directory. */
		break;
	case SYS_MKDIR:
		/* Create a directory. */
		break;
	case SYS_READDIR:
		/* Reads a directory entry. */
		break;
	case SYS_ISDIR:
		/* Tests if a fd represents a directory. */
		break;
	case SYS_INUMBER:
		/* Returns the inode number for a fd. */
		break;
	case SYS_SYMLINK:
		/* Returns the inode number for a fd. */
		break;

	/* Extra for Project 2 */
	case SYS_DUP2:
		/* Duplicate the file descriptor */
		break;

	case SYS_MOUNT:
		break;
	case SYS_UMOUNT:
		break;

	default:
		PANIC("[ERROR] system call %lld is not implemented!\n", f->R.rax);
		thread_exit();
		break;
	}
}

void halt(void) { power_off(); }

void exit(int status) {
	struct thread *curr = thread_current();
	curr->exit_status = status;
	thread_exit();
}

int exec(const char *cmd_line) {
	char *cmd_copy = strpdup(cmd_line);
	if (cmd_copy == NULL)
		return -1;

	int ret = process_exec(cmd_copy);

    return ret < 0 ? -1 : ret;
}

int write(int fd, const void *buffer, unsigned size) {
	if (fd == 1) {
		putbuf(buffer, size);
		return size;
	}
	return -1;
}

