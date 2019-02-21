#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (1)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

//int shockState = LOW;

void setup() {
  nfc.begin();

  nfc.setPassiveActivationRetries(0xFF);

  // configure board to read RFID tags
  nfc.SAMConfig();

  //DDRB = DDRB | B00100010;
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  Serial.begin(115200);
  //Serial.println("Ready");

}

unsigned long prevTime;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

bool success;

void loop() {
  if (success) {
    // LED light off
    //PORTB = B00100000;
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);

    //Serial.println("Closing shockwaves");
    delay(1000);

    //shockState = LOW;

    success = false;

    //PORTB = B00000010;
  }

  unsigned int shockTime = millis();
  if (millis() - shockTime >= 10000) {
    // LED light on
    //PORTB = B00100000;
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);


    prevTime = millis();
    //shockState = HIGH;
    while (millis() - prevTime <= 20) {
      //Serial.println("shockkkkkkk");
    }

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

    //PORTB = B00000010;
  }

}
