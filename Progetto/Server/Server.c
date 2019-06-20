#include "lib_server.h"
#include "../scan_int/scan_int.h"

pthread_mutex_t sem=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void print_server_data(int flag)
{
int hostname_ok;
char hostname[256];
struct hostent *host_data;
char *Ip;
hostname_ok=gethostname(hostname,sizeof(hostname));
host_data=gethostbyname(hostname);
Ip=inet_ntoa( *((struct in_addr*)host_data->h_addr_list[0]) );
if (flag==0)printf("Hostname:%s    Ip;%s \n",hostname,Ip);
else {
	printw("Hostname:%s    Ip;%s \n",hostname,Ip);
	refresh();
     }

}

int genseed(){
    //Crea un seed per la tavola di gioco
    srand(time(NULL));
    int seed;
    seed=rand();
    printf("seed: %d\n",seed);
    return seed;
}

int login(int connfd,struct thread_data *tmp){
    char username[10];
    char password[10];
    char point_user[10];
    char point_pass[10];
    if(read(connfd,username,sizeof(username))==0){
        close(connfd);
        return -1;
    }
    if(read(connfd,password,sizeof(password))==0){
        close(connfd);
        return -1;
    }
    FILE *fp=fopen("login_credentials.db","r");

    while(fgets(point_user,sizeof(point_user),fp)) {
        fgets(point_pass,sizeof(point_pass),fp);
        point_user[strcspn(point_user, "\n")] = 0;
        point_pass[strcspn(point_pass, "\n")] = 0;
        if(strcmp(username,point_user)==0 && strcmp(password,point_pass)==0){
            fclose(fp);
            strcpy(tmp->name,username);
            return 1;
            }
    }
    fclose(fp);
    return 0;
}

//monitoring threads

void *activity_monitoring(void *arg)
{
	struct monitor *data=arg;
	int *server_status=data->server_status;
	int *game_status=data->game_status;
	int *game_time=data->game_time;
	int saved_gametime=*game_time;
	int pid=data->pid;
	int enter_press=0;
	const char *message="Il monitoraggio del server e' attivo.\nPremi Invio per fare in modo che ilserver termini la sua attivita' dopo questa partita\nPremi K per killare istantanemanete il processo\nPremi D per interrompere o far ripartire le sessioni,dopo questa partita\nPremi S per visualizzare le informazioni sul server\n";
	initscr();
	noecho();
	keypad(stdscr,TRUE);
	curs_set(0);
	printw(message);
	while(enter_press!=10)
	{
		enter_press=getch();
		refresh();
		switch(enter_press)
			{
				case'k':
					kill(pid,SIGINT);
					break;
				case 10:
					printw("\nIl server si disattivera' dopo questa partita\n");
					*game_status=SERVER_GAME_END;
					*server_status=SERVER_END;
					break;
				case'd':
					if(*game_status==SERVER_GAME_END)
						{
							printw("\nil gioco verra' avviato di nuovo dopo questa partita\n");
							*game_status=SERVER_GAME_ISACTIVE;
							*game_time=saved_gametime;
						}
					else {
						printw("\nl gioco verra' disattivato dopo questa partita\n");
						*game_status=SERVER_GAME_END;
						*game_time=1;
					     }
					break;
				case's':
					print_server_data(1);
					break;
			}
	}
	refresh();
	sleep(2);
	endwin();
pthread_exit(NULL);
}

void *clock_thread(void *arg)
{
struct monitor *tmp=arg;
int *game_time=tmp->game_time;
int saved_gametime=*game_time;
int *session_stat=tmp->session_status;
int *server_stat=tmp->server_status;
int *game_stat=tmp->game_status;
struct player_list *P=tmp->L;
struct player_list *Deaths=tmp->Dead;
while(*server_stat==SERVER_ISACTIVE)
	{
		while(*game_stat==SERVER_GAME_ISACTIVE)
			{
			while(*session_stat==SESSION_START)
				{
					sleep(1);
					pthread_mutex_lock(&sem);
					if(*game_time>0)
						*game_time=(*game_time)-1;
					else *session_stat=SESSION_END;
					pthread_mutex_unlock(&sem);
				}
			pthread_mutex_lock(&sem);
			*session_stat=SESSION_START;
			freeList(P->first);
			freeList(Deaths->first);
			*game_time=saved_gametime;
			//pthread_cond_broadcast(&c);
			pthread_mutex_unlock(&sem);
			}
	}
pthread_exit(NULL);
}

int sign_up(int connfd){
        char username[10];
        char password[10];
        if(read(connfd,username,sizeof(username))==0){
            close(connfd);
            return 0;
        }
        if(read(connfd,password,sizeof(password))==0){
            close(connfd);
            return 0;
        }
        FILE *fp=fopen("login_credentials.db","a");
        fprintf(fp,"%s\n%s\n",username,password);
        printf("new user created\n");
        fclose(fp);
        return 1;
}

void *send_dim(void *arg){
	int **board;
	int *GlobalGameTime;
    pthread_mutex_t thread_sem;
    pthread_mutex_init(&thread_sem,NULL);
    struct thread_data *tmp=arg;
	struct player_list *P=tmp->L;
	struct player_list *Deaths=tmp->Dead;
	GlobalGameTime=tmp->GameTime;
    int connfd=tmp->connfd;
    int *game_status=tmp->GameStatus;
    int *session_status=tmp->session_status;
    int dim[1];
	dim[0]=tmp->dim;
    int login_successful[1];
    int seed=tmp->seed[0];
    login_successful[0]=0;
    //printf("%d%d\n",connfd,seed[0]);
    int choice[1];
    if(read(connfd,choice,sizeof(choice))==0){
        close(connfd);
        return NULL;
    }
    //1=login 2=sign up
    if (choice[0]==3){
        close(connfd);
        return NULL;
    }
    if(choice[0]==2){
        if(sign_up(connfd)==0){
            return NULL;
        }
    }
    while(!login_successful[0]){
        login_successful[0]=login(connfd,tmp);
        if(login_successful[0]==-1){
            return NULL;
        }
        pthread_mutex_lock(&sem);
        if(write(connfd,login_successful,sizeof(login_successful))==0){
            close(connfd);
            return NULL;
        }
        pthread_mutex_unlock(&sem);
            }
    printf("%s connesso\n",tmp->name);
    while(*game_status==SERVER_GAME_ISACTIVE)
	{
		while(*session_status==SESSION_START)
		{
    		pthread_mutex_lock(&sem);
    		write(connfd,dim, sizeof(dim));
    		pthread_mutex_unlock(&sem);
    		server_game(tmp->name,connfd,GlobalGameTime,P,Deaths,dim[0],seed);
		}
		
	}
pthread_exit(NULL);
}

void server_log(char *data){
	time_t *ConnectionTime=malloc(sizeof(time_t));
	char PlayerTimestamp[70];
	time(ConnectionTime);
	strcpy(PlayerTimestamp,ctime(ConnectionTime));
	int fd,size;
	size=sizeof(data);
	char *logentry=malloc(size+100);
	fd=open("Log.txt",O_WRONLY|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
	sprintf(logentry,"%s   %s \n",data,PlayerTimestamp);
	//printf("%s ",logentry);
	if(write(fd,logentry,strlen(logentry))<0)
	{
		perror("errore nella compilazione del log");
	}
	close(fd);
}

int main(){
    //Variabili
    int sockfd, connfd, len,optval;
    int pid;
    pthread_t tid,time_tid,monitor_tid;
    int *server_status=malloc(sizeof(int));
    int *game_status=malloc(sizeof(int));
    int *session_status=malloc(sizeof(int));
    int GlobalGametime[1];
    char player_address[60];
    char connection_log[70];
    struct monitor monitor_data;
    *server_status=SERVER_ISACTIVE;
    *game_status=SERVER_GAME_ISACTIVE;
    *session_status=SESSION_START; 
    monitor_data.game_time=GlobalGametime;
    monitor_data.session_status=session_status;
    monitor_data.server_status=server_status;
    monitor_data.game_status=game_status;
    monitor_data.pid=getpid();
    print_server_data(0);
    //Creazione della connesione TCP
    struct sockaddr_in servaddr, cli;
    FILE *db;
    int seed;
    struct player_list *Players=malloc(sizeof(struct player_list));
    Players->first=NULL;
    struct player_list *Deaths=malloc(sizeof(struct player_list));
    Deaths->first=NULL;
    monitor_data.L=Players;
    monitor_data.Dead=Deaths;
    seed=genseed();
	db=fopen("login_credentials.db","r+");
	if(!db){
        printf("nessun utente registrato, creazione database\n");
        db=fopen("login_credentials.db","w+");
	}
	fclose(db);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Fallita creazione socket\n");
        exit(0);
    }
    else
        printf("La creazione del socket ha avuto successo..\n");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind fallito...\n");
        exit(0);
    }
    else
        printf("Socket bind ha avuto successo..\n");
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen fallito...\n");
        exit(0);
    }
    else
        printf("selezionare dimensione griglia (min:10  max:50)\n");
    int dim=scan_int(10,40);
    printf("setta un tempo di gioco (min:50  max:360)\n");
    GlobalGametime[0]=scan_int(50,360);
    printf("dimensione griglia: %d\n",dim);
    printf("Server listening..\n");
    //Inizia due thread necessari per fare monitoring
    pthread_create(&monitor_tid,NULL,activity_monitoring,&monitor_data);
    pthread_create(&time_tid,NULL,clock_thread,&monitor_data);
    //Gestisci connessioni e inizia la sessione
    struct thread_data thread_sd;
    while(*server_status==SERVER_ISACTIVE){
    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept fallito..\n");
        exit(0);
    }
    else{ 
        printw("server accept avvenuto con successo...\n");
        thread_sd.connfd=connfd;
        thread_sd.L=Players;
        thread_sd.Dead=Deaths;
        thread_sd.GameTime=GlobalGametime;
	thread_sd.GameStatus=game_status;
        thread_sd.dim=dim;
        thread_sd.seed[0]=seed;
	thread_sd.session_status=session_status;
	strcpy(player_address,inet_ntoa(cli.sin_addr));
	sprintf(connection_log,"Connessione da:%s",player_address);
	server_log(connection_log);
        //printf("%d %d\n",thread_sd[0],thread_sd[1]);
        pthread_create(&tid,NULL,send_dim,&thread_sd);
        }
    }
    close(sockfd);
    return 0;
}
