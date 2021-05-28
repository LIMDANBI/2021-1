/*
 * Copyright 2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <semaphore.h>

/*
 * 흥연자 문제에서 사용하는 네 가지 세마포
 */
sem_t *tabacco, *paper, *matches, *done;

/*
 * 말린 담배잎을 잘게 썰어 놓은 일명 봉초만 가지고 있는 흡연자
 */
void *tabacco_smoker(void *arg)
{
    while (1)
    {
        /*
         * 종이와 성냥을 얻는다.
         */
        sem_wait(paper);
        sem_wait(matches);
        /*
         * 담배를 피운다.
         */
        printf("tabacco       )\n"
               "             (\n"
               " _ ___________ )\n"
               "[_[___________#\n");
        /*
         * 다 피웠음을 에이전트에게 알린다.
         */
        sem_post(done);
    }
}

/*
 * 봉초를 말 수 있는 종이만 가지고 있는 흡연자
 */
void *paper_smoker(void *arg)
{
    while (1)
    {
        /*
         * 봉초와 성냥을 얻는다.
         */
        sem_wait(tabacco);
        sem_wait(matches);
        /*
         * 담배를 피운다.
         */
        printf("paper         )\n"
               "             (\n"
               " _ ___________ )\n"
               "[_[___________#\n");
        /*
         * 다 피웠음을 에이전트에게 알린다.
         */
        sem_post(done);
    }
}

/*
 * 성냥만 가지고 있는 흡연자
 */
void *matches_smoker(void *arg)
{
    while (1)
    {
        /*
         * 봉초와 종이를 얻는다.
         */
        sem_wait(tabacco);
        sem_wait(paper);
        /*
         * 담배를 피운다.
         */
        printf("matches       )\n"
               "             (\n"
               " _ ___________ )\n"
               "[_[___________#\n");
        /*
         * 다 피웠음을 에이전트에게 알린다.
         */
        sem_post(done);
    }
}

/*
 * 봉초, 종이, 성냥을 무한히 생산할 수 있는 에이전트
 * 임의로 두 가지만 생산해서 나머지를 가지고 있는 흡연자가 담배를 만들어 피울 수 있게 한다.
 * 이 과정을 무한 반복한다.
 */
void *agent(void *arg)
{
    int turn;

    srand(202106);
    while (1)
    {
        /*
         * 임의로 두 가지만 생산해서 나머지를 가진 흡연자가 피울 수 있게 한다.
         */
        turn = rand() % 3;
        switch (turn)
        {
        case 0:
            sem_post(paper);
            sem_post(matches);
            break;
        case 1:
            sem_post(tabacco);
            sem_post(matches);
            break;
        case 2:
            sem_post(tabacco);
            sem_post(paper);
            break;
        default:;
        }
        sem_wait(done);
        /*
         * 흡연자가 담배를 다 피울 때까지 기다린다.
         */
    }
}

/*
 * 메인 함수는 세마포를 초기화하고 세 개의 흡연자 스레드와 에이전트 스레드를 생성한다.
 * 생성된 스레드가 일을 할 동안 10초 동안 기다렸다가 모든 스레드를 철회하고 종료한다.
 */
int main(void)
{
    pthread_t tabacco_id, paper_id, matches_id, agent_id;

    /*
     * 세마포를 초기화 한다. 다만 오류 검사는 생략한다.
     */
    tabacco = sem_open("tabacco", O_CREAT, 0600, 0);
    paper = sem_open("paper", O_CREAT, 0600, 0);
    matches = sem_open("matches", O_CREAT, 0600, 0);
    done = sem_open("done", O_CREAT, 0600, 0);
    /*
     * 스레드를 생성한다. 다만 오류 검사는 생략한다.
     */
    pthread_create(&tabacco_id, NULL, tabacco_smoker, NULL);
    pthread_create(&paper_id, NULL, paper_smoker, NULL);
    pthread_create(&matches_id, NULL, matches_smoker, NULL);
    pthread_create(&agent_id, NULL, agent, NULL);
    /*
     * 스레드가 실행할 동안 10초 기다린다.
     */
    sleep(10);
    /*
     * 스레드를 모두 철회한다.
     */
    pthread_cancel(tabacco_id);
    pthread_cancel(paper_id);
    pthread_cancel(matches_id);
    pthread_cancel(agent_id);
    /*
     * 세마포를 모두 지우고 정리한다.
     */
    sem_close(tabacco);
    sem_unlink("tabacco");
    sem_close(paper);
    sem_unlink("paper");
    sem_close(matches);
    sem_unlink("matches");
    sem_close(done);
    sem_unlink("done");
    /*
     * 스레드가 조인될 때까지 기다린다.
     */
    pthread_join(tabacco_id, NULL);
    pthread_join(paper_id, NULL);
    pthread_join(matches_id, NULL);
    pthread_join(agent_id, NULL);
    return 0;
}
