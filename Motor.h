//File for class Motor

//int resetNumber = 0;
void resetAll();
void stopAll();
boolean getLED();

//Motors (5 of them). Motor-driver works with 1 PWM pin per motor and 2 direction pins.
//Motor Directions
const int motorGF = 47;
const int motorGB = 48;
const int motorGL = 16;
const int motorGR = 17;
const int motorGD = 19;
const int motorGU = 18;
const int motorYL = 22;
const int motorYR = 23;

const int cockpitLED = 51;

class Motor
{
    boolean IN_RESET;
    int directionA; //Directions for the motor to go, an output on the Arduino
    int directionB;
    int limitSwitchA; //limit switches, an input_pullup
    int limitSwitchB;
    int motorSpeed; //Speed, and PWM pin
    int speedPin;

    int MAX_SPEED = 255;
    int MIN_SPEED = 0;
    int DEFAULT_SPEED = 127;

  public:
    Motor(int dA, int dB, int sP, int lSA, int lSB, int mS) //Constructor
    {
      directionA = dA;
      directionB = dB;
      speedPin = sP;
      limitSwitchA = lSA;
      limitSwitchB = lSB;
      if (mS <= MAX_SPEED && mS >= MIN_SPEED)
      {
        motorSpeed = mS;
      }
      else
      {
        motorSpeed = DEFAULT_SPEED;
      }
      IN_RESET = false;
    }

    void moveDirectionA()
    {
      pitchServo.write(posP);
      rollServo.write(posR);
      if (digitalRead(limitSwitchA) == LOW && IN_RESET == false) //If limit switches are touched, then reset
      {
        IN_RESET = true;
        //Serial.println("Resetting.");
        resetMotor();
      }
      else //Otherwise move
      {
        digitalWrite(directionA, HIGH);
        digitalWrite(directionB, LOW);
        analogWrite(speedPin, motorSpeed);
        //Serial.println("Moving in direction A");
        //delay(1);
      }
      //Check if cockpit is moving, and turn on LED if it is
      if (digitalRead(motorYL) == HIGH || digitalRead(motorYR) == HIGH || getLED() == true)
      {
        digitalWrite(cockpitLED, HIGH);
      }
      else if (digitalRead(motorYL) == LOW && digitalRead(motorYR) == LOW && getLED() == false)
      {
        digitalWrite(cockpitLED, LOW);
      }
    }

    void moveDirectionB()
    {
      pitchServo.write(posP);
      rollServo.write(posR);
      if (digitalRead(limitSwitchB) == LOW && IN_RESET == false) //If limit switches are touched, then reset
      {
        IN_RESET = true;
        //Serial.println("Resetting.");
        resetMotor();
      }
      else //Otherwise move
      {
        digitalWrite(directionB, HIGH);
        digitalWrite(directionA, LOW);
        analogWrite(speedPin, motorSpeed);
        //Serial.println("Moving in direction B");
        //delay(1);
      }
      if (digitalRead(motorYL) == HIGH || digitalRead(motorYR) == HIGH || getLED() == true)
      {
        digitalWrite(cockpitLED, HIGH);
      }
      else if (digitalRead(motorYL) == LOW && digitalRead(motorYR) == LOW && getLED() == false)
      {
        digitalWrite(cockpitLED, LOW);
      }
    }

    void stopMotor() //Function to stop motor motion (turns off motors)
    {
      pitchServo.write(posP);
      rollServo.write(posR);
      digitalWrite(directionA, LOW);
      digitalWrite(directionB, LOW);
      if (digitalRead(motorYL) == HIGH || digitalRead(motorYR) == HIGH || getLED() == true)
      {
        digitalWrite(cockpitLED, HIGH);
      }
      else if (digitalRead(motorYL) == LOW && digitalRead(motorYR) == LOW && getLED() == false)
      {
        digitalWrite(cockpitLED, LOW);
      }
    }

    void resetMotor() //Function to reset the gantry to the starting position
    {
      stopAll();
      pitchServo.write(posP);
      rollServo.write(posR);
      boolean reset;
      reset = false;
      //resetNumber++;
      while (reset == false) //While still resetting
      {
        if (digitalRead(limitSwitchA) == LOW) //If hits the limit switch, stops
        {
          stopMotor();
          //Serial.println("Reset accomplished");
          reset = true;
          continue;
        }
        else //Otherwise keep moving towards the limit switch
        {
          //Serial.println("Still trying to reset");
          moveDirectionA();
          delay(250);
        }
      }
      //Serial.println("Moving left for two seconds after reset.");
      //Moves the other direction for 2 seconds, then exits reset
      stopMotor();
      moveDirectionB();
      delay(2000);
      stopMotor();
      IN_RESET = false;
      /*if (resetNumber >= 5)
        {
        resetAll();
        }*/
    }

    boolean getLEDM() //if yaw is moving, return true
    {
      if ((digitalRead(motorYR) == HIGH) || (digitalRead(motorYL) == HIGH))
      {
        return true;
      }
      else
      {
        return false;
      }
    }

};
