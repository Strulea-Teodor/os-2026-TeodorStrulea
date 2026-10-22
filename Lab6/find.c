#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// Return a pointer to the last component of path.
char *
basename(char *path)
{
  char *p;
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

// show_inode != 0 : print the inode number next to each match (stretch goal)
void
find(char *path, char *name, int show_inode)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    if(strcmp(basename(path), name) == 0){
      if(show_inode)
        printf("%s %d\n", path, st.ino);
      else
        printf("%s\n", path);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      fprintf(2, "find: path too long\n");
      break;
    }
    // a directory itself can match too (find a b prints a/b only,
    // because we test every entry before descending into it)
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", buf);
        continue;
      }
      if(strcmp(de.name, name) == 0){
        if(show_inode)
          printf("%s %d\n", buf, st.ino);
        else
          printf("%s\n", buf);
      }
      if(st.type == T_DIR)
        find(buf, name, show_inode);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3 && argc != 4){
    fprintf(2, "usage: find <dir> <name> [-n]\n");
    exit(1);
  }
  int show_inode = (argc == 4 && strcmp(argv[3], "-n") == 0);
  find(argv[1], argv[2], show_inode);
  exit(0);
}
