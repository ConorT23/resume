
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>

#include "type.h"
#include "util.c"
#include "cd_ls_pwd.c"
#include "mkdir_creat.c"
#include "rmdir.c"
#include "link_unlink.c"
#include "symlink.c"
#include "open_close.c"
#include "read_cat.c"
#include "write_cp.c"
#include "mount_umount.c"

int init()
{
  int i, j;
  MINODE *mip;
  PROC   *p;

  printf("init()\n");

  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->uid = p->gid = i;
    p->cwd = 0;
  }
}

// load root INODE and set root pointer to it
// int mount_root()
// {  
//   printf("mount_root()\n");
//   root = iget(dev, 2); 
// }

//char *disk = "disk2";
int main(int argc, char *argv[ ])
{
  disk = argv[1];
  int ino;
  char buf[BLKSIZE];

  printf("checking EXT2 FS ....");
  if ((fd = open(disk, O_RDWR)) < 0){ //open device for RW
    printf("open %s failed\n", disk);
    exit(1);
  }

  dev = fd;    // global dev same as this fd   

  /********** read super block  ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system ***********/
  if (sp->s_magic != 0xEF53){
      printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
      exit(1);
  }     
  printf("EXT2 FS OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

  get_block(dev, 2, buf); 
  gp = (GD *)buf;  //read group descriptor

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  iblk = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, iblk);

  init();  
  mount_root();
  printf("root refCount = %d\n", root->refCount);

  printf("creating P0 as running process\n");
  running = &proc[0];
  running->status = READY;
  running->cwd = iget(dev, 2); //set cwd to root
  printf("root refCount = %d\n", root->refCount);

  user = &proc[1];
  user->status = READY;

  // WRTIE code here to create P1 as a USER process
  
  while(1){
    printf("input command : [ls|cd|pwd|mkdir|creat|rmdir|link|unlink|symlink|readlink|quit] ");
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;

    if (line[0]==0)
       continue;
    pathname[0] = 0;

    sscanf(line, "%s %s %s", cmd, pathname, linker);
    printf("cmd=%s pathname=%s\n", cmd, pathname);
    


  
    if (strcmp(cmd, "ls")==0)
       ls();
    else if (strcmp(cmd, "cd")==0)
       cd();
    else if (strcmp(cmd, "pwd")==0)
       pwd(running->cwd);
    else if (strcmp(cmd, "mkdir")==0)
        mymkdir();
    else if (strcmp(cmd, "rmdir")==0)
        myrmdir();
    else if (strcmp(cmd, "creat")==0)
        mycreat();
    else if (strcmp(cmd, "link")==0)
        link();
    else if (strcmp(cmd, "unlink")==0)
        unlink();
    else if (strcmp(cmd, "symlink")==0)
        symcreat();
    else if (strcmp(cmd, "readlink")==0)
        readlink();
    else if (strcmp(cmd, "open")==0)
        open_file();
    else if (strcmp(cmd, "pfd")==0)
        pfd();
    else if (strcmp(cmd, "lseek")==0)
          mylseek();
    else if (strcmp(cmd, "close")==0)
          myclose();
    else if (strcmp(cmd, "read")==0)
          read_file();
    else if (strcmp(cmd, "write")==0)
          write_file();
    else if (strcmp(cmd, "cat")==0)
          mycat();
    else if (strcmp(cmd, "cp")==0)
          mycp();
    else if (strcmp(cmd, "mount")==0)
          mymount();
    else if (strcmp(cmd, "switch")==0)
          switchProc();
    else if (strcmp(cmd, "umount")==0)
          myumount();
    else if (strcmp(cmd, "quit")==0)
       quit();
    else
      printf("%s is not a recognized command\n", cmd);
  }
}

int quit()
{
  int i;
  MINODE *mip;
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount > 0)
      iput(mip);
  }
  exit(0);
}
