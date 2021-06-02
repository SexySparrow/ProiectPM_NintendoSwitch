#include <LiquidCrystal.h>
#include "LedControl.h"
#include <string.h>
#include <EEPROM.h>
LedControl lc = LedControl(10, 9, 8, 1); //DIN, CLK, no driver

bool doodleMatrix[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

int bombMatrix[8][8] =
{ {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 1, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

// BUTTONS
const int middleButton = 7;
const int upButton = 12;
const int leftButton = A5;
const int rightButton =  A4;
const int downButton = A3;


int middleButtonState = 0;
int upButtonState = 0;
int leftButtonState = 0;
int rightButtonState = 0;
int downButtonState = 0;


int lastMiddleButtonState = 0;
int lastUpButtonState = 0;
int lastLeftButtonState = 0;
int lastRightButtonState = 0;
int lastDownButtonState = 0;


int endGameButtonState = 0;

// LCD
int LCDdelay = 200;
int Contrast = 95;
const int RS = 12;
const int enable = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);


// joystick
const int xPin = A2;
const int yPin = A1;
const int pushPin = A0;

int xValue, yValue, pushValue;
bool xReset = true;
bool yReset = true;
int minThreshold = 400;
int maxThreshold = 600;
bool lastPushValue = 1;
int prevX = 3, prevY = 3, posX = 3, posY = 3;
boolean movedX = false, movedY = false;


long long blinkTime;
boolean Delay = true;

// menu
int highscore = 0;
bool clickedMenu = 0;
bool clickedGamesMenu = 0;
int page_counter = 1;
int game_page_counter = 1;
int maxName = 15;


unsigned long long int lastMillis = 0;
unsigned int startTime = 0;
unsigned int levelTime = 0;
int level = 0;
bool startedFunc = 1;

// maze game
int bombGameSwState, lastBombGameSwState;
int bombGamePrevX = 0, bombGamePrevY = 0, bombGamePosX = 0, bombGamePosY = 0;
int bombX, bombY;
long randNumber;
int lives = 3;
boolean bombDelay = true;
int winX = 7, winY = 7;
int mazeScore = 0;
bool mazeGameEnd = 0;

// snake game
int snakeGameSwState, lastsnakeGameSwState;
int snakeHeadY = random(8);
int snakeHeadX = random(8);
int snakeBodyX[64] = {snakeHeadX};
int snakeBodyY[64] = {snakeHeadY}; //snake's position at the beginning
int snakeLength = 1;
int snakeDirection = 0;

bool foodFound = false;
int foodX = 0;
int foodY = 0;

int snakeGameScore = 0;
int snakeGameSpeed = 300;
bool snakeGameOn = true;
int snakeGameLevel = 0;



void setup() {

  Serial.begin(9600);

  analogWrite(6, Contrast);
  lcd.begin(16, 2);

  pinMode(middleButton, INPUT_PULLUP);
  /*pinMode(upButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP); */

  pinMode(pushPin, INPUT_PULLUP);

  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 5); // sets brightness (0-15) possible values
  lc.clearDisplay(0); // clear screen

}


/// SNAKE GAME
void setFood() {
  /*
    set the food on the matrix randomly
    also, if the food is on the snake body, recall the function
  */
  int flag = 0;
  int verifPoz = 0;
  int i;
  while (flag == 0) {
    foodX = getFood();
    foodY = getFood();
    verifPoz = 1;
    for (i = 0; i < snakeLength; i++) {
      if (snakeBodyX[i] == foodX && snakeBodyY[i] == foodY)
        verifPoz = 0;
    }
    if (verifPoz)
      flag = 1;
  }
}

int getFood() {
  return random(8);
}

void displaySnakeOn() {
  //display the snake body
  for (int i = 0; i < snakeLength; i++) {
    lc.setLed(0, snakeBodyX[i], snakeBodyY[i], true);
  }
}


void displaySnakeOff() {
  //delete the snake body
  for (int i = 0; i < snakeLength; i++) {
    lc.setLed(0, snakeBodyX[i], snakeBodyY[i], false);
  }
}


void snakeMovement() {

  /*
    direction
    1 = up
    2 = rigt
    3 = down
    4 = left
  */

  int flag = 0;
  xValue = analogRead(xPin);
  yValue = analogRead(yPin);

  Serial.println(xValue);
  Serial.println(yValue);

/*
  upButtonState = digitalRead(upButton);
  downButtonState = digitalRead(downButton);
  rightButtonState = digitalRead(rightButton);
  leftButtonState = digitalRead(leftButton); */

  //flag is for maintaining classical directions


  if ( xValue > 600  && flag == 0 && snakeDirection != 1 )
  {

    //right

    snakeHeadX++;
    if (snakeHeadX > 7)
      snakeHeadX = 0;
    for (int i = snakeLength - 1; i > 0; i--) {
      snakeBodyX[i] = snakeBodyX[i - 1];
      snakeBodyY[i] = snakeBodyY[i - 1];
      //change the head and the body of the snake
    }
    snakeBodyX[0] = snakeHeadX;
    snakeBodyY[0] = snakeHeadY;
    flag = 1;
    snakeDirection = 3;
    lastRightButtonState = rightButtonState;

  }
  else if ( xValue < 400  && flag == 0 && snakeDirection != 3)
  {

    //left

    snakeHeadX--;
    if (snakeHeadX < 0)
      snakeHeadX = 7;
    for (int i = snakeLength - 1; i > 0; i--) {
      snakeBodyX[i] = snakeBodyX[i - 1];
      snakeBodyY[i] = snakeBodyY[i - 1];
    }
    snakeBodyX[0] = snakeHeadX;
    snakeBodyY[0] = snakeHeadY;
    flag = 1;
    snakeDirection = 1;
    lastLeftButtonState = leftButtonState;
  }


  if ( yValue > 600  && flag == 0 && snakeDirection != 2)
  {

    //down

    snakeHeadY--;
    if (snakeHeadY < 0)
      snakeHeadY = 7;
    for (int i = snakeLength - 1; i > 0; i--) {
      snakeBodyX[i] = snakeBodyX[i - 1];
      snakeBodyY[i] = snakeBodyY[i - 1];
    }
    snakeBodyX[0] = snakeHeadX;
    snakeBodyY[0] = snakeHeadY;
    flag = 1;
    snakeDirection = 4;
    

  }
  else if ( yValue < 400  && flag == 0 && snakeDirection != 4)
  {

    //up

    snakeHeadY++;
    if (snakeHeadY > 7)
      snakeHeadY = 0;
    for (int i = snakeLength - 1; i > 0; i--) {
      snakeBodyX[i] = snakeBodyX[i - 1];
      snakeBodyY[i] = snakeBodyY[i - 1];
    }
    snakeBodyX[0] = snakeHeadX;
    snakeBodyY[0] = snakeHeadY;
    flag = 1;
    snakeDirection = 2;
 
  }




  if (xValue > 400 && xValue < 600 && yValue > 400 && yValue < 600) {
    //no changes
    if (snakeDirection == 2) {
      snakeHeadY++;
      if (snakeHeadY > 7)
        snakeHeadY = 0;
      for (int i = snakeLength - 1; i > 0; i--) {
        snakeBodyX[i] = snakeBodyX[i - 1];
        snakeBodyY[i] = snakeBodyY[i - 1];
      }
      snakeBodyX[0] = snakeHeadX;
      snakeBodyY[0] = snakeHeadY;
      flag = 1;
      snakeDirection = 2;
    }
    else if (snakeDirection == 4) {
      snakeHeadY--;
      if (snakeHeadY < 0)
        snakeHeadY = 7;
      for (int i = snakeLength - 1; i > 0; i--) {
        snakeBodyX[i] = snakeBodyX[i - 1];
        snakeBodyY[i] = snakeBodyY[i - 1];
      }
      snakeBodyX[0] = snakeHeadX;
      snakeBodyY[0] = snakeHeadY;
      flag = 1;
      snakeDirection = 4;
    }
    else if (snakeDirection == 1) {
      snakeHeadX--;
      if (snakeHeadX < 0)
        snakeHeadX = 7;
      for (int i = snakeLength - 1; i > 0; i--) {
        snakeBodyX[i] = snakeBodyX[i - 1];
        snakeBodyY[i] = snakeBodyY[i - 1];
      }
      snakeBodyX[0] = snakeHeadX;
      snakeBodyY[0] = snakeHeadY;
      flag = 1;
      snakeDirection = 1;
    }
    else if (snakeDirection == 3) {
      snakeHeadX++;
      if (snakeHeadX > 7)
        snakeHeadX = 0;
      for (int i = snakeLength - 1; i > 0; i--) {
        snakeBodyX[i] = snakeBodyX[i - 1];
        snakeBodyY[i] = snakeBodyY[i - 1];
      }
      snakeBodyX[0] = snakeHeadX;
      snakeBodyY[0] = snakeHeadY;
      flag = 1;
      snakeDirection = 3;
    }
  }
  Serial.println(snakeDirection);
  displaySnakeOn();
  delay(snakeGameSpeed); //for the snake movement
  displaySnakeOff();
}

void blinkFood()
{
  if (Delay) {
    blinkTime = millis();
    Delay = !Delay;
  }

  if (millis() - blinkTime > 250 && millis() - blinkTime < 500) {
    lc.setLed(0, foodX, foodY, true);
  }

  if (millis() - blinkTime > 500) {
    Delay = !Delay;
    lc.setLed(0, foodX, foodY, false);
  }
}

void checkFood() {
  foodFound = false;

  if (snakeBodyX[0] == foodX && snakeBodyY[0] == foodY)
  {
    foodFound = true;
    lc.setLed(0, foodX, foodY, false);
    setFood();
    snakeGameScore += 10;
    if (snakeGameScore % 50 == 0)
      snakeGameLevel ++;
    snakeGameSpeed -= 10; //increase the difficulty

  }
  lc.setLed(0, foodX, foodY, true); //new food's pozition
  blinkFood();
}

bool snakeIsDying() {
  int x = snakeBodyX[0];
  int y = snakeBodyY[0];
  int flag = 0;
  for (int i = 1; i < snakeLength; i++)
    if (x == snakeBodyX[i] && y == snakeBodyY[i])
      flag = 1;

  if (flag == 1) {
    //the snake ate his body
    gameLost();
    return true;
  }
  return false;
}

void gameWon() {
  snakeGameOn = false;

  if (millis() - lastMillis >= LCDdelay) {
    lastMillis = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  You Won ! :)");
    lcd.setCursor(0, 1);
    lcd.print("   score:  ");
    lcd.print(snakeGameScore);
  }

}

void displayScore() {

  if (millis() - lastMillis >= LCDdelay) {
    lastMillis = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Playing: snake");
    lcd.setCursor(0, 1);
    lcd.print("   score:  ");
    lcd.print(snakeGameScore);
  }

}

bool growSnake() {
  //the eaten food is going to be the snake's tail
  snakeLength++;
  snakeBodyX[snakeLength - 1] = foodX;
  snakeBodyY[snakeLength - 1] = foodY;
}

void gameLost() {

  snakeGameOn = false;

  lc.clearDisplay(0);

  lc.setLed(0, 1, 2, true);
  lc.setLed(0, 2, 2, true);
  lc.setLed(0, 3, 2, true);

  lc.setLed(0, 1, 5, true);
  lc.setLed(0, 2, 5, true);
  lc.setLed(0, 3, 5, true);

  lc.setLed(0, 6, 2, true);
  lc.setLed(0, 5, 3, true);
  lc.setLed(0, 5, 4, true);
  lc.setLed(0, 6, 5, true);

  if (millis() - lastMillis >= LCDdelay) {
    lastMillis = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   You lost :(");
    lcd.setCursor(0, 1);
    lcd.print("   score:  ");
    lcd.print(snakeGameScore);

    delay(3500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("press b for menu");
    lcd.setCursor(0, 1);
    lcd.print("joystck to retry");

  }
}

void restartSnake()
{
  lcd.clear();
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      lc.setLed(0, i, j, false);

  snakeHeadY = random(8);
  snakeHeadX = random(8);
  snakeBodyX[64] = {snakeHeadX};
  snakeBodyY[64] = {snakeHeadY}; //snake's position at the beginning
  snakeLength = 1;
  snakeDirection = 0;

  foodFound = false;
  foodX = 0;
  foodY = 0;

  snakeGameScore = 0;
  snakeGameSpeed = 300;
  snakeGameOn = true;
  snakeGameLevel = 0;

}


void Snake_loop() {

  snakeGameSwState = digitalRead(pushPin);

  if (snakeGameOn) {
    displayScore();
    snakeMovement();
    checkFood();
    if (foodFound)
      growSnake();

    if (snakeIsDying())
      snakeGameOn = false;
    if (snakeGameScore == 600)
      gameWon();


  }
  if (snakeGameSwState != lastsnakeGameSwState && snakeGameSwState == LOW)
  {
    Serial.println("pushed");
    restartSnake();
  }

  lastsnakeGameSwState = snakeGameSwState;
}

/// SNAKE GAME END







/// DOODLE GAME
void readDoodleJoystick()
{
  if (yValue < 400)
  {
    if (movedY == false)
    {
      prevY = posY;
      posY++;   //--
      movedY = true;
    }
  }
  else
  {
    if (yValue > 600)
    {
      if (movedY == false)
      {
        prevY = posY;
        posY--;  // ++
        movedY = true;
      }
    }
    else movedY = false;
  }

  if (xValue < 400)
  {
    if (movedX == false)
    {
      prevX = posX;
      posX--;
      movedX = true;
    }
  }
  else
  {
    if (xValue > 600)
    {
      if (movedX == false)
      {
        prevX = posX;
        posX++;
        movedX = true;
      }
    }
    else movedX = false;
  }
}

void blinkLed()
{
  if (Delay) {
    blinkTime = millis();
    Delay = !Delay;
  }

  if (millis() - blinkTime > 250 && millis() - blinkTime < 500) {
    lc.setLed(0, posX, posY, true);
  }

  if (millis() - blinkTime > 500) {
    Delay = !Delay;
    lc.setLed(0, posX, posY, false);
  }
}

void Doodle_loop()
{



  xValue = analogRead(xPin);
  yValue = analogRead(yPin);

  /*  Serial.print("X : ");
    Serial.println(xValue);
    Serial.print("Y : ");
    Serial.println(yValue);
  */

  if (millis() - lastMillis >= LCDdelay) {
    lastMillis = millis();

    lcd.setCursor(0, 0);
    lcd.print("Playing: Doodle");
    lcd.setCursor(0, 1);
    lcd.print("Press b to back");
  }


  readDoodleJoystick();

  if (posY == 8) posY = 0;
  if (posY == -1) posY = 7;
  if (posX == 8) posX = 0;        /// row=x   ;   col = y;
  if (posX == -1) posX = 7;


  lc.setLed(0, prevX, prevY, false);
  //lc.setLed(0, posX, posY, true);


  if (posX != prevX || posY != prevY) {
    prevX = posX;
    prevY = posY;
  }


  blinkLed();

  if (pressedButtonFunc())
    doodleMatrix[posX][posY] = 1;

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      if (doodleMatrix[i][j] == 1 )
        lc.setLed(0, i, j, true);


}
/// DOODLE GAME END




/// MAZE GAME

void loseLife() {
  lives--;
  bombGamePosX = 0;
  bombGamePosY = 0;

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      lc.setLed(0, i, j, false);
  delay(10);
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      if (i == j || i + j == 7)
        lc.setLed(0, i, j, true);
  delay(1000);

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      lc.setLed(0, i, j, false);
  bombMatrix[bombX][bombY] = 0;

  randNumber = random(0, 63);
  bombY = randNumber % 8;
  bombX = randNumber / 8;


  while (bombMatrix[bombX][bombY] == 1 || (bombX == 0 && bombY == 0))
  {
    randNumber = random(0, 63);
    bombY = randNumber % 8;
    bombX = randNumber / 8;
  }

  bombMatrix[bombX][bombY] = -1;

}


void displayWin() {

  bombGamePosX = 0;
  bombGamePosY = 0;

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      lc.setLed(0, i, j, false);
  delay(10);

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      if (j > 3 && i + j == 9)
        lc.setLed(0, i, j, true);
      else if (j <= 3 && i - j == 3 && j != 0)
        lc.setLed(0, i, j, true);
  delay(1000);

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      lc.setLed(0, i, j, false);

  delay(10);
  bombMatrix[bombX][bombY] = 0;

  randNumber = random(0, 63);
  bombY = randNumber % 8;
  bombX = randNumber / 8;
  while (bombMatrix[bombX][bombY] == 1 || (bombX == 0 && bombY == 0))
  {
    randNumber = random(0, 63);
    bombY = randNumber % 8;
    bombX = randNumber / 8;
  }
  bombMatrix[bombX][bombY] = -1;
}


void refillLives() {
  lcd.clear();
  lives = 3;
  mazeGameEnd = 0;
  mazeScore = 0;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      lc.setLed(0, i, j, false);

  bombMatrix[bombX][bombY] = 0;

  randNumber = random(0, 64);
  bombY = randNumber % 8;
  bombX = randNumber / 8;
  while (bombMatrix[bombX][bombY] == 1 || (bombX == 0 && bombY == 0))
  {
    randNumber = random(0, 63);
    bombY = randNumber % 8;
    bombX = randNumber / 8;
  }
  bombMatrix[bombX][bombY] = -1;
}


void Maze_loop() {
  // put your main code here, to run repeatedly:
  bombGameSwState = digitalRead(pushPin);

  if (lives > 0) {


    if (millis() - lastMillis >= LCDdelay) {
      lastMillis = millis();
      lcd.setCursor(0, 0);
      lcd.print("Playing: Maze");
      lcd.setCursor(0, 1);
      lcd.print("lives:");
      lcd.print(lives);
      lcd.print("  scr:");
      lcd.print(mazeScore);
    }

    xValue = analogRead(xPin);
    yValue = analogRead(yPin);
    for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++)
        if (bombMatrix[i][j] == 1)
          lc.setLed(0, i, j, true);


    if (yValue > maxThreshold)
    {
      if (movedY == false && bombMatrix[bombGamePosX][bombGamePosY - 1] != 1 && bombGamePosY > 0)
      {
        bombGamePrevY = bombGamePosY;
        bombGamePosY--;
        movedY = true;
      }
    }
    else
    {
      if (yValue < minThreshold)
      {
        if (movedY == false && bombMatrix[bombGamePosX][bombGamePosY + 1] != 1 && bombGamePosY < 7)
        {
          bombGamePrevY = bombGamePosY;
          bombGamePosY++;
          movedY = true;
        }

      }
      else movedY = false;
    }
    if (xValue < minThreshold)
    {
      if (movedX == false && bombMatrix[bombGamePosX - 1][bombGamePosY] != 1 && bombGamePosX > 0)
      {
        bombGamePrevX = bombGamePosX;
        bombGamePosX--;
        movedX = true;
      }
    }
    else
    {
      if (xValue > maxThreshold)
      {
        if (movedX == false && bombMatrix[bombGamePosX + 1][bombGamePosY] != 1 && bombGamePosX < 7)
        {
          bombGamePrevX = bombGamePosX;
          bombGamePosX++;
          movedX = true;
        }
      }
      else movedX = false;
    }


    lc.setLed(0, bombGamePrevX, bombGamePrevY, false);

    //lc.setLed(0, bombGamePosX, bombGamePosY, true);
    if (bombGamePosX != bombGamePrevX || bombGamePosY != bombGamePrevY) {
      bombGamePrevX = bombGamePosX;
      bombGamePrevY = bombGamePosY;
    }


    if (bombGamePosX == bombX && bombGamePosY == bombY)
    {
      loseLife();
    }

    if (bombGamePosX == winX && bombGamePosY == winY)
    {
      mazeScore = mazeScore + 10;
      displayWin();

    }

    // blink play`er and bomb
    if (bombDelay) {
      blinkTime = millis();
      bombDelay = !bombDelay;
    }
    if (millis() - blinkTime > 250 && millis() - blinkTime < 500) {
      lc.setLed(0, bombGamePosX, bombGamePosY, true);
    }

    if (millis() - blinkTime > 500 && millis() - blinkTime < 750) {
      lc.setLed(0, bombX, bombY, false);
      lc.setLed(0, bombGamePosX, bombGamePosY, false);

    }
    if (millis() - blinkTime > 750 && millis() - blinkTime < 1000) {
      lc.setLed(0, bombGamePosX, bombGamePosY, true);
    }
    if (millis() - blinkTime > 1000) {
      bombDelay = !bombDelay;
      lc.setLed(0, bombX, bombY, true);
      lc.setLed(0, bombGamePosX, bombGamePosY, false);
    }


    if (lives == 0) {
      for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
          lc.setLed(0, i, j, false);
    }

  }

  else {    /// lives = 0
    lc.setLed(0, 1, 2, true);
    lc.setLed(0, 2, 2, true);
    lc.setLed(0, 3, 2, true);

    lc.setLed(0, 1, 5, true);
    lc.setLed(0, 2, 5, true);
    lc.setLed(0, 3, 5, true);

    lc.setLed(0, 6, 2, true);
    lc.setLed(0, 5, 3, true);
    lc.setLed(0, 5, 4, true);
    lc.setLed(0, 6, 5, true);


    if (mazeGameEnd == 0)
      if (millis() - lastMillis >= LCDdelay) {
        lastMillis = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   You lost :(");
        lcd.setCursor(0, 1);
        lcd.print("   score:  ");
        lcd.print(mazeScore);

        delay(3500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("press b for menu");
        lcd.setCursor(0, 1);
        lcd.print("joystck to retry");
        mazeGameEnd = 1;
      }


    if (bombGameSwState != lastBombGameSwState && bombGameSwState == LOW) {
      Serial.println("pushed");
      refillLives();
    }
  }

  /*  Serial.print(bombGamePosX);
    Serial.print(" ");
    Serial.print(bombGamePosY);
    Serial.print(" ");
    Serial.print(bombGameSwState);
    Serial.print(" ");
    Serial.println(lastBombGameSwState);
    //Serial.println(bombDelay);
    delay(10); */
  lastBombGameSwState = bombGameSwState;

}

bool pressedButtonFunc() {

  pushValue = digitalRead(pushPin);

  if (pushValue == 0 && lastPushValue == 1) {
    lastPushValue = 0;
    return 1;
  }

  if (pushValue && lastPushValue == 0)
    lastPushValue = 1;

  return 0;
}




void selectMenuFunc() {

  int xValue = analogRead(xPin);

  if (xValue < minThreshold && xReset) {
    page_counter--;
    if (page_counter < 1)
      page_counter = 1;
    xReset = 0;
  }

  if (xValue > maxThreshold && xReset) {
    page_counter++;
    if (page_counter > 3)
      page_counter = 3;
    xReset = 0;
  }

  if (xReset == 0 && xValue >= minThreshold && xValue <= maxThreshold)
    xReset = 1;

  //------- Switch function to write and show what you want---//

  if (millis() - lastMillis >= LCDdelay) {
    lastMillis = millis();
    lcd.clear();
    lcd.setCursor(0, 0);

    switch (page_counter) {

      case 1: {    // home page
          lcd.setCursor(3, 0);
          lcd.print("Game Menu");
          lcd.setCursor(0, 1);
          lcd.print("Games <- ");
        }
        break;

      case 2: { // select highscore
          lcd.setCursor(0, 0);
          lcd.print("Highscores <-");
          lcd.setCursor(0, 1);
          lcd.print("Settings");
        }
        break;

      case 3: {   //select settings
          lcd.setCursor(0, 0);
          lcd.print("Highscores ");
          lcd.setCursor(0, 1);
          lcd.print("Settings <-");
        }
        break;

    }//switch end
  }// if end

}

bool gameEndFunc() {
  if (startedFunc == false && millis() - startTime > 10000)
  {
    delay(1000);
    return true;
  }
  return false;
}




void displayGamesFunc() {

  int xValue = analogRead(xPin);

  if (xValue < minThreshold && xReset) {
    game_page_counter--;
    if (game_page_counter < 1)
      game_page_counter = 1;
    xReset = 0;
  }

  if (xValue > maxThreshold && xReset) {
    game_page_counter++;
    if (game_page_counter > 5)
      game_page_counter = 5;
    xReset = 0;
  }

  if (xReset == 0 && xValue >= minThreshold && xValue <= maxThreshold)
    xReset = 1;

  //------- Switch function to write and show what you want---//

  if (millis() - lastMillis >= LCDdelay) {
    lastMillis = millis();
    lcd.clear();
    lcd.setCursor(0, 0);

    switch (game_page_counter) {

      case 1: {    // home page
          lcd.setCursor(3, 0);
          lcd.print("Games Menu");
          lcd.setCursor(0, 1);
          lcd.print("1. Snake <- ");
        }
        break;

      case 2: { // select highscore
          lcd.setCursor(0, 0);
          lcd.print("2. Tetris  <-");
          lcd.setCursor(0, 1);
          lcd.print("3. Doodle");
        }
        break;

      case 3: {   //select settings
          lcd.setCursor(0, 0);
          lcd.print("2. Tetris ");
          lcd.setCursor(0, 1);
          lcd.print("3. Doodle <-");
        }
        break;

      case 4: {   //select settings
          lcd.setCursor(0, 0);
          lcd.print("4. Maze <-");
          lcd.setCursor(0, 1);
          lcd.print("5. Go Back");
        }
        break;

      case 5: {   //select settings
          lcd.setCursor(0, 0);
          lcd.print("4. Maze ");
          lcd.setCursor(0, 1);
          lcd.print("5. Go Back <-");
        }
        break;



    }//switch end
  }// if end

}


void moreSettingsFunc() {

  if (millis() - lastMillis > LCDdelay) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("<-back ");
    lcd.setCursor(1, 1);
    lcd.print("Start level: "); lcd.print(level);
    lastMillis = millis();
  }

  xValue = analogRead(xPin);

  if (xValue < minThreshold && xReset) {

    level++;
    if (level > 9)
      level = 0;
    xReset = 0;
  }
  else if (xValue > maxThreshold && xReset) {
    level--;
    if (level < 0)
      level = 9;
    xReset = false;
  }
  else if (xValue >= minThreshold && xValue <= maxThreshold)
    xReset = true;

}

void highscoreFunc() {

  if (millis() - lastMillis > LCDdelay) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("<-back ");
    lcd.setCursor(1, 1);
    lcd.print("anon" );
    lcd.print(" : ");
    lcd.print(highscore);

    lastMillis = millis();
  }

  if (millis() - lastMillis > LCDdelay) {

    lcd.clear();

    lcd.print("anon" );
    lcd.print(" ");
    lcd.print(highscore);

    if (pressedButtonFunc())
      clickedMenu = false;

  }
}


void loop() {


  if (!clickedMenu) {

    if (pressedButtonFunc())
    {
      clickedMenu = true;
      game_page_counter = 1;
      clickedGamesMenu = false;
    }

    else selectMenuFunc();                     // if(!pressedButtonFunc())
  }

  else {                                     // if(clickedMenu)

    if (page_counter == 1)       // Games menu
    {

      if (!clickedGamesMenu) {

        if (pressedButtonFunc())
        {
          clickedGamesMenu = true;
          page_counter == 1;

          /// clear the doodle matrix
          for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
              doodleMatrix[i][j] = 0 ;

          // reset the setup for the maze game
          lc.clearDisplay(0);

          randomSeed(analogRead(0));
          randNumber = random(0, 63);
          bombY = randNumber % 8;
          bombX = randNumber / 8;
          while (bombMatrix[bombX][bombY] == 1 || (bombX == 0 && bombY == 0))
          {
            randNumber = random(0, 63);
            bombY = randNumber % 8;
            bombX = randNumber / 8;
          }
          bombMatrix[bombX][bombY] = -1;

          // set food for the snake game
          setFood();

        }

        else displayGamesFunc();     /// displaying the games
      }

      else     // if(clickedGamesMenu
      {

        if (game_page_counter == 1)   // SNAKE GAME
        {
          middleButtonState = digitalRead(middleButton);

          Snake_loop();

          if (middleButtonState == LOW) /// quit the game thorough the middle button
          {
            clickedMenu = false;
            game_page_counter = 1;
            lc.clearDisplay(0);
          }

           lastMiddleButtonState = middleButtonState;
        }


        if (game_page_counter == 3 )    /// DOODLE GAME
        {


          middleButtonState = digitalRead(middleButton);

          Doodle_loop();

          if (middleButtonState == LOW) /// quit the game thorough the middle button
          {
            clickedMenu = false;
            game_page_counter = 1;
            lc.clearDisplay(0);
          }
          lastMiddleButtonState = middleButtonState;
        }

        if (game_page_counter == 4 )
        {
          middleButtonState = digitalRead(middleButton);

          Maze_loop();

          if (middleButtonState == LOW) /// quit the game thorough the middle button
          {
            clickedMenu = false;
            game_page_counter = 1;
            lc.clearDisplay(0);
          }
          
           lastMiddleButtonState = middleButtonState;
        }


      }


      if (game_page_counter == 5)
        if ( pressedButtonFunc() )
        {
          clickedMenu = false;
          game_page_counter = 1;
        }

    }

    else if (page_counter == 2)
    { highscoreFunc();
      if (
        pressedButtonFunc() )
        clickedMenu = false;
    }

    else if (page_counter == 3)
    {
      moreSettingsFunc();
      if ( pressedButtonFunc() )
        clickedMenu = false;
    }

  }


}//loop end
