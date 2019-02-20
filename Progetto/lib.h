#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
//these are codes that the server sends the client 
#define SESSION_END 3333
#define LOGIN_OK 6666
#fdefine SIGNUP_OK 7777
//these are codes that the client sends the server in order to signal that it wants to make a move in the game 
#define MOVE_LEFT 4900
#define MOVE_RIGHT 5000
#define MOVE_UP 5100
#define MOVE_DOWN 5200
#define NULL_MOVE 3000
#define LOGIN_REQUEST 4444
#fdefine SIGNUP_REQUEST 5555
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
	int position[1][1];
	int speed;
	int score;
	int ID;
}Player;
Matrix GenerateRandomMap();
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