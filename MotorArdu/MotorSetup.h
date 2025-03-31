MotorBase motor1;
MotorBase motor2;
TaskHandle_t Task2;
Comands orderComands;

// Funktion für Task2
void TaskForMotor( void * parameter )
{
  CommandGCode command;

  for(;;)
  {
    if(newComand)
    {
      //Serial.println(zwComand);
      orderComands.AnalyseOrder(zwComand,&command);
      motor1.CheckComand(&command);
      motor2.CheckComand(&command);

      if(command.Commands.size()!=0)
      {
        Serial.print(" count ");
        Serial.println(command.Commands.size());
        command.Commands.clear();
      }
      zwComand="";
      newComand=false;
    }

    orderComands.CheckForAutoPowerON();

    motor1.Tick();
    motor2.Tick();
    orderComands.Tick();
    delayMicroseconds(1);
  }
}

void MotorSetup()
{
  //P4
  //orderComands.initializePin(5);
  //orderComands.initializePin(4);
  //orderComands.initializePin(36);
  //orderComands.initializePin(32);
  //orderComands.initializePin(25);
  
  orderComands.initializeSleepPin(19);
  
  //Motor 1 (1, 4,5,6, 1); 6 OK
  orderComands.initializePin(4);  //Motor1 Dir
  orderComands.initializePin(5);  //Motor1 Step
  orderComands.initializePin(6);  //M1-Enable
  
  //Motor 2 (2, 2,1,42, 1); 42 OK
  orderComands.initializePin(2);  //Motor2 Dir
  orderComands.initializePin(1);  //Motor2 Step
  orderComands.initializePin(42);  //M2-Enable

  //Motor 3 (1, 7,15,16, 1); 6 OK
  orderComands.initializePin(7);   //Motor3 Dir
  orderComands.initializePin(15);  //Motor3 Step
  orderComands.initializePin(16);  //M3-Enable

  //Motor 4 (1, 39,40,41, 1); 42 OK
  orderComands.initializePin(39);  //Motor4 Dir
  orderComands.initializePin(40);  //Motor4 Step
  orderComands.initializePin(41);  //M4N-Enable

  //Motor 5 (1, 17,18,8, 1); 3 OK
  orderComands.initializePin(17);  //Motor5 Dir
  orderComands.initializePin(18);   //Motor5 Step
  orderComands.initializePin(8);   //M5-Enable

  //Motor 6 (1, 37,38,36, 1); 35 OK
  orderComands.initializePin(37);  //Motor6 Dir
  orderComands.initializePin(38);   //Motor6 Step
  orderComands.initializePin(36);   // M6N-Enable

  //Motor 7 (1, 3,9,14, 1); 3
  orderComands.initializePin(3);   //Motor7 Dir
  orderComands.initializePin(9);  //Motor7 Step
  orderComands.initializePin(14);   //M7N-Enable

  //Motor 8 (1, 21,47,20, 1); 35 OK
  orderComands.initializePin(21);   //Motor8 Dir
  orderComands.initializePin(47);   //Motor8 Step
  orderComands.initializePin(20);   //M8N-Enable



  motor1.InitializierenWithEnable(1, 3,9,14, 1);
  motor2.InitializierenWithEnable(2, 21,47,20, -1);


  orderComands.AddMotorModus(&motor1.ModusDesMotors);
  orderComands.AddMotorModus(&motor2.ModusDesMotors);

  Serial.println("Motor finished");

  //String commandText="M03 x2000";
  //CommandGCode command;
  //orderComands.AnalyseOrder(commandText,&command);
  //motor1.CheckComand(&command);

  xTaskCreatePinnedToCore(
                    TaskForMotor,   // Task-Funktion
                    "Task2",     // Name der Task
                    10000,       // Stack-Größe
                    NULL,        // Parameter
                    1,           // Priorität
                    &Task2,      // Task-Handle
                    1);          // Core-ID
}

