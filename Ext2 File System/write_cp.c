#include "type.h"

int write_file()
{
    int i, n;
    char *buf[BLKSIZE];

    i = atoi(pathname);
    n = strlen(linker);
    
    
    if(running->fd[i]->mode != 0)
    {
        mywrite(i, linker, n);
        return 1;
    }
    else{
        printf("File is not open or is wrong mode\n");
    }
    return 0;
}

int mywrite(int fd, char *buf, int nbytes)
{
    int lbk, startByte, blk, remain, count;
    u32 ibuf[256], ibuf2[256], ibuf3[256];

    char wbuf[BLKSIZE];

    OFT *oftp;
    MINODE *mip;
    
    oftp = running->fd[fd];

    mip = oftp->minodePtr;

    count = 0;

    while(nbytes > 0)
    {
          lbk       = oftp->offset / BLKSIZE;
          startByte = oftp->offset % BLKSIZE;

        if (lbk < 12)   // direct block
        {  

            if (mip->INODE.i_block[lbk] == 0)  // if no data block yet
            { 
                mip->INODE.i_block[lbk] = balloc(mip->dev);// MUST ALLOCATE a block
            }

            blk = mip->INODE.i_block[lbk];      // blk should be a disk block now

        }
        else if (lbk >= 12 && lbk < 256 + 12){ // INDIRECT blocks 
            if (mip->INODE.i_block[12] == 0){
                mip->INODE.i_block[12] = balloc(mip->dev);// MUST ALLOCATE a block
                bzero(ibuf, 256);
                put_block(mip->dev, mip->INODE.i_block[12], ibuf);
            }
            get_block(mip->dev, mip->INODE.i_block[12], ibuf);
            if (ibuf[lbk - 12] == 0){
                ibuf[lbk - 12] = balloc(mip->dev);
                put_block(mip->dev, mip->INODE.i_block[12], ibuf);
            }
            blk = ibuf[lbk-12];
            
        }
        else{
            int indblk = (lbk - (256 + 12)) / 256; //indirect block
            int indoffset = (lbk - (256 + 12)) % 256; //indirect offset
            if (mip->INODE.i_block[13] == 0){
                mip->INODE.i_block[13] = balloc(mip->dev);// MUST ALLOCATE a block
                bzero(ibuf, 256);
                put_block(mip->dev, mip->INODE.i_block[13], ibuf);
            }
            get_block(mip->dev, mip->INODE.i_block[13], ibuf); //get indirect


            if(ibuf[indblk] == 0) 
            {
                ibuf[indblk] = balloc(mip->dev); //allocate block
                bzero(ibuf2, 256);
                put_block(mip->dev, ibuf[indblk], ibuf2); 
                //put_block(mip->dev, mip->INODE.i_block[13], ibuf);
            }

            get_block(mip->dev, ibuf[indblk], ibuf2); //get double indirect block
            if(ibuf2[indoffset] == 0)
            {
                ibuf2[indoffset] = balloc(mip->dev);
            }
            blk = ibuf2[indoffset]; //set block to logical block

        }
        get_block(mip->dev, blk, wbuf);   // read disk block into wbuf[ ]  
        char *cp = wbuf + startByte;      // cp points at startByte in wbuf[]
        
        remain = BLKSIZE - startByte;     // number of BYTEs remain in this block

        while (remain > 0){               // write as much as remain allows  
            *cp++ = *buf++;              // cq points at buf[ ]
            nbytes--; remain--;         // dec counts
            oftp->offset++;             // advance offset
            if (oftp->offset > mip->INODE.i_size)  // especially for RW|APPEND mode
                mip->INODE.i_size++;    // inc file size (if offset > fileSize)
            if (nbytes <= 0) break;     // if already nbytes, break
        }
        put_block(mip->dev, blk, wbuf);   // write wbuf[ ] to disk

    }

    mip->dirty = 1;       // mark mip dirty for iput() 
    iput(mip);
    printf("wrote %d char into file descriptor fd=%d\n", nbytes, fd);           
    return nbytes;
}

int mycp()            //cp "src" "dst"
{
    // int nbytes, f_d, ino, n;
    // int i=0;
    char buf[BLKSIZE], fname[128], str[128];
    // MINODE *mip;
    // OFT *oftp;

    int src, dst;
    char temp[128];


    strcpy(temp, linker);       
    strcpy(linker, "0");
    src = open_file();      //open "scr" "0" (temp = dst)


    pfd();

    strcpy(pathname, temp);
    strcpy(linker, "1");
    dst = open_file();      //open "dst" "1"

    pfd();

    while(n = myread(src, buf, BLKSIZE))
    {
        mywrite(dst, buf, n);
    }


    close_file(src);
    close_file(dst);

    return 0;

}