
#include "type.h"
int decFreeInodes(int dev)
{
    char buf[BLKSIZE];

    // dec free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count--;
    put_block(dev, 2, buf);
}

int decFreeBlocks(int dev)
{
    char buf[BLKSIZE];

    // dec free blocks count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count--;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count--;
    put_block(dev, 2, buf);
}

int tst_bit(char *buf, int bit)
{
    int i, j;
    i = bit / 8;
    j = bit % 8; // 8 = number of bits in a byte.

    //set bit to 1 if 0
    if(buf[i] & (1 << j))
    {
        return 1;
    }
    return 0;
} // in Chapter 11.3.1

int set_bit(char *buf, int bit)
{
    int i, j;
    i = bit / 8;
    j = bit % 8;

    buf[i] |= (1 << j);
    return 0;
} // in Chapter 11.3.1

int clr_bit(char *buf, int bit)
{
    int i, j;
    i = bit / 8;
    j = bit % 8;

    buf[i] &= ~(1 << j);
    return 0;
}

int ialloc(int dev)  // allocate an inode number from inode_bitmap
{
  int  i;
  char buf[BLKSIZE];

  // read inode_bitmap block
  get_block(dev, imap, buf);

  for (i=0; i < ninodes; i++){
    if (tst_bit(buf, i)==0){
        set_bit(buf, i);
        put_block(dev, imap, buf);
        printf("allocated ino = %d\n", i+1); // bits count from 0; ino from 1
        decFreeInodes(dev);
        return i+1;
    }
  }
  return 0;
}

int balloc(int dev)  // allocate an inode number from inode_bitmap
{
  int  i;
  char buf[BLKSIZE];

  get_block(dev, bmap, buf);

  for (i=0; i < ninodes; i++){
    if (tst_bit(buf, i)==0){
        set_bit(buf, i);
        put_block(dev, bmap, buf);
        decFreeBlocks(dev);
        return i+1;
    }
  }
  return 0;
}

int mymkdir()
{
    char temp[256], parent[256], child[256];
    int pino, ino;
    MINODE *pmip, *cwd;

    strcpy(temp, pathname); //temp because dirname()/basename() destroy string
    strcpy(parent, dirname(temp));
    strcpy(temp, pathname);
    strcpy(child, basename(temp));

    pino = getino(parent);
    pmip = iget(dev, pino);

    if((pmip->INODE.i_mode & 0100) != 0100){ //checking if parent is dir
        printf("Parent not a dir\n");
        return 0;
    }
    if(ino = search(pmip, child) != 0) //make sure child doesnt exist already in parent dir
    {
        printf("Directory %s already exists\n", child);
        return 0;
    }

    kmkdir(pmip, child);
    pmip->INODE.i_links_count +=1; //increment links 
    pmip->dirty = 1;                //mark dirty

    iput(pmip);
    


}

int kmkdir(MINODE *pip, char *myname)//create dir
{
    int ino, bno;
    char *cp;
    
    if((ino = ialloc(dev)) == 0)//allocate Inode
    {
        printf("Unable to allocate Inode\n");
        return 0;
    }

    if((bno = balloc(dev)) == 0)//allocate disk block
    {
        printf("Unable to allocate block\n");
        return 0;
    }

    MINODE *mip = iget(dev, ino);
    INODE *ip = &mip->INODE;
    ip->i_mode = 0x41ED; // 040755: DIR type and permissions
    ip->i_uid = running->uid; // owner uid
    ip->i_gid = running->gid; // group Id
    ip->i_size = BLKSIZE; // size in bytes
    ip->i_links_count = 2; // links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks
    ip->i_block[0] = bno; // new DIR has one data block
    for(int i = 1; i < 15; i++) //ip->i_block[1] to ip->i_block[14] = 0;
    {
        ip->i_block[i] = 0;
    }
    mip->dirty = 1; // mark minode dirty
    iput(mip); // write INODE to disk

    char buf[BLKSIZE];
    //bzero(buf, BLKSIZE); // optional: clear buf[ ] to 0
    cp = buf;
    dp = (DIR *)buf;

    // make . entry
    dp->inode = ino;
    dp->rec_len = 12;
    dp->name_len = 1;
    strcpy(dp->name, ".");

    // make .. entry: pino=parent DIR ino, blk=allocated block
    cp += dp->rec_len;
    dp = (DIR *)cp;
    dp->inode = pip->ino;
    dp->rec_len = BLKSIZE-12; // rec_len spans block
    dp->name_len = 2;
    strcpy(dp->name, "..");

    put_block(dev, bno, buf); // write to blk on disk
    
    enter_name(pip, ino, myname);

}

int enter_name(MINODE *pip, int ino, char *myname)//enters a [ino, name] as a new dir_entry into a parent directory
{
    char buf[BLKSIZE], *cp;
    DIR *dp;
    int i, remain, need_length, ideal_length;

    for(i = 0; i < 12 && pip->INODE.i_block[i] !=0; i++)//iterate blocks to last used block(assume only 12 direct blocks)
    {
        get_block(dev, pip->INODE.i_block[i], buf);//(1) get parents data block in buf
        dp = (DIR *)buf;
        cp = buf;
        while (cp + dp->rec_len < buf + BLKSIZE){ //(4) step tp last entry in data block
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        // dp NOW points at last entry in block
    }


        need_length = 4*( (8 + strlen(myname) + 3)/4 );
        ideal_length =  4*( (8 + dp->name_len + 3)/4 );
        remain = dp->rec_len - ideal_length; //last entry rec_len - ideal length

        if (remain >= need_length){//room on block
            dp->rec_len = ideal_length; //trim previous entry rec_len to its ideal length
            cp += dp->rec_len; 
            dp = (DIR *)cp;

            //printf("room on block\n");
            //printf("Adding to block %d\n", i-1);
            dp->inode = ino;
            dp->rec_len = remain;
            dp->name_len = strlen(myname);
            strcpy(dp->name, myname);

            put_block(dev, pip->INODE.i_block[i - 1], buf);
        }
        else{//no room on block
            int bno = balloc(dev); //allocate new data block
            pip->INODE.i_size += BLKSIZE; //increment parent size by block size 

            //printf("no room on block\n");
            get_block(dev, bno, buf);
            dp = (DIR *)buf;
            dp->inode = ino;
            dp->rec_len = BLKSIZE;
            dp->name_len = strlen(myname);
            strcpy(dp->name, myname);

            put_block(dev, bno, buf);
        }

    
}

int mycreat()
{
    char temp[256], parent[256], child[256];
    int pino, ino;
    MINODE *pmip, *cwd;

    strcpy(temp, pathname); //temp because dirname()/basename() destroy string
    strcpy(parent, dirname(temp));
    strcpy(temp, pathname);
    strcpy(child, basename(temp));

    pino = getino(parent);
    pmip = iget(dev, pino);

    if((pmip->INODE.i_mode & 0100) != 0100){ //checking if parent is dir
        printf("Parent not a dir\n");
        return 0;
    }
    if(ino = search(pmip, child) != 0) //make sure child doesnt exist already in parent dir
    {
        printf("File or Directory %s already exists\n", child);
        return 0;
    }

    kcreat(pmip, child);
    pmip->dirty = 1;                //mark dirty

    iput(pmip);
}

int kcreat(MINODE *pmip, char *myname)//creat file
{
    int ino, bno;
    char *cp;
    
    if((ino = ialloc(dev)) == 0)//allocate Inode
    {
        printf("Unable to allocate Inode\n");
        return 0;
    }

    MINODE *mip = iget(dev, ino);
    INODE *ip = &mip->INODE;
    ip->i_mode = 0x81A4; // 0644: REG file type and permissions
    ip->i_uid = running->uid; // owner uid
    ip->i_gid = running->gid; // group Id
    ip->i_size = 0; // size in bytes (no allocated block so 0)
    ip->i_links_count = 1; // links count=1
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 0; // LINUX: Blocks count in 512-byte chunks
    for(int i = 1; i < 15; i++) //ip->i_block[1] to ip->i_block[14] = 0;
    {
        ip->i_block[i] = 0;
    }

    mip->dirty = 1; // mark minode dirty
    iput(mip); // write INODE to disk
    
    enter_name(pmip, ino, myname);
}