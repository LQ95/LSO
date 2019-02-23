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

//these are codes that the server sends the client 
#define SESSION_END 3333
#define CONNECTION_OK 6666
#define LOGIN_OK 6666
#define SIGNUP_OK 7777
//these are codes that the client sends the server 
#define MOVE_LEFT 4900
#define MOVE_RIGHT 5000
#define MOVE_UP 5100
#define MOVE_DOWN 5200
#define NULL_MOVE 3000
#define LOGIN_REQUEST 4444
#define SIGNUP_REQUEST 5555


typedef struct P{
	int position[2];
	int speed;
	int score;
	int ID;
	int socket_desc;
}Player;
//seeing as there is the possibility of connecting players during the game,we need a data structure that can insert and remove an undertermined number of players
typedef struct list{
	Player P;
	struct list *next;
}PlayerList;

void ServerLog(char *data);
//PlayerList functions
PlayerList CreateList();
PlayerList insert(Player P,PlayerList L);
PlayerList eliminate(int ID,PlayerList L);
PlayerList search(int ID,,PlayerList L);
void FreeList(PlayerList L);

#define MAX 80
#define PORT 8080
#define SA struct sockaddr