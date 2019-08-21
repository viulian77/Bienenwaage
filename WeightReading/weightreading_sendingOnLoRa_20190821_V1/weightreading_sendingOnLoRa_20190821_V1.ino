/*
 * https://circuits4you.com
 * 2016 November 25
 * Load Cell HX711 Module Interface with Arduino to measure weight in Kgs
 Arduino 
 pin 
 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND

 Most any pin on the Arduino Uno will be compatible with DOUT/CLK.
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
*/

#include "HX711.h"  //You must have this library in your arduino library folder

  #include <lmic.h>
  #include <hal/hal.h>
  #include <SPI.h>

#define DOUT  3  
#define CLK  4 

  static const PROGMEM u1_t NWKSKEY[16] = { 0x96, 0x14, 0xAC, 0x56, 0xD8, 0x5A, 0x7B, 0x1B, 0x55, 0x13, 0x6A, 0xB1, 0xEA, 0x58, 0x8C, 0x29 };
  static const u1_t PROGMEM APPSKEY[16] = { 0x9D, 0x1E, 0xC3, 0x51, 0x30, 0x94, 0x3E, 0x63, 0x3F, 0x12, 0x17, 0x74, 0xDC, 0x88, 0x6B, 0xD8 };
  static const u4_t DEVADDR = 0x26011DCE ;

  void os_getArtEui (u1_t* buf) { }
  void os_getDevEui (u1_t* buf) { }
  void os_getDevKey (u1_t* buf) { }
  static uint8_t mydata[4];
  static osjob_t sendjob;
  const unsigned TX_INTERVAL = 60;

  const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2, 6, 7},
};
  

HX711 scale(DOUT, CLK);

//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
float calibration_factor = -96650; //-106600 worked for my 40Kg max scale setup 


       void onEvent (ev_t ev) 
       {
          Serial.print(os_getTime());
          Serial.print(": ");
          switch(ev) {
                 case EV_SCAN_TIMEOUT:
                        Serial.println(F("EV_SCAN_TIMEOUT"));
                 break;
                 case EV_BEACON_FOUND:
                        Serial.println(F("EV_BEACON_FOUND"));
                 break;
                 case EV_BEACON_MISSED:
                       Serial.println(F("EV_BEACON_MISSED"));
                 break;
                 case EV_BEACON_TRACKED:
                        Serial.println(F("EV_BEACON_TRACKED"));
                 break;
                 case EV_JOINING:
                        Serial.println(F("EV_JOINING"));
                 break;
                 case EV_JOINED:
                        Serial.println(F("EV_JOINED"));
                 break;
                 case EV_RFU1:
                        Serial.println(F("EV_RFU1"));
                 break;
                 case EV_JOIN_FAILED:
                        Serial.println(F("EV_JOIN_FAILED"));
                 break;
                 case EV_REJOIN_FAILED:
                        Serial.println(F("EV_REJOIN_FAILED"));
                 break;
                 break;
                 case EV_TXCOMPLETE:
                        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
                        if(LMIC.dataLen) {
                           // data received in rx slot after tx
                          Serial.print(F("Data Received: "));
                          Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
                          Serial.println();
                        }
                      // Schedule next transmission
                        os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
                 break;
                 case EV_LOST_TSYNC:
                        Serial.println(F("EV_LOST_TSYNC"));
                 break;
                 case EV_RESET:
                        Serial.println(F("EV_RESET"));
                 break;
                 case EV_RXCOMPLETE:
                    // data received in ping slot
                    Serial.println(F("EV_RXCOMPLETE"));
                 break;
                 case EV_LINK_DEAD:
                        Serial.println(F("EV_LINK_DEAD"));
                 break;
                 case EV_LINK_ALIVE:
                        Serial.println(F("EV_LINK_ALIVE"));
                 break;
                 default:
                        Serial.println(F("Unknown event"));
                 break;
          }
      }
      
      void do_send(osjob_t* j)
      {
         // Check if there is not a current TX/RX job running
         if (LMIC.opmode & OP_TXRXPEND) {
                Serial.println(F("OP_TXRXPEND, not sending"));
         } else {
               // Prepare upstream data transmission at the next possible time.
               LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
               Serial.println(F("Packet queued"));
         }
        // Next TX is scheduled after TX_COMPLETE event.
      
      }


//=============================================================================================
//                         SETUP
//=============================================================================================
void setup() {

              /* LoRaWan Start Setup  */
    #ifdef VCC_ENABLE
      pinMode(VCC_ENABLE, OUTPUT);
      digitalWrite(VCC_ENABLE, HIGH);
      delay(1000);
    #endif
    os_init();
    LMIC_reset();
    #ifdef PROGMEM
      uint8_t appskey[sizeof(APPSKEY)];
      uint8_t nwkskey[sizeof(NWKSKEY)];
      memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
      memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
      LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
    #else
      LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
    #endif
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
    LMIC_setLinkCheckMode(0);
    LMIC_setDrTxpow(DR_SF7,14);
    do_send(&sendjob);
        /* LoRaWan End Setup */

        /*HX711 Start Setup  */
  Serial.begin(9600);
  /*
  Serial.println("HX711 Calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press a,s,d,f to increase calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press z,x,c,v to decrease calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press t for tare");   */
  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  /*
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);  */
      /*HX711 End Setup */
}


//=============================================================================================
//                         LOOP
//=============================================================================================
void loop() {

  
  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  
    unsigned long l = (unsigned long) ( scale.get_units() * 100 );
  
     mydata[3] = l & 0x00FF;
     mydata[2] = ( l >> 8 ) & 0x00FF ;
     mydata[1] = ( l >> 16 ) & 0x00FF;
     mydata[0] = l >> 24;

  unsigned long l1 = ((unsigned long) (( mydata[0] << 24) +  ( mydata[1] << 16) +  ( mydata[2] << 8)  + mydata[3] ) )  ;
  
  /* Serial.print(((float)l1)/100); */

  
  
  os_runloop_once();  
  /*
  Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);  */
  /*Serial.println(); */

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
    else if(temp == 's')
      calibration_factor += 100;  
    else if(temp == 'x')
      calibration_factor -= 100;  
    else if(temp == 'd')
      calibration_factor += 1000;  
    else if(temp == 'c')
      calibration_factor -= 1000;
    else if(temp == 'f')
      calibration_factor += 10000;  
    else if(temp == 'v')
      calibration_factor -= 10000;  
    else if(temp == 't')
      scale.tare();  //Reset the scale to zero
  }
}
//=============================================================================================
