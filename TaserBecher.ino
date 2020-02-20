/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <U8g2lib.h>
#include "myTypes.h"
#include "quickSort.h"
#include "myEEPROM.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#ifndef APSSID
#define APSSID "TazyBoi69"
#define APPSK  "nbisschenschockierend"
#endif

#define buzzPin D8
#define relaysPin D4
#define sensorPin A0


//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 21, /* data=*/ 20, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED
//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
//U8G2_SSD1306_128X32_WINSTAR_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
const char *ssid = APSSID;
const char *password = APPSK;
String   fname;         // string for incoming serial data
//String   inputTime;         // string for incoming serial data
unsigned long startTime, inputTimeUL, finishTime;
int remainingTime;
String remainingTimeStr;
const char *inputTime;
int dataAddr = 0;
enum State {Countdown, Running, Ready};
enum Counter {Three, Two, One, Start};
State gameStart = Ready;
Counter countdown = Three;
competitor_t dataArray[25];

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {   //not in use
  String msg;
  loadEEPROM(dataArray);
  msg += EEPROMToHTML(dataArray);
  server.send(200, "text/html", msg);
}

void webpage() {
  server.send(200, "text/html", "<html><body><form  name='frm'  method='post'>  Name: <input type='text' name='fname'><br>Time(ms): <input type='number' name='inputTime'><br><input type='submit' value='Start'>   </form></body></html>");
}


void response(){
  if(server.hasArg("fname") && (server.arg("fname").length()>0)&&server.hasArg("inputTime") && (server.arg("inputTime").length()>0)){ 
    Serial.print("User entered:\t");
    Serial.println(server.arg("fname") + server.arg("inputTime"));
    fname = server.arg("fname");
    inputTime = server.arg("inputTime").c_str();
    inputTimeUL = atol(inputTime);
    competitor_t data;
    strcpy(data.fname, fname.c_str());
    data.inputTime = inputTimeUL;
    //writeToEEPROM(&data);
    gameStart = Countdown;
    startTime = millis();
    server.send(200, "text/html", "<html><body><h1>The Game is Starting</h1><body><p>Contestant: " + fname +  "</p><p>Time(ms):" + inputTime + "</p><a href='/'>Home</a></body></html><\body>");
  } else {
    server.send(400, "text/html", "<html><body><h1>HTTP Error 400</h1><p>Bad request. Please enter a value.</p></body></html>");
  }
}



void handleShock(){
  Serial.println("Handleshock");
  String msg; 
  digitalWrite(relaysPin, LOW);
  delay(250);   //ms
  digitalWrite(relaysPin, HIGH);
  msg += "<h1>Hallo Kevin</h1>";
  msg += "<body>";

  msg += "<a href='http://192.168.4.1/shock'><button>Zack da geht er hin</button></a> </body>";
  
  msg += "</body>";
  server.send(200, "text/html", msg);
}



void setup() {
  delay(1000);
  Serial.begin(74880);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/", handleRoot);
  server.on("/shock", handleShock);
  server.on("/admin",HTTP_GET, webpage);
  server.on("/admin",HTTP_POST,response);

  server.begin();
  Serial.println("HTTP server started");

  pinMode(relaysPin, OUTPUT);
  digitalWrite(relaysPin, HIGH);

  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB14_tr);

  loadEEPROM(dataArray);
}

void loop() {
  server.handleClient();
  if(gameStart==Countdown){
    if(millis()>startTime + 3000 && countdown == Start){
      Serial.println("Countdown0");
      countdown = Three;
      u8g2.clearBuffer();
      u8g2.drawStr(0,20,"GO");
      u8g2.sendBuffer();
      finishTime = millis() + inputTimeUL;      
      tone(buzzPin, 700);
      delay(500);
      noTone(buzzPin); 
      gameStart = Running;
    }
    else if(millis()>startTime + 2000 && countdown == One){
      Serial.println("Countdown1");
      countdown = Start;
      u8g2.clearBuffer();
      u8g2.drawStr(0,20,"1");
      u8g2.sendBuffer();
      tone(buzzPin, 300);
      delay(150);
      noTone(buzzPin);      
    } 
    else if(millis()>startTime + 1000 && countdown == Two){
      Serial.println("Countdown2");
      countdown = One;
      u8g2.clearBuffer();
      u8g2.drawStr(0,20,"2");
      u8g2.sendBuffer();  
      tone(buzzPin, 200);
      delay(150);
      noTone(buzzPin);     
    }
    else if(millis()>startTime && countdown == Three){
      Serial.println("Countdown3");
      countdown = Two;
      u8g2.clearBuffer();
      u8g2.drawStr(0,20,"3");
      u8g2.sendBuffer();    
      tone(buzzPin, 100);
      delay(150);
      noTone(buzzPin); 
    }
  }
  if(gameStart == Running){
    Serial.println("GameStart"); 
    if(millis()<finishTime){
      remainingTime = finishTime - millis();
     // sprintf(remainingTimeStr, "%i.%i", (int) remainingTime/1000, remainingTime%1000);         //Hier ist was kaputt
      remainingTimeStr = String((int) remainingTime/1000) + "." + String(remainingTime%1000);
      u8g2.clearBuffer();
      u8g2.drawStr(0,20,"remainingTimeStr");
      u8g2.sendBuffer();
      Serial.println("2"); 
      if(analogRead(sensorPin)<=100){
        gameStart = Ready;
        Serial.println("Win");
      }
    }
    else {
      digitalWrite(relaysPin, LOW);
      delay(250);   //ms
      digitalWrite(relaysPin, HIGH);
      gameStart = Ready;
      Serial.println("Too slow"); 
    }
  }
}
