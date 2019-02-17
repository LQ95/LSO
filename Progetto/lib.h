#include <sya/socket.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
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
void ServerGame(Matrix M);
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