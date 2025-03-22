#ifndef RWSHARED_H
#define RWSHARED_H

#define RW_MAJ		501
#define RW_MIN		3

#define IOCTL_WRITE_TO	0x4242

struct write_to {
	pid_t	dst_pid;
	off_t	off_from_elf;
	char	value;
};

// The device should implement the ioctl IOCTL_WRITE_TO
// This ioctl should write the byte value to the destination pid
// The value should be written at an offset from the base of the "rw_user" elf file
// The offset is given by the off_from_elf entry in the structure
// The offset is given as a number of bytes offset from the base of the elf64
// The byte to write is given by the value entry
// The pid of the copy of rw_user to write to is given by dst_pid


#endif
