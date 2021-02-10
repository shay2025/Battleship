#include "cell.h"

void grid_print(map*);
void grid_print_attack(map*);
void setBoardUser(int,int,int,map*); //occurs the placement of ships on the player's board
Point dropBomb(map*,int,int,int); //receives coordinates of attack and changes matrix and other attributes as appropriate
