#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <limits.h>
#include <stdio.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>
#include <limits.h>
#define BUFDIM 1400
#define MAXGAMETIME 4000
//Signals
//keep every single one of these at the same length!
#define SIGSIZE 4
#define SignalSize sizeof(char)*SIGSIZE
#define DISPLAYSIGSIZE 5
#define DisplaySignalSize sizeof(char)*DISPLAYSIGSIZE
//these are codes that the server sends the client
#define SESSION_END 3333
#define CONNECTION_OK 6666
#define LOGIN_OK 6767
#define SIGNUP_OK 7777
#define MOVE_OK 1000
#define SQUARE_OCCUPIED 1222
#define ELIMINATED 3805
#define WIN 3883
//these are codes that the client sends the server
#define MOVE_LEFT 4900
#define MOVE_RIGHT 5000
#define MOVE_UP 5100
#define MOVE_DOWN 5200
#define NULL_MOVE 3000
#define LOGIN_REQUEST 4444
#define SIGNUP_REQUEST 5555
#define QUIT 4345
#define DISPLAY_USERS 1343
#define DISPLAY_USER_LOCATIONS 1344
#define DISPLAY_USER_DEATHS 1345
#define DISPLAY_REMAINING_TIME 1346
//the socket descriptor in theis structure is the socket descriptor the server makes when the connection with the player is accepted
typedef struct P{
	int position[2];
	int score;
	int socket_desc;
}Player;
//seeing as there is the possibility of connecting players during the game,we need a data structure that can insert and remove an undertermined number of players
typedef struct list{
	Player P;
	struct list *next;
}PlayerNode;
typedef PlayerNode *player_list;
void ServerLog(char *data);
int **create_board(int seed,int dim);

//player_list functions
/*player_list CreateList();
player_list insert(player_list L,int sd);
player_list eliminate(int ID,player_list L);
player_list eliminate_disconnect(int ID,player_list L);
player_list search(int ID,player_list L);
void free_list(player_list L);
int list_size(player_list L);
player_list search_by_SD(int sd,player_list L);*/
//ServerGame functions
/*int **create_position_map(int dim);
int check_free(int x,int y,int **position,int dim);
int check_bomb(int coord[2],int **map);
int check_win(player_list L,int dim);
int **init_positions(int **board,int **positions,int dim,player_list P,int connfd);
char *display(player_list L,int flag,player_list deaths,char *data);
void server_game(int **board,int **positions,player_list L,int dim,player_list P,player_list Dead,int *GameTime);*/

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
