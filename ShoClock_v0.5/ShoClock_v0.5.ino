/*
  @author: Mingde Zeng
  @date: Feb 16, 2019
*/

#include "arduinoFFT.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// Display stuff on OLED
//#include <Arduino.h>
//#include <U8g2lib.h>
#include <U8x8lib.h>

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/
#define CHANNEL A0
PROGMEM const unsigned int samples = 64; //This value MUST ALWAYS be a power of 2
PROGMEM const int samplingFrequency = 500; //Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
unsigned long microseconds;

/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];
double vR[samples];
double vI[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (7)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:
  
// Use this line for a breakout with a SPI connection:
//Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
//Adafruit_PN532 nfc(PN532_SS);

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
   #define Serial SerialUSB
#endif

// UTF8 OLED
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display

unsigned const int shockPin = 9;
int shockState = LOW;

void setup()
{

  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  //  u8g2.begin();
  //  u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function
  // u8g2.setFont(u8g2_font_6x10_tf);  // use font
  //  u8g2.setFontDirection(0); // set font print direction

  
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print(F("Didn't find PN53x board"));
    while (1); // halt
  }
  
  // Got ok data, print it out!
  /* Serial.print(F("Found chip PN5")); */
  /* Serial.println((versiondata>>24) & 0xFF, HEX);  */
  /* Serial.print(F("Firmware ver). "); */
  /* Serial.print((versiondata>>16) & 0xFF, DEC);  */
  /* Serial.print('.'); */
  /*F( Serial.println((versiondata>>8) & 0xFF, DEC)); */
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println(F("Waiting for an ISO14443A card"));
  
  pinMode(shockPin,OUTPUT);

  sampling_period_us = round(1000000*(1.0/samplingFrequency));

  for(int i = 0; i < samples; ++i){
    vImag[i] = 0;
  }
  
  Serial.begin(115200);
  Serial.println(F("Ready"));
}

double x;

char occupied = 0;

unsigned long prevTime;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

char success = 0;

unsigned int available = Serial.available();

void loop() {  
  /*SAMPLING*/
  if(!occupied){
    for(int i=0; i<samples; i++) {
      vReal[i] = analogRead(CHANNEL);
      vImag[i] = 0;
    }
    occupied = 1;
  }else{
    for(int i = 0; i < samples - 1; ++i){
      vReal[i] = vReal[i + 1];
    }
    vReal[samples - 1] = analogRead(CHANNEL);
    vImag[samples - 1] = 0;
  }

  //copy from vReal to 
  for(int j = 0; j < samples; ++j){
    vR[j] = vReal[j];
    vI[j] = 0;
  }

  /* Print the results of the sampling according to time */
  //Serial.println("Data:");
  //PrintVector(vR, samples, SCL_TIME);
  FFT.Windowing(vR, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  //Serial.println("Weighed data:");
  //PrintVector(vR, samples, SCL_TIME);
  FFT.Compute(vR, vI, samples, FFT_FORWARD); /* Compute FFT */
  //Serial.println("Computed Real values:");
  //PrintVector(vR, samples, SCL_INDEX);
  //Serial.println("Computed Imaginary values:");
  //PrintVector(vI, samples, SCL_INDEX);
  FFT.ComplexToMagnitude(vR, vI, samples); /* Compute magnitudes */
  //Serial.println("Computed magnitudes:");
  //PrintVector(vR, (samples >> 1), SCL_FREQUENCY);
  x = FFT.MajorPeak(vR, samples, samplingFrequency);
  Serial.println(x, 6); //Print out what frequency is the most dominant.
  //while(1); /* Run Once */

 
  
  //----------------NFC

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)

  if (success) {
    Serial.println(F("Found a card!"));
    available = 0;

    /* u8g2.clearBuffer(); // clear screen */
    /* u8g2.setCursor(0, 15); // set printing position */
    /* u8g2.print(F("UID Length (bytes): ")); // set print content */
    /* u8g2.setCursor(78, 15); */
    /* u8g2.print(uidLength, DEC); */

    
    u8x8.drawString(0,0,uidLength);
    
    //        Serial.print("UID Value: ");
    //        for (uint8_t i=0; i < uidLength; i++)
    //        {
    //          Serial.print(" 0x");Serial.print(uid[i], HEX);
    //        }
    //        Serial.println("");
    // Wait 1 second before continuing

    //u8g2.sendBuffer(); // display screen
    

    shockState = LOW;

    //    Serial.available = 0;
    //  } else {
    //    Serial.println("Card not used yet!");

    success = false;
      
    //delay(1000);
  }

  //if triggers the electricity then
  trigger();
  
}

int inByte = 0;

void trigger() {

  //mock data

  // shock not started and need to start now
  // SHOCK YOU!
  if (available > 0) {
    inByte = Serial.read();    // get incoming byte:
    /* Serial.print("LOL"); */
    /* Serial.print(F("inByte: ")); */
    /* Serial.println(inByte); */

    prevTime = millis();
    shockState = HIGH;
    unsigned long currentTime = millis();

    
    tone(shockPin, 1000);

    Serial.println(F("Shock!"));
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
    /* while (currentTime - prevTime <= 5000) { */
    /*   currentTime = millis(); */
    /*   Serial.println("shockkkkkkk"); */
    /* } */

    noTone(shockPin);
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)

  }
  //  digitalWrite(shockPin, shockState);
  
}


/* void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType){ */
  
/*   for (uint16_t i = 0; i < bufferSize; i++) */
/*   { */
/*     double abscissa; */
/*     /\* Print abscissa value *\/ */
/*     switch (scaleType) */
/*     { */
/*       case SCL_INDEX: */
/*         abscissa = i; */
/*         break; */
/*       case SCL_TIME: */
/*         abscissa = ((i * 1.0) / samplingFrequency); */
/*         break; */
/*       case SCL_FREQUENCY: */
/*         abscissa = ((i * 1.0 * samplingFrequency) / samples); */
/*         break; */
/*     } */
/*     if(scaleType==SCL_INDEX){ */
/*       Serial.print(abscissa); */
/*     }else if(scaleType==SCL_TIME){ */
/*       Serial.print(abscissa, 6); */
/*     }else if(scaleType==SCL_FREQUENCY){ */
/*       Serial.print(abscissa, 6); */
/*       Serial.print("Hz"); */
/*     } */
/*     Serial.print(" "); */
/*     Serial.println(vData[i], 4); */
/*   } */
/*   Serial.println(); */
/* } */
