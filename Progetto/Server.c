#include "lib.h"

int main(int argc,char **argv)
{
	Serverdata GameServer;
	GameServer=ServerInit();
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

Serverdata ServerInit()
{
	Serverdata data;
	int sd;
	struct sockaddr_in ServAddr;
	//defines a socket and an address for the server,returns the serve data,after setting up the server;
	ServAddr.sin_family=AF_INET;
	ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sd=socket(PF_INET,SOCK_STREAM,0);
	data.socket_desc=sd;
	data.address=ServAddr;
	//Server setup here(binding,listening,etc)
	
	return ServAddr;	
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