#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cell.h"

map* new_map(int size) {
  
  map* m = (map*)malloc(sizeof(map));
  
  #ifdef QUAD
  Point start,end;
  start.x = -1;
  start.y = -1;
  end.x = size;
  end.y = size;
  m->matrix = newQuad(start,end);
  #else
  m->matrix = (cell***)malloc(size*sizeof(cell**));
  for(int i=0; i<size; i++)
      m->matrix[i] = (cell**)malloc(size * sizeof(cell*));
  for(int i=0; i<size; i++)
    for(int j=0; j<size; j++)
      m->matrix[i][j] = NULL; //inicializes all positions in the matrix to empty(NULL)
  #endif
  
  m->size = size;
  m->misses = 0;
  m->posMissedShots = 0;
  return m;
  
}

//CRIEI ESTAS FUNÇÕES PQ N PODE HAVER ACESSO DIRETO A VALORES DA HEAP PORQUE PD GERAR MEMORY LEAK E CONSEQUENTEMENTE SEGMENTATION FAULT
#ifdef QUAD
Node* getValueQuad(Quad* q, Point p) {
  return search(q,p);
}
#else
cell* getValueMat(map* m, Point p) {
  return m->matrix[p.x][p.y];
}
#endif

int inLimits(map* m, int x, int y) {
  return (x >= 0 && x < m->size && y >= 0 && y < m->size);
}

void destroy_map(map* m) {

  #ifdef QUAD
  quadtree_free(m->matrix); //destroys the matrix created dynamically
  #else
  for(int i=0; i<m->size; i++) {
    for(int j=0; j<m->size; j++)
      free(m->matrix[i][j]); //destroys the struct cell created dynamically
    free(m->matrix[i]); //destroys the array of struct cells created dynamically
  }
  free(m->matrix); //destroys the matrix created dynamically
  #endif
  free(m); //destroys the map created dynamically
}

int canInsert(ship* s, int x, int y, int dir, map* m) {
  //since the given coordinates correspond to the ship's center
  //we start the search on the middle of the ship's bitmap
  for(int i=0; i<3; i++){
    if(!inLimits(m,x+i,y) || !inLimits(m,x-i,y) || !inLimits(m,x,y-i) || !inLimits(m,x,y+i)) return 0;
    Point p1,p2,p3,p4;
    p1.x = x+i; p1.y = y;
    p2.x = x-i; p2.y = y;
    p3.x = x; p3.y = y-i;
    p4.x = x; p4.y = y+i;
    #ifdef QUAD
    Node* n1 = getValueQuad(m->matrix,p1);
    Node* n2 = getValueQuad(m->matrix,p2);
    Node* n3 = getValueQuad(m->matrix,p3);
    Node* n4 = getValueQuad(m->matrix,p4);
    #else
    cell* n1 = getValueMat(m,p1);
    cell* n2 = getValueMat(m,p2);
    cell* n3 = getValueMat(m,p3);
    cell* n4 = getValueMat(m,p4);
    #endif

    if(s->bitmap[2+i][2] == '1' && n1 != NULL) //checks down half when ship is placed vertically
      return 0;

    if(s->bitmap[2-i][2] == '1' && n2 != NULL) //checks upper half when ship is placed vertically
      return 0;

    if(s->bitmap[2][2-i] == '1' && n3 != NULL) //checks upper half when ship is placed horizontally
      return 0;

    if(s->bitmap[2][2+i] == '1' && n4 != NULL) //checks down half when ship is placed horizontally
      return 0;
    
  }
  
  return 1;
 
}

int checks_number(char str[]) {
  for(int i=0; i<strlen(str); i++)
    if(isdigit(str[i]) != 0) return 1;
  return 0;
}

void setship(ship* s, int x, int y, int dir,  map* m, int option) {
  
  int insertion = canInsert(s,x,y,dir,m);
  int i;
  char str_row[2], str_column[2];
  
  if(insertion == 1) {

    //since the given coordinates correspond to the ship's center
    //we start the search on the middle of the ship's bitmap
    for(i=0; i<3; i++) {

      #ifdef QUAD
      Point p1,p2,p3,p4;
      p1.x = x+i; p1.y = y;
      p2.x = x-i; p2.y = y;
      p3.x = x; p3.y = y-i;
      p4.x = x; p4.y = y+i;
      #endif
	      
      if(s->bitmap[2-i][2] == '1') {  //checks upper half when ship is placed vertically

	cell* c = (cell*)malloc(sizeof(cell)); //creates a cell that will reference a ship's piece
	c->s = s;
	c->shot_count = 0;
	c->xBitmap = i;
	c->yBitmap = 2;
	#ifdef QUAD
	Node* n1 = newNode(p2,c);
	insert(m->matrix,n1);
	#else
	m->matrix[x-i][y] = c;
	#endif
	    
      }

      if(s->bitmap[2+i][2] == '1') {  //checks down half when ship is placed vertically

	cell* c = (cell*)malloc(sizeof(cell)); //creates a cell that will reference a ship's piece
	c->s = s;
	c->shot_count = 0;
	c->xBitmap = i;
	c->yBitmap = 2;
	#ifdef QUAD
	Node* n2 = newNode(p1,c);
	insert(m->matrix,n2);
	#else
	m->matrix[x+i][y] = c;
	#endif
	    
      }

      if(s->bitmap[2][2-i] == '1') {  //checks upper half when ship is placed horizontally

	cell* c = (cell*)malloc(sizeof(cell)); //creates a cell that will reference a ship's piece
	c->s = s;
	c->shot_count = 0;
        c->xBitmap = 2;
	c->yBitmap = i;
	#ifdef QUAD
	Node* n3 = newNode(p3,c);
	insert(m->matrix,n3);
	#else
	m->matrix[x][y-i] = c;
	#endif
      }
   
      if(s->bitmap[2][2+i] == '1') {  //checks down half when ship is placed horizontally

	cell* c = (cell*)malloc(sizeof(cell)); //creates a cell that will reference a ship's piece
	c->s = s;
	c->shot_count = 0;
        c->xBitmap = 2;
	c->yBitmap = i;
	#ifdef QUAD
	Node* n4 = newNode(p4,c);
	insert(m->matrix,n4);
	#else
	m->matrix[x][y+i] = c;
	#endif
      }
      
    }
    
  } else {
    
    while(insertion != 1) { //in case the ship can't be placed on the giver coordinates
	
      if(option == 0) { //if it was chosen a manual matrix
	
	printf("CAN NOT INSERT | ");
	printf("OTHER LINE AND COLUM: ");
	scanf("%s",str_row);
	scanf("%s",str_column);
	//since player inserts coordinates between 1 and board_size
	//and the matrix boundaries are between 0 and board_size-1
	while(!checks_number(str_row) && (atoi(str_row) < 1 || atoi(str_row) >= m->size)) {
	  printf("CAN NOT INSERT | OTHER LINE AND COLUM: ");
	  scanf("%s",str_row);
	}

	while(!checks_number(str_column) && (atoi(str_column) < 1 || atoi(str_column) >= m->size)) {
	  printf("CAN NOT INSERT | OTHER LINE AND COLUM: ");
	  scanf("%s",str_column);
	}
      
	x = atoi(str_row)-1;
	y = atoi(str_column)-1;
	insertion = canInsert(s,x,y,dir,m);
	  
      } else { //if it was chosen a random matrix

	//generates the coordinates (x,y) from 0 to m->size-1 each
        x = rand() % m->size;
	y = rand() % m->size;

	insertion = canInsert(s,x,y,dir,m);
      
      }
	  
    }
      
    setship(s,x,y,dir,m,option);
      
  }
  
}
