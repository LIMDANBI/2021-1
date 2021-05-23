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
#define BSIZE 256

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

    while (1)
    {

        clnt_len = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_len);
        if (clnt_sock < 0)
            fatal("accept error");

        char buf[BSIZE] = "";
        int cnt = 0, fd, nread;
        DIR *dir;
        struct dirent *dent;
        struct stat st;
        dir = opendir(".");
        while ((dent = readdir(dir)))
        {
            if (lstat(dent->d_name, &st) < 0)
                fatal("lstat error");
            if (!(S_ISDIR(st.st_mode)))
                cnt++;
        }
        sprintf(buf, "%d", cnt);
        write(clnt_sock, buf, BSIZE); // 총 파일 수 보냄

        rewinddir(dir);
        while ((dent = readdir(dir))) // 파일 목록을 클라이언트에게 보냄
        {

            if (lstat(dent->d_name, &st) < 0)
                fatal("lstat error");
            if (!(S_ISDIR(st.st_mode)))
            {
                sprintf(buf, "%s", dent->d_name);
                // printf("%s\n", buf);
                write(clnt_sock, buf, sizeof(buf));
            }
        }
        closedir(dir);

        read(clnt_sock, buf, BSIZE); // 요청 파일 이름을 읽어옴
        // printf("요청받은 파일 : <%s> \n", buf);
        if ((fd = open(buf, O_RDWR)) < 0) // 해당 파일을 O_RDONLY로 열어서
            fatal("file open error");

        while ((nread = read(fd, buf, BSIZE)) > 0) // 파일 내용을 클라이언트에 전송
            write(clnt_sock, buf, nread);

        close(clnt_sock);
    }
    close(serv_sock);
}
