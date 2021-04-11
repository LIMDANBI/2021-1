/* 
   A simple server in the internet domain using TCP
   Usage:./server port (E.g. ./server 10000 )
*/
#include <stdio.h>
#include <sys/types.h>  // socket.h, netinet/in.h 에 사용되는 여러 데이터 타입 정의
#include <sys/socket.h> // sockets에 필요한 structures 정의, e.g. sockaddr
#include <netinet/in.h> // internet domain addresses에 필요한 constants 와 structures , e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd; // socket descriptor
    int portno;            // port number
    socklen_t clilen;
    char buffer[1024];
    // sockaddr_in: Structure Containing an Internet Address
    // sin_family-AF_INET(필수), sin_port-포트번호(2 bytes), sin_addr-호스트 IP주소(4 bytes), sin_zero-8 bytes dummy data (반드시 모두 0)
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) // 올바른 형식으로 파일을 실행했는지 확인
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    /*Create a new socket
       AF_INET: Address Domain is Internet 
       SOCK_STREAM: Socket Type is STREAM Socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) // socket 생성 실패
        error("open error");

    memset(&serv_addr, 0, sizeof(serv_addr)); //serv_addr을 sizeof(serv_addr) 바이트만큼 0 으로 채움
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 서버의 IP 주소를 자동으로 찾아 대입
    serv_addr.sin_port = htons(atoi(argv[1]));     //convert from host to network byte order

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // 소켓에 IP주소와 포트번호를 할당 (통신에 사용할 수 있도록 준비)
        error("binding error");

    if (listen(sockfd, 5) < 0) // clinet 요청 가능 상태로 변경, 대기열 크키 = 5
        error("listening error");

    while (1)
    {
        int n, m;                                                          // newsockfd 에서 읽어온 바이트 수 , 요청한 파일에서 읽어온 바이트 수 저장
        clilen = sizeof(cli_addr);                                         //clien_addr 구조체의 크기
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen); // socket과 클라이언트를 연결하는 함수 (sockfd, 클라이언트 주소 정보를 담고 있는 구조체, 2번째 인자 길이)

        if (newsockfd < 0)
            error("accept error");

        bzero(buffer, 1024);
        n = read(newsockfd, buffer, 1024);
        if (n < 0)
            error("reading error from socket");
        printf("%s\n", buffer); // request message 출력

        char requested[1024] = {};
        char file[4096];
        int tmp = 5; // file name 시작 index

        while (buffer[tmp] != ' ')
            tmp++;
        strncpy(requested, &buffer[5], tmp - 5); // request message에서 파일 이름 추출

        char *rmsg; // response message
        int fd = open(requested, O_RDONLY);
        if (fd < 0) // 요청한 파일 X -> 404 error
        {
            if (write(newsockfd, "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n", 52) < 0) // header
                error("wirte error");
            else
            {
                if (write(newsockfd, "<HTML><BODY><H1><pre></pre><pre></pre>404 NOT FOUND</H1></BODY></HTML>\n", 72) < 0) // body
                    error("wirte error");
            }
        }
        else // 요청한 파일 O
        {
            char *rmsg;
            if (strstr(requested, ".html")) //html
            {
                rmsg = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=UTF-8\r\n\r\n";
                if (write(newsockfd, rmsg, strlen(rmsg)) < 0)
                    error("ERROR writing html");
            }
            else if (strstr(requested, ".gif")) // gif
            {
                rmsg = "HTTP/1.1 200 OK\r\nContent-Type: image/gif;\r\n\r\n";
                if (write(newsockfd, rmsg, strlen(rmsg)) < 0)
                    error("ERROR writing gif");
            }
            else if (strstr(requested, ".jpg") || strstr(requested, ".jpeg")) // jpg or jpeg
            {
                rmsg = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg;\r\n\r\n";
                if (write(newsockfd, rmsg, strlen(rmsg)) < 0)
                    error("ERROR writing jpeg(jpg)");
            }
            else if (strstr(requested, ".mp3")) // mp3
            {
                rmsg = "HTTP/1.1 200 OK\r\nContent-Type: audio/mp3;\r\n\r\n";
                if (write(newsockfd, rmsg, strlen(rmsg)) < 0)
                    error("ERROR writing mp3");
            }
            else if (strstr(requested, ".pdf")) // pdf
            {
                rmsg = "HTTP/1.1 200 OK\r\nContent-Type: application/pdf;\r\n\r\n";
                if (write(newsockfd, rmsg, strlen(rmsg)) < 0)
                    error("ERROR writing pdf");
            }
            else // 위 유형을 제외한 파일 -> 404 error
            {
                if (write(newsockfd, "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n", 52) < 0) // header
                    error("wirte error");
                else
                {
                    if (write(newsockfd, "<HTML><BODY><H1><pre></pre><pre></pre>404 NOT FOUND</H1></BODY></HTML>\n", 72) < 0) // body
                        error("wirte error");
                }
            }

            while (1)
            {
                m = read(fd, file, 4095);
                if (m == 0)
                    break;
                else if (m < 0)
                    error("read error : file ");
                else
                {
                    if (write(newsockfd, file, m) < 0)
                        error("wirte error : file");
                }
            }
            close(fd);
        }
        close(newsockfd); // close client socket
    }
    close(sockfd); // close server socket

    return 0;
}