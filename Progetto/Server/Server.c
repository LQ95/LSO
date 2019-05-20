#include "lib_server.h"
#include "../scan_int/scan_int.h"
pthread_mutex_t sem=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

int login(int connfd){
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
            return 1;
            }
    }
    fclose(fp);
    return 0;
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
	int **board,**positions;
	int *GlobalGameTime;
    struct thread_data *tmp=arg;
	struct player_list **P=tmp->L;
	struct player_list **Deaths=tmp->Dead;
	positions=tmp->posmap;
	GlobalGameTime=tmp->GameTime;
    int connfd=tmp->connfd;
    int dim[1];
	dim[0]=tmp->dim;
    int login_successful[1];
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
        login_successful[0]=login(connfd);
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

    pthread_mutex_lock(&sem);
    write(connfd,dim, sizeof(dim));
    pthread_mutex_unlock(&sem);
    //positions=init_positions(board,positions,seeddim[1],search_by_SD(connfd,P),connfd);
    //server_game(board,positions,P,seeddim[1],search_by_SD(connfd,P),Deaths,GlobalGameTime);
}

void server_log(char *data){
	int fd,size;
	size=sizeof(data);
	fd=open("Log.txt",O_WRONLY|O_CREAT|O_APPEND,S_IRUSR);
	if(write(fd,data,size)<0)
	{
		perror("errore nella compilazione del log");
	}
	close(fd);
}

int main(){
    //Creazione della connesione TCP
    int sockfd, connfd, len;
    int pid;

    pthread_t tid;
    struct sockaddr_in servaddr, cli;
    FILE *db;
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
        printf("selezionare dimensione griglia (min:10)\n");
        int dim=scan_int(10,INT_MAX);
        printf("dimensione griglia: %d\n",dim);
        printf("Server listening..\n");
        struct thread_data thread_sd;
        //int **positions=create_position_map(dim);
    struct player_list **Players=malloc(sizeof(struct player_list*));
    struct player_list **Deaths=malloc(sizeof(struct player_list*));
    int *GlobalGametime=malloc(sizeof(int));
    while (1){
    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server acccept fallito..\n");
        exit(0);
    }
    else{
        printf("server accept avvenuto con successo...\n");
        *GlobalGametime=rand()%MAXGAMETIME;
        //Players=insert(Players,connfd);
        thread_sd.connfd=connfd;
        thread_sd.L=Players;
        thread_sd.Dead=Deaths;
        thread_sd.posmap=NULL;
        thread_sd.GameTime=GlobalGametime;
        thread_sd.dim=dim;
        //printf("%d %d\n",thread_sd[0],thread_sd[1]);
        pthread_create(&tid,NULL,send_dim,&thread_sd);
        }
    }
    close(sockfd);
    return 0;
}
