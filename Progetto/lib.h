#ifndef GAME_LIB
#define GAME_LIB
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <fcntl.h>

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
//this definition might probably need to be modified
//as it is it's simply used as a return type for the ServerInit function to store server data 
typedef struct serv{
	int socket_desc;
	struct sockaddr_in address;
}Serverdata;

typedef struct square{
	int bombflag;
}tile;

typedef tile **Matrix;

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

Matrix GenerateRandomMap(int height,int width);
void ServerGame(Matrix M,int time);
void ClientGame(Matrix M);
void ConnectToServer(char *address);
void Signup();
int Login(char *user,char *psw);
void ClientLoginPrompt();
void ShowOnline();
void ShowTime();
void ShowObstacles();
void ShowPositions();
Serverdata ServerInit();
void ClientInit();
void ServerLog(char *data);

//PlayerList functions
PlayerList CreateList();
PlayerList insert(Player P,PlayerList L);
PlayerList eliminate(int ID,PlayerList L);
Player search(int ID,,PlayerList L);
PlayerList FreeList(PlayerList L);
#endif