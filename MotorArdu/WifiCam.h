#include <ArduinoWebsockets.h>
#include <esp_wifi.h>

WiFiClient client;
DNSServer dnsServer;

#define READ_IMAGE_LENGTH           255
Arducam_Mega myCAM(CS);
bool streamStart=false;
ArducamCamera* cameraInstance;
unsigned long startMillis = 0;

TaskHandle_t Task1;

using namespace websockets;
WebsocketsServer WebSocketServer;

bool wasConnectet= false;
std::vector<WebsocketsClient> all_clients;
////////////////////////////////////////////
//Node MCU 1.0 Modul
/*
Camera pin    Development board pins
VCC           3V3
GND           GND
SCK           D05 14
MISO          D06 12
MOSI          D07 13
CS            D0  16*/

////////////////////////////////////////////
//esp32S3 Dev Modul
/*
Camera pin    Development board pins
VCC           3V3
GND           GND
SCK           12
MISO          13
MOSI          11
CS            10
    */
/*
    CAM_CONTRAST_LEVEL_MINUS_3 = 6, <Level -3 
    CAM_CONTRAST_LEVEL_MINUS_2 = 4, <Level -2 
    CAM_CONTRAST_LEVEL_MINUS_1 = 2, <Level -1 
    CAM_CONTRAST_LEVEL_DEFAULT = 0, <Level Default
    CAM_CONTRAST_LEVEL_1       = 1, <Level +1 
    CAM_CONTRAST_LEVEL_2       = 3, <Level +2 
    CAM_CONTRAST_LEVEL_3       = 5, <Level +3 
} CAM_CONTRAST_LEVEL;*/
    /*
typedef enum {
    CAM_STAURATION_LEVEL_MINUS_3 = 6, <Level -3 
    CAM_STAURATION_LEVEL_MINUS_2 = 4, <Level -2 
    CAM_STAURATION_LEVEL_MINUS_1 = 2, <Level -1 
    CAM_STAURATION_LEVEL_DEFAULT = 0, <Level Default
    CAM_STAURATION_LEVEL_1       = 1, <Level +1 
    CAM_STAURATION_LEVEL_2       = 3, <Level +2 
    CAM_STAURATION_LEVEL_3       = 5, <Level +3 
} CAM_STAURATION_LEVEL;*/

/*
typedef enum {
    CAM_IMAGE_MODE_QQVGA  = 0x00,  //<160x120
    CAM_IMAGE_MODE_QVGA   = 0x01,  //<320x240
    CAM_IMAGE_MODE_VGA    = 0x02,  //<640x480
    CAM_IMAGE_MODE_SVGA   = 0x03,  //<800x600
    CAM_IMAGE_MODE_HD     = 0x04,  //<1280x720
    CAM_IMAGE_MODE_SXGAM  = 0x05,  //<1280x960
    CAM_IMAGE_MODE_UXGA   = 0x06,  //<1600x1200
    CAM_IMAGE_MODE_FHD    = 0x07,  //<1920x1080
    CAM_IMAGE_MODE_QXGA   = 0x08,  //<2048x1536
    CAM_IMAGE_MODE_WQXGA2 = 0x09,  //<2592x1944
    CAM_IMAGE_MODE_96X96  = 0x0a,  //<96x96
    CAM_IMAGE_MODE_128X128 = 0x0b, //<128x128
    CAM_IMAGE_MODE_320X320 = 0x0c, //<320x320
} CAM_IMAGE_MODE;

HIGH_QUALITY    = 0,
    DEFAULT_QUALITY = 1,
    LOW_QUALITY     = 2,
} IMAGE_QUALITY;

*/

/*typedef enum {
    CAM_BRIGHTNESS_LEVEL_MINUS_4 = 8, 
    CAM_BRIGHTNESS_LEVEL_MINUS_3 = 6, 
    CAM_BRIGHTNESS_LEVEL_MINUS_2 = 4, 
    CAM_BRIGHTNESS_LEVEL_MINUS_1 = 2, 
    CAM_BRIGHTNESS_LEVEL_DEFAULT = 0, 
    CAM_BRIGHTNESS_LEVEL_1       = 1, 
    CAM_BRIGHTNESS_LEVEL_2       = 3, 
    CAM_BRIGHTNESS_LEVEL_3       = 5, 
    CAM_BRIGHTNESS_LEVEL_4       = 7, 
} CAM_BRIGHTNESS_LEVEL;*/
CAM_IMAGE_MODE CameraMode=CAM_IMAGE_MODE_320X320;


void reportCameraInfo()
{
    cameraInstance = myCAM.getCameraInstance();
    
    char buff[400];
    
    sprintf(buff,
            "ReportCameraInfo\r\nCamera Type:%s\r\nCamera Support Resolution:%d\r\nCamera Support "
            "specialeffects:%d\r\nCamera Support Focus:%d\r\nCamera Exposure Value Max:%ld\r\nCamera Exposure Value "
            "Min:%d\r\nCamera Gain Value Max:%d\r\nCamera Gain Value Min:%d\r\nCamera Support Sharpness:%d\r\n",
            cameraInstance->myCameraInfo.cameraId, cameraInstance->myCameraInfo.supportResolution,
            cameraInstance->myCameraInfo.supportSpecialEffects, cameraInstance->myCameraInfo.supportFocus,
            cameraInstance->myCameraInfo.exposureValueMax, cameraInstance->myCameraInfo.exposureValueMin,
            cameraInstance->myCameraInfo.gainValueMax, cameraInstance->myCameraInfo.gainValueMin,
            cameraInstance->myCameraInfo.supportSharpness);
    Serial.println(buff);
}

void loginInspecifictWLan(char* WLan_Name, char* WLan_Password) 
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.hostname("c1");
  Serial.println(WLan_Name);
  WiFi.begin(WLan_Name, WLan_Password);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter == 30) 
    {
      Serial.println("WiFi not connected!!!");
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    //wir geben die IP adresse die uns das Wlan gegeben hat aus
    Serial.println(WiFi.localIP());

    Serial.print("Hostname: ");
    Serial.println(WiFi.getHostname());
  }

  Serial.println(WiFi.RSSI());
}

void hotspot(char* WLan_Name,char* password, int channels ) 
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    return;
  }
  
  WiFi.disconnect(true);
  //WIFI_PHY_MODE_11B → Nur 802.11b
  //WIFI_PHY_MODE_11G → Nur 802.11g
  //WIFI_PHY_MODE_11N → Nur 802.11n

  //WiFi.setPhyMode(WIFI_PHY_MODE_11G);// test to Disable 802.11b. nur esp32
  //esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);



  //WiFi.softAP(WLan_Name, password);

  //test mich am bestimmten channel
  //2.4GHz: use channels 1, 5, 9 or 13
  WiFi.softAP(WLan_Name, password, channels, false, 4);
  
  dnsServer.start(53, "*", WiFi.softAPIP());

  Serial.println("WiFi created");
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void handleRoot() 
{ 
  String combinedString = String(Libraryhtml) + String(htmlPage);

  server.send(200, "text/html", combinedString);
}

void handleNotFound() 
{
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

void handleStream() 
{
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  Serial.println("Stream start");
  client = server.client();
  
  client.print("--frame\r\n");
  client.print("Content-Type: image/jpg\r\n\r\n");

  myCAM.takePicture(CameraMode,CAM_IMAGE_PIX_FMT_JPG);
  streamStart=true;
  startMillis=millis();
  cameraInstance = myCAM.getCameraInstance();

}

void handleButtonStopStream()
{  
  streamStart=false;
  Serial.print("ende Bild ");
  client.flush();
  client.print("\r\n");
  client.stop();
  server.send(200, "text/plain", "OK Stream Button"); 
}

void readPicture()
{
  if(streamStart==false)
  {
    return;
  }

  uint32_t check =myCAM.getReceivedLength();
  uint8_t buff[READ_IMAGE_LENGTH];
  uint8_t rtLength = readBuff(cameraInstance, buff, READ_IMAGE_LENGTH);
  //Serial.print(rtLength);

  if(!client.connected())
  {    
    streamStart=false;
    Serial.println("Bild down");
    client.flush();
    client.print("\r\n");
    client.stop();
    
    return;
  }

  client.write(&buff[0], rtLength);
  //client.flush();

  if (check == 0) 
  {    
    //client.flush();
    client.print("\r\n");

    //Serial.print("ende Bild ");
    long elapsedMillis = millis() - startMillis;
    //Serial.print(elapsedMillis);
    //Serial.println("   neu start");
    client.print("--frame\r\n");
    client.print("Content-Type: image/jpg\r\n\r\n");
    myCAM.takePicture(CameraMode,CAM_IMAGE_PIX_FMT_JPG);
    startMillis=millis();
  }
}

void handler(WebsocketsClient &client, WebsocketsMessage message)
{
    WSInterfaceString data = message.data();

    Serial.println(data);
    
    zwComand=data;
    newComand=true;
    client.send("status:" + data);
}

void poll_all_clients()
{
  if (WebSocketServer.available())
  {
      if (WebSocketServer.poll())
      {
          WebsocketsClient c = WebSocketServer.accept();
          Serial.println("Client accepted");
          SetConnectet();
          c.onMessage(handler);
          all_clients.push_back(c);
      }
  }
  for (auto &client : all_clients)
  {
      if(client.poll())
      {
        //*lastCommand = zwComand;
        break;
      }
  }
}

void Task1code( void * parameter )
{
  for(;;)
  {
    
    poll_all_clients();
    dnsServer.processNextRequest();
    server.handleClient();
    readPicture();
    delay(1);
  }
}

void ShowALLWIFILan()
{
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) 
  {
      Serial.println("no networks found");
  } 
  else 
  {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < n; ++i) 
    {
        // Print SSID and RSSI for each network found
        Serial.printf("%2d",i + 1);
        Serial.print(" | ");
        Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
        Serial.print(" | ");
        Serial.printf("%4d", WiFi.RSSI(i));
        Serial.print(" | ");
        Serial.printf("%2d", WiFi.channel(i));
        Serial.println(" | ");
    }
  }
}

void SetupWiFi()
{
  WebSocketServer.listen(81);

  server.on("/", handleRoot);
  //server.on("/a", handleRoot);
  //server.on("/stream", handleStream);
  server.on("/buttonstopstream", handleButtonStopStream);
  //server.onNotFound(handleNotFound);

  server.begin();

  //Serial.println("Camera start");
  //myCAM.begin();
  //Serial.println("Camera nun offen");

  //myCAM.reset();
  //Serial.println("Reset Camera");

  //reportCameraInfo();

  //myCAM.setBrightness(CAM_BRIGHTNESS_LEVEL_4);

  //myCAM.setContrast(CAM_CONTRAST_LEVEL_3);

  //myCAM.setImageQuality(HIGH_QUALITY);

  //myCAM.setAbsoluteExposure(30000);
  //myCAM.setSaturation(CAM_STAURATION_LEVEL_3 );
    // Task1 auf Core 0 starten
  xTaskCreatePinnedToCore(
                    Task1code,   // Task-Funktion
                    "Task1",     // Name der Task
                    10000,       // Stack-Größe
                    NULL,        // Parameter
                    1,           // Priorität
                    &Task1,      // Task-Handle
                    0);          // Core-ID
                    
}

void SetupWiFiCam()
{
  WebSocketServer.listen(81);

  server.on("/", handleRoot);
  //server.on("/a", handleRoot);
  server.on("/stream", handleStream);
  server.on("/buttonstopstream", handleButtonStopStream);
  //server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("Camera start");
  myCAM.begin();
  Serial.println("Camera nun offen");

  myCAM.reset();
  Serial.println("Reset Camera");

  reportCameraInfo();

  myCAM.setBrightness(CAM_BRIGHTNESS_LEVEL_4);

  myCAM.setContrast(CAM_CONTRAST_LEVEL_3);

  myCAM.setImageQuality(HIGH_QUALITY);

  myCAM.setAbsoluteExposure(30000);
  myCAM.setSaturation(CAM_STAURATION_LEVEL_3 );
    // Task1 auf Core 0 starten
  xTaskCreatePinnedToCore(
                    Task1code,   // Task-Funktion
                    "Task1",     // Name der Task
                    10000,       // Stack-Größe
                    NULL,        // Parameter
                    1,           // Priorität
                    &Task1,      // Task-Handle
                    0);          // Core-ID
                    
}