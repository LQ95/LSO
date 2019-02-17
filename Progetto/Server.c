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