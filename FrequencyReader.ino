#include "arduinoFFT.h"
#include "LedControl.h"

String inString = "";
int pos = 0;
int i;

// Arduino Pin 13 to DIN, 9 to Clk, 11 to LOAD, no.of devices is 1
LedControl lc = LedControl(13,9,11,1);
arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

#define CHANNEL A0
const uint16_t samples = 128; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 950; //Hz, must be less than 10000 due to ADC
unsigned int sampling_period_us;
unsigned long microseconds;

/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/

double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void setup()
{
sampling_period_us = round(1000000*(1.0/samplingFrequency));
Serial.begin(115200);
// Initialize the MAX7219 device
lc.shutdown(0, false); // Enable display
lc.setIntensity(0, 3); // Set brightness level (0 is min, 15 is max)
lc.clearDisplay(0); // Clear display register
for (int i = 0; i < 8; i++) {
lc.setDigit(0, i, i + 1, false);
}
}

void loop()
{
microseconds = micros();
for(int i=0; i<samples; i++)
{
vReal[i] = analogRead(CHANNEL);
vImag[i] = 0;
while(micros() - microseconds < sampling_period_us){
//empty loop
}
microseconds += sampling_period_us;
}

FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /*
Weigh data */
FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

double x = FFT.MajorPeak(vReal, samples, samplingFrequency);
Serial.println(x, 6); //Print out what frequency is the most dominant.

float y = x ;
int divvy = 1;
for (i = 1; i < 9; i++)
{
int num = int(y / divvy) % 10;
lc.setDigit(0, i-1, num, false);
Serial.print(i-1);
Serial.print(" : ");
Serial.println(num);
divvy *= 10;
}
Serial.println("---");
delay(1000); /* Repeat after delay */
}
