#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <ncurses.h>
#include <limits.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>
#include <limits.h>
#define BUFDIM 1400
//Server activity flags,used for the server's internal function
#define SERVER_GAME_ISACTIVE 9000
#define SERVER_GAME_END 9200
#define SERVER_ISACTIVE 9300
#define SERVER_END 9400
#define SESSION_START 2222
#define SESSION_END 3333
extern pthread_mutex_t sem;
extern pthread_cond_t c;
//the socket descriptor in theis structure is the socket descriptor the server makes when the connection with the player is accepted
struct P{
    char name[10];
	int position[2];
	int score;
	int socket_desc;
	struct P *next;
};

struct player_list{
    struct P *first;
};

void server_log(char *data);

struct thread_data{
	struct player_list *L;
	struct player_list *Dead;
    int connfd;
	int *GameTime;
	int *GameStatus;
	int *session_status;
	int dim;
	char name[10];
    int seed[1];
    FILE *db;
};
struct monitor
{
	struct player_list *L;
	struct player_list *Dead;
	int *game_time;
	int *session_status;
	int *server_status;
	int *game_status;
	int pid;
};
//player_list functions
struct P *add_player(int sockfd,struct P *in,char name[10],int dim);
void print_list(struct P *in);
struct P *disconnect(struct P *Players,int ID);
struct P *search(int sockfd,struct P *list);
int list_size(struct P *list);
struct P *freeList(struct P *list);
//ServerGame functions
int server_game(char name[10],int sockfd,int *time,struct player_list *players,struct player_list *deaths,int dim,int seed);


#define MAX 80
#define PORT 8080
#define SA struct sockaddr
