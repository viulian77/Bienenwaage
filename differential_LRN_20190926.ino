#include <LowPower.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <TheThingsNetwork.h>

const char *appEui = "70B3D57ED0011F10";
const char *appKey = "78A02F658605F6DF3ACE5D398D6D2010";
const uint8_t sendeCounter = 150; //150 x 8 = 20 min sendeinterval
uint8_t counter = 0;
#define freqPlan TTN_FP_EU868

#define loraSerial Serial1
#define debugSerial Serial
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
 Adafruit_ADS1115 ads(0x4B);  /* Use this for the 16-bit version */
 Adafruit_ADS1115 ads1(0x4A);
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

void setup(void)
{
  loraSerial.begin(57600);
  Serial.begin(9600);
  while (!debugSerial && millis() < 10000);
  debugSerial.println("-- STATUS");
  ttn.showStatus();
  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 6.144V (0.1875mV/ADS1115)");
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
   //ads1.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
   //ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
   //ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  ads.setGain(GAIN_SIXTEEN);
  // ads.setGain(GAIN_ONE);
   ads1.setGain(GAIN_SIXTEEN); // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
 

  
 
  ads.begin();

  ads1.begin();
}

void loop(void)
{
  int16_t results, results2, results3, results4;
  
  /* Be sure to update this value based on the IC and the gain settings! */
  /*float   multiplier = 3.0F; */   /* ADS1015 @ +/- 6.144V gain (12-bit results) */
  //float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */
  byte mydata[4];
  //send only every sendeCounter x 8 seconds
  if (counter == sendeCounter) {
      results = ads.readADC_Differential_0_1();
      results2 =   ads.readADC_Differential_2_3();
      results3 = ads1.readADC_Differential_0_1();
      results4 =   ads1.readADC_Differential_2_3();
    
      Serial.print("Differential1: "); Serial.print(results); Serial.print("("); Serial.print(results * 0.0078125); Serial.println("mV)");
      Serial.print("Differential2: "); Serial.print(results2); Serial.print("("); Serial.print(results * 0.0078125); Serial.println("mV)");
      Serial.print("Differential3: "); Serial.print(results3); Serial.print("("); Serial.print(results * 0.0078125); Serial.println("mV)");
      Serial.print("Differential4: "); Serial.print(results4); Serial.print("("); Serial.print(results * 0.0078125); Serial.println("mV)");
 
      mydata[0] = results;
      mydata[1] = results2;
      mydata[2] = results3;
      mydata[3] = results4;
      ttn.sendBytes(mydata, sizeof(mydata));
      counter = 0;  //reset counter
  }
  else {
    Serial.print(counter * 8); Serial.println(" seconds from last transmision");
    counter++; 
  }

 
   LowPower.idle(SLEEP_8S, ADC_OFF, TIMER4_OFF, TIMER3_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART1_OFF, TWI_OFF, USB_OFF);
}
