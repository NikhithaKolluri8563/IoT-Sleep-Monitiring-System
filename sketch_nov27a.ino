#include<M5Stack.h>
//#include <SoftwareSerial.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include<WiFi.h>
#include <HTTPClient.h>
#include "ThingSpeak.h"

#define SECRET_SSID "mamaekpegla"                               
#define SECRET_PASS "Nenetypechestha"                            

#define SECRET_CH_ID 1226627                                 
#define SECRET_WRITE_APIKEY "NUU3SB0NQTYS4HOM" 

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

char ssid[] = SECRET_SSID;   
char pass[] = SECRET_PASS;   
int keyIndex = 0;            
WiFiClient  client;

MPU6050 mpu6050(Wire);
//SoftwareSerial MySerial(7, 8); // pin 7 connects to TX of HC-05 | pin 8 connects to RX of HC-05

long timer = 0;
long sleep_timer_start=0, sleep_timer_end=0,sleep_timer_end2=0;
float x,y,z;
int activate, interrupt,stage_sleep_time, interrupt_sleep_timer,interrupt_for_deep_sleep, total_sleep, total_deep_sleep, total_light_sleep, deep_sleep, light_sleep, interrupt_timer=0;
int httpCode;
int l_sleep= 2;
int d_sleep= 3;
int awake = 1;
void setup() {
Serial.begin(9600);
Wire.begin();
mpu6050.begin();
mpu6050.calcGyroOffsets(true);
 //Serial.begin(115200);
  //delay(100);

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);
   if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
   // Wait 20 seconds to update the channel again
  //delay(20000);

}

void loop() {
mpu6050.update();                //gyrosensor sensor update

if(millis() - timer > 1000){ 
x = mpu6050.getGyroX();
y = mpu6050.getGyroY();
z = mpu6050.getGyroZ();
Serial.print("main start");
Serial.print("x:");
  Serial.print(x);
  Serial.print("y:");
  Serial.print(y);
  Serial.print("z:");
  Serial.print(z);
if (activate == 0){ // first sleep confirmation
if ((x<=20 || x>=-20) && (y<=20 || y>=-20) && (z<=20 || z>=-20)) {
  httpCode = ThingSpeak.writeField(myChannelNumber, 1, awake, myWriteAPIKey);
  Serial.print("timer start");
  Serial.print("x:");
  Serial.print(x);
  Serial.print("y:");
  Serial.print(y);
  Serial.print("z:");
  sleep_timer_start = millis()/1000-sleep_timer_end;

  Serial.print(z);
if (sleep_timer_start == 30){
activate = 1;
Serial.println("l_sleep");
Serial.println(l_sleep);
 
 Serial.println("httpcode");
Serial.println(httpCode);
}
}
if ((x>=20 || x<=-20) || (y>=20 || y<=-20) || (z>=20 || z<=-20)){
  Serial.print("timer end");
  Serial.print("x:");
  Serial.print(x);
  Serial.print("y:");
  Serial.print(y);
  Serial.print("z:");
  Serial.print(z);
  
sleep_timer_end =(millis()/1000);

}
}

if (activate == 1){ // sleeping mode
light_sleep = (millis()/1000)-sleep_timer_end;

if (interrupt == 0){
  Serial.println("light_sleep");
  Serial.println(light_sleep);
   Serial.println("interrupt_for_deep_sleep");
  Serial.println(interrupt_for_deep_sleep);
   Serial.println("third");
  Serial.println(light_sleep-interrupt_for_deep_sleep);
  
  if (interrupt_for_deep_sleep < 420){
    
  httpCode = ThingSpeak.writeField(myChannelNumber, 1, l_sleep, myWriteAPIKey);
  }
if (light_sleep >= 420){
if (interrupt_for_deep_sleep > 420){
if (light_sleep - interrupt_sleep_timer >= 60){
deep_sleep = light_sleep - interrupt_for_deep_sleep;
Serial.println("interrupt_sleep_timer: ");
Serial.print(interrupt_sleep_timer);
 httpCode = ThingSpeak.writeField(myChannelNumber, 1, d_sleep, myWriteAPIKey);
}
 else
 {
   httpCode = ThingSpeak.writeField(myChannelNumber, 1, l_sleep, myWriteAPIKey);
 }

}
}
}
light_sleep = light_sleep - deep_sleep;

if ((x>=20 || x<=-20) || (y>=20 || y<=-20) || (z>=20 || z<=-20)){
interrupt_sleep_timer = (millis()/1000)-sleep_timer_end; 
interrupt_for_deep_sleep = light_sleep;
interrupt =interrupt+1;
delay(8000);}



if ((millis()/1000)- sleep_timer_end -interrupt_sleep_timer > 30) {
interrupt =0; 
}

if ((millis()/1000)- sleep_timer_end - interrupt_sleep_timer <= 30){
if (interrupt >=5){
sleep_timer_end =(millis()/1000);
if (light_sleep >= 90){ // second sleep confirmation
total_light_sleep = total_light_sleep + light_sleep;
total_deep_sleep = total_deep_sleep + deep_sleep;
total_sleep = total_light_sleep + total_deep_sleep; }
activate =0;
interrupt =0;
deep_sleep= 0;
light_sleep= 0;
interrupt_sleep_timer=0;
interrupt_for_deep_sleep=0;
}
}

}
stage_sleep_time = light_sleep + deep_sleep; 
if (stage_sleep_time >= 1000){
sleep_timer_end =(millis()/1000);
total_light_sleep = total_light_sleep + light_sleep;
total_deep_sleep = total_deep_sleep + deep_sleep;
total_sleep = total_light_sleep + total_deep_sleep; 
activate =0;
interrupt =0;
deep_sleep= 0;
light_sleep= 0;
interrupt_sleep_timer=0;
interrupt_for_deep_sleep=0; 
}
Serial.print("seconds: ");
Serial.print(sleep_timer_start); // just to know code and sensor working fine
Serial.print(",");
if (light_sleep >= 90){ 
Serial.print("lite sleep seconds: ");
Serial.print(light_sleep/60);
Serial.print(",");
Serial.print(deep_sleep/60);
Serial.print(","); 
Serial.print(total_light_sleep/60);
Serial.print(",");
Serial.print(total_deep_sleep/60);
Serial.print(",");
Serial.print(total_sleep/60);
Serial.print(";");
Serial.println(" ");}
else {
  Serial.print("seconds: ");
Serial.print(0);
Serial.print(",");
Serial.print(0);
Serial.print(","); 
Serial.print(total_light_sleep/60);
Serial.print(",");
Serial.print(total_deep_sleep/60);
Serial.print(",");
Serial.print(total_sleep/60);
Serial.print(";");
Serial.println(" ");
} 
timer = millis();
}

}
