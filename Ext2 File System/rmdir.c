

#include "type.h"
int incFreeInodes(int dev)
{
  char buf[BLKSIZE];
  // inc free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, buf);
  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buf);
}

int incFreeBlocks(int dev)
{
  char buf[BLKSIZE];
  // inc free blocks count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_blocks_count++;
  put_block(dev, 1, buf);
  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_blocks_count++;
  put_block(dev, 2, buf);
}

int idalloc(int dev, int ino)
{
  int i;  
  char buf[BLKSIZE];

  if (ino > ninodes){  
    printf("inumber %d out of range\n", ino);
    return;
  }
  
  get_block(dev, imap, buf);  // get inode bitmap block into buf[]
  
  clr_bit(buf, ino-1);        // clear bit ino-1 to 0

  put_block(dev, imap, buf);  // write buf back
  incFreeInodes(dev);
}

int bdalloc(int dev, int bno)
{
  int i;  
  char buf[BLKSIZE];

  if (bno > nblocks){  
    printf("Block %d out of range\n", bno);
    return;
  }
  
  get_block(dev, bmap, buf);  // get inode bitmap block into buf[]
  
  clr_bit(buf, bno-1);        // clear bit ino-1 to 0

  put_block(dev, bmap, buf);  // write buf back
  incFreeBlocks(dev);
}

int myrmdir()
{
  char *cp, buf[BLKSIZE], temp[256];
  int ino, pino, count;
  MINODE *mip, *pmip;
  DIR *dp;

  ino = getino(pathname);
  mip = iget(dev, ino);

  if((mip->INODE.i_mode & 0100) != 0100) //check if dir
  {
    printf("%s is not a directory\n", pathname);
    return 0;
  }

  if(mip->refCount != 1)//check if busy
  {
    printf("Cannot remove %s, directory is busy\n", pathname);
    return 0;
  }

  if(mip->INODE.i_links_count > 2) //check if empty
  {
    printf("Directory %s is not empty\n", pathname);
    //printf("Directory has %d links\n", mip->INODE.i_links_count);
    return 0;
  }

  if(!access(pathname,mip->INODE.i_mode))
  {
    printf("Permissions not correct\n");
    return 0;
  }

  pino = search(mip, "..");//get parent ino and inode
  pmip = iget(dev, pino);

  strcpy(temp, pathname);
  rm_child(pmip, basename(temp));//remove dir_entry of name from parent directory 

  pmip->INODE.i_links_count -= 1; //decrease parents link count, set dirty
  pmip->dirty = 1;
  iput(pmip);

  for(int i = 0; i<12; i++) //deallocate blocks and inode
  {
    if(mip->INODE.i_block[i] == 0)
      continue;
    bdalloc(mip->dev, mip->INODE.i_block[i]);
  }
  idalloc(mip->dev, mip->ino);
  iput(mip);

  
}

int rm_child(MINODE *pmip, char *myname)//removes the dir_entry of name from a parent directory minode pointed by pmip.
{
  int i, j;
  char *cp, *pcp, buf[BLKSIZE], tempname[256];
  DIR *dp, *pdp;

  for(i = 0; i < 12; i++)//assume only 12 direct blocks; Search parent INODE’s data block(s) for the entry of name
  {
    get_block(dev, pmip->INODE.i_block[i], buf); //get block buf
    dp = (DIR *)buf;
    cp = buf;
    
    while (cp < buf + BLKSIZE){ //iterates through dir until matching name is found, exits when found
      strncpy(tempname, dp->name, dp->name_len); 
      tempname[dp->name_len] = 0;

      if(strcmp(tempname, myname) == 0)
      {
        break; //exit while loop
      }

      pcp = cp; //save previous cp
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }

    if(cp == buf) //entry is first and only
    {
      bdalloc(dev, pmip->INODE.i_block[i]);
      pmip->INODE.i_size -= BLKSIZE; //reduce parents file size by BLKSIZE

      for(j=i; j+i<12; j++)//compact parents i_bock array to account for deallocated block
      {
        pmip->INODE.i_block[j] = pmip->INODE.i_block[j + 1];
      }
      break;
    }
    else if(cp + dp->rec_len == buf + BLKSIZE) //entry is last on the block
    {
      pdp = (DIR *)pcp; //get prev entry
      pdp->rec_len += dp->rec_len; //abosrb rec len into predecessor entry
      break;
    }
    else//entry is in the middle of block
    {
      int rec_len = dp->rec_len;
        
        pcp = cp;
        cp += dp->rec_len;
        dp = (DIR *)cp;
        pdp = (DIR *)pcp;
        while(cp + dp->rec_len < buf + BLKSIZE)// move all trailing entries left to overlay the deleted entry
        {
          memcpy(pcp, cp, dp->rec_len);
          cp += dp->rec_len;
          dp = (DIR *)cp;
          pcp += pdp->rec_len;
          pdp = (DIR *)pcp;
        }
        memcpy(pcp, cp, dp->rec_len);

        //update last entry with rec_len of deleted entry
        dp = (DIR *)pcp;
        dp->rec_len += rec_len;
        break;
    }
  }
  put_block(dev, pmip->INODE.i_block[i], buf);
}