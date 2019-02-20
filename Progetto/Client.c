#include "lib.h"

int main(int argc,char **argv)
{
	char *address;
	ClientInit();
	if(argc>1)
	ConnectToServer(argv[1]);
	else
	{
		printf("please insert the IP or hostname for the server you are interested in connecting to.")
		scanf("%s",address);
		ConnectToServer(address);
	}
	return 0;
}

void ClientInit()
{
	
}

void ClientLoginPrompt()
{
	
}
void ConnectToServer(char *address)
{
	
}