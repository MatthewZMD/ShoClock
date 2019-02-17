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


  Influenced from Arduino FFT library Example 3

*/

extern "C" {
#include "Cirque.h"
}

#include "arduinoFFT.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */


#define CHANNEL A0

//const uint16_t samples = 64; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 100; //Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
unsigned long microseconds;

/*
  These are the input and output vectors
  Input vectors receive computed results from FFT
*/
//double vR[samples];
//double vImag[samples];
Cirque * vReal;
Cirque * vImag;

// samples -> limit
const unsigned int limit = 64; // buffer limit
double buf; // the double information that will be stored

double analogReal;
double imagTmp = 0;

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

//----------

int allOccupied = 0;
int f = 0;


void ComputeFFTData() {
  Serial.println("Printing stuff....");
  delay(1000);
  /* Print the results of the sampling according to time */
  Serial.println("Data:");
  //PrintVector(vReal->entries, limit, SCL_TIME);
  FFT.Windowing(vReal->entries, limit, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  Serial.println("Weighed data:");
  //PrintVector(vReal->entries, samples, SCL_TIME);
  FFT.Compute(vReal->entries, vImag->entries, limit, FFT_FORWARD); /* Compute FFT */
  Serial.println("Computed Real values:");
  //  PrintVector(vReal, samples, SCL_INDEX);
  Serial.println("Computed Imaginary values:");
  //  PrintVector(vImag, samples, SCL_INDEX);
  FFT.ComplexToMagnitude(vReal->entries, vImag->entries, limit); /* Compute magnitudes */
  Serial.println("Computed magnitudes:");
  //  PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);

  //Print Vectors

  double abscissa;

  //doesn't matter print from head, just print 0 to f
  for (int i = 0; i < f; ++i) {
    abscissa = ((i * 1.0 * samplingFrequency) / limit);

    Serial.print(abscissa, 6);
    Serial.print("Hz ");
    Serial.println(vReal->entries[i], 4);
    Serial.println();

  }
  double x = FFT.MajorPeak(vReal->entries, limit, samplingFrequency);
  Serial.println(x, 6); //Print out what frequency is the most dominant.

}

//------------

void setup() {
  Serial.begin(115200);
  vReal = cirqueCreate();
  vImag = cirqueCreate();

  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));

  Serial.println("Ready");
  delay(100);
}


void loop() {
  /*SAMPLING*/
  //for(int i=0; i<samples; i++) {
  microseconds = micros();    //Overflows after around 70 minutes!

  // if not all occupied but exceeds capacity limit
  if (!allOccupied && f >= limit) {
    allOccupied = 1;
  }
  // if not all occupied and doesn't exceed capacity limit!
  if (!allOccupied && f < limit) {
    ++f;
  }

  //if all occupied, remove oldest
  if (allOccupied) {
    cirqueRemove(vReal);
  }
  //Serial.println("Getting analog from channelA0");

  //read from CHANNEL
  analogReal = analogRead(CHANNEL);

  Serial.println(analogReal);
  //insert into vReal and vImag
  //  vReal[i] = analogReal;

  cirqueInsert(vReal, analogReal);
  cirqueInsert(vImag, imagTmp);

  Serial.println("Inserted!");
  delay(100);
  //noInterrupts();
  ComputeFFTData();
  //interrupts();
  //while(1); /* Run Once */
  delay(1000); /* Repeat after delay */
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
        abscissa = (i * 1.0);
        break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
        break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / limit);
        break;
    }
    Serial.print(abscissa, 6);
    if (scaleType == SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}
