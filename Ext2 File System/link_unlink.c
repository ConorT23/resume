
#include "type.h"
int link()                      // link old_file new_file
{
    int oino, pino;
    MINODE *omip, *pmip;
    char parent[256], child[256], temp[256];
    //link is only for files, error check to make sure link is linking a file not a dir
    oino = getino(pathname);    //get inode of old file -> gpath[1]
    if(oino == 0)
    {
        printf("%s does not exist\n", pathname);
        return 0;
    }
    omip = iget(dev, oino);     //get minode using old inode

    strcpy(temp, linker); //temp because dirname()/basename() destroy string
    strcpy(parent, dirname(temp));
    strcpy(temp, linker);
    strcpy(child, basename(temp));
 
    if((omip->INODE.i_mode & 0xF000) == 0x4000)
    {
      printf("cannot link a dir\n");
      return 0;
    }
    else if(getino(linker) != 0)
    {
      printf("%s already exist\n", linker);
      return 0;
    }

    pino = getino(parent);
    pmip = iget(dev, pino);

    printf("GOT PINO AND PMIP\n");
    enter_name(pmip, oino, child);

    omip->INODE.i_links_count++;
    omip->dirty = 1;
    iput(omip);
    iput(pmip);

    
    
}

int unlink()
{
    int ino, pino;
    MINODE *mip, *pimp;
    char parent[256], child[256], temp[256];


    ino = getino(pathname);
    mip = iget(dev, ino);

    strcpy(temp, pathname); //temp because dirname()/basename() destroy string
    strcpy(parent, dirname(temp));
    strcpy(temp, pathname);
    strcpy(child, basename(temp));

    if((mip->INODE.i_mode & 0xF000) == 0x4000)
    {
        printf("cannot unlink a dir\n");
        return 0;
    }

    if(!access(pathname, mip->INODE.i_mode))
    {
        printf("Permissions not correct\n");
        return 0;
    }

    pino = getino(parent);
    pimp = iget(dev, pino);

    rm_child(pimp, pathname);
    pimp->dirty = 1;
    iput(pimp);

    mip->INODE.i_links_count--;
    if(mip->INODE.i_links_count > 0)
    {
        mip->dirty = 1;
    }
    else
    {
        idalloc(dev, ino);
    }
    iput(mip);

}