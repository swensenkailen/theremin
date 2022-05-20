/**
 * @file IrDistanceSensor.h
 * @author Will McDonald
 * @brief Interface Class for Sharp IR distance sensors
 * @version 0.1
 * @date 2022-05-19
 * 
 */

// #include <math.h>

#ifndef SMOOTHING_SAMPLES
#define SMOOTHING_SAMPLES (5u)
#endif

#ifndef SENSOR_PERIOD
#define SENSOR_PERIOD (40u) // minimum time allowed between samples
#endif

#ifndef SENSOR_COEFF
#define SENSOR_COEFF (12.472f)
#endif

#ifndef SENSOR_EXPONENT
#define SENSOR_EXPONENT (-1.068f)
#endif

#define ADC_TO_VOLTS 3.3f / ((0x01 << ADC_RESOLUTION)) // max voltage div by max val

// NOTE: it is possible for the sum to overflow, if 1 << sizeof(int) - ADC_RESOLUTION is less than SMOOTHING_SAMPLE_COUNT.
// this should not be the case, as RP2040 ADC_RESOLUTION = 12bits and int >= 16bits.  giving us more than enough samples for smoothing.

class IrDistanceSensor
{
private:
  int _pin = 0;

  int samples[SMOOTHING_SAMPLES] = { 0 };
  int curr_sample = 0;

  unsigned long last_sample_millis = 0;

  void storeSample(int sample)
  {
    samples[curr_sample++] = sample;

    if (curr_sample >= SMOOTHING_SAMPLES)
    {
      curr_sample = 0;
    }
  }

  int getSmoothed(void)
  {
    int sum = 0;

    for (int i = 0; i < SMOOTHING_SAMPLES; ++i)
    {
      sum += samples[i];
    }

    return sum / SMOOTHING_SAMPLES;
  }

  int readADC(void)
  {
    // TODO: consider pushing/popping ADC resolution, Other libraries/ user code may be expecting a different value.
    analogReadResolution(ADC_RESOLUTION);
    return analogRead(_pin);
  }

public:
  IrDistanceSensor(int pin) : _pin(pin)
  {
  }

  void Init(void)
  {
    pinMode(_pin, INPUT);
    analogReadResolution(ADC_RESOLUTION);

    for (int i = 0; i < SMOOTHING_SAMPLES; ++i) {
      storeSample(analogRead(_pin));
      delay(SENSOR_PERIOD);
    }
  }

  void sensorLoop(void)
  {
    unsigned long curr_millis = millis();

    if (curr_millis - last_sample_millis > SENSOR_PERIOD)
    {
      storeSample(analogRead(_pin));
      last_sample_millis = curr_millis;
    }
  }

  float GetDistance(void)
  {
    return SENSOR_COEFF * pow(getSmoothed() * ADC_TO_VOLTS, SENSOR_EXPONENT);
  }
};

class IrDistanceSensor_Relative : public IrDistanceSensor {
private:
  int low = 0, high = 1;

public:
  IrDistanceSensor_Relative(int pin) : IrDistanceSensor(pin)
  {
  }

  void SetLow(void)
  {
    low = IrDistanceSensor::GetDistance();
  }

  void SetHigh(void)
  {
    high = IrDistanceSensor::GetDistance();
  }

  float GetDistance(void)
  {
    float val = IrDistanceSensor::GetDistance() / (high - low);
    return min(1, max(val, 0)); // clamp 
  }
};
