#include "lib.h"

#include "scan_int/scan_int.h"

int * UpdatePos(int * position, int moveflag) {
  if (moveflag == MOVE_UP) {
    position[1]++;
  } else if (moveflag == MOVE_DOWN) {
    position[1]--;
  } else if (moveflag == MOVE_RIGHT) {
    position[0]++;
  } else if (moveflag == MOVE_LEFT) {
    position[0]--;
  }
  return position;
}

void print_gamepos(int width, int height, int * position) {
  int x, y;
  printf("x:%d y:%d\n", position[0], position[1]);
  for (y = height; y > -1; y--) {
    for (x = 0; x < width; x++) {
      if (x == position[0] && y == position[1]) printf(".■");
      else printf(". ");
    }
    printf("\n");
  }

}
//the sd variable is the Client's own socket descriptor once it's been connected to the server,and the position and bomb matrices are passed by the server
//the moveflag variable is used to signal to the client how is has mmoved if it received the MOVE_OK signal
//the ID variable is the client's Player ID in the current game(has to be sent from the server)
void ClientGame(int sd, int width, int height) {
  int game_status, displaysize, moveflag, ID, * position;
  game_status = LOGIN_OK;
  char input;
  char buf[BUFDIM];
  char GameOverMsg[35];
  char * answer = calloc(6, sizeof(char));
  position = malloc(sizeof(int) * 2);
  read(sd, buf, SignalSize);
  position[0] = atoi(buf);
  read(sd, buf, SignalSize);
  position[1] = atoi(buf);
  printf("ricevo:%d %d", position[0], position[1]);
  while (game_status != SESSION_END) {
    printf("\nnext move:");
    scanf(" %c", & input);
    while ((getchar()) != '\n'); //this is used to clean stdin
    switch (input) {
    case 'w':
      sprintf(buf, "%d", MOVE_UP);
      moveflag = MOVE_UP;
      write(sd, buf, SignalSize);
      break;

    case 's':
      sprintf(buf, "%d", MOVE_DOWN);
      moveflag = MOVE_DOWN;
      write(sd, buf, SignalSize);
      break;

    case 'a':
      sprintf(buf, "%d", MOVE_LEFT);
      moveflag = MOVE_LEFT;
      write(sd, buf, SignalSize);
      break;

    case 'd':
      sprintf(buf, "%d", MOVE_RIGHT);
      moveflag = MOVE_RIGHT;
      write(sd, buf, SignalSize);
      break;

    case 'm':
      sprintf(buf, "%d", QUIT);
      write(sd, buf, SignalSize);
      break;

    case '0':
      sprintf(buf, "%d", NULL_MOVE);
      moveflag = 0;
      write(sd, buf, SignalSize);
      break;

    default:
      sprintf(buf, "%d", NULL_MOVE);
      moveflag = 0;
      write(sd, buf, SignalSize);
      break;
    }
    read(sd, buf, SignalSize);
    game_status = atoi(buf);
    printf("\nstatus %d display options:", game_status);
    printf("\n");
    sprintf(answer, "%d", NULL_MOVE);
    if (game_status != ELIMINATED && game_status != WIN) {
      if (moveflag != 0 && game_status != SQUARE_OCCUPIED) position = UpdatePos(position, moveflag);
      scanf(" %c", & input);
      while ((getchar()) != '\n');
      switch (input) {
      case '1':
        sprintf(buf, "%d", DISPLAY_USERS);
        sprintf(answer, "%d", DISPLAY_USERS);
        write(sd, buf, SignalSize);
        break;

      case '2':
        sprintf(buf, "%d", DISPLAY_USER_LOCATIONS);
        sprintf(answer, "%d", DISPLAY_USER_LOCATIONS);
        write(sd, buf, SignalSize);
        break;

      case '3':
        sprintf(buf, "%d", DISPLAY_USER_DEATHS);
        sprintf(answer, "%d", DISPLAY_USER_DEATHS);
        write(sd, buf, SignalSize);
        break;

      default:
        sprintf(buf, "%d", NULL_MOVE);
        sprintf(answer, "%d", NULL_MOVE);
        write(sd, buf, SignalSize);
        break;

      }
      if (strcmp(answer, "3000") != 0) {
        printf("\ndisplaying:");
        read(sd, buf, DisplaySignalSize);
        displaysize = atoi(buf);
        read(sd, buf, displaysize);
        printf("%s", buf);
        sprintf(answer, "%d", MOVE_OK);
      }
    } else {
      if (game_status == ELIMINATED) {
        sprintf(GameOverMsg, "\nYou died! \n");
      } else if (game_status == WIN) {
        sprintf(GameOverMsg, "\nYou Win! \n");
      } else {
        sprintf(GameOverMsg, "\nGame Over \n");
      }
      game_status = SESSION_END;
    }
    //sends and receives signals from the server,prints the map after every move as long as it participates in the game
    printf("\n");
    print_gamepos(width, height, position);
  }
  printf("\n%s\n", GameOverMsg);
  return;
}

void genrcv(int sockfd) {
  //riceve il seed dal server per generare la board
  int buff[1];
  int seed = 0;
  read(sockfd, buff, sizeof(buff));
  printf("seed: %d\n", buff[0]);

}

void sign_up(int connfd) {
  char username[10];
  char pass1[10];
  char pass2[10];
  printf("Nome Utente: ");
  scanf("%s", username);
  do {
    printf("Password: ");
    scanf("%s", pass1);
    printf("Reinserire Password: ");
    scanf("%s", pass2);
  }
  while ((strcmp(pass1, pass2)) != 0);
  write(connfd, username, sizeof(username));
  write(connfd, pass1, sizeof(pass1));
  return;
}
int login(int connfd) {
  int succ[1];
  succ[0] = 0;
  char username[10];
  char pass[10];
  printf("nome utente: ");
  scanf("%s", username);
  printf("password:");
  scanf("%s", pass);
  write(connfd, username, sizeof(username));
  write(connfd, pass, sizeof(pass));
  read(connfd, succ, sizeof(succ));
  return succ[0];
}
int menu(int connfd) {
  int choice[1];
  int seed[1];
  printf("benvenuto!\n1.Effettuare il login\n2.Crea un nuono utente\n");
  choice[0] = scan_int(1, 2);
  write(connfd, choice, sizeof(choice));
  if (choice[0] == 2) {
    sign_up(connfd);
    printf("Nuovo utente Registrato!\n\n");
  }
  int succ_login = 0;
  succ_login = login(connfd);
  while (!succ_login) {
    printf("login errato\n");
    succ_login = login(connfd);
  }
  read(connfd, seed, sizeof(seed));
  return seed[0];
}

int main() {
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;

  // socket create and varification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Creazione del socket fallita...\n");
    exit(0);
  } else
    printf("Socket creato con successo..\n");
  bzero( & servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // connect the client socket to server socket
  if (connect(sockfd, (SA * ) & servaddr, sizeof(servaddr)) != 0) {
    printf("connessione al server fallita...\n");
    exit(0);
  } else
    printf("connesso al server..\n");
  int seed = menu(sockfd);
  printf("seed:%d\n", seed);
  ClientGame(sockfd, 10, 10);
  close(sockfd);
}
