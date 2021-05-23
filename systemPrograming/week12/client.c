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
#define BSIZE 1024

void fatal(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int clnt_sock;
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

    char filename[BSIZE] = "\0";
    printf("Please enter the filename: ");
    fgets(filename, BSIZE, stdin); // 원하는 파일명 입력
    filename[strlen(filename) - 1] = '\0';

    write(clnt_sock, filename, BSIZE); // 쓰고 
    read(clnt_sock, filename, BSIZE); // 읽어와서
    printf("%s\n", filename); // 출력 

    close(clnt_sock);

}
