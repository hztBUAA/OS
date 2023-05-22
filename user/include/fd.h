#ifndef _USER_FD_H_
#define _USER_FD_H_ 1

#include <fs.h>

#define debug 0

#define MAXFD 32
#define FILEBASE 0x60000000
#define FDTABLE (FILEBASE - PDMAP)

#define INDEX2FD(i) (FDTABLE + (i)*BY2PG)
#define INDEX2DATA(i) (FILEBASE + (i)*PDMAP)

// pre-declare for forward references
struct Fd;
struct Stat;
struct Dev;

// Device struct:
// It is used to read and write data from corresponding device.
// We can use the five functions to handle data.
// There are three devices in this OS: file, console and pipe.
struct Dev {
	int dev_id;
	char *dev_name;
	int (*dev_read)(struct Fd *, void *, u_int, u_int);
	int (*dev_write)(struct Fd *, const void *, u_int, u_int);
	int (*dev_close)(struct Fd *);
	int (*dev_stat)(struct Fd *, struct Stat *);
	int (*dev_seek)(struct Fd *, u_int);
};

// file descriptor
//记录打开文件的状态，以便对文件进行管理和读写，不对应物理实体，单纯内存数据
struct Fd {
	u_int fd_dev_id;//文件所在设备的id
	u_int fd_offset;//读写文件时距离文件开头的偏移量
	u_int fd_omode;//文件打开时的读写模式
};

// State
struct Stat {
	char st_name[MAXNAMELEN];
	u_int st_size;
	u_int st_isdir;
	struct Dev *st_dev;
};

// file descriptor + file
//文件描述符+文件的组合形式，对应磁盘的物理实体，也包含内存数据
struct Filefd {
	struct Fd f_fd;//文件描述符Fd
	u_int f_fileid;//文件的id
	struct File f_file;//文件控制块
};

int fd_alloc(struct Fd **fd);
int fd_lookup(int fdnum, struct Fd **fd);
void *fd2data(struct Fd *);
int fd2num(struct Fd *);
int dev_lookup(int dev_id, struct Dev **dev);
int num2fd(int fd);
extern struct Dev devcons;
extern struct Dev devfile;
extern struct Dev devpipe;

#endif
