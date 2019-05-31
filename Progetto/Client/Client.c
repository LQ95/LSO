#include "lib_client.h"
#include "../scan_int/scan_int.h"

pthread_mutex_t sem=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

//the sd variable is the Client's own socket descriptor once it's been connected to the server,and the position and bomb matrices are passed by the server
//the moveflag variable is used to signal to the client how is has mmoved if it received the MOVE_OK signal
//the ID variable is the client's Player ID in the current game(has to be sent from the server)

void genrcv(int sockfd){
    //riceve il seed dal server per generare la board
    int buff[1];
    int seed=0;
    read(sockfd, buff, sizeof(buff));
    printf("seed: %d\n",buff[0]);

}

void sign_up(int connfd){
    clear_screen();
    char username[10];
    char pass1[10];
    char pass2[10];
    printf("Nome Utente: ");
    scanf("%s",username);
    do{
        printf("Password: ");
        scanf("%s",pass1);
        printf("Reinserire Password: ");
        scanf("%s",pass2);
    }
    while((strcmp(pass1,pass2))!=0);
    write(connfd,username,sizeof(username));
    write(connfd,pass1,sizeof(pass1));
    return;
    }

int login(int connfd,struct data *out){
    int succ[1];
    succ[0]=0;
    char username[10];
    char pass[10];
    printf("nome utente: ");
    scanf("%s",username);
    printf("password:" );
    scanf("%s",pass);
    write(connfd,username,sizeof(username));
    write(connfd,pass,sizeof(pass));
    read(connfd,succ,sizeof(succ));
    strcpy(out->name,username);
    return succ[0];
}

int barmenu(const char **array,const int row, const int col, const int arraylength, const int width, int menulength, int selection){
        int counter,offset=0,ky=0;
        char formatstring[7];
        curs_set(0);

        if (arraylength < menulength)
                menulength=arraylength;
        if (selection > menulength)
                offset=selection-menulength+1;

        sprintf(formatstring,"%%-%ds",width); // remove - sign to right-justify the menu items

        while(ky != 27)
                {
                for (counter=0; counter < menulength; counter++)
                        {
                        if (counter+offset==selection)
                                attron(A_REVERSE);
                        mvprintw(row+counter,col,formatstring,array[counter+offset]);
                        attroff(A_REVERSE);
                        }

                ky=getch();

                switch(ky){
                        case KEY_UP:
                                if (selection){
                                        selection--;
                                        if (selection < offset)
                                                offset--;
                                        }
                                break;
                        case KEY_DOWN:
                                if (selection < arraylength-1){
                                        selection++;
                                        if (selection > offset+menulength-1)
                                                offset++;
                                        }
                                break;
                        case 10: //enter
                                return selection;
                                break;
                        case KEY_F(1): // function key 1
                                return -1;
                        case 27: //esc
                                // esc twice to get out, otherwise eat the chars that don't work
                                //from home or end on the keypad
                                ky=getch();
                                if (ky == 27){
                                        curs_set(0);
                                        mvaddstr(9,77,"   ");
                                        return -1;
                                        }
                                else
                                        if (ky=='['){
                                                getch();
                                                getch();
                                                }
                                        else
                                                ungetch(ky);
                        }
                }
        return -1;
    }

struct data *menu(int connfd){
    //lncurses stuff
    struct data *out=malloc(sizeof(struct data));
    int selection,row=1, col=3, arraylength=3, width=3, menulength=3;
    int choice[1];
    int dim[1];
    int succ_login=0;
    const char *choicesarray[]={"Login", "Sign Up", "Exit"};
    initscr();
    noecho();
    keypad(stdscr,TRUE);
    choice[0]=barmenu(choicesarray,row,col,arraylength,width,menulength,0);
    choice[0]++;
    write(connfd,choice,sizeof(choice));
    endwin();
     //sending data
     if(choice[0]==2){
        sign_up(connfd);
        clear_screen();
        printw("Nuovo utente Registrato!\n\n");
    }
    if(choice[0]==3){
        close(connfd);
        return NULL;
    }
    succ_login=login(connfd,out);
    while(!succ_login){
        printf("login errato\n");
        succ_login=login(connfd,out);
        }
    read(connfd, dim, sizeof(dim));
    out->dimension=dim[0];
    return out;
}

int waiting_menu()
{
	int status=CHECK_IF_SERVER_ISACTIVE;
	int choice=0;
	initscr();
    	noecho();
   	keypad(stdscr,TRUE);
	printw("Do you want to keep playing?(Y/N)\n");
	while(choice!=0)
		{
		choice=getch();
		switch(choice)
			{
			case 'y':
				choice=0;
				break;
			case 'n':
				status=END_CLIENT_ACTIVITY;
				choice=0;
				break;
			}
		}
	endwin();
	return status;
}

int main(){
    int sockfd, connfd,server_status,client_status;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Creazione del socket fallita...\n");
        exit(0);
    }
    else
        printf("Socket creato con successo..\n");
    bzero(&servaddr, sizeof(servaddr));
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    int pos[2];
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connessione al server fallita...\n");
        exit(0);
    }
    else
        printf("connesso al server..\n");
        struct data *send;
        send=menu(sockfd);
        send->time=1;
        send->connfd=sockfd;
	send->positions=malloc(sizeof(int)*2); 
        if(send!=NULL){
            printf("%s %d\n",send->name,send->dimension);
            //pthread_create(&tid,NULL,client_game_send,&send);
            pthread_t thread_receive;
            pthread_create(&thread_receive,NULL,client_game_recv,send);
            pthread_t thread_send;
            pthread_create(&thread_send,NULL,client_game_send,send);
	    //pthread_join(thread_receive,NULL);
            //pthread_join(thread_send,NULL);
            while(1){

            }
            close(sockfd);
    }
    return 0;
}
