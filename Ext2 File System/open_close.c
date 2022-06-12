
#include "type.h"
int open_file()
{
    int ino, mode, i;
    MINODE *mip;
    OFT *oftp;

    ino = getino(pathname);
    if(ino == 0)
    {
        mycreat();
        ino = getino(pathname);
    }

    mip = iget(dev,ino);

    //mode = 0|1|2|3 for R|W|RW|APPEND 
    mode = atoi(linker); 
    if((mode > 3) || (mode < 0))
    {
        printf("Please provide a valid mode for open()\n");
        iput(mip);
        return 0;
    }


    //check if file is REG type
    if((mip->INODE.i_mode & 0xF000) != 0x8000)
    {
        printf("%s is not a REG type\n", pathname);
        iput(mip);
        return 0;
    }

    //get file descriptor
    i = getfd();

    //check if file is open for an incompatible mode
    if(running->fd[i])
    {
        if(running->fd[i]->mode != 0)
        {
            printf("%s already open in W, WR, or APPEND\n", pathname);
            iput(mip);
            return 0;
        }
    }


    //allocate an openTable entry OFT; initialize OFT entries
    oftp = (OFT *)malloc(sizeof(OFT));
    //oftp = running->fd[i];
    oftp->mode = mode;
    oftp->refCount = 1;
    oftp->minodePtr = mip;
    printf("running->fd[%d] ino = %d\n", i, mip->ino);

    switch(mode){
        case 0 : oftp->offset = 0;    // R: offset = 0
                break;
        case 1 : truncate(mip);        // W: truncate file to 0 size
                oftp->offset = 0;
                break;
        case 2 : oftp->offset = 0;     // RW: do NOT truncate file
                break;
        case 3 : oftp->offset =  mip->INODE.i_size;  // APPEND mode
                break;
        default: printf("invalid mode\n");
                return(-1);
    }
 
    //iterate to smallest i where fd[] is null

    for(i=0; running->fd[i]; i++){}

    running->fd[i] = oftp;
    printf("ino added = %d\n", running->fd[i]->minodePtr->ino);

    if(mode == 0)//for read mode only
        mip->INODE.i_atime = time(0L); //touch time
    else//for W|RW|APPEND mode
    {
        mip->INODE.i_atime = time(0L);
        mip->INODE.i_mtime = time(0L);
    }

    mip->dirty = 1;
    iput(mip);
    return i;//return i as the file descriptor

}


int getfd()
{
    int ino, i=0;
    MINODE *mip;


    ino = getino(pathname);
    mip = iget(dev,ino);


    if(!running->fd[i])
    {
        return 0;
    }

    for(i = 0; running->fd[i]; i++)
    {
        if(running->fd[i]->minodePtr == mip)
            break;
    }
    return i;
}

int truncate(MINODE *mip)
{
    int i;
    char buf[BLKSIZE];
    
    for(i = 0; i < 12; i++)
    {
        get_block(dev, mip->INODE.i_block[i], buf);
        bzero(buf, BLKSIZE);
        put_block(dev, mip->INODE.i_block[i], buf);
    }
    
    mip->INODE.i_size = 0;
    mip->dirty = 1;
}

//pfd() searches through all fd and prints their info, if in use 
int pfd()
{
    int i;
    printf(" fd    mode    offset    INODE\n");
    printf("----  ------  --------  --------\n");
    for(i = 0; i < NFD; i++)
    {
        if(running->fd[i]){
        printf(" %d   ", i);
        switch(running->fd[i]->mode){
            case 0 : printf("    R     ");    
                    break;
            case 1 : printf("    W     ");
                    break;
            case 2 : printf("    RW    ");    
                    break;
            case 3 : printf("  APPEND  "); // APPEND mode
                    break;
            }
        printf("   %d   ", running->fd[i]->offset);
        printf("   [%d, %d]\n", dev, running->fd[i]->minodePtr->ino);
        }
    }

}


//lseek() adjust the offset of file according to user input 
int mylseek()
{
    int newoff, ino, i;
    MINODE *mip;

    newoff = atoi(linker);

    i = atoi(pathname);

    if(running->fd[i]->minodePtr->INODE.i_size <= newoff)
        running->fd[i]->offset = running->fd[i]->minodePtr->INODE.i_size;
    else if(newoff >=0)
        running->fd[i]->offset = newoff;
    else
        running->fd[i]->offset = 0;

    printf("offset set to %d\n", running->fd[i]->offset);
    return running->fd[i]->offset;

}

int myclose()
{
    int myfd = atoi(pathname);
    close_file(myfd);
    return 0;

}

//close_file() closes file with corresponding fd
int close_file(int myfd)
{
    int i;
    OFT *oftp;
    MINODE *mip;

    //i = atoi(pathname);
    if(myfd > NFD)
    {
        printf("Select descriptor 0-10\n");
        return 0;
    }

    if(!running->fd[myfd])
    {
        printf("Descriptor doesnt exist\n");
        return 0;
    }

    oftp = running->fd[myfd];
    running->fd[myfd] = 0;
    oftp->refCount--;

    if(oftp->refCount > 0)
        return 0;

    mip = oftp->minodePtr;
    iput(mip);

    running->fd[myfd] = 0;

    return 0;
    
}