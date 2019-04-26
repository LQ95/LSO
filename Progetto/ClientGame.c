#include "lib.h"
void PrintStatus(int status)
{
int statuses[]={SESSION_END,MOVE_OK,SQUARE_OCCUPIED,ELIMINATED,WIN};
if(status==statuses[0])
	{
		printf("The session has ended\n");
	}
else if(status==statuses[2])
	{
		printf("You cannot move there\n");
	}
else if(status==statuses[3])
	{
		printf("An anti-tank mine just blew up under your car.\n");
	}
else return;
}

void ClientGame(int sd,int dim)
{
	int game_status,displaysize,moveflag,ID,*position;
	game_status=LOGIN_OK;
	char input;
	char buf[BUFDIM];
	char GameOverMsg[35];
	char *answer=calloc(6,sizeof(char));
	position=malloc(sizeof(int)*2);
	read(sd,buf,SignalSize);
	position[0]=atoi(buf);
	read(sd,buf,SignalSize);
	position[1]=atoi(buf);
	printf("ricevo:%d %d",position[0],position[1]);
	while(game_status!=SESSION_END)
	{
		printf("\nnext move:");
		input=getchar();
		while ((getchar()) != '\n'); //this is used to clean stdin
		switch(input)
		{
			case 'w':
			sprintf(buf, "%d", MOVE_UP);
			moveflag=MOVE_UP;
			write(sd,buf,SignalSize);
			break;

			case 's':
			sprintf(buf, "%d", MOVE_DOWN);
			moveflag=MOVE_DOWN;
			write(sd,buf,SignalSize);
			break;

			case 'a':
			sprintf(buf, "%d", MOVE_LEFT);
			moveflag=MOVE_LEFT;
			write(sd,buf,SignalSize);
			break;

			case 'd':
			sprintf(buf, "%d", MOVE_RIGHT);
			moveflag=MOVE_RIGHT;
			write(sd,buf,SignalSize);
			break;

			case 'm':
			sprintf(buf, "%d", QUIT);
			write(sd,buf,SignalSize);
			break;

			case '0':
			sprintf(buf, "%d", NULL_MOVE);
			moveflag=0;
			write(sd,buf,SignalSize);
			break;

			default:
			sprintf(buf, "%d", NULL_MOVE);
			moveflag=0;
			write(sd,buf,SignalSize);
			break;
		}
		read(sd,buf,SignalSize);
		game_status=atoi(buf);
		PrintStatus(game_status);
		printf("\n");
		sprintf(answer, "%d", NULL_MOVE);
		if(game_status!=ELIMINATED && game_status!=WIN)
		{
			if (moveflag!=0 && game_status!=SQUARE_OCCUPIED) position=UpdatePos(position,moveflag);
			printf("\ndisplay options:");
			input=getchar();
			while ((getchar()) != '\n');
			switch(input)
			{
				case '1':
				sprintf(buf, "%d", DISPLAY_USERS);
				sprintf(answer, "%d", DISPLAY_USERS);
				write(sd,buf,SignalSize);
				break;

				case '2':
				sprintf(buf, "%d", DISPLAY_USER_LOCATIONS);
				sprintf(answer, "%d", DISPLAY_USER_LOCATIONS);
				write(sd,buf,SignalSize);
				break;

				case '3':
				sprintf(buf, "%d", DISPLAY_USER_DEATHS);
				sprintf(answer, "%d", DISPLAY_USER_DEATHS);
				write(sd,buf,SignalSize);
				break;

				case '4':
				sprintf(buf, "%d", DISPLAY_REMAINING_TIME);
				sprintf(answer, "%d", DISPLAY_REMAINING_TIME);
				write(sd,buf,SignalSize);
				break;

				default:
				sprintf(buf, "%d", NULL_MOVE);
				sprintf(answer, "%d", NULL_MOVE);
				write(sd,buf,SignalSize);
				break;

			}
		if(strcmp(answer,"3000")!=0)
			{
				printf("\ndisplaying:");
				read(sd,buf,DisplaySignalSize);
				displaysize=atoi(buf);
				read(sd,buf,displaysize) ;
				printf("%s",buf);
				sprintf(answer, "%d", MOVE_OK);
			}
		}
	else
		{
		if(game_status==ELIMINATED)
			{
			sprintf(GameOverMsg, "\nYou died! \n");
			}
		else if(game_status==WIN)
			{
			sprintf(GameOverMsg, "\nYou Win! \n");
			}
		else
		    {
		    sprintf(GameOverMsg, "\nGame Over \n");
		    }
		game_status=SESSION_END;
		}
	//sends and receives signals from the server,prints the map after every move as long as it participates in the game
	printf("\n");
	//TODO reads that read an array of positions
	print_gamepos(dim,dim,position);
	}
printf("\n%s\n",GameOverMsg);
return;
}
