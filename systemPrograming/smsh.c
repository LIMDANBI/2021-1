#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 1024
char rootPath[1024];
char currentPath[1024];
char historyPath[1024];
char prompt[1024] = "~";
int backgorund = 0;   // 몇번째인지
int isbackgorund = 0; // background 실행인지

void fatal(const char *str);                                     // 오류 처리
void saveHistory(char *cmdline);                                 // history 저장
void printHistory();                                             // history 출력
void changeDirectory(char **arg);                                // cd
int parsing(char *cmdline, const char *delimiters, char **args); // 파싱
int redirectIn(char **args);                                     // redirecting Input
int redirectOut(char **args);                                    // redirecting Ouput
int redirectOutAppend(char **args);                              // redirecting Ouput append
void pipefunc(char **args, int pipeNum);                         // pipe

int main()
{
    char cmdline[MAX_LINE];

    getcwd(rootPath, 1024);
    getcwd(currentPath, 1024);
    char tmp[1024];
    getcwd(tmp, 1024);
    strcat(historyPath, tmp);
    strcat(historyPath, "/history.txt");

    while (1)
    {
        printf("\n%s%s", prompt, "$ "); // 프롬프트 출력
        fflush(stdout);

        char *cmd[512];
        fgets(cmdline, MAX_LINE, stdin); // 사용자 입력 받아오기
        cmdline[strlen(cmdline) - 1] = '\0';
        int spaceremove = strlen(cmdline) - 1;
        while (cmdline[spaceremove] == ' ') // 뒤 공백 제거
        {
            cmdline[spaceremove] = '\0';
            spaceremove--;
        }
        saveHistory(cmdline); //history 저장

        int pcnt = 0; // 괄호 match 확인
        for (int i = 0; i < strlen(cmdline); i++)
        {
            if (cmdline[i] == '(')
                pcnt++;
            else if (cmdline[i] == ')')
                pcnt--;
        }
        if (pcnt != 0)
        {
            printf("syntex error : parentheses");
            continue; // 다시 명령어 입력 받음
        }

        // 맨뒤 백그라운드 확인 -> 있는 경우 null로 변경 -> 괄호 적용 여부
        int prange = 1; // background 실행 적용 범위 체크
        if (cmdline[strlen(cmdline) - 1] == '&')
        {
            isbackgorund = 1;
            int pmatch = 1;
            int i = strlen(cmdline) - 2;
            while (cmdline[i] == ' ') // 공백인 경우
                i--;
            if (cmdline[i] == ')') // 괄호를 통해 백그라운드 적용 되는 경우 -> 적용 범위 체크
            {
                i--;
                while (pmatch != 0 && i >= 0)
                {
                    if (cmdline[i] == '(')
                        pmatch--;
                    else if (cmdline[i] == ')')
                        pmatch++;
                    else if (cmdline[i] == ';')
                        prange++;
                    i--;
                }
            }
            cmdline[strlen(cmdline) - 1] = '\0'; // background 정보 저장 후 지움
        }

        // printf("%d\n",prange); // 잘 출력되나 확인
        // printf("%s\n",cmdline); //잘 지워졌나 확인

        // 괄호->공백 (뒤에서 어짜피 공백 기준으로 파싱함 !)
        for (int i = 0; i < strlen(cmdline); i++)
        {
            if (cmdline[i] == '(' || cmdline[i] == ')')
                cmdline[i] = ' ';
        }
        // printf("%s\n", cmdline); //잘 지워졌나 확인

        int cmdNum = parsing(cmdline, ";", cmd);  // ;을 기준으로 분할
        for (int turn = 0; turn < cmdNum; turn++) // 명령어 순차 실행
        {
            char *args[512];
            int cnt = parsing(cmd[turn], " ", args); // cmdline 파싱
            // printf("args .%s.\n", args[0]); // 파싱 확인

            args[cnt] = NULL;               // 배열의 마지막 인자는 null
            if (strstr(args[0], "history")) // shell 내장 명령(history)
                printHistory();
            else if (strstr(args[0], "cd")) // shell 내장 명령(cd)
                changeDirectory(args);
            else // shell 외부 명령
            {
                pid_t pid;
                if ((pid = fork()) < 0)
                    fatal("fork error");
                else if (pid == 0) //child
                {
                    int pipeNum = 0;
                    for (int i = 0; args[i] != NULL; i++) // pipe 검사
                    {
                        if (!strcmp(args[i], "|"))
                            pipeNum++;
                    }
                    if (pipeNum > 0)
                        pipefunc(args, pipeNum); // 실행 => 함수 내부에서 redirection 검사함
                    else
                    {
                        for (int i = 0; args[i] != NULL; i++) // redirection 검사 (<, >, >> 3개 지원)
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
                            else if (!strcmp(args[i], ">>"))
                            {
                                redirectOutAppend(args);
                                if (execvp(args[0], args) == -1)
                                    fatal("execvp error");
                                break;
                            }
                            else
                            {
                                if (execvp(args[0], args) == -1) // redirection 과 pipe 없는 경우
                                    fatal("execvp error");
                            }
                        }
                    }
                }
                else //parent
                {
                    if (isbackgorund) // ** 수정 필요
                    {
                        printf("prange %d turn %d cmdNum %d\n", prange, turn, cmdNum); // 잘 출력되나 확인

                        if (cmdNum - prange == turn) // 첫 백그라운드 실행
                        {
                            backgorund++;
                            printf("\n[%d] %d\n", backgorund, getpid());
                            continue;
                        }
                        else if (cmdNum - prange < turn)
                        {
                            if (turn == cmdNum - 1)
                            {
                                backgorund = 0;
                                backgorund--;
                            }
                            continue;
                        }
                    }
                    wait(NULL);
                }
            }
        }
    }
}

void fatal(const char *str) // error 처리
{
    perror(str);
    exit(-1);
}

void saveHistory(char *cmdline) // history 저장
{
    FILE *fp = fopen(historyPath, "a+");
    fputs(cmdline, fp);
    fputs("\n", fp);
    fclose(fp);
}

void printHistory() // history 출력
{
    FILE *fp = fopen(historyPath, "r");
    if (fp != NULL)
    {
        char tmp[1024];
        int i = 0;
        while (fgets(tmp, 1024, fp) != NULL)
            printf("%d %s", ++i, tmp);
        fclose(fp);
    }
}

void changeDirectory(char **args)
{
    char tmp[1024] = "";
    char *arg;
    for (int i = 0; args[i] != NULL; i++) // 이동하고 싶은 디렉토리 찾기
    {
        if (!strcmp(args[i], "cd"))
        {
            arg = args[i + 1];
            if (arg == NULL || !strcmp(arg, "~"))
            {
                chdir(rootPath);
                strcpy(currentPath, rootPath);
                strcpy(prompt, "~");
                return;
            }
            else
                break;
        }
    }

    if (!strcmp(arg, "..")) // 상위 directory로 이동
    {
        if (strcmp(rootPath, currentPath)) // 이동 가능한 경우
        {
            int n = 0;
            for (int i = strlen(currentPath) - 1; i > 0; i--)
            {
                if (currentPath[i] == '/')
                {
                    n = i;
                    break;
                }
            }
            strncpy(tmp, currentPath, n);
            chdir(tmp);
            strcpy(currentPath, tmp);

            char tmp2[1024] = "";
            for (int i = strlen(prompt) - 1; i > 0; i--)
            {
                if (prompt[i] == '/')
                {
                    n = i;
                    break;
                }
            }
            strncpy(tmp2, prompt, n);
            strcpy(prompt, tmp2);
        }
    }
    else if (!strcmp(arg, ".")) // 현재 directory 그대로
        ;
    else // 하위 directory로 이동
    {
        strcpy(tmp, currentPath);
        strcat(tmp, "/");
        strcat(tmp, arg);
        if (chdir(tmp) == -1)
            printf(" No such file or directory \n");
        else
        {
            strcpy(currentPath, tmp);
            strcat(prompt, "/");
            strcat(prompt, arg);
        }
    }
}

int parsing(char *cmdline, const char *delimiters, char **args)
{
    char *tmp;
    int i = 0, cnt = 0;

    if (cmdline == NULL || delimiters == NULL)
        fatal("parsing error");
    tmp = strtok(cmdline, delimiters);
    while (tmp != NULL)
    {
        args[i++] = tmp;
        cnt++;
        tmp = strtok(NULL, delimiters);
    }
    return cnt;
}

int redirectIn(char **args) //redirecting Input <
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

int redirectOut(char **args) //redirecting Ouput >
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

int redirectOutAppend(char **args) //redirecting Ouput append >>
{
    int i, fd;
    for (i = 0; args[i] != NULL; i++)
    {
        if (!strcmp(args[i], ">>"))
            break;
    }
    if (args[i])
    {
        if (!args[i + 1])
            fatal("redirectOut error");
        else
        {
            if ((fd = open(args[i + 1], O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
                fatal("file open error");
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            args[i + 1] = NULL;
        }
    }
    return 0;
}

void pipefunc(char **args, int pipeNum) // pipe | (크게 3 part로 분리 : 첫번째 파이프전 - 마지막 파이프 전까지 - 마지막 파이프 뒤)
{
    int pipes[pipeNum][2];
    pid_t pid;
    int idx = 0, k = 0;
    char *arg1[512], *filename;

    for (idx = 0; strcmp(args[idx], "|"); idx++)
        arg1[idx] = args[idx];
    arg1[idx] = NULL;
    idx++;

    if (pipe(pipes[0]) < 0) // pipe 생성
        fatal("pipe error");

    // 1. 첫번째 파이프 전 명령어
    if ((pid = fork()) < 0)
        fatal("fork error");
    else if (pid == 0)
    {
        close(STDOUT_FILENO);
        dup2(pipes[0][1], STDOUT_FILENO);
        if (execvp(*arg1, arg1) < 0)
            fatal("execvp error");
    }
    close(pipes[0][1]);
    wait(NULL);

    // 2. 마지막 pipe 전까지
    for (int i = 0; i < pipeNum - 1; i++)
    {
        if (pipe(pipes[i + 1]) < 0) // pipe 생성
            fatal("pipe error");
        bzero(arg1, sizeof(arg1));
        for (k = 0; strcmp(args[idx], "|"); k++)
        {
            arg1[k] = args[idx];
            idx++;
        }
        arg1[k] = NULL;
        idx++;

        if ((pid = fork()) < 0)
        {
            fprintf(stderr, "fork() error\n");
            exit(1);
        }
        else if (pid == 0)
        {
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            dup2(pipes[i][0], STDIN_FILENO);
            dup2(pipes[i + 1][1], STDOUT_FILENO);
            if (execvp(*arg1, arg1) < 0)
                fatal("execvp error");
        }
        close(pipes[i + 1][1]);
        wait(NULL);
    }

    // 3. 마지막 파이프 뒤 명령어
    int isredirectout = 0, isredirectoutappend = 0; // redirection (>, >> ) 있는지
    bzero(arg1, sizeof(arg1));
    for (k = 0; args[idx] != NULL; k++)
    {
        if (!strcmp(args[idx], ">")) // > 명령어가 있는 경우
        {
            isredirectout = 1;
            filename = args[idx + 1];
            if (filename == NULL)
            {
                printf("Enter filename\n");
                return;
            }
            break;
        }
        else if (!strcmp(args[idx], ">>")) // >> 명령어가 있는 경우
        {
            isredirectoutappend = 1;
            filename = args[idx + 1];
            if (filename == NULL)
            {
                printf("Enter filename\n");
                return;
            }
            break;
        }
        else
        {
            arg1[k] = args[idx];
            idx++;
        }
    }
    arg1[k] = NULL;

    // 3. 마지막 pipe 뒤
    if ((pid = fork()) < 0)
        fatal("fork error");
    else if (pid == 0)
    {
        int fd;
        close(STDIN_FILENO);
        dup2(pipes[pipeNum - 1][0], STDIN_FILENO);
        if (isredirectout)
        {
            if ((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1)
                fatal("file open error");
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        else if (isredirectoutappend)
        {
            if ((fd = open(filename, O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
                fatal("file open error");
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (execvp(*arg1, arg1) < 0)
            fatal("execvp error");
    }
    wait(NULL);
}