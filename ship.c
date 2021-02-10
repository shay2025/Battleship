#include <stdio.h>
#include <stdlib.h>

#include "ship.h"

int dim(int type) {
 
  switch (type) {
  case 1: //Carrier
    return 5;
    break;
  case 2: //Battleship
    return 4;
    break;
  case 3: //Cruiser
    return 3;
    break;
  case 4: //Submarine
    return 3;
    break;
  case 5: //Destroyer
    return 2;
    break;
  default:
    return -1;
    break;
  }
  
}

ship* create(int type) {
  
  ship* s = (ship*)malloc(sizeof(ship));
  int i, j;
  int n = dim(type);
  
  //inicializes all positions of the bitmap to empty ('0') 
  for(i=0; i<5; i++)
    for(j=0; j<5; j++)
      s->bitmap[i][j] = '0';
  
  //given a type, the bitmap's ship is filled in the middle
  //by default, the ships are placed horizontally on its bitmap
  if(type != 2) { //line
    
      for(i=0; i<n; i++)
	s->bitmap[2][i] = '1';
      
  } else { //L shape 
    
      for(i=0; i<n-1; i++)
	  s->bitmap[i][2]= '1';
      
      s->bitmap[i-1][3]= '1';
      
  }
  
  s->type = type;
  s->dimention = n;
  
  return s;
  
}
  
void rotate(int dir, ship* s) {
  
  int i, j, n=5;
  s->dir = dir;

  if(dir == 90) { //vertical
    
      for(i=0; i<n/2; i++)
	for(j=i; j<n-i-1; j++) {
	  
	    char temp = s->bitmap[i][j];
	    s->bitmap[i][j] = s->bitmap[n-1-j][i];
	    s->bitmap[n-1-j][i] = s->bitmap[n-1-i][n-1-j];
	    s->bitmap[n-1-i][n-1-j] = s->bitmap[j][n-1-i];
	    s->bitmap[j][n-1-i] = temp;
	    
	}
      
  } else if(dir == 180) { //horizontal
    
    for(i=0; i<n; i++){
      for(j=0; j<i; j++){
	
	char temp = s->bitmap[i][j];
	s->bitmap[i][j] = s->bitmap[j][i];
	s->bitmap[j][i] = temp;
	
      }
    }
      
  } else if(dir == 270) { //vertical
    
    for(i=0; i<n/2; i++) {
      for(j=i; j<n-i-1; j++) {
	    
	char temp = s->bitmap[i][j];
	s->bitmap[i][j] = s->bitmap[j][n-i-1];
	s->bitmap[j][n-i-1] = s->bitmap[n-1-i][n-1-j];
	s->bitmap[n-1-i][n-1-j] = s->bitmap[n-1-j][i];
	s->bitmap[n-1-j][i] = temp;
	      
      }
    }
      
  } else
    return; //0º horizontal
 
}

int Did_The_Ship_Sink(ship* s) {
  
  s->dimention--; //since a position of the ship has been hit its dimention decreases
  
  if(s->dimention == 0) return 1;
  
  return 0;
  
}

int validDirection(int dir) {

  switch(dir) {
  case 0:
    return 0;
    break;
  case 90:
    return 0;
    break;
  case 180:
    return 0;
    break;
  case 270:
    return 0;
    break;
  default:
    return -1;
    break;
  }
  
}
