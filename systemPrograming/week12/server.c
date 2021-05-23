#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>

#define PORT 7000
#define BSIZE 1024

void fatal(char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
    int serv_sock;
    int clnt_sock;
    int clnt_len;
    static struct sockaddr_in serv_adr;
    static struct sockaddr_in clnt_adr;

    // 서버 소켓 생성
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0)
        fatal("generate error");

    // serv_adr 구조체 초기화
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(PORT);

    // bind()
    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) < 0)
        fatal("bind error");

    //listen()
    if (listen(serv_sock, 5) < 0)
        fatal("listen error");

    clnt_len = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_len);
    if (clnt_sock < 0)
        fatal("accept error");

    char buf[BSIZE] = "\0";
    read(clnt_sock, buf, BSIZE);
    printf("%s\n", buf);               // 출력
    write(clnt_sock, buf, BSIZE); // 다시 씀

    close(clnt_sock);
    close(serv_sock);
}
