
#if defined(ESP8266)
  #pragma message "ESP8266 stuff happening!"
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);
  const int CS = 16;
#elif defined(ESP32)
  #pragma message "ESP32 stuff happening!"
  #include <WiFi.h>
  #include <WebServer.h>
  WebServer server(80);
  const int CS = 10;
#else
#error "This ain't a ESP8266 or ESP32, dumbo!"
#endif


//Farbcode
//rot  Error oder hochfahren                --> Abort
//gelb LadeModus                            --> Stopped
//blau Lademodus aus                        --> Idle
//grün bewegung                             --> Execute

bool newComand=false;
String zwComand;

#include "Status.h"
#include <Arduino.h>
#include <DNSServer.h>
#include "Arducam_Mega.h"
#include "Index.html.h"
#include "Library.html.h"
#include "WifiCam.h"

#include "Comands.h"
#include "MotorBase.h"
#include "MotorSetup.h"


void setup() 
{
  Serial.begin(115200);
  Serial.println("Start Car Communikation");

  SetupStatusLED();
  SetAbort();

  ShowALLWIFILan();

  loginInspecifictWLan("test","test");
  hotspot("Car3","",3);

  SetupWiFi();
  //SetupWiFiCam();

  MotorSetup();

  IPAddress apIP = WiFi.softAPIP();
  if(WiFi.status() == WL_CONNECTED || apIP != IPAddress(0, 0, 0, 0)) 
  {
    SetStopped();
  }
  else
  {
    Serial.println("Start failed");
  }

  //delay(1000);
  //orderComands.PowerON();
  //SetIDLE();
}



void loop() 
{
 
}
