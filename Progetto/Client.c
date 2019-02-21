#include "lib.h"

int main(int argc,char **argv)
{
	int socket,port;
	char *address;
	socket=ClientInit();
	if(argc>2)
	ConnectToServer(argv[1],socket,argv[2]);
	else
	{
		printf("please insert the IP or hostname,and port for the server you are interested in connecting to.")
		scanf("%s",address);
		scanf("%d",&port);
		ConnectToServer(address,socket,port);
	}
	return 0;
}

int ClientInit()
{
	int sd;
	sd=socket(PF_INET,SOCK_STREAM,0);
	return sd;
}

void ClientLoginPrompt()
{
	
}
int ConnectToServer(char *address,int socket,int port)
{
	int status
	struct sockaddr_in ServAddress;
	ServAddress.sin_family=AF_INET;
	ServAddress.sin_port=port;
	inet_aton(address,&ServAddress.sin_address);
	status=connect(socket,(struct sockaddr *)&ServAddress,sizeof(ServAddress));
	return status;
}