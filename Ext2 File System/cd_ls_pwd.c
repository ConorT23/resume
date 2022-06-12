/************* cd_ls_pwd.c file **************/
#include "type.h"
int cd()
{
  MINODE *mip;

  if(pathname[0] == 0){ // if no pathname provided
    mip = iget(dev, 2); //set mip to root
    running->cwd = mip; 
    iput(running->cwd); 
    return 0;
  }

  int ino = getino(pathname);
  if(!ino){
    printf("ino = 0\n");
    return 0;
  }

  mip = iget(dev, ino);


  if((mip->INODE.i_mode & 0100) != 0100) //check if file is dir 
  {
    printf("file not a directory\n");
    return -1;
  }
  
  
  iput(running->cwd);
  running->cwd = mip;
  iput(mip);
}
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int ls_file(int ino, char *name)
{
  int r, i;
  char ftime[64];
  
  MINODE *mip = iget(dev, ino);
  INODE *ip = &(mip->INODE);


  if ((ip->i_mode & 0xF000) == 0x8000) // if (S_ISREG())
    printf("%c",'-');
  if ((ip->i_mode & 0xF000) == 0x4000) // if (S_ISDIR())
    printf("%c",'d');
  if ((ip->i_mode & 0xF000) == 0xA000) // if (S_ISLNK())
    printf("%c",'l');

  for (i=8; i >= 0; i--){
    if (ip->i_mode & (1 << i)) // print r|w|x
      printf("%c", t1[i]);
    else
      printf("%c", t2[i]); // or print -
    }

  printf("%4d ",ip->i_links_count); // link count
  printf("%4d ",ip->i_gid); // gid
  printf("%4d ",ip->i_uid); // uid
  printf("%8d ",ip->i_size); // file size
  printf("%4d ", ino);

  // print time
  strcpy(ftime, ctime(&ip->i_mtime)); // print time in calendar form
  ftime[strlen(ftime)-1] = 0; // kill \n at end
  printf("%s ",ftime);
  
  // print name
  printf("%s", basename(name)); // print file basename
  if((strcmp(name, ".") != 0) && (strcmp(name, "..") != 0) && (strcmp(name, "lost+found") != 0) && ((ip->i_mode & 0xF000) != 0x4000) && ((ip->i_mode & 0xF000) == 0xA000))
  {
    printf(" -> %s", ip->i_block);
  } 
  printf("\n");
  
  iput(mip);
}




int ls_dir(int ino)
{
  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;
  MINODE *mip;

  mip = iget(dev, ino);
  
  get_block(dev, mip->INODE.i_block[0], buf); //gets block buffer
  dp = (DIR *)buf;
  cp = buf;
  
  while (cp < buf + BLKSIZE){ //iterates through dir and calls ls_file to print
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
	
     ls_file(dp->inode, temp);

     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  iput(mip); 

}

int ls()
{
  int ino;
  MINODE *mip;
  char *filename;

  // default to CWD if no pathname
  if(strcmp(pathname, ""))
    filename = pathname;
  else
    filename = ".";

  ino = getino(filename);
  if(ino == 0)
    return 0;

  mip = iget(dev, ino); //get inode from ino

  if((mip->INODE.i_mode & 0100) == 0100) //check if file is dir
    ls_dir(ino);
  else
    ls_file(ino, filename);
  iput(mip);
}

char *pwd(MINODE *wd)
{
  if (wd == root){
    printf("/\n");
    return;
  }
  else
    rpwd(wd);

  printf("\n");
}
int rpwd(MINODE *wd)
{
  char buf[BLKSIZE], my_name[256], *cp;
  int my_ino, parent_ino;
  MINODE *pip;
  DIR *dp;

  if(wd == root) //stop when root is reached
    return 0;

  my_ino = search(wd, "."); //get current inode
  parent_ino = search(wd, ".."); //get parent inode
  pip = iget(dev, parent_ino);

  //repeat similar process as in ls_dir()
  get_block(dev, pip->INODE.i_block[0], buf); //get block buf
  dp = (DIR *)buf;
  cp = buf;

  while (cp < buf + BLKSIZE){ //iterates through dir until matching inode is found; name found too
    strncpy(my_name, dp->name, dp->name_len); 
    my_name[dp->name_len] = 0;
	
    if(dp->inode == my_ino) //correct inode found, breaks and rpwd is called
      break;

    cp += dp->rec_len;
    dp = (DIR *)cp;
  }

  rpwd(pip); //repeat until pip is root 
  printf("/%s", my_name);
  iput(pip); //put back inode

}



