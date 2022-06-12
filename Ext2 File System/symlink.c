

#include "type.h"
int symcreat()
{
    char temp[256], parent[256], child[256];
    int pino, ino, parino;
    MINODE *pmip, *parmip;

    pino = getino(pathname);
    pmip = iget(dev, pino);


    if(getino(linker) != 0)
    {
        printf("%s in already being used as a name", linker);
        return 0;
    }

    strcpy(temp, linker); //temp because dirname()/basename() destroy string
    strcpy(parent, dirname(temp));
    strcpy(temp, linker);
    strcpy(child, basename(temp));

    printf("parent: %s     child: %s", parent, child);    

    parino = getino(parent);
    parmip = iget(dev, parino);

    ksymlink(parmip, child, pathname);

    pmip->dirty = 1;                //mark dirty

    iput(pmip);

}

int ksymlink(MINODE *pmip, char *myname, char *pathino)//creat file
{
    int ino, bno, newino, pino;
    MINODE *mino, *parmino;
    char *cp;
    char temp[256], parent[256];
    
    if((ino = ialloc(dev)) == 0)//allocate Inode
    {
        printf("Unable to allocate Inode\n");
        return 0;
    } 

    MINODE *mip = iget(dev, ino);
    INODE *ip = &mip->INODE;
    ip->i_mode = 0xA1FF; // 0644: link type with reg permission
    ip->i_uid = running->uid; // owner uid
    ip->i_gid = running->gid; // group Id
    ip->i_size = pmip->INODE.i_size; // size in bytes (no allocated block so 0)
    ip->i_links_count = 1; // links count=1
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 0; // LINUX: Blocks count in 512-byte chunks
    for(int i = 1; i < 15; i++) //ip->i_block[1] to ip->i_block[14] = 0;
    {
        ip->i_block[i] = 0;
    }

    strncpy(ip->i_block, pathino, 84);

    mip->dirty = 1; // mark minode dirty
    iput(mip); // write INODE to disk
    

    enter_name(pmip, ino, myname);


}

int readlink()
{
    int pino, linkino, parino;
    MINODE *pmip, *linkmip, *parmip;
    char *buf;
    char parent[256], temp[256];

    pino = getino(pathname);
    pmip = iget(dev, pino);

    if((pmip->INODE.i_mode & 0xF000) != 0xA000)
    {
        printf("File is not a link type\n");
        return 0;
    }

    strcpy(buf,pmip->INODE.i_block);

    printf("buf: %s\n", buf);

    strcpy(temp, linker); //temp because dirname()/basename() destroy string
    strcpy(parent, dirname(temp));

    parino = getino(parent);
    parmip = iget(dev, parino);

    linkino = search(parmip, buf);
    linkmip = iget(dev, linkino);

    printf("size of %s in %s = %d\n", pathname, buf, linkmip->INODE.i_size);



}