#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

void fatal(const char *str);                                     // error 처리
int parsing(char *cmdline, const char *delimiters, char **args); //cmdline 파싱
int redirectIn(char **args);                                     //redirecting Input
int redirectOut(char **args);                                    //redirecting Ouput
int pipefunc(char **args);                                       // pipe

int isbackground = 0;

int main(void)
{
    char cmdline[MAX_LINE];       // 명령어 파싱 전 문자열
    char *args[MAX_LINE / 2 + 1]; /* command line arguments */
    int should_run = 1;           /* flag to determine when to exit program */

    while (should_run)
    {
        printf("osh> ");
        fflush(stdout);

        fgets(cmdline, MAX_LINE, stdin); // 사용자 입력 받아오기
        cmdline[strlen(cmdline) - 1] = '\0';
        int cnt = parsing(cmdline, " ", args); // cmdline 파싱
        args[cnt] = NULL;                      // 배열의 마지막 인자는 null

        if (!strcmp(args[cnt - 1], "&"))
        {
            isbackground = 1;
            args[cnt - 1] = NULL;
        }

        if (!strcmp("exit", args[0])) // exit 입력시 쉘 종료
            should_run = 0;
        else // 명령어 실행
        {
            pid_t pid;
            pid = fork();

            if (pid < 0) // fork error
                fatal("fork error");
            else if (pid == 0) //child process
            {
                for (int i = 0; args[i] != NULL; i++) // redirection 검사
                {
                    if (!strcmp(args[i], "<"))
                    {
                        redirectIn(args);
                        if (execvp(args[0], args) == -1)
                            fatal("execvp error");
                        break;
                    }
                    else if (!strcmp(args[i], ">"))
                    {
                        redirectOut(args);
                        if (execvp(args[0], args) == -1)
                            fatal("execvp error");
                        break;
                    }
                }

                for (int i = 0; args[i] != NULL; i++) // pipe 검사
                {
                    if (!strcmp(args[i], "|"))
                    {
                        pipefunc(args); // 실행
                        break;
                    }
                }

                if (execvp(args[0], args) == -1)
                    fatal("execvp error");
            }
            else //parent process
            {
                if (isbackground)
                {
                    printf("background_pid : %d \n", getpid());
                    printf("----------------\n");
                    isbackground = 0;
                    continue;
                }
                wait(NULL);
                continue;
            }
        }
    }
    return 0;
}

void fatal(const char *str) // error 처리
{
    perror(str);
    exit(-1);
}

int parsing(char *cmdline, const char *delimiters, char **args) // cmdline 파싱 함수
{

    char *result;
    int i = 0, cnt = 0;

    if (cmdline == NULL || delimiters == NULL)
        fatal("parsing error");
    result = strtok(cmdline, delimiters);
    while (result != NULL)
    {
        args[i++] = result;
        cnt++;
        result = strtok(NULL, delimiters);
    }
    return cnt;
}

int redirectIn(char **args) //redirecting Input
{
    int i, fd;
    for (i = 0; args[i] != NULL; i++)
    {
        if (!strcmp(args[i], "<"))
            break;
    }
    if (args[i])
    {
        if (!args[i + 1])
            fatal("redirectIn error");
        else
        {
            if ((fd = open(args[i + 1], O_RDONLY)) == -1)
                fatal("file open error");
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
            args[i + 1] = NULL;
        }
    }
    return 0;
}

int redirectOut(char **args) //redirecting Ouput
{
    int i, fd;
    for (i = 0; args[i] != NULL; i++)
    {
        if (!strcmp(args[i], ">"))
            break;
    }
    if (args[i])
    {
        if (!args[i + 1])
            fatal("redirectOut error");
        else
        {
            if ((fd = open(args[i + 1], O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1)
                fatal("file open error");
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            args[i + 1] = NULL;
        }
    }
    return 0;
}

int pipefunc(char **args) // pipe
{

    int i, j, existpipe = 0, fd[2]; //fd[0]-readEnd , fd[1]-rwriteEnd
    pid_t pid;
    char *args1[MAX_LINE / 2 + 1], *args2[MAX_LINE / 2 + 1];

    //pipe 명령어를 기점으로 두 배열로 나눠 담음
    for (i = 0; args[i] != NULL; i++)
    {
        if (!strcmp(args[i], "|"))
        {
            existpipe = 1;
            break;
        }
        args1[i] = args[i];
    }

    if (existpipe) // 파이프 명령어가 있는 경우
    {
        args1[i] = NULL;
        for (j = 0; args[j + i + 1] != NULL; j++)
            args2[j] = args[j + i + 1];
        args2[j + i + 1] = NULL;

        if (pipe(fd) == -1)
            fatal("pipe error");

        pid = fork();
        if (pid < 0) // fork error
            fatal("fork error");
        else if (pid == 0)
        {
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            if (execvp(args1[0], args1) == -1)
                fatal("execvp error");
        }
        else
        {
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
            if (execvp(args2[0], args2) == -1)
                fatal("execvp error");
        }
    }
    else //pipe 명령어가 없는 경우
        return 0;
    return 1;
}