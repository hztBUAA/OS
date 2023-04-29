/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;
	u_int op_read = DEV_DISK_OPERATION_READ;
	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno; //?
		/* Exercise 5.3: Your code here. (1/2) */
		uint32_t offset;
		offset = begin + off;
		if (syscall_write_dev((u_int) &temp, DEV_DISK_ADDRESS|DEV_DISK_ID, 4) <0){
			panic_on("ide_read panic");
		}
		if (syscall_write_dev((u_int) &offset, DEV_DISK_ADDRESS|DEV_DISK_OFFSET, 4) <0){
			panic_on("ide_read panic");
		}
		if (syscall_write_dev((u_int) &op_read, DEV_DISK_ADDRESS|DEV_DISK_START_OPERATION, 4) <0){
			panic_on("ide_read panic");
		}
		u_int result;
		if (syscall_read_dev((u_int) &result, DEV_DISK_ADDRESS|DEV_DISK_STATUS, 4) <0){
			panic_on("ide_read panic");
		}
		if (!result)
		{
			panic_on("ide_read panic");
		}
		if (syscall_read_dev((u_int)dst+off, DEV_DISK_ADDRESS|DEV_DISK_BUFFER, BY2SECT) <0)
		{
			panic_on("ide_read panic");
		}
		
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
		uint32_t offset;
		u_int op_write = DEV_DISK_OPERATION_WRITE;
		offset = begin + off;
		if (syscall_write_dev((u_int) &temp, DEV_DISK_ADDRESS|DEV_DISK_ID, 4) <0){
			panic_on("ide_write diskno panic");
		}
		if (syscall_write_dev((u_int) &offset, DEV_DISK_ADDRESS|DEV_DISK_OFFSET, 4) <0){
			panic_on("ide_write offset panic");
		}
		if (syscall_write_dev((u_int)src+off, DEV_DISK_ADDRESS|DEV_DISK_BUFFER, BY2SECT) <0)
		{
			panic_on("ide_write buff panic");
		}
		if (syscall_write_dev((u_int) &op_write, DEV_DISK_ADDRESS|DEV_DISK_START_OPERATION, 4) <0){
			panic_on("ide_write operation panic");
		}
		u_int result;
		if (syscall_read_dev((u_int) &result, DEV_DISK_ADDRESS|DEV_DISK_STATUS, 4) <0){
			panic_on("ide_write read result panic");
		}
		if (!result)
		{
			panic_on("ide_write result panic");
		}
	}
}
