#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#define MAXARGS 31
#define MAXFILENAME 1024
#define MAXPATHSTR 2048

extern char **environ;
/**
 * 첫 번째 인자인 file 프로그램을 뒤에 따르는 인자들을 이용해 실행한다.
 * @param file	실행할 프로그램의 이름
 * @param args	stdarg(3)을 통해 처리할 인자들
 * @return 실패했을 경우 -1, 성공 시 반환 값 없음
 */
int myexeclp(const char *file, const char *args, ...){
  
  // 환경변수로부터 path 받아와서 parsing -> 저장
  char *envpath = getenv("PATH");
  char *parsed[MAXARGS];
  int cnt = 0;

  char *tmp = strtok(envpath, ":");
  while (tmp != NULL){
    char path[MAXPATHSTR];
    sprintf(path, "%s/%s", tmp, file);
    parsed[cnt] = (char *)malloc(sizeof(path));
    strcpy(parsed[cnt], path);
    tmp = strtok(NULL, ":");
    cnt++;
  }

  // argv에 가변변수 저장
  char *argv[MAXARGS];
  va_list ap;

  va_start(ap, args);
  argv[0] = (char *)args;
  for(int i=1; argv[i-1] != NULL; i++){
    argv[i] = va_arg(ap, char *);
  }
  va_end(ap);

  // 환경변수에서 받아온 path 뒤에서부터 실행
  while (0 <= cnt){
    if (execve(parsed[cnt-1], argv, environ) < 0){;}
    cnt--;
  }

	return -1;
}

int main(void) {
  char path[MAXPATHSTR];
  sprintf(path, "PATH=%s:%s", getcwd(NULL, 0), getenv("PATH"));
  putenv(path);
  system("gcc -o hello hello.c"); // prepare the executable file named "hello"
  // execlp("hello", "hello", "-a", "-b", "-c", (char *) 0);
  myexeclp("hello", "hello", "-a", "-b", "-c", (char *) 0);
  return 0;
}