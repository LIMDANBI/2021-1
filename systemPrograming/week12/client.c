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
#include <dirent.h>

#define PORT 7000 // IP 주소 172.30.1.4
#define BSIZE 256

void fatal(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int clnt_sock, cnt, fd, nread;
    static struct sockaddr_in serv_adr;
    struct hostent *host;

    host = gethostbyname(argv[1]);
    if (host == (struct hostent *)NULL)
        fatal("gethostbyname");

    // 소켓 생성
    clnt_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (clnt_sock < 0)
        fatal("generate error");

    // serv_adr 구조체 초기화
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    memcpy(&serv_adr.sin_addr, host->h_addr, host->h_length);
    serv_adr.sin_port = htons(PORT);

    // connect()
    if (connect(clnt_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) < 0)
        fatal("connect error");

    char buf[BSIZE]= "", filename[BSIZE]= "";
    read(clnt_sock, buf, BSIZE); // cnt 읽어옴
    cnt = atoi(buf);

    for (int i = 0; i < cnt; i++) // 파일 목록 출력
    {
        read(clnt_sock, buf, BSIZE);
        printf("%s\n", buf);
    }

    printf("\nPlease enter the filename: ");
    fgets(filename, BSIZE, stdin); // 원하는 파일명 입력
    filename[strlen(filename) - 1] = '\0';
    write(clnt_sock, filename, BSIZE); //해당 파일을 서버에 요청

    if ((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0) // 파일만들고
        fatal("file open error");
    while ((nread = read(clnt_sock, buf, BSIZE)) > 0) // 서버로부터 내용 받아와서 저장
        write(fd, buf, nread);
    close(fd);
    close(clnt_sock);
}
