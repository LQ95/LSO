#include "lib.h"

int main(int argc,char **argv)
{
	Matrix Map;
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
	bind(sd,(struct sockaddr *)&ServAddr,sizeof(ServAddr));
	return data;	
}

int Login(char *user,char *psw)
{
	//this routine looks up usernames and passwords after having them passed to it from the server
	//returns 1 if login is successful,otherwise it returns 0
	
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
	//This subroutine converts a random number into a binary number,and then generates a vector of zeroes and ones and converta that into a Matrix type map
Matrix GenerateRandomMap(int height,int width)
{
	Matrix M;
	int i,j,z=0;
	M=malloc(height*sizeof(tile*));
	for(i=0;i<height;i++)
	{
		M[i]=malloc(width*sizeof(tile));
	}
	srand(time(NULL));
	int bit,size,seed;
	int *vec;
	while (size<height*width)
		{
			seed=rand();
			size=ceil(log2(seed));
			vec=malloc(size*sizeof(int));
			for(i=size;i>-1;i--)
				{
					bit=1<<i;
					//select every bit as the cycle goes on
					bit=bit&seed;
					if(bit) bit=1;
					vec[i]=bit;
				}
		}
	
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
			{
				M[i][j].bombflag=vec[z];
				z++;
			}
	}
	return M;
}