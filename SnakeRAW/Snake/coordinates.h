#ifndef coordinates_h
#define coordinates_h

#include <Arduino.h>

class coordinates {
  private:
    int X = 0;
    int Y = 0;
    

  public:
    coordinates(int x,int y);
    coordinates();
    int get_coordinates_Y();
    int get_coordinates_X();
};

#endif