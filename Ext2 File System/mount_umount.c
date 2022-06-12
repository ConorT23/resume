#include "type.h"

// load root INODE and set root pointer to it
int mount_root()
{  
    char *diskName = &disk;
    printf("mount_root()\n");
    root = iget(dev, 2);
    mountTable[0].dev = root->dev;
    mountTable[0].bmap = bmap;
    mountTable[0].iblk = iblk;
    mountTable[0].imap = imap;
    mountTable[0].nblocks = nblocks;
    mountTable[0].ninodes = ninodes;
    root = iget(dev, 2);

    mountTable[0].dev = root->dev;
    strcpy(mountTable[0].mount_name, "/");
    strcpy(mountTable[0].name, disk);
    //mountTable[0].name = (char*)disk;
}

// MOUNT *getmptr(int dev)
// {

//   for(int i=0; i<8; i++)
//   {
//     if(mountTable[i].dev == dev)
//     {
//       return mountTable[i].dev;
//     }
    
//   }

// }

int mymount()
{
    char lne[256], mnt[128], dir[128];

    //ask for device and directory to be mounted on 
    printf("enter rootdev name and directory (enter for list of mounts): ");
    fgets(lne, 256, stdin);
    lne[strlen(lne)-1] = 0;

    if(strcmp(lne, "") == 0)
    {
        printf("displaying mounts\n");
        displaymnt();
        return;
    }
    sscanf(lne, "%s %s", mnt, dir);
    printf("mnt=%s dir=%s \n", mnt, dir);

    if(!checkmnt(mnt))//check if FS is already mounted
    {
        allocatemnt(mnt, dir);//allocate FS
    }

}

int myumount()
{
    MOUNT *mnt;
    //check if FS is mounted already
    if((mnt = checkmnt(pathname)) == 0)
    {
        return 0;
    }

    //check if files are open in FS
    for(int i = 0; i<NMINODE; i++) 
    {
        // printf("SEARCHING FOR OPEN FILES IN FS\n");
        // printf("%d\n",minode[i].dev);
        if(mnt->dev == minode[i].dev);
        {
            printf("File is still open in FS\n");
            return 0;
        }
    }

    mnt->mounted_inode->mounted = 0;
    mnt->mounted_inode->dirty = 1;
    iput(mnt->mounted_inode);

    mnt->dev = 0;
    return 1;
}

int displaymnt()
{
    for(int i=0; i<8; i++)
    {
        if(mountTable[i].dev == 0)
        {
            break;
        }
        else
        {
            printf("%s is mounted on %s\n", mountTable[i].name, mountTable[i].mount_name);
        }
        
    }
}

int checkmnt(char check[]) //checks if FS is mounted already return MTABLE value if mounted
{
    MOUNT *mnt;
    printf("check: %s\n", check);
    for(int i=0; i<8; i++)
    {
        if(strcmp(mountTable[i].name, check) == 0)
        {
            printf("%s is already mounted\n", check);
            
            mnt = &(mountTable[i]);

            return mnt;
        }
    }
    return 0;
}

int allocatemnt(char mnt[], char dir[]) //creates FS
{
    int mntfd, ino, cpdev;

    MINODE *mip;

    MOUNT *mntp;

    char buf[BLKSIZE];

    for(int i=0; i<8; i++)
    {
        if(mountTable[i].dev == 0)
        {
            printf("mounttable[%d] free\n", i);
            
            if((mntfd = open(mnt, O_RDWR)) < 0)
            {
                printf("open %s failed\n", mnt);
                exit(1);
            }

            mntp = &(mountTable[i]);

            mntp->dev = mntfd;

            get_block(mntfd, 1, buf);
            sp = (SUPER *)buf;

            if (sp->s_magic != 0xEF53){
            printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
            exit(1);
            }     
            printf("EXT2 FS OK\n");
            cpdev = dev;

            //get mount point
            ino = getino(dir);
            mip = iget(dev, ino);
            if(mip ==0)
            {
                printf("%s doesnt exist\n");
                close(mntfd);
                return 0;
            }


            
            if((mip->INODE.i_mode & 0100) != 0100) //check if mount point isdir
            {
                printf("%s is not a directory\n", dir);
                close(mntfd);
                return 0;
            }

            if(mip->refCount != 1)//check if mount point is busy
            {
                printf("Cannot remove %s, directory is busy\n", mnt);
                close(mntfd);
                return 0;
            }

            //allocate mountable entries
            mntp->ninodes = sp->s_inodes_count;
            mntp->nblocks = sp->s_blocks_count;

            get_block(mntfd, 2, buf); 
            gp = (GD *)buf;

            mountTable[i].bmap = gp->bg_block_bitmap;
            mountTable[i].imap = gp->bg_inode_bitmap;
            mountTable[i].iblk = gp->bg_inode_table;

            mip->mounted = 1; //mark mounted
            mip->mptr = mntp;
            mountTable[i].mounted_inode = mip;

            strcpy(mountTable[i].name, mnt);
            strcpy(mountTable[i].mount_name, dir);

            dev = cpdev; //restore dev
            break;

        }
        else
        {
            printf("mounttable[%d] filled\n", i);
        }
        
    }

}