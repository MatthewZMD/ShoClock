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

#include "arduinoFFT.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/
#define CHANNEL A0
const uint16_t samples = 128; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 100; //Hz, must be less than 10000 due to ADC

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

void setup()
{
  sampling_period_us = round(1000000*(1.0/samplingFrequency));
  Serial.begin(115200);
  Serial.println("Ready");
}

double x;

int occupied = 0;

void loop()
{
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
  delay(10); /* Repeat after delay */
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
