/*
  @author: Mingde Zeng
  @date: Feb 16, 2019
*/

#include "arduinoFFT.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/
#define CHANNEL A0
const uint16_t samples = 64; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 500; //Hz, must be less than 10000 due to ADC

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
#define PN532_IRQ   (2)
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


const int outputPin = 9;
int outputState = LOW;

void setup()
{
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A card");
  
  pinMode(outputPin,OUTPUT);

  sampling_period_us = round(1000000*(1.0/samplingFrequency));

  for(int i = 0; i < samples; ++i){
    vImag[i] = 0;
  }
  
  Serial.begin(115200);
  Serial.println("Ready");
}

double x;

int occupied = 0;

unsigned long prevTime;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

bool success = false;

//bool started = false;

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


  //if triggers the electricity then
  trigger();
  
}

int inByte = 0;

void trigger() {
  if (success) {
    Serial.println("Found a card!");
    //        Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    //        Serial.print("UID Value: ");
    //        for (uint8_t i=0; i < uidLength; i++)
    //        {
    //          Serial.print(" 0x");Serial.print(uid[i], HEX);
    //        }
    //        Serial.println("");
    // Wait 1 second before continuing
    delay(1000);

    shockState = LOW;

    //    Serial.available = 0;
    //  } else {
    //    Serial.println("Card not used yet!");

    success = false;
  }

  //mock data
  unsigned int available = Serial.available();

  // shock not started and need to start now
  if (available > 0) {
    inByte = Serial.read();    // get incoming byte:
    Serial.print("inByte: ");
    Serial.println(inByte);

    prevTime = millis();
    shockState = HIGH;
    unsigned long currentTime = millis();
    while (currentTime - prevTime <= 50) {
      currentTime = millis();
      Serial.println("shockkkkkkk");
    }

    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  }

  digitalWrite(shockPin, shockState);
}


void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = i;
        break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
        break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
        break;
    }
    if(scaleType==SCL_INDEX){
      Serial.print(abscissa);
    }else if(scaleType==SCL_TIME){
      Serial.print(abscissa, 6);
    }else if(scaleType==SCL_FREQUENCY){
      Serial.print(abscissa, 6);
      Serial.print("Hz");
    }
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}
