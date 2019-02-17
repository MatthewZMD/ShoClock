#include <TimerOne.h>

volatile int inputValue;
bool newSample;

// Setting input and output pins
int inputPin = A0;
int outputPin = LED_BUILTIN;

// Setting some variables
int lastInputValue = 0;
int outputValue = 0;
int hpOutputValue = 0;
int lpOutputValue = 0;
int hpMeanValue = 0;
int lpMeanValue = 0;


// Scaling factor equivalent to time constant
float alpha = 0.5;

String outputString;

void setup() {
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  
  Timer1.initialize(500);
  Timer1.attachInterrupt(takeSample);
}

void loop() {
  if(newSample) {
    inputValue = inputValue - 512;
    
    //LP-filter
    lpOutputValue = lpOutputValue + alpha * (inputValue - lpOutputValue);
    
    //HP-filter
    hpOutputValue = alpha * (hpOutputValue + inputValue - lastInputValue);
    
    //Taking the mean of each filter
    lpMeanValue = lpMeanValue + alpha * (abs(lpOutputValue) - lpMeanValue);
    hpMeanValue = hpMeanValue + alpha * (abs(hpOutputValue) - hpMeanValue);
    
    //Comparator
    if (hpMeanValue > lpMeanValue) {
      outputValue = 1;
    } else {
      outputValue = 0;
    }
    digitalWrite(outputPin, outputValue);
    
    lastInputValue = inputValue;
    newSample = false;
  }
}

// Interrupt-handler
void takeSample(void) {
  inputValue = analogRead(0); // inputValuer på A0
  newSample = true;
}
