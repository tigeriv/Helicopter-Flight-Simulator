#include <Servo.h> //Include Servo library
#include "myServo.h"
#include "Motor.h"
//Setting the pins for the motors

//Motor Speed Pins
const int gantryFBSpeedPin = 2;
const int gantryUDSpeedPin = 3;
const int gantryLRSpeedPin = 4;
const int yawSpeedPin = 5;

//Motor Speeds for PWM (int 0 - 255)
int gantryFBSpeed = 255;
int gantryUDSpeed = 175;
int gantryLRSpeed = 255;
int yawSpeed = 125;

//Limit Switches, one at each end, which turns ON when touched
const int GANTRY_FORWARD = 25;
const int GANTRY_BACKWARD = 24;
const int GANTRY_LEFT = 27;
const int GANTRY_RIGHT = 26;
const int GANTRY_UP = 28;
const int GANTRY_DOWN = 29;
const int YAW_LEFT = 30;
const int YAW_RIGHT = 31;

//Buttons, one for each direction of movement except for the gantry
const int GLB = 38;
const int GRB = 36;
const int YLB = 41;
const int YRB = 42;

//Joystick
const int GBB = 35;
const int GFB = 37;
const int GUB = 44;
const int GDB = 43;

//Buttons to select modes
const int manualButton = 45;
const int automaticButton = 46;

//LED's to display current mode
const int manualLED = 32;
const int autoLED = 33;

int checkButtons(); //to check which option to use (auto or manual)

//Used to pass whether the cockpit is moving or not (if it is, an LED turns on)
int cockPitMoving;
boolean movingReturn;

//Make motors
Motor *gantryFB = new Motor(motorGF, motorGB, gantryFBSpeedPin, GANTRY_FORWARD, GANTRY_BACKWARD, gantryFBSpeed);
Motor *gantryLR = new Motor(motorGL, motorGR, gantryLRSpeedPin, GANTRY_LEFT, GANTRY_RIGHT, gantryLRSpeed);
Motor *gantryUD = new Motor(motorGU, motorGD, gantryUDSpeedPin, GANTRY_UP, GANTRY_DOWN, gantryUDSpeed);
Motor *yaw = new Motor(motorYL, motorYR, yawSpeedPin, YAW_LEFT, YAW_RIGHT, yawSpeed);

//Setting the pin modes (input, output, and starting the serial monitor for debugging)
void setup() {
  pinMode(GANTRY_FORWARD, INPUT_PULLUP); //Limit Switches, input_pullup enables a pullup resistor
  pinMode(GANTRY_BACKWARD, INPUT_PULLUP);
  pinMode(GANTRY_LEFT, INPUT_PULLUP);
  pinMode(GANTRY_RIGHT, INPUT_PULLUP);
  pinMode(YAW_LEFT, INPUT_PULLUP);
  pinMode(YAW_RIGHT, INPUT_PULLUP);
  pinMode(GANTRY_UP, INPUT_PULLUP);
  pinMode(GANTRY_DOWN, INPUT_PULLUP);
  pinMode(motorGF, OUTPUT); //Motor Directions
  pinMode(motorGB, OUTPUT);
  pinMode(motorGU, OUTPUT);
  pinMode(motorGD, OUTPUT);
  pinMode(motorGL, OUTPUT);
  pinMode(motorGR, OUTPUT);
  pinMode(motorYL, OUTPUT);
  pinMode(motorYR, OUTPUT);
  pinMode(gantryFBSpeedPin, OUTPUT); //Motor Speeds
  pinMode(gantryUDSpeedPin, OUTPUT);
  pinMode(gantryLRSpeedPin, OUTPUT);
  pinMode(yawSpeedPin, OUTPUT);
  pinMode(GLB, INPUT_PULLUP);
  pinMode(GRB, INPUT_PULLUP);
  pinMode(GFB, INPUT_PULLUP);
  pinMode(GBB, INPUT_PULLUP);
  pinMode(GUB, INPUT_PULLUP);
  pinMode(GDB, INPUT_PULLUP);
  pinMode(YLB, INPUT_PULLUP);
  pinMode(YRB, INPUT_PULLUP);
  pinMode(cockpitLED, OUTPUT);
  pinMode(pitchLeft, INPUT_PULLUP);
  pinMode(pitchRight, INPUT_PULLUP);
  pinMode(rollLeft, INPUT_PULLUP);
  pinMode(rollRight, INPUT_PULLUP);
  pinMode(manualButton, INPUT_PULLUP);
  pinMode(automaticButton, INPUT_PULLUP);
  pinMode(autoLED, OUTPUT);
  pinMode(manualLED, OUTPUT);
  pitchServo.attach(pitchPin);
  rollServo.attach(rollPin);
  //Serial.begin(9600);
  // initialize all the readings to 0:
  /*for (int thisReading = 0; thisReading < numReadings; thisReading++)
    {
    readings[thisReading] = 0;
    }*/
  resetPitch();
  resetRoll();
  delay(1000);
}

void loop()
{
  stopAll(); //Stop everything
  digitalWrite(manualLED, LOW); //Turn LED's off
  digitalWrite(autoLED, LOW);
  int optionNumber = checkButtons(); //Check if a mode buttton is pushed
  if (optionNumber == 1) //If manual is pushed, enter manual mode
  {
    digitalWrite(manualLED, HIGH); //Turn on manual mode LED
    for (int i = 0; i < 250; i++)
    {
      manualMode(); //Run through this for about 30 seconds
    }
  }
  else if (optionNumber == 2) //Otherwise if auto button pushed, enter auto mode
  {
    digitalWrite(autoLED, HIGH);
    //Serial.println("Started automatic mode!");
    automaticMode();
  }
  else //If nothing is pushed, have servos maintain position
  {
    pitchServo.write(posP);
    rollServo.write(posR);
    delay(20);
  }
}

void automaticMode() //Function for automatic mode
{
  individualSample(); //Calls individual sample
  rollServo.write(50); //Moves the servos
  posR = 50;
  for (int i = 0; i < 30; i++) //Using a for loop so limit switches are checked repeatedly
  {
    gantryFB->moveDirectionA(); //Moves multiple motors at a time
    yaw->moveDirectionA();
    delay(100);
  }
  pitchServo.write(100);
  posP = 100;
  rollServo.write(100);
  posR = 100;
  for (int i = 0; i < 30; i++)
  {
    gantryFB->moveDirectionB();
    yaw->moveDirectionB();
    delay(100);
  }
  pitchServo.write(90);
  posP = 90;
  for (int i = 0; i < 30; i++)
  {
    gantryUD->moveDirectionB();
    //gantryLR->moveDirectionA();
    yaw->moveDirectionB();
    delay(100);
  }
  rollServo.write(40);
  posR = 40;
  pitchServo.write(40);
  posP = 60;
  for (int i = 0; i < 30; i++)
  {
    gantryUD->moveDirectionA();
    //gantryLR->moveDirectionB();
    yaw->moveDirectionA();
    delay(100);
  }
}

void individualSample() //Sample of each of the motors for a couple seconds
{
  for (int i = 0; i < 10; i++)
  {
    gantryFB->moveDirectionA(); //Move Direction A for 1 seconds
    delay(100);
  }
  gantryFB->stopMotor(); //Stop
  for (int i = 0; i < 10; i++)
  {
    gantryFB->moveDirectionB(); //Move direction B for 1 seconds
    delay(100);
  }
  gantryFB->stopMotor();
  for (int i = 0; i < 10; i++)
  {
    gantryUD->moveDirectionA(); //Move direction A for 2 seconds
    delay(100);
  }
  gantryUD->stopMotor();
  for (int i = 0; i < 10; i++)
  {
    gantryUD->moveDirectionB(); //Move direction B for 2 seconds
    delay(100);
  }
  gantryUD->stopMotor();
  //Commented out because not working
  /*for (int i = 0; i < 10; i++)
    {
    gantryLR->moveDirectionA(); //Move direction B for 5 seconds
    delay(500);
    }
    gantryLR->stopMotor();
    for (int i = 0; i < 10; i++)
    {
    gantryLR->moveDirectionB(); //Move direction B for 5 seconds
    delay(100);
    }
    gantryLR->stopMotor();
  */
  for (int i = 0; i < 10; i++)
  {
    yaw->moveDirectionA(); //Move direction A for 1 second
    delay(100);
  }
  yaw->stopMotor();
  for (int i = 0; i < 10; i++)
  {
    yaw->moveDirectionB(); //Move direction B for 1 second
    delay(100);
  }
  yaw->stopMotor();
  for (int i = 0; i < 2; i++) //Sweep servos a couple times
  {
    sweepPitchLeft();
    sweepRollLeft();
    sweepPitchRight();
    sweepRollRight();
  }
}

void manualMode() //Manual mode function checks buttons for each axis
{
  //checkGantryLR();
  //delay(20);
  checkGantryFB();
  delay(20);
  checkGantryUD();
  delay(20);
  checkYaw();
  delay(20);
  manualRoll();
  delay(20);
  manualPitch();
  delay(20);
}

void checkGantryUD() //If UD buttons are pushed, moves Up or Down
{
  pitchServo.write(posP); //Maintains servo positions
  rollServo.write(posR);
  if (digitalRead(GUB) == HIGH && digitalRead(GDB) == LOW)
  {
    gantryUD->moveDirectionB();
  }
  else if (digitalRead(GDB) == HIGH && digitalRead(GUB) == LOW)
  {
    gantryUD->moveDirectionA();
  }
  else
  {
    gantryUD->stopMotor();
  }
}

void checkGantryLR() //Same concept as Gantry UD
{
  pitchServo.write(posP);
  rollServo.write(posR);
  if (digitalRead(GLB) == HIGH && digitalRead(GRB) == LOW)
  {
    gantryLR->moveDirectionA();
  }
  else if (digitalRead(GRB) == HIGH && digitalRead(GLB) == LOW)
  {
    gantryLR->moveDirectionB();
  }
  else
  {
    gantryLR->stopMotor();
  }
}

void checkGantryFB() //Read checkGantryUD()
{
  pitchServo.write(posP);
  rollServo.write(posR);
  if (digitalRead(GFB) == HIGH && digitalRead(GBB) == LOW)
  {
    gantryFB->moveDirectionA();
  }
  else if (digitalRead(GBB) == HIGH && digitalRead(GFB) == LOW)
  {
    gantryFB->moveDirectionB();
  }
  else
  {
    gantryFB->stopMotor();
  }
}

void checkYaw() //Read checkGantryUD()
{
  pitchServo.write(posP);
  rollServo.write(posR);
  if (digitalRead(YLB) == HIGH && digitalRead(YRB) == LOW)
  {
    yaw->moveDirectionA();
  }
  else if (digitalRead(YRB) == HIGH && digitalRead(YLB) == LOW)
  {
    yaw->moveDirectionB();
  }
  else
  {
    yaw->stopMotor();
  }
}

void resetAll() //Resets each motor. resetNumber can be used to keep track of # of resets
{
  stopAll();
  //resetNumber = 0;
  //gantryLR->resetMotor();
  //resetNumber = 0;
  gantryUD->resetMotor();
  //resetNumber = 0;
  gantryFB->resetMotor();
  //resetNumber = 0;
  yaw->resetMotor();
  //resetNumber = 0;
}

void stopAll() //Stop all motors
{
  gantryFB->stopMotor();
  //gantryLR->stopMotor();
  gantryUD->stopMotor();
  yaw->stopMotor();
}

int checkButtons()
{
  int manualTimes = 0;
  int automaticTimes = 0;
  for (int i = 0; i < 5; i++) //Checks buttons 5 times, increasing int by 1 if pushed
  {
    if (digitalRead(manualButton) == LOW)
    {
      manualTimes++;
      delay(5);
    }
    else if (digitalRead(automaticButton) == LOW)
    {
      automaticTimes++;
      delay(5);
    }
    else
    {
      delay(5);
    }
  }
  //Serial.println("manualTimes: ");
  //Serial.print(manualTimes);
  //Serial.println("automaticTimes: ");
  //Serial.print(automaticTimes);
  if (manualTimes == 5) //return a value corresponding to button pushed
  {
    return 1;
  }
  else if (automaticTimes == 5)
  {
    return 2;
  }
  else
  {
    return 0;
  }
}

boolean getLEDforServo() //An accessor function to obtain whether the cockpit is being moved
{
  boolean ledState = gantryFB->getLEDM();
  if (ledState == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}
