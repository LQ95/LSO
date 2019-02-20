#include "lib.h"

int main(int argc,char **argv)
{
	ServerInit();
	return 0;
}

void ServerGame(Matrix M,int time)
{
	int session_status,gametime;
	gametime=time;
	//we probably need to have a thread that handles new connections in real time here
	while(session_status!=SESSION_END)
	{
		
	}
	
}

void ServerInit()
{
	//the return type for this function probably needs to be different than void(maybe a custom defined sruct that holds all server data?)
	//
	
}

int Login(char *user,char *psw)
{
	//this routine looks up usernames and passwords after having them passed to it from the server
	//returns 1 if login is successful,otherwise
	
}

void ServerLog(char *data)
{
	int fd,size;
	size=sizeof(data);
	fd=open("Log.txt",O_WRONLY|O_CREAT|O_APPEND,S_IRUSR);
	if(write(fd,data,size)<0)
	{
		perror("errore nella compilazione del log");
	}
	close(fd);
}