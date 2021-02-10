typedef struct {
  
  int type;
  int dimention;
  int dir;
  char bitmap[5][5];
  
} ship;

int dim(int); //returns the dimention of a ship given a type
void rotate(int,ship*); //rotates the ship in its bitmap given the degrees and ship to be rotated
ship* create(int); //creates a ship given the type of the ship wanted
int Did_The_Ship_Sink(ship*); //checks if the ship has sank after an attack has occured
int validDirection(int); //checks if the given direction (for rotation) is valid or not
