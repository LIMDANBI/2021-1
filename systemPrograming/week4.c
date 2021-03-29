#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>

void creatEnv();

/**
 * 파일을 복사하여 사본을 생성한다. 
 * @param buf   경로명을 저장할 버퍼 변수.
 * @param size  버퍼 변수의 크기
 * @return 버퍼 변수의 시작 주소, 즉 buf
 */
char *mygetcwd(char *buf, size_t size){

  DIR *current; // current directory
  DIR *parent;  // parent directory
  struct dirent *c_dp;
  struct dirent *p_dp;
  long c_ino;
  char path[255] = "\0";

  while (1){
    char *tmp;
    char tmp2[255] = "/";
    current = opendir(".");
    parent = opendir("..");
    c_dp = readdir(current);
    p_dp = readdir(parent);

    if (c_dp->d_ino == p_dp->d_ino) break;
    while (c_dp != NULL){
      c_ino = c_dp->d_ino;
      if (!strcmp(c_dp->d_name, ".")) break;
      c_dp = readdir(current);
    }
    while (p_dp != NULL){
      if (p_dp->d_ino == c_ino){
        tmp = p_dp->d_name;
        break;
      }
      p_dp = readdir(parent);
    }
    strcat(tmp2, tmp);
    strcat(tmp2, path);
    strcpy(path, tmp2);
    strcpy(buf, path);
    chdir("..");
  }

  closedir(current);
  closedir(parent);

  return buf;
}

int main(void)
{
  pid_t pid;
  int status;
  char buf[255];

  creatEnv();
  chdir("dir/sub");

  printf("original func: %s\n", getcwd(NULL, 0));
  printf("mygetcwd func: %s\n", mygetcwd(buf, 255));

  return 0;
}

void creatEnv()
{
  mkdir("dir", 0755);
  mkdir("dir/sub", 0755);
  mkdir("dir/sub2", 0);

  creat("dir/a", 0755);
  creat("dir/b", 0755);
  creat("dir/sub/x", 0755);
  symlink("dir/a", "dir/sl");
  symlink("dir/x", "dir/dsl");
}