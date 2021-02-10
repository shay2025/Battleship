#include "quadTree.h"

#define MAX_MISSES 100

typedef struct {

  #ifdef QUAD
  Quad* matrix;
  #else
  cell*** matrix;
  #endif
  int size;
  Point missedShots[MAX_MISSES]; //stores the coordinates of missed shots
  int misses; //counts the missed shots
  int posMissedShots; //keeps track of the current position in missedShots[]
  
} map;

map* new_map(int); //creates a map for each player that contains a matrix of cells and its attributes
void destroy_map(map*); //destroys everything that was created by dynamic memory
int inLimits(map*,int,int);
int canInsert(ship*,int,int,int,map*); //checks if a ship with given coordinates can be placed in the matrix
int checks_number(char*);
void setship(ship*,int,int,int,map*,int); //places the ship in the matrix
cell* getValueMat(map*,Point);
Node* getValueQuad(Quad*,Point);
