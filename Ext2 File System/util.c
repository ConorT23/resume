

#include "type.h"

int get_block(int dev, int blk, char *buf)
{
   lseek(dev, (long)blk*BLKSIZE, 0);
   read(dev, buf, BLKSIZE);
}   

int put_block(int dev, int blk, char *buf)
{
   lseek(dev, (long)blk*BLKSIZE, 0);
   write(dev, buf, BLKSIZE);
}   

int tokenize(char *pathname)
{
  int i;
  char *s;
  printf("tokenize %s\n", pathname);

  strcpy(gpath, pathname);   // tokens are in global gpath[ ]
  n = 0;

  s = strtok(gpath, "/");
  while(s){
    name[n] = s;
    n++;
    s = strtok(0, "/");
  }
  name[n] = 0;
  
  for (i= 0; i<n; i++)
    printf("%s  ", name[i]);
  printf("\n");
}

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino)
{
  int i;
  MINODE *mip;
  char buf[BLKSIZE];
  int blk, offset, cpiblk;
  INODE *ip;
  MOUNT *mptr;

  //level 3
  for(i = 0; i<8; i++) //adjusts iblk for current dev
  {
    if(mountTable[i].dev == dev)
    {
      mptr = &(mountTable[i]);
      cpiblk = iblk;
      iblk = mptr->iblk;
    }
  }

  //finds minode or allocates one
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount && mip->dev == dev && mip->ino == ino){
       mip->refCount++;
       //printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
       iblk = cpiblk;
       return mip;
    }
  }
    
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount == 0){
       //printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
       mip->refCount = 1;
       mip->dev = dev;
       mip->ino = ino;

        
       // get INODE of ino to buf    
       blk    = (ino-1)/8 + iblk;
       offset = (ino-1) % 8;

       //printf("iget: ino=%d blk=%d offset=%d\n", ino, blk, offset);

       get_block(dev, blk, buf);
       ip = (INODE *)buf + offset;
       // copy INODE to mp->INODE
       mip->INODE = *ip;
       return mip;
    }
  }   
  printf("PANIC: no more free minodes\n");
  return 0;
}

void iput(MINODE *mip)
{
 int i, block, offset, cpiblk;
 char buf[BLKSIZE];
 INODE *ip;
 MOUNT *mptr;

for(int i=0; i<8; i++) //adjust iblk for current dev
{
   if(mountTable[i].dev == mip->dev)
    {
        mptr = &(mountTable[i]);
        cpiblk = iblk;
        iblk = mptr->iblk;
        //mip = mtpr->mounted_inode;
    }
    
  }

 if (mip==0) 
     return;

 mip->refCount--;
 
 if (mip->refCount > 0) return;
 if (!mip->dirty)       return;
 
 /* write INODE back to disk */
 
//mailmans algorithm
block = ((mip->ino - 1) / 8) + iblk;
offset =  ((mip->ino - 1) % 8);

get_block(mip->dev, block, buf);
ip = (INODE *)buf + offset;
*ip = mip->INODE;

put_block(mip->dev, block, buf);

} 

int search(MINODE *mip, char *name)
{
   int i; 
   char *cp, c, sbuf[BLKSIZE], temp[256];
   DIR *dp;
   INODE *ip;


    //upward traversal (cd ..)
    if(!strcmp(name, "..") && mip->ino == 2 && mip->dev != fd)
    {
      MOUNT *mnt;

      for(int i=0; i < 8; i++)
      {
        if(mountTable[i].dev == mip->dev){ //cross mount point
          mnt = &(mountTable[i]);
        }
      }
      dev = fd; //restore dev
      mip = mnt->mounted_inode; // mip is now on dev of original disk

    }
    
  printf("search for %s in MINODE = [%d, %d]\n", name,mip->dev,mip->ino);
  ip = &(mip->INODE);

   /*** search for name in mip's data blocks: ASSUME i_block[0] ONLY ***/

   get_block(dev, ip->i_block[0], sbuf);
   dp = (DIR *)sbuf;
   cp = sbuf;
   printf("  ino   rlen  nlen  name\n");

   while (cp < sbuf + BLKSIZE){
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
     printf("%4d  %4d  %4d    %s\n", 
           dp->inode, dp->rec_len, dp->name_len, dp->name);
     if (strcmp(temp, name)==0){
        printf("found %s : ino = %d\n", temp, dp->inode);
        return dp->inode;
     }
     cp += dp->rec_len;
     dp = (DIR *)cp;
   }
   return 0;
}

int getino(char *pathname)
{
  int i, ino, blk, offset;
  char buf[BLKSIZE];
  INODE *ip;
  MINODE *mip;

  printf("getino: pathname=%s\n", pathname);
  if (strcmp(pathname, "/")==0)
      return 2;
  
  // starting mip = root OR CWD
  if (pathname[0]=='/'){
     dev = root->dev;
     mip = root;
  }
  else{
    dev = running->cwd->dev;
    mip = running->cwd;
  }
    

  mip->refCount++;         // because we iput(mip) later
  
  tokenize(pathname);

  for (i=0; i<n; i++){
      int odev = dev;
      printf("===========================================\n");
      printf("getino: i=%d name[%d]=%s\n", i, i, name[i]);
 
      ino = search(mip, name[i]);
      

      if (ino==0){
         iput(mip);
         printf("name %s does not exist\n", name[i]);
         return 0;
      }
      iput(mip);
      mip = iget(dev, ino);

      //LEVEL 3 MODIFICATIONS
      if(mip->mounted)
      {
         dev = mip->mptr->dev;
         mip = iget(dev, 2);
        ino = mip->ino;

      }
   }

   iput(mip);
   return ino;
}

int access(char *filename, char mode)
{
  int r, ino;
  MINODE *mip;


  if (running->uid == 0)   // SUPERuser always OK
     return 1;

  // NOT SUPERuser: get file's INODE
  ino = getino(filename);
  mip = iget(dev, ino);

  if(mip->INODE.i_uid == running->uid)
  {
    if(strcmp(cmd, "cd")==0)
      r = (mode & 1);
    else
      r = 1;
  }
  else
    r = 0;  // by tst_bit()

  iput(mip);
  
  return r;
}

int switchProc()
{
  running->uid = !running->uid;
  printf("Switched process to %d\n", running->uid);
}
// #endif
