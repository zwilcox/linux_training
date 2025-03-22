#ifndef SHUFSHARED_H
#define SHUFSHARED_H

#define SHUF_MAJ        0x123
#define SHUF_MIN	0x456

#define SHUFBUF_SZ	0x100

// The Shuf_driver should create a device with the above Major / Minor number
// Provided is a userspace program that will try to interact with the buffer, which should be run as root
// The buffer should start off as all zeros
// The driver should have the following file operations implemented for interacting with a buffer of size SHUFBUF_SZ

// Read
// 	Read does a normal read at the correct offset in the buffer
// 	If a read tries to read over the edge of the buffer, it will read only until the end
// 	If a read tries to read when it's file offset is already at SHUFBUF_SZ, read returns zero

// Write
// 	Write will not write bytes to the buffer, but rather xors them with the existing bytes in the buffer
// 	After a write, the affected area in the buffer has been xored with the input byte for byte
// 	If a write tries to write past the end of a buffer, it will only write up until the end of the buffer

// Lseek
// 	Lseek acts as expected, honoring SEEK_SET SEEK_CUR and SEEK_END
// 	Lseek will clamp to 0 or SHUFBUF_SZ if the offset tries to become too low or too high, respectively

// Other Tips
// 	Make sure to protect the buffer with some sort of lock
// 	Don't forget to unregister the Maj/Min on unload

#endif
