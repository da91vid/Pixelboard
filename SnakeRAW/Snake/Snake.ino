#include <JS.h>
#include <FastLED.h>
#include "coordinates.h"
#include <LEDMatrix.h>

#define LED_PIN        32
#define LED_PIN2       33
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define MATRIX_WIDTH   32  // Set this negative if physical led 0 is opposite to where you want logical 0
#define MATRIX_HEIGHT  -8  // Set this negative if physical led 0 is opposite to where you want logical 0
#define MATRIX_TYPE    VERTICAL_ZIGZAG_MATRIX  // See top of LEDMatrix.h for matrix wiring types

int w8_cycle;
int get_x = 0;
int get_y = 0;
int x = 15;
int y = 7;
int x_fruit = random(1,31),y_fruit = random(1,15);
int direction_ = 5;
int lenght_snake =8;
bool collision = false;
coordinates cords_[420];

//coordinates cords(1,1);



cLEDMatrix<-MATRIX_WIDTH, -MATRIX_HEIGHT, MATRIX_TYPE> leds;
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds2;

uint8_t angle = 0;
JS joystick(27,26,25);


void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds2[0], leds2.Size());
  FastLED.setBrightness(100);
  FastLED.clear(true);
  Serial.begin(9600);


  FastLED.show();
  delay(5000);
  leds.ShiftLeft();
  
   for (int i = 0; i< 420; i++){
    cords_[i]=coordinates(16,8);
  }

  cords_[0]=coordinates(16,8);
  

}

void loop() 
{
  get_y = joystick.getX();
  get_x = joystick.getY();

  if(get_x > 3000 && direction_ != 2 ) direction_ = 0;
  else if(get_x < 1200 && direction_ != 0) direction_ = 2;
  else if(get_y > 3000 && direction_ != 1) direction_ = 3;
  else if(get_y < 1200 && direction_ != 3) direction_ =1;

  switch(direction_)
  {
    case 0: if(x >0) x--; break;
    case 1: if(y <15) y++; break;
    case 2: if(x <31) x++; break;
    case 3: if(y > 0) y--; break;
  }

  Serial.print("x: ");
  Serial.print(x);
  Serial.print("y: ");
  Serial.println(y);
  
  for (int i = 0; i < lenght_snake;i++)
  {
    cords_[lenght_snake-i-1] = cords_[lenght_snake-i-2];
  }
  
  cords_[0] = coordinates(x,y);

  for (int i = 0; i < lenght_snake;i++)
  {
    drawPoint(cords_[i].get_coordinates_X(), cords_[i].get_coordinates_Y());
  }

  drawPoint(x_fruit,y_fruit);

  if(cords_[0].get_coordinates_X() == x_fruit &&cords_[0].get_coordinates_Y() == y_fruit)
  {
    lenght_snake ++;
    x_fruit = random(1,31);
    y_fruit = random(1,15);
  }
  
  if(direction_ != 5){  w8_cycle++;}

  if (w8_cycle >= 10)
  {
    for (int i = 1; i < lenght_snake -1;i++)
    {
      if(cords_[0].get_coordinates_X() == cords_[i].get_coordinates_X()&&cords_[0].get_coordinates_Y() == cords_[i].get_coordinates_Y())
      {
        collision = true;
        Serial.println();
      }
    }
  }
  
  Serial.print(collision);
  
  if(cords_[0].get_coordinates_X() == 0 ||cords_[0].get_coordinates_Y() == 0||cords_[0].get_coordinates_X() == 31 ||cords_[0].get_coordinates_Y() == 15||collision)
  {
    collision = false;
    w8_cycle = 0;

    for (int i = 0; i< 5 ;i++) 
    {
      FastLED.show(); 
      delay(100);
      FastLED.clear();
      delay(100);
      for (int i = 0; i < lenght_snake;i++)
      {
        drawPoint(cords_[i].get_coordinates_X(), cords_[i].get_coordinates_Y());
      } 
    }
   
    cords_[0] = coordinates(15,8);
    x = 15;
    y = 7;
    direction_ = 5;
    lenght_snake =8;
    w8_cycle = 0; 
  }

  FastLED.show();  
  delay(100);
  
  FastLED.clear();
  drawMap();
}

void drawPoint(int x, int y)
{
  if (y > 7) 
  {
    leds2.DrawLine(x, y-8, x, y-8, CRGB(255, 0, 255));
  }else{
    leds.DrawLine(x, y, x, y, CRGB(255, 0, 255));
  }
}

void drawMap()
{
  // Panel 1 Wall
  leds.DrawLine(0, 0, 31, 0, CRGB(255, 0, 0));
  leds.DrawLine(0, 0, 0, 7, CRGB(255, 0, 0));
  leds.DrawLine(31, 0, 31, 7, CRGB(255, 0, 0));

  // Panel 2 Wall
  leds2.DrawLine(0, 7, 31, 7, CRGB(255, 0, 0));
  leds2.DrawLine(0, 0, 0, 7, CRGB(255, 0, 0));
  leds2.DrawLine(31, 7, 31, 0, CRGB(255, 0, 0));
}
