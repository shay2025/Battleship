#include "ship.h"

typedef struct {
  
  int x;
  int y;
  
}Point;

typedef struct {
  
  ship* s;
  int shot_count; //counts the number of shots of a ship's piece
  int xBitmap; //stores the x coordinate of the bitmap
  int yBitmap; //stores the y coordinate of the bitmap
  
} cell;

typedef struct {
  
  Point pos;
  cell* data;
  
}Node;

typedef struct Quad Quad;

struct Quad {
  
  Point topLeft;
  Point botRight;
  Node *n;
  Quad *topLeftTree;
  Quad *topRightTree;
  Quad *botLeftTree;
  Quad *botRightTree;
  
};

Quad* newQuad(Point,Point); //creates a quadtree
Node* newNode(Point,cell*); //creates a node
void insert(Quad*,Node*); 
Node* search(Quad*,Point);
int inBoundary(Point,Point,Point); //checks if point given are inside the boundaries that are also the arguments in the function
void quadtree_free(Quad*); //frees the memory occupied by the quadtrees
