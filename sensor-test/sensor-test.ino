/**
 * @file sensor-test.ino
 * @author Will McDonald, Kailen Swensen
 * @brief Simple test of IR sensor functionality
 * @version 0.1
 * @date 2022-05-04
 */

#include <math.h>

#define ADC_PIN A0
#define ADC_TO_VOLTS 3.3f / ((0x01 << ADC_RESOLUTION)) // max voltage div by max val

#define SMOOTHING_SAMPLE_COUNT 3

int curr_sample = 0;
int samples[SMOOTHING_SAMPLE_COUNT];

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(A0, INPUT);

  Serial.begin(9600);
  Serial.println("sensor-test.ino serial init");

}

// NOTE: if it is unacceptable for the first (SMOOTHING_SAMPLE_COUNT - 1) values to be a bit low,
// you would want to track samples collected and div by min(samples_collected, SMOOTHING_SAMPLE_COUNT).

// NOTE: it is possible for the sum to overflow, if 1 << sizeof(int) - ADC_RESOLUTION is less than SMOOTHING_SAMPLE_COUNT.
// this should not be the case, as RP2040 ADC_RESOLUTION = 12bits and int >= 16bits.  giving us at least 8 samples for smoothing.

// return average of last N samples,
int get_smoothed(int curr_val) {
  int sum = 0;

  // store new sample
  samples[curr_sample++] = curr_val;
  if (curr_sample >= SMOOTHING_SAMPLE_COUNT) {
    curr_sample = 0;
  }

  // get sum of all samples
  for (int i = 0; i < SMOOTHING_SAMPLE_COUNT; ++i) {
    sum += samples[i];
  }

  // return average of samples.
  return sum / SMOOTHING_SAMPLE_COUNT;
}

// the loop function runs over and over again forever
void loop() {
  analogReadResolution(ADC_RESOLUTION);
  int smoothed_val = get_smoothed(analogRead(ADC_PIN));

  // convert raw adc value to volts
  float volts = smoothed_val * ADC_TO_VOLTS;
  
  // convert voltage to distance in cm, using estimation based on datasheet
  int distance = 12.472 * pow(volts, -1.068);

  // only print data that is somewhat within the expected range
  if (distance <= 35) {
    Serial.println(distance);
  }

  delay(40);
}