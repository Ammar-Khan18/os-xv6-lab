#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"  // for MAXARG

char* fmtname(char *path) {
  static char buf[DIRSIZ+1];
  char *p;

  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void runexec(char *filename, int argc, char *argv[]) {
  char *nargv[MAXARG];
  int i;

  for(i = 0; i < argc; i++){
    nargv[i] = argv[i];
  }
  nargv[argc] = filename;   // add the found file at the end
  nargv[argc+1] = 0;        // null terminate

  if(fork() == 0){
    exec(nargv[0], nargv);
    fprintf(2, "exec %s failed\n", nargv[0]);
    exit(1);
  } else {
    wait(0); // parent waits for child
  }
}

void find(char *path, char *filename, int execflag, int argc, char *argv[]) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(strcmp(fmtname(path), filename) == 0) {
      if(execflag)
        runexec(path, argc, argv);
      else
        printf("%s\n", path);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, filename, execflag, argc, argv);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if(argc < 3){
    fprintf(2, "usage: find <path> <filename> [-exec cmd args...]\n");
    exit(1);
  }

  int execflag = 0;
  int cmdstart = 0;

  // look for -exec
  for(int i = 3; i < argc; i++){
    if(strcmp(argv[i], "-exec") == 0){
      execflag = 1;
      cmdstart = i + 1;
      break;
    }
  }

  if(execflag && cmdstart >= argc){
    fprintf(2, "find: -exec requires a command\n");
    exit(1);
  }

  if(execflag)
    find(argv[1], argv[2], 1, argc - cmdstart, argv + cmdstart);
  else
    find(argv[1], argv[2], 0, 0, 0);

  exit(0);
}
