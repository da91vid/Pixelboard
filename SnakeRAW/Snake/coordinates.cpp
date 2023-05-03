#include "coordinates.h"

coordinates::coordinates(int x, int y) 
{
  X=x;
  Y=y;
}
coordinates::coordinates() 
{
  X = 44;
  Y= 44;
}
int coordinates::get_coordinates_Y()
{
  return Y;
}
int coordinates::get_coordinates_X()
{
  return X;
}
