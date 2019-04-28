#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


#define PORT 8080
#define SA struct sockaddr
#define BUFDIM 1400
#define SIGSIZE 4
#define SignalSize sizeof(char)*SIGSIZE
#define DISPLAYSIGSIZE 5
#define DisplaySignalSize sizeof(char)*DISPLAYSIGSIZE

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
//these are codes that the server sends the client
#define SESSION_END 3333
#define CONNECTION_OK 6666
#define LOGIN_OK 6767
#define SIGNUP_OK 7777
#define MOVE_OK 1000
#define SQUARE_OCCUPIED 1222
#define ELIMINATED 3805
#define WIN 3883

//Client functions
int *update_pos(int *position,int moveflag);
void print_gamepos(int dim,int *position);
void client_game(int sd,int dim);
