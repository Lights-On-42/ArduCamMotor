//#include "Comands.h"

class MotorBase {
public:
  void Tick();
  void CheckComand(CommandGCode* com);
  void MakeComand(CommandGCodeForMotor* order);

  void Initializieren(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,int multiplication);
  void InitializierenWithEnableAndHold(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,uint16_t pinEnable, int multiplication);
  void InitializierenWithEnable(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,uint16_t pinEnable, int multiplication);
  void InitializierenWithSleep(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,uint16_t pinSleep, int multiplication);
  void Stop();
  int getPosition();

  uint16_t ModusDesMotors = 0;
  //Modus in dem der Motor gerade fährt
  //0 Motor steht
  //1 Motor bewegt sich im Test Modus
  //2 Motor bewegt sich auf eine Position
  //3 Motor bewegt sich dauerhaft

private:
  uint16_t motorIndex = 0;
  uint16_t motorPinDirection = 0;
  uint16_t motorPinSteps = 0;
  uint16_t motorPinSleep=255;
  uint16_t motorPinEnable=255;

  bool motorHoldEnable=false;
  int motorMultiplication=1;

  bool lastStatus = LOW;
  void CheckEnable();


  int counterIntervall = 0;
  int motorSpeed = 200;    //Geschwindikeit
  int newMotorSpeed = 0;  //fals eine neue Geschwindigkeit runter kommet

  int targetCounter = 0;  //ziel wo der Motor hinfahren soll
  bool forward = false;   //true vorwärts false ruckwärts
  int stepCounter = 0;    //Anzahl an vergangen Schritte hierraus kann man die Position berechnen
};

void MotorBase::Initializieren(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,int multiplication) 
{
  motorIndex = index;
  motorPinDirection = pinDirection;
  motorPinSteps = pinSteps;
  motorMultiplication=multiplication;

  pinMode(motorPinDirection, OUTPUT);
  pinMode(motorPinSteps, OUTPUT);
}

void MotorBase::InitializierenWithEnableAndHold(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,uint16_t pinEnable, int multiplication) 
{
  motorIndex = index;
  motorPinDirection = pinDirection;
  motorPinSteps = pinSteps;
  motorMultiplication=multiplication;
  motorPinEnable=pinEnable;
  motorHoldEnable=true;

  pinMode(motorPinDirection, OUTPUT);
  pinMode(motorPinSteps, OUTPUT);

  pinMode(pinEnable, OUTPUT);

  //Enable wird angeschalten und niewieder aus
  digitalWrite(motorPinEnable, LOW);
}

void MotorBase::InitializierenWithEnable(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,uint16_t pinEnable, int multiplication) 
{
  motorIndex = index;
  motorPinDirection = pinDirection;
  motorPinSteps = pinSteps;
  motorMultiplication=multiplication;
  motorPinEnable=pinEnable;

  pinMode(motorPinDirection, OUTPUT);
  pinMode(motorPinSteps, OUTPUT);

  pinMode(pinEnable, OUTPUT);
  //
  digitalWrite(motorPinEnable, HIGH);
  //digitalWrite(motorPinEnable, LOW);
}

void MotorBase::InitializierenWithSleep(uint16_t index, uint16_t pinDirection, uint16_t pinSteps,uint16_t pinSleep, int multiplication) 
{
  motorIndex = index;
  motorPinDirection = pinDirection;
  motorPinSteps = pinSteps;
  motorMultiplication=multiplication;
  motorPinSleep=pinSleep;

  pinMode(motorPinDirection, OUTPUT);
  pinMode(motorPinSteps, OUTPUT);

  pinMode(motorPinSleep, OUTPUT);
  digitalWrite(motorPinSleep, LOW);
}



void MotorBase::MakeComand(CommandGCodeForMotor* order)
{
  // es muss ein Comando für mein Motor sein
  // Befehl ist stop also motor anhaten
  if (order->whatComand == DriveTypesStop) 
  {
    ModusDesMotors = 0;
    Serial.print("Motor Stop  ");
    Serial.println(motorIndex);
    CheckEnable();
    return;
  }
  if (order->whatComand == DriveTypesDriveWithSpeed) 
  {
    //digitalWrite(motorPinEnable, LOW);
    ModusDesMotors = 3;
    counterIntervall = 0;

    int speedzw = motorMultiplication * order->motorSpeed;

    if (speedzw == 0) 
    {
      ModusDesMotors = 0;
      Serial.print("Motor Stop  by speed 0");
      return;
    }
    if (speedzw < 0) 
    {
      digitalWrite(motorPinDirection, true);
      speedzw = speedzw * -1;
      Serial.print("backwarts ");
    } 
    else 
    {
      Serial.print("forward ");
      digitalWrite(motorPinDirection, false);
    }
    CheckEnable();
    newMotorSpeed=speedzw;
    Serial.print(motorIndex);
    Serial.print(" new speed :  ");
    Serial.println(newMotorSpeed);
    return;
  }
  if (order->whatComand == DriveTypesDriveToPos)
  {
    ModusDesMotors = 2;
    motorSpeed = 300;
    counterIntervall = 0;
    //multiplizieren mit der geänderten motor richtung
    targetCounter = order->motorTarget* motorMultiplication;
    if (targetCounter > stepCounter) 
    {
      forward = true;
      digitalWrite(motorPinDirection, true);
    } 
    else if (targetCounter < stepCounter) 
    {
      forward = false;
      digitalWrite(motorPinDirection, false);
    } 
    else
    {
      Serial.println("Motor am ziel");
    }
    CheckEnable();
    return;
  }
  if (order->whatComand == DriveTypesDriveAddPos)
  {
    ModusDesMotors = 2;
    motorSpeed = 300;
    counterIntervall = 0;
    targetCounter = targetCounter + (order->motorTarget* motorMultiplication*-1);
    if (targetCounter > stepCounter) 
    {
      forward = true;
      digitalWrite(motorPinDirection, true);
    } 
    else if (targetCounter < stepCounter) 
    {
      forward = false;
      digitalWrite(motorPinDirection, false);
    } 
    else
    {
      Serial.println("Motor am ziel");
    }
    CheckEnable();
    return;
  }
  if (order->whatComand == DriveTypesGetActiveParams)
  {
    Serial.print("ModusDesMotors :  ");
    Serial.println(ModusDesMotors);
    Serial.print("Motor steps :  ");
    Serial.println(stepCounter);
    return;
  }
}

void MotorBase::CheckComand(CommandGCode* com) 
{
  int speedzw = 0;
  int it=0;

  //wir schauen nach ob es ein comando für diesen Motor gibt
  for (auto n : com->Commands)
  {
    if(n.motorNumber==motorIndex)
    {
      //Serial.println("Hab einen Befehl");
      MakeComand(&n);
      //den richtigen löschen
      com->Commands.erase(com->Commands.begin()+it);
      return;
    }
    it++;
  }
}

int MotorBase::getPosition() 
{
  return stepCounter;
}

void MotorBase::Stop() 
{
  ModusDesMotors = 0;
  CheckEnable();
}

///Muss zeitlich immer exakt abgearbeitet werden
void MotorBase::Tick() 
{
  if (ModusDesMotors == 0) 
  {
    if(motorPinSleep!=255)
    {
      digitalWrite(motorPinSleep, LOW);
    }
    return;  //Motor steht nix machen
  }

  if(motorPinSleep!=255)
  {
    digitalWrite(motorPinSleep, HIGH);
  }

  counterIntervall++;
  if (counterIntervall == motorSpeed)  // es ist zeit zum schalten
  {
    digitalWrite(motorPinSteps, lastStatus);

    //Serial.println(counterIntervall);
    //Serial.println(motorSpeed);
    //Serial.println(stepCounter);
    //Serial.println(targetCounter);
    //Serial.println(ModusDesMotors);

    counterIntervall = 0;

    if (lastStatus == LOW){
      lastStatus = HIGH;
    } else {
      lastStatus = LOW;
    }

    if (forward){
      stepCounter++;
    } else {
      stepCounter--;
    }

    if (stepCounter == targetCounter && ModusDesMotors == 2) 
    {
      Serial.print("Motor am ziel bewegung  -->");
      Serial.println(stepCounter);
      ModusDesMotors = 0;
    }

    if (ModusDesMotors == 3 && newMotorSpeed != 0) 
    {
      //Serial.print("new Motorspeed  ");
      //Serial.println(newMotorSpeed);
      motorSpeed = newMotorSpeed;
      newMotorSpeed = 0;
    }
  }
}

void MotorBase::CheckEnable()
{
  //beim halten wird das enable nie abgeschaltet
  if(motorHoldEnable==true)
  {
    return;
  }
  //wir haben zu fahren also enable setzen
  if(ModusDesMotors!=0)
  {
    //Serial.print("Motor StartE  ");
    digitalWrite(motorPinEnable, LOW);
    return;
  }
  //Serial.print("Motor StoppE");
  //wir fahren nicht also raus
  digitalWrite(motorPinEnable, HIGH);
}
