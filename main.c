#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/sem.h>

#include "game.h"

#define FIFO_FILE "/tmp/fifo_twoway"

char* path_1 ="./pathconfig.txt";

void check_input(char *str, int min, int max) {
  while(!checks_number(str) || atoi(str) < min || atoi(str) > max) {
    printf("Please insert a number between/or %d and %d: ",min,max);
    scanf("%s", str);
  }
}

void rules() {
  printf("\n");
  printf("               ____        __        ____             _   __                  __\n");
  printf("              / __ )____ _/ /_____ _/ / /_  ____ _   / | / /___ __   ______ _/ /\n");
  printf("             / __  / __ '/ __/ __ '/ / __ |/ __ '/  /  |/ / __ '/ | / / __ '/ /\n");
  printf("            / /_/ / /_/ / /_/ /_/ / / / / / /_/ /  / /|  / /_/ /| |/ / /_/ / /\n");
  printf("           /_____/|__,_/|__/|__,_/_/_/ /_/|__,_/  /_/ |_/|__,_/ |___/|__,_/_/\n");
  printf("\n");

  printf("======================================================================================\n");
  printf("------------------------------------------RULES---------------------------------------\n");
  printf("======================================================================================\n");

  printf("\n");

  printf("STARTING THE GAME\n");
  printf("--------------------------------------------------------------------------------------\n");
  printf("1 - Each player places the number of ships chose on their board.\n");
  printf("2 - The ships can only be placed vertically or horizontally(diagonal placement is not allowed).\n");
  printf("3 - No part of a ship may hang off the edge of the board.\n");
  printf("4 - Ships may not overlap each other(no ships may be placed on another ship).\n");
  printf("5 - Once the guessing begins, the players may not move the ships.\n");
  printf("--------------------------------------------------------------------------------------\n");
  
  printf("\n");
  
  printf("DURING THE GAME\n");
  printf("--------------------------------------------------------------------------------------\n");
  printf("1 - Players take turns guessing by calling out the coordinates.\n");
  printf("2 - Once an attack happens, it is announced a 'hit' or a 'miss' as appropriate.\n");
  printf("3 - If the position attacked is a 'hit' it is recorded in the attacked player's board.\n");
  printf("4 - When all of the squares of a player's ship have been hit, the ship will sunk. And it is announced that a ship has sunk.\n");
  printf("5 - As soon as all of one player's ships have been sunk, the game ends.\n");
  printf("--------------------------------------------------------------------------------------\n");
}

void header(int player,int board_size) {
  for(int i=0; i<board_size*3.1; i++) printf("=");
  printf("\n");
  for(int i=0; i<(board_size*3.1-12)/2; i++) printf("-");
  printf("PLAYER%d turn",player);
  for(int i=0; i<(board_size*3.1-12)/2; i++) printf("-");
  printf("\n");
  for(int i=0; i<board_size*3.1; i++) printf("=");
  printf("\n");
}

void gridOpponent(char opponent[][40],int board_size) {
  printf("  ");
  if(board_size < 10) printf("%d ",1);
  else printf(" %2d ",1);
  for(int i=2; i<=board_size; i++)
    printf("%2d ",i);
  printf("\n");
  for(int i=0; i<board_size; i++) {
    if(i+1<10) printf(" %d ",i+1);
    else printf("%d ",i+1);
    for(int j=0; j<board_size; j++)
      printf(" %c ",opponent[i][j]);
    printf("\n");
  }
}

int main() {
  int fd;
  int read_bytes;
  int n_ships_hit = 0;
  int option,player,mode;
  char str[2];
  time_t t;
  int board_size,nBoats,max_boats;
  map* m;
  int flag=0,attackBegin=0,first=0;
  char opponent[40][40];
  
  /* Initializes random number generator */
  srand((unsigned) time(&t));

  printf(":: One terminal (1)\n");
  printf(":: Two terminal with Named Pipes(2)\n");
  printf(":: Two terminal with Semaphore and text file (3)\n");
  scanf("%d",&mode);

  while(mode!=1 && mode!=2 && mode!=3) {
    printf("Please insert a valid mode (1,2 or 3).\nMode: ");
    scanf("%d",&mode);
  }

  if(mode == 1) { //without IPC - only one shell
    
    int xBomb1,yBomb1,xBomb2,yBomb2;
    int n_ships_hit_player1 = 0;
    int n_ships_hit_player2 = 0;
    map* m1;
    map* m2;
    Point result1,result2;

    /* Intializes random number generator */
    srand((unsigned) time(&t));
  
    printf("Board size wanted(between 20 and 40): ");
    scanf("%s", str);
    check_input(str,20,40);
    board_size = atoi(str);
  
    m1 = new_map(board_size);
    m2 = new_map(board_size);
  
    max_boats = (board_size*board_size)/(5*5);
    printf("Min number of boats = 5\n");
    printf("Max number of boats = %d\n", max_boats);

    printf("Number of boats: ");
    scanf("%s", str);
    check_input(str,5,max_boats);

    nBoats = atoi(str);

    printf("\n");
    printf("PLAYER1-Manual(0) or random(1)? ");
    scanf("%s", str);

    check_input(str,0,1);
    option = atoi(str);
    setBoardUser(option,nBoats,1,m1);
    printf("\nPLAYER2-Manual(0) or random(1)? ");
    scanf("%s", str);
    check_input(str,0,1);
    option = atoi(str);
    setBoardUser(option,nBoats,2,m2);

    printf("\n");
    rules();
    printf("\n");

    //GAME -> game ends when one of the players' ships have all sank
    while(n_ships_hit_player1 != nBoats) {
    
      //player 1
      header(1,board_size);
      printf("\n");
      grid_print_attack(m2);
      printf("\n");
      printf("Attack coordinates: ");
      scanf("%s", str);
      check_input(str,1,board_size);
      xBomb1 = atoi(str);
      scanf("%s", str);
      check_input(str,1,board_size);
      yBomb1 = atoi(str);
      //since player inserts coordinates between 1 and board_size and the matrix boundaries are between 0 and board_size-1
      xBomb1--;
      yBomb1--;
      result1 = dropBomb(m2, xBomb1, yBomb1, n_ships_hit_player2);
      n_ships_hit_player2 = result1.x;
      if(n_ships_hit_player2 == nBoats) break;

      //player2
      header(2,board_size);
      grid_print_attack(m1);
      printf("\n");
      printf("Attack coordinates: ");
      scanf("%s", str);
      check_input(str,1,board_size);
      xBomb2 = atoi(str);
      scanf("%s", str);
      check_input(str,1,board_size);
      yBomb2 = atoi(str);
      //since player inserts coordinates between 1 and board_size and the matrix boundaries are between 0 and board_size-1
      xBomb2--;
      yBomb2--;
      result2 = dropBomb(m1, xBomb2, yBomb2, n_ships_hit_player1);
      n_ships_hit_player1 = result2.x;
     
    }
  
    if(n_ships_hit_player1 == nBoats)
      printf("PLAYER 2 WON!\n"); 
    else
      printf("PLAYER 1 WON!\n");

    printf("\n");

    printf("PLAYER1 attempts: %d\n", m2->misses);
    printf("PLAYER2 attempts: %d\n", m1->misses);

    destroy_map(m1);
    destroy_map(m2);
  
  } else if(mode == 2) { //Named Pipes

    printf("PLAYER: ");
    scanf("%d",&player);

    while(player!=1 && player!=2) {
      printf("Please insert player 1 or 2 only.\nPLAYER: ");
      scanf("%d",&player);
    }
  
    if(player == 1) {
      printf("Board size wanted(between 20 and 40): ");
      scanf("%s", str);

      check_input(str,20,40);
  
      board_size = atoi(str);

      m = new_map(board_size);
      for(int i=0; i<board_size; i++) {
	for(int j=0; j<board_size; j++)
	  opponent[i][j]='.';
      }

      max_boats = (board_size*board_size)/(5*5);
      printf("Min number of boats = 5\n");
      printf("Max number of boats = %d\n", max_boats);

      printf("Number of boats: ");
      scanf("%s", str);

      check_input(str,5,max_boats);
      nBoats = atoi(str);

      printf("\n");
      rules();
      printf("\n");

      printf("\n");
      m = new_map(board_size);
      printf("PLAYER1 - Manual(0) or random(1)? ");
      scanf("%s", str);
      printf("\n");

      check_input(str,0,1);
      option = atoi(str);
      setBoardUser(option,nBoats,1,m);
      printf("\n");
  
      fd = open(FIFO_FILE, O_CREAT|O_RDWR);

      Point result;
      int x,y; //will store the player's attack coordinates
      while(1) {

	int attack[2],attacked[2];
	char readbuf[50];
    
	if(flag == 0) {
	  sprintf(readbuf,"%d %d",board_size,nBoats); //the player needs to set up the game: board size and the number o boats in the game
	  write(fd,readbuf,strlen(readbuf)); //that information will be passed onto the PLAYER2
	  flag = 1; //set up is done
	  continue; //transmits to the other player
	}

	if(attackBegin == 1) { //if first attack has occured already
	  read_bytes = read(fd, readbuf, sizeof(readbuf)); //reads the attack done by the opponent
	  readbuf[read_bytes] = '\0';
	  if(strcmp(readbuf,"PLAYER 1 WON!\n") == 0) { //when the player receives the message that he/she has won
	    write(STDOUT_FILENO,readbuf,strlen(readbuf)); //let's the player know he/she has won
	    destroy_map(m); //the player's map is freed
	    close(fd); //the pipe is closed
	    break; //breaks from the infinite loop and the game ends for the player
	  }
	  int resultY;
	  sscanf(readbuf,"%d %d %d",&attacked[0],&attacked[1],&resultY);
	  if(resultY == 0) {
	    printf("IT WAS A HIT!\n");
	    opponent[x][y] = '@';
	  } else if(resultY == 1) {
	    printf("THIS POSITION HAS ALREADY BEEN ATTACKED BEFORE.\n");
	  } else if(resultY == 2) {
	    printf("IT WAS A MISS!\n");
	    opponent[x][y] = 'X';
	  }
	
	  //it's done +1 because coordinates are from range 0 to board_size-1 internally but in
	  //the player's view they are from 1 to board_size
	  printf("\nOpponent attacked position: %d %d and ",attacked[0]+1,attacked[1]+1);
	  //result will save the number of shits hit by the opponent's attack and what resulted from that same attack
	  result = dropBomb(m,attacked[0],attacked[1],n_ships_hit); 
	  n_ships_hit = result.x;
	}
	attackBegin = 1; //the first attack has occured already

	if(result.x == nBoats) { //n_ships_hit == nBoats
      
	  strcpy(readbuf,"PLAYER 2 WON!\n");
	  write(fd,readbuf,strlen(readbuf)); //the message that player2 has won is passed to player2
	  printf("PLAYER 2 WON!\n"); 
	  destroy_map(m); //the player's map is freed
	  close(fd); //the pipe is closed
	  break;
      
	} else {
      
	  header(1,board_size);
	  //printf of the opponent's grid
	  gridOpponent(opponent,m->size);
	  printf("\nAttack coordinates: ");
	  scanf("%s", str);
	  check_input(str,1,board_size);
	  attack[0] = atoi(str);
	  attack[0]--;
	  x = attack[0];
	  scanf("%s", str);
	  check_input(str,1,board_size);
	  attack[1] = atoi(str);
	  attack[1]--;
	  y = attack[1];
	  char buffer[strlen(readbuf)];
	  sprintf(buffer,"%d %d %d",attack[0],attack[1],result.y);
	  //the player's attack information and the result of the opponent's attack are passed to the opponent
	  write(fd,buffer,strlen(buffer)); 
	  // sleep - this is to make sure other process reads this, otherwise this process would retrieve the message
	  sleep(10);      
	}
      }
    
    } else {
    
      printf("\n");
      rules();
      printf("\n");

      /* Create the FIFO if it does not exist */
      mkfifo(FIFO_FILE, S_IFIFO|0640);
      fd = open(FIFO_FILE, O_RDWR);

      Point result;
      int x,y;
      //initializes the opponent's board that is shown to the player
      for(int i=0; i<40; i++) {
	for(int j=0; j<40; j++)
	  opponent[i][j]='.';
      }
      while(1) {

	int attack[2],attacked[2];
	char readbuf[50]; 
    
	if(flag == 0) { //set up of player2's board
      
	  read_bytes = read(fd,readbuf,sizeof(readbuf)); //reads the attack done by the opponent
	  readbuf[read_bytes] = '\0';
	  //the player receives the board's information
	  sscanf(readbuf,"%d %d",&board_size,&nBoats);
	  printf("Board size: %d\n",board_size);
	  printf("Number of boats: %d\n",nBoats);
	  printf("\nPLAYER2 - Manual(0) or random(1)? ");
	  scanf("%s", str);
	  printf("\n");
	  check_input(str,0,1);
	  option = atoi(str);
	  m = new_map(board_size);
	  setBoardUser(option,nBoats,2,m);
	  printf("\n");
	  flag = 1;
	  continue;
      
	}
    
	read_bytes = read(fd, readbuf, sizeof(readbuf));
	readbuf[read_bytes] = '\0';
	if(strcmp(readbuf,"PLAYER 2 WON!\n") == 0) {  //when the player receives the message that he/she has won
	  write(STDOUT_FILENO,readbuf,strlen(readbuf)); //let's the player know he/she has won
	  destroy_map(m); //the player's map is freed
	  close(fd); //the pipe is closed
	  break; //breaks from the infinite loop and the game ends for the player
	}
      
	int resultY;
	sscanf(readbuf,"%d %d %d",&attacked[0],&attacked[1],&resultY);
	if(first == 1) { //if player2 has done it's first attack already
	  //result of the player's attack
	  if(resultY == 0) {
	    printf("IT WAS A HIT!\n");
	    opponent[x][y] = '@';
	  } else if(resultY == 1) {
	    printf("THIS POSITION HAS ALREADY BEEN ATTACKED BEFORE.\n");
	  } else if(resultY == 2) {
	    printf("IT WAS A MISS!\n");
	    opponent[x][y] = 'X';
	  }
	}

	//it's done +1 because coordinates are from range 0 to board_size-1 internally but in
	//the player's view they are from 1 to board_size
	printf("\nOpponent attacked position: %d %d and ",attacked[0]+1,attacked[1]+1);
	//result will save the number of shits hit by the opponent's attack and what resulted from that same attack
	result = dropBomb(m,attacked[0],attacked[1],n_ships_hit);
	n_ships_hit = result.x;
    
	if(result.x == nBoats) { //n_ships_hit == nBoats
	  strcpy(readbuf,"PLAYER 1 WON!\n");
	  write(fd,readbuf,strlen(readbuf)); //the message that player2 has won is passed to player2
	  printf("PLAYER 1 WON!\n");
	  destroy_map(m); //the player's map is freed
	  close(fd); //the pipe is closed
	  break;
	}
    
	header(2,board_size);
	//printf of the opponent's grid
	gridOpponent(opponent,m->size);
	printf("\nAttack coordinates: ");
	scanf("%s", str);
	check_input(str,1,board_size);
	attack[0] = atoi(str);
	attack[0]--;
	x = attack[0];
	scanf("%s", str);
	check_input(str,1,board_size);
	attack[1] = atoi(str);
	attack[1]--;
	y = attack[1];
	first = 1; //player2 has done his/her attack
	char buffer[strlen(readbuf)];
	sprintf(buffer,"%d %d %d",attack[0],attack[1],result.y);
	//the player's attack information and the result of the opponent's attack are passed to the opponent
	write(fd,buffer,strlen(buffer));
	// sleep - this is to make sure other process reads this, otherwise this process would retrieve the message
	sleep(10);
      }
    
    }
    
  } else if(mode == 3) { //semaphores

    printf("\nPLAYER: ");
    scanf("%d",&player);

    while(player!=1 && player!=2) {
      printf("Please insert player 1 or 2 only.\n\nPLAYER: ");
      scanf("%d",&player);
    }

    for(int i=0; i<40; i++) {
	for(int j=0; j<40; j++)
	  opponent[i][j] = '.';
    }

    if(player == 1) {

      FILE* fd;
      int resultY;
      int xBomb,yBomb;
      Point result;
      map* m1;
      int n_ships_hit1=0;

      int semaphore_id; //semaphore for archive
      if((semaphore_id=semget(1,1,IPC_CREAT))==-1)
	puts("error");
      int semaphore_control_m1; //create semaphore for player1
      if((semaphore_control_m1=semget(2,1,IPC_CREAT))==-1)
	puts("error");
      int semaphore_control_m2; //create semaphore for player2
      if((semaphore_control_m2=semget(3,1,IPC_CREAT))==-1)
	puts("error");
      semctl(semaphore_control_m1,0,SETVAL,0); //green light for player1

      printf("Board size wanted(between 20 and 40): ");
      scanf("%s", str);
      check_input(str,20,40);
      board_size = atoi(str);

      max_boats = (board_size*board_size)/(5*5);
      printf("Min number of boats = 5\n");
      printf("Max number of boats = %d\n", max_boats);

      printf("Number of boats: ");
      scanf("%s", str);
      check_input(str,5,max_boats);
      nBoats = atoi(str);

      printf("\n");
      rules();
      printf("\n");
      
      printf("\nPLAYER1 - Manual(0) or random(1)? ");
      scanf("%s", str);
      check_input(str,0,1);
      option = atoi(str);
	
      m1 = new_map(board_size);
      setBoardUser(option,nBoats,1,m1);

      fd = fopen(path_1,"w"); //writes information for opponent
      fprintf(fd,"%d %d",board_size,nBoats);
      fclose(fd);
      semctl(semaphore_control_m1,0,SETVAL,1); //red light for player1

      //while player2 doesn't finish his/her move, the player1 isn't allowed to continue
      while(semctl(semaphore_control_m2, 0,GETVAL) != 1) {
	puts("Waiting for PLAYER2...");
	sleep(2);
      }

      semctl(semaphore_id,0,SETVAL,1); //it's player1 who does the first attack
      int flag = 0;

      while(1) { 

	if(semctl(semaphore_id,0,GETVAL) == 1) { //player1's turn
	  if(flag == 0) { //first attack
	    header(1,board_size);
	    sleep(1);
	    gridOpponent(opponent,board_size);

	    printf("\n\nAttack coordinates: ");
	    scanf("%s", str);
	    check_input(str,1,board_size);
	    xBomb = atoi(str);
	    scanf("%s", str);
	    check_input(str,1,board_size);
	    yBomb = atoi(str);
	    xBomb--;
	    yBomb--;
		    
	    fd = fopen(path_1,"w"); //writes attack for opponent
	    fprintf(fd,"%d %d",xBomb,yBomb);
	    fclose(fd);
	    flag++; //first attack occured already
	    semctl(semaphore_id,0,SETVAL,2); //signal for player2
		    
	    while(semctl(semaphore_id,0,GETVAL) != 1) { //waits for opponent's answer
	      printf("Waiting for the answer...");
	      sleep(2);
	    }

	    fd = fopen(path_1,"r"); //reads the result of the attack done by the player
	    fscanf(fd,"%d",&resultY);
	    fclose(fd);

	    //receives the result of the attack he/she made towards the opponent
	    if(resultY==0) {
	      printf("YOU HIT!\n");
	      opponent[xBomb][yBomb] = '@';
	    } else if(resultY==1) {
	      printf("THIS POSITION HAS ALREADY BEEN ATTACKED BEFORE...\n");
	    } else if(resultY == 2) {
	      printf("SORRY YOU MISSED...\n");
	      opponent[xBomb][yBomb] = 'X';
	    } else {
	      printf("YOU WON!!");
	      destroy_map(m1); //frees the player's map
	      break;
	    }

	    semctl(semaphore_id,0,SETVAL,2); //player2's turn

	  } else { //it isn't the first attack

	    fd = fopen(path_1,"r"); //reads the attack done by the opponent
	    fscanf(fd,"%d %d",&xBomb,&yBomb);
	    result = dropBomb(m1,xBomb,yBomb,n_ships_hit1); //result of the attack
	    fclose(fd);
	    n_ships_hit1 = result.x;
	    
	    if(result.x == nBoats) { //n_ships_hit == nBoats
	      
	      int r = 3;
	      fd = fopen(path_1,"w");
	      fprintf(fd,"%d",r);
	      fclose(fd);
	      printf("PLAYER 2 WON!\n");
	      destroy_map(m1); //frees the player's map
	      break;
	      
	    }

	    //writes to opponent the result of the attack done 
	    fd = fopen(path_1,"w");
	    fprintf(fd,"%d",result.y);
	    fclose(fd);
	    
	    semctl(semaphore_id,0,SETVAL,2); //signal for player2
	    while(semctl(semaphore_id,0,GETVAL) != 1) {
	      puts("Wait for your turn...");
	      sleep(2);    
	    }

	    header(1,board_size);
	    sleep(1);
	    gridOpponent(opponent,board_size);
	    printf("\n\nAttack coordinates: ");
	    scanf("%s", str);
	    check_input(str,1,board_size);
	    xBomb = atoi(str);
	    scanf("%s", str);
	    check_input(str,1,board_size);
	    yBomb = atoi(str);
	    xBomb--;
	    yBomb--;

	    //writes attack to the opponent
	    fd = fopen(path_1,"w");
	    fprintf(fd,"%d %d",xBomb,yBomb);
	    fclose(fd);

	    semctl(semaphore_id,0,SETVAL,2); //signal for player2
	    while(semctl(semaphore_id,0,GETVAL) != 1) {
	      printf("Waiting for the answer...");
	      sleep(2);
	    }

	    fd = fopen(path_1,"r");

	    fscanf(fd,"%d",&resultY);
	    fclose(fd);

	    //receives the result of the attack he/she made towards the opponent
	    if(resultY == 0) {
	      printf("YOU HIT!\n");
	      opponent[xBomb][yBomb] = '@';
	    } else if(resultY == 1) {
	      printf("THIS POSITION HAS ALREADY BEEN ATTACKED BEFORE...\n");
	    } else if(resultY == 2){
	      printf("SORRY YOU MISS...\n");
	      opponent[xBomb][yBomb] = 'X';
	    } else {
	      printf("YOU WON!!");
	      destroy_map(m1); //frees the player's map
	      break;
	    }
		    
	    semctl(semaphore_id,0,SETVAL,2);
	  }
	}
      }

      //removal of semaphores after the end of the game
      if (semctl (semaphore_id, 0, IPC_RMID) == -1) {
	puts ("semctl IPC_RMID");
      }
      if (semctl (semaphore_control_m2, 0, IPC_RMID) == -1) {
	puts ("semctl IPC_RMID");
      }
      if (semctl (semaphore_control_m1, 0, IPC_RMID) == -1) {
	puts ("semctl IPC_RMID");
      }
	
    } else {

      FILE* fd;
      int resultY;
      int xBomb,yBomb;
      int n_ship_hits2 = 0;
      Point result;
      map* m2;

      int semaphore_id; //semaphore for archive
      if((semaphore_id = semget(1,1,IPC_CREAT))==-1)
	puts("error");
      int semaphore_control_m1; //create semaphore for player1
      if((semaphore_control_m1=semget(2,1,IPC_CREAT))==-1)
	puts("error");
      int semaphore_control_m2; //create semaphore for player2
      if((semaphore_control_m2=semget(3,1,IPC_CREAT))==-1)
	puts("error");
      semctl(semaphore_control_m2,0,SETVAL,0); //green light for player2

      printf("\n");
      rules();
      printf("\n");

      //reads the set up information provided by the player1
      fd = fopen(path_1,"r");

      fscanf(fd,"%d",&board_size);
      fscanf(fd,"%d",&nBoats);

      printf("Board size: %d\n",board_size);
      printf("Number of boats: %d\n",nBoats);
      printf("\nPLAYER2 - Manual(0) or random(1)? ");
      scanf("%s", str);
      printf("\n");
      check_input(str,0,1);
      option = atoi(str);
      m2 = new_map(board_size);
      setBoardUser(option,nBoats,2,m2);

      semctl(semaphore_control_m2,0,SETVAL,1); //red light for player2 

      while(1) {

	if(semctl(semaphore_id,0,GETVAL) == 2) {

	  //reads the attack done by the opponent
	  fd=fopen(path_1,"r");
	  fscanf(fd,"%d %d",&xBomb,&yBomb);
	  result = dropBomb(m2,xBomb,yBomb,n_ship_hits2); //result of the attack
	  fclose(fd);
	  n_ship_hits2 = result.x;

	  if(result.x == nBoats) { //n_ships_hit == nBoats

	    int r = 3;
	    fd = fopen(path_1,"w");
	    fprintf(fd,"%d",r);
	    fclose(fd);
	    printf("PLAYER 1 WON!\n");
	    destroy_map(m2); //frees the player's map
	    break;
	    
	  }

	  //writes to opponent the result of the attack done
	  fd = fopen(path_1,"w");
	  fprintf(fd,"%d",result.y);
	  fclose(fd);

	  semctl(semaphore_id,0,SETVAL,1); //signal for player1
	  while(semctl(semaphore_id,0,GETVAL) != 2) {
	    puts("Wait for your turn...");
	    sleep(2);
	  }

	  header(2,board_size);
	  sleep(1);
	  gridOpponent(opponent,board_size);
	  printf("\n\nAttack coordinates: ");
	  scanf("%s", str);
	  check_input(str,1,board_size);
	  xBomb = atoi(str);
	  scanf("%s", str);
	  check_input(str,1,board_size);
	  yBomb = atoi(str);
	  xBomb--;
	  yBomb--;

	  //writes attack to the opponent
	  fd = fopen(path_1,"w");
	  fprintf(fd,"%d %d",xBomb,yBomb);
	  fclose(fd);

	  semctl(semaphore_id,0,SETVAL,1); //signal for player1
	  while(semctl(semaphore_id,0,GETVAL)!=2) {
	    printf("Waiting for the answer...\n");
	    sleep(2);
	  }

	  fd = fopen(path_1,"r");

	  fscanf(fd,"%d",&resultY);
	  fclose(fd);

	  //receives the result of the attack he/she made towards the opponent
	  if(resultY == 0) {
	    printf("YOU HIT!\n");
	    opponent[xBomb][yBomb] = '@';
	  } else if(resultY == 1) {
	    printf("THIS POSITION HAS ALREADY BEEN ATTACKED BEFORE...\n");
	  } else if(resultY == 2) {
	    printf("SORRY YOU MISS...\n");
	  } else {
	    printf("YOU WON!!");
	    destroy_map(m2); //frees the player's map
	    break;
	  }
		
	  semctl(semaphore_id,0,SETVAL,1);

	}
      }

      //removal of semaphores after the end of the game
      if (semctl (semaphore_id, 0, IPC_RMID) == -1) {
	puts ("semctl IPC_RMID");
      }
      if (semctl (semaphore_control_m2, 0, IPC_RMID) == -1) {
	puts ("semctl IPC_RMID");
      }
      if (semctl (semaphore_control_m1, 0, IPC_RMID) == -1) {
	puts ("semctl IPC_RMID");
      }

    }
  }
  
  return 0;
  
}
