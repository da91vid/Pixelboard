#include <coordinates.h>
#include <FastLED.h>
#include <JS.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>

#define LED_PIN 32
#define LED_PIN2 33

#define COLOR_ORDER GRB
#define CHIPSET WS2812B

#define MATRIX_WIDTH -32
#define MATRIX_HEIGHT -8
#define MATRIX_TYPE VERTICAL_ZIGZAG_MATRIX

// time to wait
#define waiting 200

// MENU END

// SNAKE START
int w8_cycle;
int get_x = 0;
int get_y = 0;
int x = 15;
int y = 7;
int x_fruit = random(1, 32), y_fruit = random(1, 16);
int direction_ = 5;
int lenght_snake = 8;
bool collision = false;
coordinates cords_[420];

double xb = 6;
double yb = 8;
int py = 6;
int score = 100;
bool way = true;
bool up = true;
double k;
double angle = 50;




#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// define two tasks for Blink & AnalogRead
void TaskBlink(void *pvParameters);
void TaskAnalogReadA3(void *pvParameters);

cLEDMatrix<-MATRIX_WIDTH, -MATRIX_HEIGHT, MATRIX_TYPE> leds;
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds2;

int choice = 0;
int time_now = 0;
int change = 0;

int counter = 0;

unsigned long lastDebounceTime = 0;  // Time of the last joystick movement
unsigned long debounceDelay = 100;


cLEDText ScrollingMsg;

// Flankenerkennung

JS joystick(25, 26, 27);


TaskHandle_t TaskAusgabeA_Handle;
TaskHandle_t TaskAusgabeB_Handle;
TaskHandle_t TaskAusgabeC_Handle;
TaskHandle_t TaskAusgabeD_Handle;

// Snake
TaskHandle_t vTaskCode_Handle;

// Pong
TaskHandle_t gameTask_Handle;

TaskHandle_t TaskHauptprogramm_Handle;



// the setup function runs once when you press reset or power the board
void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds2[0], leds2.Size());

  FastLED.setBrightness(10);
  FastLED.clear(true);
  delay(500);
  FastLED.show();

  // draws the border of our two pixel boards (pixelboard)
  drawMap();
  drawMenu();

  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&leds2, 32, ScrollingMsg.FontHeight() + 1, 0, 2);
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  // Task Definition

  xTaskCreatePinnedToCore(
    TaskHauptprogramm, "TaskHauptprogramm", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskAusgabeA, "TaskAusgabeA", 4096, NULL, 2, &TaskAusgabeA_Handle, ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskAusgabeB, "TaskAusgabeB", 4096, NULL, 2, &TaskAusgabeB_Handle, ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskAusgabeC, "TaskAusgabeC", 4096, NULL, 2, &TaskAusgabeC_Handle, ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskAusgabeD, "TaskAusgabeD", 4096, NULL, 2, &TaskAusgabeD_Handle, ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    vTaskCode, "vTaskCode", 4096, NULL, 2, &vTaskCode_Handle, ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(gameTask, "gameTask", 4096, NULL, 2, &gameTask_Handle, ARDUINO_RUNNING_CORE);
}

void loop() {}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskHauptprogramm(void *pvParameters)  // This is a task.
{
  (void)pvParameters;
  int joystickValue = 0;
  int joystickValueAlt = 0;
  bool steigendeFlanke = false;
  // int yValue = 0;
  int pressed = false;

  vTaskDelay(10);
  for (;;) {
    joystickValue = joystick.getSW();
    steigendeFlanke = (!joystickValueAlt && joystickValue);
    joystickValueAlt = joystickValue;

    // FOR BUTTON PRESS
    if (steigendeFlanke) {
      pressed = true;
    }

    // die ganze Menu switch Logik
    switchMenu();


    switch (counter) {
      case 0:
        vTaskSuspend(TaskAusgabeB_Handle);
        vTaskSuspend(TaskAusgabeC_Handle);
        vTaskSuspend(TaskAusgabeD_Handle);
        vTaskSuspend(vTaskCode_Handle);
        vTaskSuspend(gameTask_Handle);
        vTaskResume(TaskAusgabeA_Handle);

        if (pressed == true) {
          pressed = false;
          vTaskSuspend(TaskAusgabeA_Handle);
          vTaskResume(vTaskCode_Handle);
          vTaskSuspend(TaskHauptprogramm_Handle);
        }
        break;
      case 1:
        vTaskSuspend(TaskAusgabeA_Handle);
        vTaskSuspend(TaskAusgabeC_Handle);
        vTaskSuspend(TaskAusgabeD_Handle);
        vTaskSuspend(vTaskCode_Handle);
        vTaskSuspend(gameTask_Handle);
        vTaskResume(TaskAusgabeB_Handle);

        if (pressed == true) {
          pressed = false;
          vTaskSuspend(TaskAusgabeB_Handle);
          vTaskResume(gameTask_Handle);
          vTaskSuspend(TaskHauptprogramm_Handle);
        }

        break;
      case 2:
        vTaskSuspend(TaskAusgabeA_Handle);
        vTaskSuspend(TaskAusgabeB_Handle);
        vTaskSuspend(TaskAusgabeD_Handle);
        vTaskSuspend(vTaskCode_Handle);
        vTaskSuspend(gameTask_Handle);
        vTaskResume(TaskAusgabeC_Handle);
        break;
      case 3:
        vTaskSuspend(TaskAusgabeA_Handle);
        vTaskSuspend(TaskAusgabeB_Handle);
        vTaskSuspend(TaskAusgabeC_Handle);
        vTaskSuspend(vTaskCode_Handle);
        vTaskSuspend(gameTask_Handle);
        vTaskResume(TaskAusgabeD_Handle);
        break;
      default:
        break;
    }
    vTaskDelay(100);
  }
}


void TaskAusgabeA(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    standardBorderColors();
    changeMenu(counter);
    vTaskDelay(100);
    FastLED.show();
  }
}

void TaskAusgabeB(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    standardBorderColors();
    changeMenu(counter);
    vTaskDelay(100);
    FastLED.show();
  }
}

void TaskAusgabeC(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    standardBorderColors();
    changeMenu(counter);
    vTaskDelay(100);
    FastLED.show();
  }
}

void TaskAusgabeD(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    // standardBorderColors() ==> setzt die Farbe vom Rand wieder zurück, da beim Wechsel der Menüauswahl, manchmal die alte Farbe bleibt (problematisch)
    standardBorderColors();
    changeMenu(counter);
    vTaskDelay(100);
    FastLED.show();
  }
}

void vTaskCode(void *pvParameters) {
  (void)pvParameters;
  bool steigendeFlanke = false;
  int joystickValue = 0;
  int joystickValueAlt = 0;
  // int yValue = 0;
  int pressed = false;
  int applesEaten = 0;

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds2[0], leds2.Size());
  FastLED.setBrightness(100);
  FastLED.clear(true);
  Serial.begin(9600);

  FastLED.show();
  delay(5000);
  leds.ShiftLeft();

  for (int i = 0; i < 420; i++) {
    cords_[i] = coordinates(16, 8);
  }

  cords_[0] = coordinates(16, 8);

  while (true) {
    joystickValue = joystick.getSW();
    get_y = joystick.getX();
    get_x = joystick.getY();
    steigendeFlanke = (!joystickValueAlt && joystickValue);
    joystickValueAlt = joystickValue;

    // FOR BUTTON PRESS
    if (steigendeFlanke) {
      vTaskResume(TaskAusgabeA_Handle);
      vTaskResume(TaskHauptprogramm_Handle);
      pressed = false;
      counter = 0;
      FastLED.clear();
      vTaskSuspend(vTaskCode_Handle);
    }

    if (get_x > 3000 && direction_ != 2)
      direction_ = 0;
    else if (get_x < 1200 && direction_ != 0)
      direction_ = 2;
    else if (get_y > 3000 && direction_ != 3)
      direction_ = 1;
    else if (get_y < 1200 && direction_ != 1)
      direction_ = 3;

    switch (direction_) {
      case 0:
        if (x > 0)
          x--;
        break;
      case 1:
        if (y < 15)
          y++;
        break;
      case 2:
        if (x < 31)
          x++;
        break;
      case 3:
        if (y > 0)
          y--;
        break;
    }



    for (int i = 0; i < lenght_snake; i++) {
      cords_[lenght_snake - i - 1] = cords_[lenght_snake - i - 2];
    }
    cords_[0] = coordinates(x, y);

    for (int i = 0; i < lenght_snake; i++) {
      drawPointSnake(cords_[i].get_coordinates_X(), cords_[i].get_coordinates_Y());
    }

    drawPointSnake(x_fruit, y_fruit);

    if (cords_[0].get_coordinates_X() == x_fruit && cords_[0].get_coordinates_Y() == y_fruit) {
      lenght_snake++;
      applesEaten++;
      if (applesEaten % 5 == 0) {
        // Spawn rainbow apple
        // Code to spawn rainbow apple goes here
        // Increase the length of the snake by 5
        x_fruit = random(1, 31);
        y_fruit = random(1, 15);
        drawRainbowPointSnake(x_fruit, y_fruit);
        lenght_snake += 5;
      } else {
        // Spawn regular apple
        x_fruit = random(1, 31);
        y_fruit = random(1, 15);
      }
    }
    if (direction_ != 5) { w8_cycle++; }

    if (w8_cycle >= 10) {
      for (int i = 1; i < lenght_snake - 1; i++) {
        if (cords_[0].get_coordinates_X() == cords_[i].get_coordinates_X() && cords_[0].get_coordinates_Y() == cords_[i].get_coordinates_Y()) {
          collision = true;
        }
      }
    }

    if (cords_[0].get_coordinates_X() == 0 || cords_[0].get_coordinates_Y() == 0 || cords_[0].get_coordinates_X() == 31 || cords_[0].get_coordinates_Y() == 15 || collision) {
      collision = false;
      w8_cycle = 0;

      for (int i = 0; i < 5; i++) {
        FastLED.show();
        delay(100);
        FastLED.clear();
        delay(100);
        for (int i = 0; i < lenght_snake; i++) {
          drawPointSnake(cords_[i].get_coordinates_X(), cords_[i].get_coordinates_Y());
        }
      }

      cords_[0] = coordinates(15, 8);
      x = 15;
      y = 7;
      direction_ = 5;
      lenght_snake = 8;
      w8_cycle = 0;
    }
    FastLED.show();
    delay(100);

    FastLED.clear();
    drawSnakeMap();
  }
}

////////// PONG ///////////
void gameTask(void *pvParameters) {
  bool steigendeFlanke = false;
  int joystickValue = 0;
  int joystickValueAlt = 0;
  // int yValue = 0;
  int pressed = false;

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds2[0], leds2.Size());
  FastLED.setBrightness(25);
  FastLED.clear(true);
  Serial.begin(9600);

  FastLED.show();
  vTaskDelay(5000);
  leds.ShiftLeft();

  while (1) {
    get_y = joystick.getX();
    get_x = joystick.getY();

    joystickValue = joystick.getSW();

    steigendeFlanke = (!joystickValueAlt && joystickValue);
    joystickValueAlt = joystickValue;

    // FOR BUTTON PRESS
    if (steigendeFlanke) {
      vTaskResume(TaskAusgabeB_Handle);
      vTaskResume(TaskHauptprogramm_Handle);
      pressed = false;
      counter = 1;
      FastLED.clear();
      vTaskSuspend(gameTask_Handle);
    }

    if (get_x > 3000)
      direction_ = 0;
    else if (get_x < 1200)
      direction_ = 2;
    else if (get_y > 3000)
      direction_ = 1;
    else if (get_y < 1200)
      direction_ = 3;

    switch (direction_) {
      case 0:
        if (x > 0)
          x--;
        break;
      case 1:
        if (y > 1)
          y--;
        break;
      case 2:
        if (x < 31)
          x++;
        break;
      case 3:
        if (y < 11)
          y++;
        break;
    }
    direction_ = 0;

    if (xb >= 30 || xb <= 1) {
      way = !way;
      k = calculate_k(angle + (random(-1, 1) / 10), 1);
      xb = 16.00;
      yb = 8.00;
      FastLED.clear();

      for (int i = 4; i <= 12; i++) {
        for (int o = 8; o <= 24; o++) {
          if (score != 0) {
            score = score - 1;
            drawPointPong(o, i);
            FastLED.show();
            vTaskDelay(pdMS_TO_TICKS(150));
          }
        }
      }
      vTaskDelay(pdMS_TO_TICKS(3000));
    }

    Serial.println(yb);

    if (xb == 2 && ((int)yb == y || (int)yb == y + 1 || (int)yb == y + 2 || (int)yb == y + 3)) {
      way = !way;
      k = calculate_k(angle + (random(-1, 1) / 10), 1);
      score = score + 1;
    }

    if (xb == 29) {
      way = !way;
      k = calculate_k(angle + (random(-1, 1) / 10), 1);
    }

    if (yb >= 14 || yb <= 1) {
      up = !up;
    }
    if ((int)yb >= 13) {
      py = 12;
    } else if ((int)yb <= 1) {
      py = 2;
    } else {
      py = (int)yb;
    }

    drawPointPong(xb, yb);

    drawPadel(2, y);

    drawPadel(29, (int)py - 1);

    drawPongMap();
    if (up) {
      yb = yb + fabs(k);
    } else {
      yb = yb - fabs(k);
    }

    if (way == true) {
      xb++;
    } else {
      xb--;
    }

    FastLED.show();
    vTaskDelay(pdMS_TO_TICKS(100));

    FastLED.clear();
  }
}



void drawPoint(int x, int y, int r, int g, int b) {
  if (y > 7) {
    leds2.DrawLine(x, y - 8, x, y - 8, CRGB(r, g, b));
  } else {
    leds.DrawLine(x, y, x, y, CRGB(r, g, b));
  }
}

void switchMenu() {
  int xValue = joystick.getX();
  int yValue = joystick.getY();

  Serial.println(counter);

  // rauf
  if (xValue < 1000) {
    if ((lastDebounceTime + millis()) > debounceDelay) {
      if (counter == 3) {
        counter = 0;
      } else if (counter == 2) {
        counter = 1;
      }
      lastDebounceTime = millis();
    }

    // runter
  } else if (xValue > 3000) {  // Add this block for downward movement
    if ((lastDebounceTime + millis()) > debounceDelay) {
      if (counter == 0) {
        counter = 3;
      } else if (counter == 1) {
        counter = 2;
      }
      lastDebounceTime = millis();
    }
  }

  // links
  else if (yValue > 3000) {  // Add this block for downward movement
    if ((lastDebounceTime + millis()) > debounceDelay) {
      if (counter == 1) {
        counter = 0;
      } else if (counter == 2) {
        counter = 3;
      }
      lastDebounceTime = millis();
    }
  }

  // rechts
  else if (yValue < 1000) {  // Add this block for downward movement
    if ((lastDebounceTime + millis()) > debounceDelay) {
      if (counter == 0) {
        counter = 1;
      } else if (counter == 3) {
        counter = 2;
      }
      lastDebounceTime = millis();
    }
  }
}


void drawMap() {
  // Panel 1 Wall
  leds.DrawLine(0, 0, 31, 0, CRGB(255, 0, 0));
  leds.DrawLine(0, 0, 0, 7, CRGB(255, 0, 0));
  leds.DrawLine(31, 0, 31, 7, CRGB(255, 0, 0));

  // Panel 2 Wall
  leds2.DrawLine(0, 7, 31, 7, CRGB(255, 0, 0));
  leds2.DrawLine(0, 0, 0, 7, CRGB(255, 0, 0));
  leds2.DrawLine(31, 7, 31, 0, CRGB(255, 0, 0));
}

// Zone wird erstellt (Angangspunk angeben [x,y] und Länge nach Rechts [x] und Runter [y] angeben)
void drawZone(int startX, int startY, int x, int y, int r, int g, int b) {
  for (int i = startX; i <= startX + x; i++) {
    for (int j = startY; j <= startY + y; j++) {
      drawPoint(i, j, r, g, b);
    }
  }
}

void drawMenu() {

  borderUpperLeft(255, 255, 0);

  borderUpperRight(255, 255, 0);

  borderBottomLeft(255, 255, 0);

  borderBottomRight(255, 255, 0);
}


// d35
void borderUpperLeft(int r, int g, int b) {
  // upper left border
  leds.DrawLine(5, 1, 14, 1, CRGB(r, g, b));
  leds.DrawLine(14, 2, 14, 7, CRGB(r, g, b));
  leds.DrawLine(13, 7, 5, 7, CRGB(r, g, b));
  leds.DrawLine(5, 6, 5, 1, CRGB(r, g, b));
  // Snake
  drawSnakeUpperLeft();
}

void borderUpperRight(int r, int g, int b) {
  // upper right border
  leds.DrawLine(17, 1, 26, 1, CRGB(r, g, b));
  leds.DrawLine(26, 2, 26, 7, CRGB(r, g, b));
  leds.DrawLine(25, 7, 17, 7, CRGB(r, g, b));
  leds.DrawLine(17, 6, 17, 1, CRGB(r, g, b));
  // Pong
  drawPongUpperRight();
}

void borderBottomLeft(int r, int g, int b) {
  // bottom left border
  leds2.DrawLine(5, 0, 14, 0, CRGB(r, g, b));
  leds2.DrawLine(14, 1, 14, 6, CRGB(r, g, b));
  leds2.DrawLine(13, 6, 5, 6, CRGB(r, g, b));
  leds2.DrawLine(5, 5, 5, 0, CRGB(r, g, b));
  // Cloud
  drawCloudBottomLeft();
}

void borderBottomRight(int r, int g, int b) {
  // bottom right border
  leds2.DrawLine(17, 0, 26, 0, CRGB(r, g, b));
  leds2.DrawLine(26, 1, 26, 6, CRGB(r, g, b));
  leds2.DrawLine(25, 6, 17, 6, CRGB(r, g, b));
  leds2.DrawLine(17, 5, 17, 0, CRGB(r, g, b));
}

void changeMenu(int choice) {
  switch (choice) {
    case 0:

      if (millis() > time_now + waiting) {
        time_now = millis();
        if (change == 0) {
          borderUpperLeft(255, 255, 0);
          change++;
        } else {
          borderUpperLeft(255, 0, 0);
          change--;
        }
      }
      break;

    case 1:
      if (millis() > time_now + waiting) {
        time_now = millis();
        if (change == 0) {
          borderUpperRight(255, 255, 0);
          change++;
        } else {
          borderUpperRight(255, 0, 0);
          change--;
        }
      }
      break;

    case 2:
      if (millis() > time_now + waiting) {
        time_now = millis();
        if (change == 0) {
          borderBottomRight(255, 255, 0);
          change++;
        } else {
          borderBottomRight(255, 0, 0);
          change--;
        }
      }
      break;

    case 3:
      if (millis() > time_now + waiting) {
        time_now = millis();
        if (change == 0) {
          borderBottomLeft(255, 255, 0);
          change++;
        } else {
          borderBottomLeft(255, 0, 0);
          change--;
        }
      }
      break;
  }
}

// muss noch eingebaut werden, da beim Wechsel manchmal der veränderte (rote) Rand rot bleibt
void standardBorderColors() {
  borderBottomLeft(255, 255, 0);
  borderBottomRight(255, 255, 0);
  borderUpperRight(255, 255, 0);
  borderUpperLeft(255, 255, 0);
}

void drawSnakeUpperLeft() {
  drawZone(6, 2, 7, 4, 0, 0, 0);
  drawPoint(6, 3, 0, 255, 0);
  drawPoint(7, 3, 0, 255, 0);
  drawPoint(7, 4, 0, 255, 0);
  drawPoint(7, 5, 0, 255, 0);
  drawPoint(8, 5, 0, 255, 0);
  drawPoint(9, 5, 0, 255, 0);
  drawPoint(9, 4, 0, 255, 0);
  drawPoint(10, 4, 0, 255, 0);
  drawPoint(10, 3, 0, 255, 0);
  drawPoint(11, 3, 0, 255, 0);
  drawPoint(12, 3, 0, 255, 0);
  drawPoint(12, 4, 0, 255, 0);
  drawPoint(13, 4, 255, 0, 0);
}

void drawPongUpperRight() {
  // TODO:: Problem mit Zonen raußfinden
  drawZone(18, 2, 7, 4, 0, 0, 0);
  drawPoint(18, 2, 81, 52, 249);
  drawPoint(18, 3, 81, 52, 249);
  drawPoint(18, 4, 81, 52, 249);
  drawPoint(20, 4, 81, 52, 249);
  drawPoint(25, 4, 81, 52, 249);
  drawPoint(25, 5, 81, 52, 249);
  drawPoint(25, 6, 81, 52, 249);
}
//305,60

void drawCloudBottomLeft() {
  drawZone(6, 9, 7, 1, 102, 224, 255);
  //drawZone(6, 11, 7, 2, 102, 224, 255);
  drawZone(7, 11, 3, 0, 102, 224, 255);
  drawPoint(6, 12, 81, 52, 249);
  drawPoint(8, 13, 81, 52, 249);
  drawPoint(9, 12, 81, 52, 249);
  drawPoint(11, 11, 81, 52, 249);
  drawPoint(11, 13, 81, 52, 249);
  drawPoint(13, 12, 81, 52, 249);
}

// SNAKE START
void drawSnakeMap() {
  // Panel 1 Wall
  leds.DrawLine(0, 0, 31, 0, CRGB(255, 0, 0));
  leds.DrawLine(0, 0, 0, 7, CRGB(255, 0, 0));
  leds.DrawLine(31, 0, 31, 7, CRGB(255, 0, 0));

  // Panel 2 Wall
  leds2.DrawLine(0, 7, 31, 7, CRGB(255, 0, 0));
  leds2.DrawLine(0, 0, 0, 7, CRGB(255, 0, 0));
  leds2.DrawLine(31, 7, 31, 0, CRGB(255, 0, 0));
}

void drawPointSnake(int x, int y) {
  if (y > 7) {
    leds2.DrawLine(x, y - 8, x, y - 8, CRGB(255, 0, 255));
  } else {
    leds.DrawLine(x, y, x, y, CRGB(255, 0, 255));
    //Serial.println("panel2");
  }
}

void drawRainbowPointSnake(int x, int y) {
  if (y > 7) {
    // Calculate rainbow color based on time
    uint8_t hue = (millis() / 1000) % 256;  // Change color every second

    // Set rainbow color for the upper panel
    leds2.DrawLine(x, y - 8, x, y - 8, CHSV(hue, 255, 255));
  } else {
    // Calculate rainbow color based on time
    uint8_t hue = (millis() / 1000) % 256;  // Change color every second

    // Set rainbow color for the lower panel
    leds.DrawLine(x, y, x, y, CHSV(hue, 255, 255));
  }
}

// SNAKE END

// PONG START
void drawPointPong(int x, int y) {
  if (y > 7) {
    leds2.DrawLine(x, y - 8, x, y - 8, CRGB(255, 255, 255));
  } else {
    leds.DrawLine(x, y, x, y, CRGB(255, 255, 255));
  }
}

void drawPadel(int x, int y) {
  for (int i = 0; i < 4; i++) {
    drawPointPong(x, y + i);
  }
}

void drawPongMap() {
  // Panel 1 Wall
  leds.DrawLine(0, 0, 31, 0, CRGB(255, 0, 0));
  leds.DrawLine(0, 0, 0, 7, CRGB(255, 0, 0));
  leds.DrawLine(31, 0, 31, 7, CRGB(255, 0, 0));

  // Panel 2 Wall
  leds2.DrawLine(0, 7, 31, 7, CRGB(255, 0, 0));
  leds2.DrawLine(0, 0, 0, 7, CRGB(255, 0, 0));
  leds2.DrawLine(31, 7, 31, 0, CRGB(255, 0, 0));
}

double calculate_k(double angle, int x) {
  return tan(angle) * x;
}

// PONG END
