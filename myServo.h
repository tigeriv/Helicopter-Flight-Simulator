//Creating the servo to control pitch
Servo pitchServo; //creates servo object
Servo rollServo;
int DEFAULT_POSITION_R = 58; //Default position
int DEFAULT_POSITION_P = 95;
int posR = DEFAULT_POSITION_R;//roll position
int posP = DEFAULT_POSITION_P; //pitch position
int MIN_POS_P = 57;
int MAX_POS_P = 130;
int MIN_POS_R = 18;
int MAX_POS_R = 100;
const int rollPin = 20; //Servo pin
const int pitchPin = 21; //Servo pin
const int potPinRoll = 0; //analog pins for potentiometers
const int potPinPitch = 1;
int valR; //values from analog pins
int valP;
const int sCockpitLED = 51; //servo cockpit LED

const int numReadings = 10; //Not being used, but is an array used to calculate an average pot reading
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


const int rollLeft = 49; //Buttons to move the servos in manual mode
const int rollRight = 50;
const int pitchLeft = 52;
const int pitchRight = 53;

boolean movingR = false; //For deciding to turn on/off the LED
boolean movingP = false;

boolean getLEDforServo();

void sweepPitchLeft() //Sweeps servo left full ROM
{
  digitalWrite(sCockpitLED, HIGH);
  //Serial.println("Moving to min position");
  pitchServo.write(MIN_POS_P);
  int delayTime = (posP - MIN_POS_P) * 10;
  //Serial.println("Delay time: ");
  //Serial.print(delayTime);
  //Serial.println("PosP: ");
  //Serial.print(posP);
  delay(delayTime);
  //Serial.println("Position P is: " + posP);
  //Serial.println("Moving left.");
  for (int pos = MIN_POS_P; pos <= MAX_POS_P; pos += 1)
  {
    pitchServo.write(pos);
    delay(10);
    posP = pos;
  }
  //Serial.println("Sweeping pitch left.");
  digitalWrite(sCockpitLED, LOW);
}

void sweepRollLeft() //Sweeps servo left full ROM
{
  digitalWrite(sCockpitLED, HIGH);
  rollServo.write(MIN_POS_R);
  delay((posR - MIN_POS_R) * 10);
  for (int pos = MIN_POS_R; pos <= MAX_POS_R; pos += 1)
  {
    rollServo.write(pos);
    delay(10);
    posR = pos;
  }
  //Serial.println("Sweeping roll left.");
  digitalWrite(sCockpitLED, LOW);
}

void sweepPitchRight() //Sweeps servo right full ROM
{
  digitalWrite(sCockpitLED, HIGH);
  pitchServo.write(MAX_POS_P);
  int delayTime = (MAX_POS_P - posP) * 10;
  delay(delayTime);
  //Serial.println("Sweeping pitch right.");
  for (int pos = MAX_POS_P; pos >= MIN_POS_P; pos -= 1)
  {
    pitchServo.write(pos);
    delay(10);
    posP = pos;
  }
  digitalWrite(sCockpitLED, LOW);
}

void sweepRollRight() //Sweeps servo right full ROM
{
  digitalWrite(sCockpitLED, HIGH);
  rollServo.write(MAX_POS_R);
  delay((MAX_POS_R - posR) * 10);
  for (int pos = MAX_POS_R; pos >= MIN_POS_R; pos -= 1)
  {
    rollServo.write(pos);
    delay(10);
    posR = pos;
  }
  //Serial.println("Sweeping roll right.");
  digitalWrite(sCockpitLED, LOW);
}

void resetRoll()
{
  digitalWrite(sCockpitLED, HIGH);
  rollServo.write(DEFAULT_POSITION_R);
  posR = DEFAULT_POSITION_R;
  delay(500);
  digitalWrite(sCockpitLED, LOW);
}

void resetPitch()
{
  digitalWrite(sCockpitLED, HIGH);
  pitchServo.write(DEFAULT_POSITION_P);
  posP = DEFAULT_POSITION_P;
  delay(500);
  digitalWrite(sCockpitLED, LOW);
}

/*
  int smooth(int inputPin) //calculates an average of 10 reads
  {
  //Set readings to 0
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = 0;
  }
  total = 0;
  readIndex = 0;
  for (int i = 0; i < numReadings; i++) //do for every position in the array
  {
      // read from the sensor:
    readings[readIndex] = analogRead(inputPin);
    // add the reading to the total:
    total = total + readings[readIndex];
    // advance to the next position in the array:
    readIndex = readIndex + 1;
    delay(1);
  }
  // calculate the average:
  average = total / numReadings;
  delay(1);        // delay in between reads for stability
  return average;
  }


  void manualRollPot()
  {
  valR = smooth(potPinRoll);            // reads the value of the potentiometer (value between 0 and 1023)
  //Serial.println("valR =  ");
  //Serial.println(valR);
  valR = map(valR, 0, 1023, MIN_POS_R, MAX_POS_R);     // scale it to use it with the servo (value between 0 and 180)
  if (posR - valR >= 2 || valR - posR >= 2)
  {
    rollServo.write(valR);                  // sets the servo position according to the scaled value
    pitchServo.write(posP);
    digitalWrite(sCockpitLED, HIGH);
    if (valR > posR)
    {
      delay((valR - posR) * 4);
    }
    else
    {
      delay((posR - valR) * 4);
    }
    delay(15);
    digitalWrite(sCockpitLED, LOW);
    posR = valR;
    //Serial.println("Manually moved roll to ");
    //Serial.println(posR);
  }
  }

  void manualPitchPot()
  {
  valP = smooth(potPinPitch);            // reads the value of the potentiometer (value between 0 and 1023)
  //Serial.println("valP =  ");
  //Serial.println(valP);
  valP = map(valP, 0, 1023, MIN_POS_P, MAX_POS_P);     // scale it to use it with the servo (value between 0 and 180)
  if (posP - valP >= 2 || valP - posP >= 2)
  {
    pitchServo.write(valP);                  // sets the servo position according to the scaled value
    rollServo.write(posR);
    digitalWrite(sCockpitLED, HIGH);
    if (valP > posP)
    {
      delay((valP - posP) * 4);
    }
    else
    {
      delay((posP - valP) * 4);
    }
    delay(15);
    digitalWrite(sCockpitLED, LOW);
    posP = valP;
    //Serial.println("Manually moved pitch to ");
    //Serial.println(posP);
  }
  }
*/

void manualPitch()
{
  if (digitalRead(pitchLeft) == HIGH && digitalRead(pitchRight) == LOW)
  {
    if (posP - 4 < MAX_POS_P && posP - 4 > MIN_POS_P) //Move if desired position is allowed
    {
      movingP = true;
      posP = posP - 4;
    }
  }
  else if (digitalRead(pitchRight) == HIGH && digitalRead(pitchLeft) == LOW)
  {
    if (posP + 4 < MAX_POS_P && posP + 4 > MIN_POS_P) //Move if desired position is allowed
    {
      movingP = true;
      posP = posP + 4;
    }
  }
  else
  {
    movingP = false; //otherwise don't move
  }
  if (movingP == true || movingR == true || getLEDforServo() == true)
  {
    digitalWrite(sCockpitLED, HIGH); //Turns on LED if cockpit is moving
  }
  else if (movingP == false && movingR == false && getLEDforServo() == false)
  {
    digitalWrite(sCockpitLED, LOW);
  }
  pitchServo.write(posP); //Maintains servo positions
  rollServo.write(posR);
  delay(15);
}

void manualRoll() //same as manualPitch()
{
  if (digitalRead(rollLeft) == HIGH && digitalRead(rollRight) == LOW)
  {
    if (posR - 4 < MAX_POS_R && posR - 4 > MIN_POS_R)
    {
      movingR = true;
      posR = posR - 4;
    }
  }
  else if (digitalRead(rollRight) == HIGH && digitalRead(rollLeft) == LOW)
  {
    if (posR + 4 < MAX_POS_R && posR + 4 > MIN_POS_R)
    {
      movingR = true;
      posR = posR + 4;
    }
  }
  else
  {
    movingR = false;
  }
  if (movingR == true || movingP == true || getLEDforServo() == true)
  {
    digitalWrite(sCockpitLED, HIGH);
  }
  else if (movingR == false && movingP == false && getLEDforServo() == false)
  {
    digitalWrite(sCockpitLED, LOW);
  }
  pitchServo.write(posP);
  rollServo.write(posR);
  delay(15);
}

boolean getLED()
{
  if (movingR == true || movingP == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}
