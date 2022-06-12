
#include "type.h"

int read_file()
{
    int i, n;
    char *buf[BLKSIZE];

    i = atoi(pathname);

    if(running->fd[i])
    {
        n = myread(i, buf, atoi(linker));
        if(n != 0)
        {
            printf("%s\n", buf);
        } 

        return 1;
    }
    else{
        printf("File not open\n");
    }

    return 0;

}

int myread(int fd, char *buf, int nbytes)
{
    int count = 0, avil, lbk, startByte, blk, remain;
    char *cq = buf, readbuf[BLKSIZE];
    u32 ibuf[256], ibuf2[256];
    MINODE *mip;
    OFT *oftp;

    //(1) obtain oft pointer and computer available bytes
    oftp = running->fd[fd];
    if(!oftp || fd > NFD)
    {
        printf("fd invalid\n");
        return 0;
    }

    //check if file is R or RW
    if((oftp->mode == 1) || (oftp->mode == 3))
    {
        printf("File not in read mode\n");
        return 0;
    }
    
    mip = oftp->minodePtr;

    //bytes available to read
    avil = mip->INODE.i_size - oftp->offset;

    //(2)find correct data block with lbk and (3) convert lbk to blk
    while(nbytes && avil)
    {
        //current byte position falls on a LOGICAL block lbk
        lbk = oftp->offset / BLKSIZE;
        startByte = oftp->offset % BLKSIZE;

       if (lbk < 12){                     // lbk is a direct block
           blk = mip->INODE.i_block[lbk]; // map LOGICAL lbk to PHYSICAL blk
       }
       else if (lbk >= 12 && lbk < 256 + 12) { //indirect blocks
           get_block(dev, mip->INODE.i_block[12], ibuf);
           blk = ibuf[lbk - 12];
       }
       else{ //double indirect blocks
             //mailmans algorithm
           int indblk = (lbk - (256 + 12)) / 256; //indirect block
           int indoffset = (lbk - (256 + 12)) % 256; //indirect offset
    

           get_block(dev, mip->INODE.i_block[13], ibuf); //get iblock 13
           get_block(dev, ibuf[indblk], ibuf2); //get double indirect block
           blk = ibuf2[indoffset]; //set block to logical block
       } 
    

    // (4) read data block into readbuf[BLKSIZE] 
    get_block(mip->dev, blk, readbuf);

    /* copy from startByte to buf[ ], at most remain bytes in this block */
    char *cp = readbuf + startByte;   
    remain = BLKSIZE - startByte;   // number of bytes remaining in block

    while (remain > 0){     // (5)copy bytes from readbuf[] into buf[]
        *cq++ = *cp++;             
        oftp->offset++;           // advance offset 
        count++;                  // inc count as number of bytes read
        avil--; nbytes--;  remain--;
        if (nbytes <= 0 || avil <= 0) 
            break;
    }

    }

    iput(mip);
    //printf("myread: read %d char from file descriptor %d\n", count, fd);  
    return count;   // (6)count is the actual number of bytes read
    
}

int mycat()
{
    int n, i;
    char buf[1024];

    strcpy(linker, "0"); //linker used in open_file as mode
    int myfd = open_file();

    
    printf("\n");
    while((n = myread(myfd, buf, 1024))){
        buf[n] = 0; // make null terminated string
        for(i=0; i<n; i++){ //spit out chars from buf
            putchar(buf[i]);
        }
    }

    printf("\n");
    myclose(myfd);
    return 1;
}