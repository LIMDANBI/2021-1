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
    int rfd, wfd, nread;
    char msgbuf[MSGSIZ];

    if (mkfifo(fifos, 0666) < 0 && errno != EEXIST) // fifo 파일 생성 (server가 client에 msg 보내는데 사용)
        fatal("server: mkfifo");

    if (mkfifo(fifoc, 0666) < 0 && errno != EEXIST) // fifo 파일 생성 (client가 server가 msg 보내는데 사용)
        fatal("client: mkfifo");

    if ((wfd = open(fifos, O_RDWR)) < 0) // server가 client에 msg 보내는데 사용
        fatal("fifos open failed");
    if ((rfd = open(fifoc, O_RDWR)) < 0) // server가 client로부터 msg 받아오는데 사용
        fatal("fifoc open failed");

    printf("* 서버 시작 \n");
    while (1)
    {
        printf("[서버] : ");
        fgets(msgbuf, MSGSIZ, stdin);       // client로 보낼 메세지 입력받음
        if (write(wfd, msgbuf, MSGSIZ) < 0) // client에 msg 보냄
            fatal("message write failed");

        printf("[클라이언트] ->  ");
        if (read(rfd, msgbuf, MSGSIZ) < 0) // client로부터 msg 받아옴
            fatal("message read failed");
        printf("%s\n", msgbuf);
    }
    exit(0);
}