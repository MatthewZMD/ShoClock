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

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

//----------

int allOccupied = 0;
int f = 0;

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
  microseconds = micros();    //Overflows after around 70 minutes!

  if(!allOccupied){
    // if not all occupied and doesn't exceed capacity limit!
    while(f < limit){
      //read from CHANNEL
      analogReal = analogRead(CHANNEL);
      Serial.print(analogReal);
      Serial.print(" ");
      
      cirqueInsert(vReal, analogReal);
      cirqueInsert(vImag, 0.0);
      Serial.println(vReal->entries[f]);
      ++f;
      
      // if exceeds capacity limit
      if (f >= limit) {
        allOccupied = 1;
        Serial.println("All occupied");
      }
    }
  }else{
    //read from CHANNEL
    analogReal = analogRead(CHANNEL);
    Serial.print(analogReal);
    Serial.print(" ");
    Serial.println(cirquePeek(vReal));
    //if all occupied, remove oldest
    cirqueRemove(vReal);
    
    cirqueInsert(vReal, analogReal);
    cirqueInsert(vImag, 0.0);
    
    Serial.println("Inserted!");
    ComputeFFTData();
  }
  Serial.println("Loop end");
}


void ComputeFFTData() {
  Serial.println("Printing stuff....");
  /* Print the results of the sampling according to time */
  Serial.println("Data:");
  //PrintVector(vReal->entries, limit, SCL_TIME);
  FFT.Windowing(vReal->entries, limit, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  Serial.println("Weighed data:");
  //PrintVector(vReal->entries, samples, SCL_TIME);
  FFT.Compute(vReal->entries, vImag->entries, limit, FFT_FORWARD); /* Compute FFT */
  //  Serial.println("Computed Real values:");
  //  PrintVector(vReal, samples, SCL_INDEX);
  //Serial.println("Computed Imaginary values:");
  //  PrintVector(vImag, samples, SCL_INDEX);
//  FFT.ComplexToMagnitude(vReal->entries, vImag->entries, limit); /* Compute magnitudes */
  Serial.println("Computed magnitudes:");
  //  PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);

  //Print Vectors

  double abscissa;

  //doesn't matter print from head, just print 0 to f
  for (int i = 0; i < f; ++i) {
    abscissa = ((i * 1.0 * samplingFrequency) / limit);

    Serial.print(i);
    Serial.print(" ");
    Serial.print(abscissa, 6);
    Serial.print("Hz ");
    Serial.println(vReal->entries[i], 4);
    Serial.println();

  }
  double x = FFT.MajorPeak(vReal->entries, limit, samplingFrequency);
  Serial.println(x, 6); //Print out what frequency is the most dominant.

}
