/*

  Example of use of the FFT libray to compute FFT for a signal sampled through the ADC.
        Copyright (C) 2018 Enrique Condés and Ragnar Ranøyen Homb

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  This is largely influenced by Arduino FFT library FFT_03 example.

*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>



//------------Analog In Out Serial

// These constants won't change. They're used to give names to the pins used:
const unsigned int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const unsigned int analogOutPin = 9; // Analog output pin that the LED is attached to

unsigned int sensorValue = 0;        // value read from the pot
unsigned int outputValue = 0;        // value output to the PWM (analog out)
unsigned int ctr = 0;

//------------NFC

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
#define Serial SerialUSB
#endif

const unsigned int shockPin = 9;
int shockState = LOW;

void setup()
{
  //  #ifndef ESP8266
  //    while (!Serial); // for Leonardo/Micro/Zero
  //  #endif

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);

  // configure board to read RFID tags
  nfc.SAMConfig();

  //  Serial.println("Waiting for an ISO14443A card");

  pinMode(shockPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("Ready");

}

unsigned long prevTime;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

bool success;

//bool started = false;

int inByte = 0;

void loop()
{

  //------------Analog In Out Serial

  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  ctr++;
  outputValue = + sensorValue;
  // map it to the range of the analog out:
  //outputValue = map(sensorValue, 0, 1023, 0, 511);
  // change the analog out value:
  if (ctr == 10) {
    analogWrite(analogOutPin, outputValue);
    //    Serial.println(outputValue);
    delay(300);
    ctr = 0;
  }

  //----------------NFC


  //if triggers the electricity then
  trigger();

}

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
