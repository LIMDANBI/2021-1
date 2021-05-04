#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MSGSIZ 256

char *fifos = "fifos";
char *fifoc = "fifoc";

void fatal(const char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
    int rfd, wfd;
    char msgbuf[MSGSIZ];

    if (mkfifo(fifos, 0666) < 0) // fifo 파일 생성 (server가 client에 msg 보내는데 사용)
    {
        if (errno != EEXIST) // 이미 fifo 파일이 있는 경우를 제외하고 예외 처리
            fatal("server: mkfifo");
    }

    if (mkfifo(fifoc, 0666) < 0) // fifo 파일 생성 (server가 client에 msg 보내는데 사용)
    {
        if (errno != EEXIST) // 이미 fifo 파일이 있는 경우를 제외하고 예외 처리
            fatal("server: mkfifo");
    }

    if ((wfd = open(fifos, O_RDWR)) < 0) // server가 client에 msg 보내는데 사용
        fatal("fifo open failed");
    if ((rfd = open(fifoc, O_RDWR)) < 0) // server가 client로부터 msg 받아오는데 사용
        fatal("fifo open failed");

    printf("* 서버 시작 \n");
    while (1)
    {
        printf("[서버] : ");
        fgets(msgbuf, MSGSIZ, stdin);
        if (write(wfd, msgbuf, MSGSIZ) < 0) // client에 msg 보냄
            fatal("message write failed");
        printf("[클라이언트] ->  ");
        if (read(rfd, msgbuf, MSGSIZ) < 0) // client로부터 msg 받아옴
            fatal("message read failed");
        else
            printf("%s\n", msgbuf);
    }
    exit(0);
}