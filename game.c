#include <stdio.h>
#include <stdlib.h>
#include "game.h"


void grid_print(map* m) {
  printf("  ");
  if(m->size < 10) printf("%d ",1);
  else printf(" %2d ",1);
  for(int i=2; i<=m->size; i++)
    printf("%2d ",i);
  printf("\n");
  
  #ifdef QUAD
  for(int i=0; i<m->size; i++) {
    Point p;
    p.x = i;
    //printf(" ");
    if(i+1<10) printf(" %d ",i+1);
    else printf("%d ",i+1);
    for(int j=0; j<m->size; j++) {
      p.y = j;
      if(search(m->matrix,p) != NULL)
	printf(" X ");
      else 
	printf(" . ");
    }
    printf("\n");
  }
   
  #else
  for(int i=0; i<m->size; i++) {
    //printf(" ");
    if(i+1<10) printf(" %d ",i+1);
    else printf("%d ",i+1);
    for(int j=0; j<m->size; j++) {
      if(m->matrix[i][j] != NULL)
	printf(" X ");
      else 
	printf(" . ");
    }
    printf("\n");
  }
    
  #endif

}

void grid_print_attack(map* m) {

  printf("  ");
  if(m->size < 10) printf("%d ",1);
  else printf(" %2d ",1);
  for(int i=2; i<=m->size; i++)
    printf("%2d ",i);
  printf("\n");
  #ifdef QUAD
  for(int i=0; i<m->size; i++) {
    Point p;
    p.x = i;
    if(i+1<10) printf(" %d ",i+1);
    else printf("%d ",i+1);
    for(int j=0; j<m->size; j++) {
      p.y = j;
      if(search(m->matrix,p) != NULL && search(m->matrix,p)->data->shot_count > 0)
	printf(" @ ");
      else printf(" . ");
    }
    printf("\n");
  }
  #else
  for(int i=0; i<m->size; i++) {
    if(i+1<10) printf(" %d ",i+1);
    else printf("%d ",i+1);
    for(int j=0; j<m->size; j++) {
      if(m->matrix[i][j] != NULL && m->matrix[i][j]->shot_count > 0)
	printf(" @ ");
      else {
	printf(" . ");
      }
    }
    printf("\n");
  }
  #endif
}

void setBoardUser(int option, int numberOfBoats, int player, map* m) {
  
  int x, y, dir, ships, typeShip;
  char *str_row = (char*)malloc(2*sizeof(char));;
  char* str_column = (char*)malloc(2*sizeof(char));;
  char* str_dir = (char*)malloc(2*sizeof(char));
  ships = numberOfBoats;
  
  if(option == 0) { //manual insertion
    
    printf("=======================================================\n");
    printf("---------------------PLAYER%d turn---------------------\n",player);
    printf("=======================================================\n");
    printf("\n");
    printf("SHIP TYPES\n");
    printf("1 - Carrier (size 5)\n");
    printf("2 - Battleship (size 4)\n");
    printf("3 - Cruiser (size 3)\n");
    printf("4 - Submarine (size 3)\n");
    printf("5 - Destroyer (size 2)\n");

    printf("\n");

    printf("POSSIBLE DIRECTIONS\n");
    printf(".....   .....   .....   .....\n");
    printf("..1..   .....   .11..   ...1.\n");
    printf("..1..   .111.   ..1..   .111.\n");
    printf("..11.   .1...   ..1..   .....\n");
    printf(".....   .....   .....   .....\n");
    printf("  0       90     180     270 \n");
    printf("\n");

    //makes sure that all 5 distinct ship types are inserted
    for(int i=1; i<=5; i++) {

      grid_print(m);
      
      printf("\n");
      printf("Ship type %d: \n",i);
      ship* s = create(i);
      printf("Direction wanted: ");
      scanf("%s",str_dir);

      printf("\n");
      while(!checks_number(str_dir) || validDirection(atoi(str_dir)) == -1) {
	printf("INVALID DIRECTION | Direction wanted: ");
	scanf("%s", str_dir);
      }
      
      dir = atoi(str_dir);
      rotate(dir,s);
      printf("LINE AND COLUM (between 1 and %d): ", m->size);
      scanf("%s",str_row);
      scanf("%s",str_column);

      //since player inserts coordinates between 1 and board_size, and the matrix boundaries are between 0 and board_size-1
      while((atoi(str_row) < 1 || atoi(str_row) > m->size)) {
	printf("%d\n",atoi(str_row));
	printf("CAN NOT INSERT | OTHER LINE AND COLUM: "); 
	scanf("%s",str_row);
      }

      while(atoi(str_column) < 1 || atoi(str_column) > m->size) {
	printf("%d\n",atoi(str_column));
	printf("CAN NOT INSERT | OTHER LINE AND COLUM: ");
	scanf("%s",str_column);
      }
      
      x = atoi(str_row)-1;
      y = atoi(str_column)-1;
      
      setship(s,x,y,dir,m,option);
      ships--;
      
    }

    printf("\n");
    while(ships != 0) {

      grid_print(m);
      printf("\n");
      printf("Ship type wanted: ");
      scanf("%d", &typeShip);
      
      while(dim(typeShip) == -1) {
	printf("INVALID TYPE | Ship type wanted: ");
	scanf("%d", &typeShip);
      }
      
      ship* s = create(typeShip);
      
      printf("Direction wanted: ");
      scanf("%s",str_dir);
      
      while( !checks_number(str_dir) || validDirection(atoi(str_dir)) == -1) {
	printf("INVALID DIRECTION | Direction wanted: ");
	scanf("%s", str_dir);
      }
      
      dir = atoi(str_dir);
      rotate(dir,s);
      
      printf("LINE AND COLUM (between 1 and %d): ", m->size);
      scanf("%s",str_row);
      scanf("%s",str_column);
      
      while(atoi(str_row) < 1 || atoi(str_row) > m->size) {
	  printf("CAN NOT INSERT | OTHER LINE AND COLUM: ");
	  scanf("%s",str_row);
      }

      while(atoi(str_column) < 1 || atoi(str_column) > m->size) {
	  printf("CAN NOT INSERT | OTHER LINE AND COLUM: ");
	  scanf("%s",str_column);
      }
      
      x = atoi(str_row)-1;
      y = atoi(str_column)-1;
      
      setship(s,x,y,dir,m,option);
      ships--;
      
      printf("\n");
      
    }
    
    grid_print(m);

    free(str_column);
    free(str_row);
    free(str_dir);
    
  } else { //random insertion
    
    int dir_aux;

    //makes sure that the all types are in the player's game
    for(int i=1;i<=5;i++) {
       
      typeShip = i;
      
      ship* s = create(typeShip);
      dir_aux = rand() % 4 + 1;
	  
      if(dir_aux == 1) dir = 0;
      else if(dir_aux == 2) dir = 90;
      else if(dir_aux == 3) dir = 180;
      else dir = 270;
      
      rotate(dir,s);
      //generates the coordinates (x,y) from 0 to m->size-1 each
      x = rand() % m->size;
      y = rand() % m->size;
      
      setship(s,x,y,dir,m,option);
      ships--;

    }

    //after all ship types are in the player's game insert remaining ships if the number
    //of ships chosen is bigger than 5
    while( ships != 0) {
      
      typeShip = rand() % 5 +1;
      ship* s = create(typeShip);
      dir_aux = rand() % 4 + 1;
	  
      if(dir_aux == 1) dir = 0;
      else if(dir_aux == 2) dir = 90;
      else if(dir_aux == 3) dir = 180;
      else dir = 270;
      
      rotate(dir,s);
      ships--;
	

      //generates the coordinates (x,y) from 0 to m->size-1 each
      x = rand() % m->size;
      y = rand() % m->size;
      
      setship(s,x,y,dir,m,option);

    }
    grid_print(m);
  }
  
}

Point dropBomb(map* m, int x, int y, int n_ship_hit) {
  
  int temp = 0;
  #ifdef QUAD
  Point attack;
  attack.x = x; attack.y = y;
  Node* c = search(m->matrix,attack);
  int shots;
  if(c != NULL) shots = search(m->matrix,attack)->data->shot_count;
  #else
  cell* c = m->matrix[x][y];
  int shots;
  if(c != NULL) shots = m->matrix[x][y]->shot_count;
  #endif

  Point result;

  //checks if given coordinates have already been used for a previous attack
  for(int k=0; k<m->posMissedShots+1; k++) {
    if(m->missedShots[k].x == x && m->missedShots[k].y == y) {
      printf("THIS POSITION HAS ALREADY BEEN ATTACKED\n");
      printf("\n");
      result.x = n_ship_hit;
      result.y = 1;
      return result;
    } 
  }
  
  //checks if the given coordinates correspond to a ship's piece, and if so checks if that piece hasn't been hit already
  if(c != NULL && shots == 0) { 

    #ifdef QUAD
    int i = search(m->matrix,attack)->data->xBitmap; //stores the x coordinate of the ship's bitmap that has a piece in the given x attack coordinate
    int j = search(m->matrix,attack)->data->yBitmap; //stores the y coordinate of the ship's bitmap that has a piece in the given y attack coordinate
    ship* sh = search(m->matrix,attack)->data->s;
    #else
    int i = m->matrix[x][y]->xBitmap; //stores the x coordinate of the ship's bitmap that has a piece in the given x attack coordinate
    int j = m->matrix[x][y]->yBitmap; //stores the y coordinate of the ship's bitmap that has a piece in the given y attack coordinate
    ship* sh = m->matrix[x][y]->s;
    #endif
      
    printf("HIT!\n");
    printf("\n");
    temp = Did_The_Ship_Sink(sh);
    result.y = 0;
      
    if(temp == 1) {
      n_ship_hit++; //increments the number of ships sank since a ship has sank
      printf("SHIP SANK!\n");
      printf("\n");
    }

    #ifdef QUAD
    search(m->matrix,attack)->data->s->bitmap[i][j] = '2'; //marks on the ship's bitmap that it has been hit ('2')
    search(m->matrix,attack)->data->shot_count++; //increments the shot_counts of the piece hit
    #else
    m->matrix[x][y]->s->bitmap[i][j] = '2'; //marks on the ship's bitmap that it has been hit ('2')
    m->matrix[x][y]->shot_count++; //increments the shot_counts of the piece hit
    #endif
    
  } else if (c != NULL && shots > 0) { //checks if the given coordinates correspond to ship's piece
    //and if so checks if that piece has been hit already
    
    printf("THIS POSITION HAS ALREADY BEEN HIT\n");
    printf("\n");
    result.y = 1;

    m->misses++; //increases the missed shots
    
  } else {

    m->missedShots[m->posMissedShots].x = x; //stores the given x attack coordinate into the x of the missedShots[count]
    m->missedShots[m->posMissedShots].y = y; //stores the given x attack coordinate into the x of the missedShots[count]
    m->posMissedShots++; //increments the missedShots position so that if the next attack is a miss it is stored in the next position of the missedShots

    m->misses++; //increases the missed shots
    
    printf("MISS!\n");
    printf("\n");
    result.y = 2;
    
  }

  result.x = n_ship_hit;
 
  return result;
  
}
