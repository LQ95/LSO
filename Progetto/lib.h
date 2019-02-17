#include <sya/socket.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
//these are codes that the server sends the client 
#define SESSION_END 3000

//these are codes that the client sends the server in order to signal that it wants to make a move in the game 
#define MOVE_LEFT 4900
#define MOVE_RIGHT 5000
#define MOVE_UP 5100
#define MOVE_DOWN 5200
#define NULL_MOVE 3333

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
void ServerGame(Matrix M,int time);
void ClientGame(Matrix M);
void ConnectToServer();
void Signup();
void Login();
void ShowOnline();
void ShowTime();
void ShowObstacles();
void ShowPositions();
void ServerInit();
void ClientInit();
void ServerLog(char *data);