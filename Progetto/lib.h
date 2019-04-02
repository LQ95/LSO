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
#include <string.h>
#include <arpa/inet.h>
#define BUFDIM 1400
#define MAXGAMETIME 4000
int gametime; // globaò variable,shared among every thread
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
	int ID;
	int socket_desc;
}Player;
//seeing as there is the possibility of connecting players during the game,we need a data structure that can insert and remove an undertermined number of players
typedef struct list{
	Player P;
	struct list *next;
}PlayerNode;
typedef PlayerNode *PlayerList;
void ServerLog(char *data);
int **create_board(int seed);

//PlayerList functions
PlayerList CreateList();
PlayerList insert(PlayerList L,int sd);
PlayerList eliminate(int ID,PlayerList L);
PlayerList eliminate_disconnect(int ID,PlayerList L);
PlayerList search(int ID,PlayerList L);
void FreeList(PlayerList L);
int ListSize(PlayerList L);
PlayerList searchbySD(int sd,PlayerList L);
//ServerGame functions
int **create_position_map(int width,int height);
int CheckFree(int x,int y,int **position,int width,int height);
int CheckBomb(int coord[2],int **map);
int CheckWin(PlayerList L,int height,int width);
int **initPositions(PlayerList L,int **board,int **positions,int height,int width);
int **initBombs(int **board,int **positions,int height,int width);
char *display(PlayerList L,int flag,PlayerList deaths,char *data);
void ServerGame(int **board,int **positions,PlayerList L,int width,int height,int gametime,PlayerList P,PlayerList Dead);

//Client functions 
int *UpdatePos(int *position,int moveflag);
void print_gamepos(int width,int height,int *position);

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
