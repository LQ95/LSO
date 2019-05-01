#include "lib_client.h"

int *update_pos(int *position,int moveflag){
	if (moveflag==MOVE_UP){
		position[1]++;
	}
	else if (moveflag==MOVE_DOWN){
		position[1]--;
	}
	else if (moveflag==MOVE_RIGHT){
		position[0]++;
	}
	else if (moveflag==MOVE_LEFT){
		position[0]--;
	}
	return position;
}
/*IntList CreatePositionList(char *PositionVector)
{
char *pos,coord;
int position[2];
const char delim1[2]="|";
IntList PosList=NULL;
pos=strtok(PositionVector,delim1);
while(pos!=NULL)
	{
		coord=pos[0];
		position[0]=atoi(&coord);           //qua bisogna cambiare strtok con strtok_r perche' strtok non e' thread-safe
		coord=pos[2];
		position[1]=atoi(&coord);
		PosList=add(PosList,position);
		pos=strtok(NULL,delim1);
		
	}
Print(PosList);
return PosList;
}
void print_gamepos(int width,int height,int *position,char *PositionVector)
{
	int x,y;
	IntList positions;
	int pos[2];
	//use strtok to transfer positions from the vector to the list
	positions=CreatePositionList(PositionVector);
        printf("\n");
	for(y=height;y>-1;y--)
	{
		for(x=0;x<width;x++)
		{
			pos[0]=x;
			pos[1]=y;
			if(x==position[0] && y==position[1])printf(".■");
			else if(IntSearch(positions,pos)==1 )
				{
				printf(".▄");
				}
			else printf(". ");
		}
		printf("\n");
	}
}*/
void print_gamepos(int dim,int *position){
	int x,y;
        printf("x:%d y:%d\n",position[0],position[1]);
	for(y=dim;y>-1;y--){
		for(x=0;x<dim;x++){
			if(x==position[0] && y==position[1])printf(".■");
			else printf(". ");
		}
		printf("\n");
	}
}
void print_status(int status)
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

void client_game(int sd,int dim){
    clear_screen();
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
	while(game_status!=SESSION_END){
		printf("\nnext move:");
		input=getchar();
		while ((getchar()) != '\n'); //this is used to clean stdin
		switch(input){
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
		clear_screen();
		read(sd,buf,SignalSize);
		game_status=atoi(buf);
		print_status(game_status);
		printf("\n");
		sprintf(answer, "%d", NULL_MOVE);
		if(game_status!=ELIMINATED && game_status!=WIN){
			if (moveflag!=0 && game_status!=SQUARE_OCCUPIED) position=update_pos(position,moveflag);
			printf("\ndisplay options:");
			input=getchar();
			while ((getchar()) != '\n');
			switch(input){
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
		if(strcmp(answer,"3000")!=0){
				printf("\ndisplaying:");
				read(sd,buf,DisplaySignalSize);
				displaysize=atoi(buf);
				read(sd,buf,displaysize) ;
				printf("%s",buf);
				sprintf(answer, "%d", MOVE_OK);
			}
		}
	else{
		if(game_status==ELIMINATED)
			sprintf(GameOverMsg, "\nYou died! \n");
		else if(game_status==WIN)
			sprintf(GameOverMsg, "\nYou Win! \n");
		else
		    sprintf(GameOverMsg, "\nGame Over \n");
		game_status=SESSION_END;
		}
	//sends and receives signals from the server,prints the map after every move as long as it participates in the game
	printf("\n");
	//TODO reads that read an array of positions
	/*read(sd,buf,DisplaySignalSize);
	displaysize=atoi(buf);
	read(sd,buf,displaysize);
	printf("%s\n",buf);*/
	print_gamepos(dim,position);
	}
    printf("\n%s\n",GameOverMsg);
    return;
}
