#include <RTClib.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#include <TheThingsNetwork.h>
    
#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

const char *appEui = "70B3D57ED0011F10";
const char *appKey = "78A02F658605F6DF3ACE5D398D6D2010";
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

File myFile;

RTC_DS1307 rtc;


void setup() {
  loraSerial.begin(57600);
  debugSerial.begin(9600);

    Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    debugSerial.println("initialization failed!");
    while (1);
  }
  debugSerial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
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

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    debugSerial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      debugSerial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    debugSerial.println("error opening test.txt");
  }

/*CLOCK*/

    
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC adjusted!");
  }

/*END CLOCK*/

      
  // Initialize LED output pin
  pinMode(LED_BUILTIN, OUTPUT);
    
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);
    
  debugSerial.println("-- STATUS");
  ttn.showStatus();
  debugSerial.println("-- JOIN");
  /*ttn.join(appEui, appKey); */
}

void loop() {
  loraSerial.println("-- LOOP");

    myFile = SD.open("test.txt", FILE_WRITE);
    DateTime now = rtc.now();

    if (myFile) {
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(" ");
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.println();
    
    myFile.close();
  } 
    
  

  
  // Prepare array of 1 byte to indicate LED status
  byte data[1];
  data[0] = (digitalRead(LED_BUILTIN) == HIGH) ? 1 : 0;
    
  // Send it off
  /* ttn.sendBytes(data, sizeof(data)); */
      
  delay(1000);
}
