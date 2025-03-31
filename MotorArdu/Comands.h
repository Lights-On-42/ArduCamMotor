enum DriveTypes
{
  DriveTypesStop=0,
  DriveTypesDriveToPos = 1, 
  DriveTypesDriveWithSpeed =2, 
  DriveTypesGetActiveParams =3,
  DriveTypesDriveAddPos = 4, 
};

struct CommandGCodeForMotor
{
  DriveTypes whatComand;
  int motorNumber;

  int motorTarget;
  int motorSpeed;
};

struct CommandGCode
{
  std::vector<CommandGCodeForMotor> Commands;
};

// testmode
// testpin-4,1;
// testpin-5,1;
// testto-6,200,500;
// newspeed-1,200;

//MotorName MotorNummer
//x         1
//y         2
//z         3
//a         4
//b         5
//c         6

//G00 --> um Positionen fahren
//G14 --> auf pos fahren
//M03 --> vorwärts


// F+Motorname(speed)
// Motorname(zielpos)

class Comands
{
    public:
      void AnalyseOrder(String Order,CommandGCode* com);
      void Tick();
      //Automodus
      void CheckForAutoPowerON();
      //External
      void PowerON();
      void PowerOff();


      void initializeSleepPin(uint16_t indexSleepPin);
      void initializePin(uint16_t indexPin);
      void AddMotorModus(uint16_t* modusNewMotor);

    private:
      int charToMotorNumber(char b);
      int tooglePIN=-1;
      long activeSteeps=0;
      int steepsMax=0;
      bool lastStatus=LOW;
      int counterIntervall=0;
      uint16_t interval_us = 1;

      //autoPower on
      bool testMode=false;
      bool lastStatusOfAutoPower=false;

      uint16_t sleepPin = 0;
      std::vector<uint16_t*> motorTreiberModus;
      std::vector<CommandGCode> commandsToWork;
};

void Comands::initializePin(uint16_t indexPin) 
{
  pinMode(indexPin,OUTPUT);
  digitalWrite(indexPin, LOW);
}

void Comands::initializeSleepPin(uint16_t indexSleepPin) 
{
  sleepPin=indexSleepPin;
  pinMode(sleepPin,OUTPUT);
  digitalWrite(sleepPin, LOW);
}

void Comands::AddMotorModus(uint16_t* modusNewMotor) 
{
  motorTreiberModus.push_back(modusNewMotor);
}

void Comands::Tick()
{
  //ob ein test pin hin und her springen soll
  if(tooglePIN!=-1)
  {
    counterIntervall++;
    if(counterIntervall==interval_us)
    {
      counterIntervall=0;
      digitalWrite(tooglePIN, lastStatus);
      activeSteeps++;
      if(lastStatus==LOW)
      {
        lastStatus=HIGH;
      }
      else
      {
        lastStatus=LOW;
      }
      if(steepsMax!=-1)
      {
        if(activeSteeps>steepsMax)
        {
          tooglePIN=-1;
          activeSteeps=0;
          Serial.println("Test Toggle Pin Finshed");
        }
      }
    }
  }
}

void Comands::PowerON()
{
  testMode=true;
  if(lastStatusOfAutoPower==true)
  {
    //nix zu tun ist alles schon so
  }
  else
  {
    lastStatusOfAutoPower=true;
    Serial.println("Auto Power on");
    digitalWrite(sleepPin, HIGH);
  }
}

void Comands::PowerOff()
{
  if(lastStatusOfAutoPower==false)
  {
    //nix zu tun ist alles schon so
  }
  else
  {
    lastStatusOfAutoPower=false;
    //Serial.println("Auto Power off");
    digitalWrite(sleepPin, LOW);
  }
}

void Comands::CheckForAutoPowerON()
{ 
  if(testMode==false)
  {
    bool sleepOn=false;
    for (uint16_t* n : motorTreiberModus)
    {
      if(*n!=0)
      {
        sleepOn=true;
      }
    }
    if(sleepOn==false)
    {
      if(lastStatusOfAutoPower==false)
      {
        //nix zu tun ist alles schon so
      }
      else
      {
        lastStatusOfAutoPower=false;
        //Serial.println("Auto Power off");
        digitalWrite(sleepPin, LOW);
      }
    }
    else
    {
      if(lastStatusOfAutoPower==true)
      {
        //nix zu tun ist alles schon so
      }
      else
      {
        lastStatusOfAutoPower=true;
        //Serial.println("Auto Power on");
        digitalWrite(sleepPin, HIGH);
      }
    }
  }
}

void Comands::AnalyseOrder(String Order,CommandGCode* com)
{
  bool WriteOrder=true;
  if(Order.length()==0)
  {
    return;
  }
  //Serial.print("I received: ");
  
  
  if(Order.indexOf("testmode") == 0)
  {
    WriteOrder=false;
    Serial.println("Testmode online");
    testMode=true;
  }
  if(Order.indexOf("testpin") == 0)
  {
    WriteOrder=false;
    Serial.println(Order);
    //seperate nachricht
    // "Comando"-"pinnumber","0oder1"
    int cmdPos = Order.indexOf("-");
    int pinNummerPos = Order.indexOf(",");
  
    String cmdText = Order.substring(0,cmdPos);
    String pinNummerText= Order.substring(cmdPos + 1, pinNummerPos);
    String zustandsText = Order.substring(pinNummerPos + 1, Order.length()-1);
    
    int pinNummer = pinNummerText.toInt();
    int highorlow = zustandsText.toInt();
    Serial.println(pinNummer);
    Serial.println(highorlow);
    if(highorlow==0)
    {
      pinMode(pinNummer, OUTPUT);
      digitalWrite(pinNummer, LOW);
    }
    if(highorlow==1)
    {
      pinMode(pinNummer, OUTPUT);
      digitalWrite(pinNummer, HIGH);
    }
    
  }
  if(Order.indexOf("testto") == 0)
  {
    WriteOrder=false;
    Serial.println(Order);
    //seperate nachricht
    // "Comando"-"pinnumber","speed","anzahlsteps"
    int cmdPos = Order.indexOf("-");
    int pinNummerPos = Order.indexOf(",");
    int speedNummerPos = Order.indexOf(",",pinNummerPos+1);
  
    String cmdText = Order.substring(0,cmdPos);
    String pinNummerText= Order.substring(cmdPos + 1, pinNummerPos);
    String speedText= Order.substring(pinNummerPos + 1, speedNummerPos);
    String steepsText = Order.substring(speedNummerPos + 1, Order.length()-1);
    
    int pinNummer = pinNummerText.toInt();
    int speedTimer = speedText.toInt();        
    int steepsNumber = steepsText.toInt();
    Serial.println(pinNummerText);
    Serial.println(speedTimer);
    Serial.println(steepsNumber);
    tooglePIN=pinNummer;
    interval_us=speedTimer;    
    steepsMax= steepsNumber;
    return;
  }
  if(Order.indexOf("PowerDown") == 0)
  {
    PowerOff();
    SetStopped();
    return;
  }
  if(Order.indexOf("PowerON") == 0)
  {
    PowerON();
    SetIDLE();
    return;
  }
  if(Order.indexOf("stop") == 0)
  {
    WriteOrder=false;
    Serial.println(Order);

    //für alle Motoren von 1 bis 6 alle stopen
    for(int i=1;i<7;i++)
    {
      CommandGCodeForMotor newOrder;
      newOrder.whatComand=DriveTypesStop;
      newOrder.motorTarget=0;
      newOrder.motorSpeed=0;
      newOrder.motorNumber=i;
      com->Commands.push_back(newOrder);
    }

  }
  if(Order.indexOf("M03") == 0)
  { 
    WriteOrder=false;
    Serial.println(Order);
    int comandPosStart=4;
    int comandPosStop=0; 
    String TextMotor="";
    String NummerSpeed="";
    do
    { 
      comandPosStop = Order.indexOf(" ",comandPosStart);
      
      if(comandPosStop==-1)
      {
        TextMotor = Order.substring(comandPosStart, comandPosStart+1);
        NummerSpeed = Order.substring(comandPosStart+1, Order.length());
        Serial.println(TextMotor);
        Serial.println(NummerSpeed);
      }
      else
      {
        TextMotor = Order.substring(comandPosStart, comandPosStop);
        NummerSpeed = Order.substring(comandPosStart+1, comandPosStop);
        comandPosStart=comandPosStop+1;
      }

      CommandGCodeForMotor newOrder;
      newOrder.whatComand=DriveTypesDriveWithSpeed;
      newOrder.motorNumber=charToMotorNumber(TextMotor[0]);
      newOrder.motorTarget=0;
      newOrder.motorSpeed=NummerSpeed.toInt();
      
      com->Commands.push_back(newOrder);      
    }while(comandPosStop!=-1);
    return;
  }
  //add Position
  if(Order.indexOf("G00") == 0)
  { 
    WriteOrder=false;
    Serial.println(Order);
    int comandPosStart=4;
    int comandPosStop=0; 
    String TextMotor="";
    String NummerSpeed="";
    
    do
    { 
      comandPosStop = Order.indexOf(" ",comandPosStart);
      if(comandPosStop==-1)
      {
        TextMotor = Order.substring(comandPosStart, comandPosStart+1);
        NummerSpeed = Order.substring(comandPosStart+1, Order.length());
      }
      else
      {
        TextMotor = Order.substring(comandPosStart, comandPosStop);
        NummerSpeed = Order.substring(comandPosStart+1, comandPosStop);
        comandPosStart=comandPosStop+1;
      }

      CommandGCodeForMotor newOrder;
      newOrder.whatComand=DriveTypesDriveAddPos;
      newOrder.motorNumber=charToMotorNumber(TextMotor[0]);
      newOrder.motorTarget=NummerSpeed.toInt();
      newOrder.motorSpeed=0;
      
      com->Commands.push_back(newOrder);      
    }while(comandPosStop!=-1);

    return;
  }
  //to Position
  if(Order.indexOf("G14") == 0)
  { 
    WriteOrder=false;
    Serial.println(Order);
    int comandPosStart=4;
    int comandPosStop=0; 
    String TextMotor="";
    String NummerSpeed="";
    
    do
    { 
      comandPosStop = Order.indexOf(" ",comandPosStart);
      if(comandPosStop==-1)
      {
        TextMotor = Order.substring(comandPosStart, comandPosStart+1);
        NummerSpeed = Order.substring(comandPosStart+1, Order.length());
      }
      else
      {
        TextMotor = Order.substring(comandPosStart, comandPosStop);
        NummerSpeed = Order.substring(comandPosStart+1, comandPosStop);
        comandPosStart=comandPosStop+1;
      }

      CommandGCodeForMotor newOrder;
      newOrder.whatComand=DriveTypesDriveToPos;
      newOrder.motorNumber=charToMotorNumber(TextMotor[0]);
      newOrder.motorTarget=NummerSpeed.toInt();
      newOrder.motorSpeed=0;
      
      com->Commands.push_back(newOrder);      
    }while(comandPosStop!=-1);

    return;
  }

  if(WriteOrder)
  {
    Serial.print("Error Befehl kenne ich nicht");
    Serial.println(Order);
  }
}

int Comands::charToMotorNumber(char b)
{
  if(b=='x')
  {
    return 1;
  }
  if(b=='y')
  {
    return 2;
  }
  if(b=='z')
  {
    return 3;
  }
  if(b=='a')
  {
    return 4;
  }
  Serial.print("Falsche Motor nummer :");
  Serial.println(b);
  return 99;
}
