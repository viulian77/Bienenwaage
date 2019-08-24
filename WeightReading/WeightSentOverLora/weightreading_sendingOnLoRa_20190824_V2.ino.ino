#include <TheThingsNetwork.h>
#include <RTClib.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "HX711.h"

  const char *appEui = "70B3D57ED0011F10";
  const char *appKey = "78A02F658605F6DF3ACE5D398D6D2010";

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

File myFile;
RTC_DS1307 rtc;
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);



#define DOUT  3
#define CLK  2

HX711 scale(DOUT, CLK);

float calibration_factor = -7050;
    
void setup() {
  loraSerial.begin(57600);
  debugSerial.begin(9600);
      
  // Initialize LED output pin
  pinMode(LED_BUILTIN, OUTPUT);

  if (!SD.begin(4)) {
    debugSerial.println("initialization failed!");
    while (1);
  }

  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    debugSerial.print("Writing to test.txt...");
    myFile.println("testing 4, 5, 6.");
    // close the file:
    myFile.close();
    debugSerial.println("done.");
  } else {
    // if the file didn't open, print an error:
    debugSerial.println("error opening test.txt");
  }
  
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);
    
  debugSerial.println("-- STATUS");
  ttn.showStatus();
  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading

  }

void loop() {
  // put your main code here, to run repeatedly:
  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if (temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }

    // Prepare array of 1 byte to indicate LED status
  byte mydata[4];


     unsigned long l = (unsigned long) ( scale.get_units() * 100 );
    Serial.println(l);
  
     mydata[3] = l & 0x00FF;
     mydata[2] = ( l >> 8 ) & 0x00FF ;
     mydata[1] = ( l >> 16 ) & 0x00FF;
     mydata[0] = l >> 24;
    
  // Send it off
   ttn.sendBytes(mydata, sizeof(mydata));
  delay(1000);
}
