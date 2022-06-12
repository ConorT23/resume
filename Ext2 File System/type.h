
 #ifndef TYPE_H
 #define TYPE_H

/*************** type.h file for LEVEL-1 ****************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   


// Block number of EXT2 FS on FD
#define SUPERBLOCK 1
#define GDBLOCK 2
#define ROOT_INODE 2

// Default dir and regular file modes
#define DIR_MODE 0x41ED
#define FILE_MODE 0x81AE
#define SUPER_MAGIC 0xEF53
#define SUPER_USER 0

// file system table sizes
#define NMINODE 128
#define NMTABLE 10
#define NPROC 2
#define NFD 10
#define NOFT 40

#define FREE        0
#define READY       1

#define BLKSIZE  1024
//#define NMINODE   128
//#define NPROC       2

// Open File Table
typedef struct oft{
  int mode;
  int refCount;
  struct minode *minodePtr;
  int offset;
}OFT;

typedef struct minode{
  INODE INODE;           // INODE structure on disk
  int dev, ino;          // (dev, ino) of INODE
  int refCount;          // in use count
  int dirty;             // 0 for clean, 1 for modified

  int mounted;           // for level-3
  struct Mount *mptr;  // for level-3  

}MINODE;

typedef struct proc{
  struct proc *next;
  int          pid;      // process ID  
  int          uid;      // user ID
  int          gid;
  int          status;
  int          ppid;
  struct minode      *cwd;      // CWD directory pointer  
  OFT          *fd[NFD];
}PROC;

typedef struct Mount{
  int    dev;       // dev (opened vdisk fd number) 0 means FREE 
  int    ninodes;   // from superblock
  int    nblocks;
  int    bmap;      // from GD block  
  int    imap;
  int    iblk;
  struct minode *mounted_inode;
  char   name[64];  // device name, e.g. mydisk
  char   mount_name[64]; // mounted DIR pathname
} MOUNT;


//GLOBAL VARIABLES
MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running, *user;
MOUNT mountTable[8];

char *disk;
char gpath[128]; // global for tokenized component
char *name[64];  // assume at most 64 components in pathname
int   n;         // number of component strings

int fd, dev;
int nblocks, ninodes, bmap, imap, iblk;
char line[128], cmd[32], pathname[128], linker[128];
#endif 