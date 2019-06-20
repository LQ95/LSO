#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>

#define PORT 8080
#define SA struct sockaddr
#define BUFDIM 1400
#define clear_screen() printf("\033[H\033[J")
//internal client flags
#define END_CLIENT_ACTIVITY 805
#define CHECK_IF_SERVER_ISACTIVE 705
#define CLIENTSESSION_OK 900
#define CLIENTSESSION_END 1100


extern pthread_mutex_t sem;
extern pthread_cond_t c;

struct data{
    char name[10];
    int dimension;
    int time;
    int connfd;
    int **board;
    int seed;
    int *positions;
    int *status;
};

struct player{
    char name[10];
    int position[2];
    int score;
    int ID;
    struct player *next;
};

//clientgame stuff
void *client_game_send(void *arg);
void *client_game_recv(void *arg);
int **create_board(struct player *in,struct player *deaths,int dim);
