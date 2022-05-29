/**
 * @author Kailen Swensen, Will McDonald
 * @date   May 2022
 * @brief  Arduino theremin with Audio/MIDI output
 * Written for RP2040
 */

#include <Button.h>
#include "IrDistanceSensor.h"
#include "ToneSynth.h"

#define PIN_SENS_L A0
#define PIN_SENS_R A1

#define AUDIO_OUT_L 2
#define AUDIO_OUT_R 3

#define PIN_BUTTON_1 10
#define PIN_BUTTON_2 11
#define PIN_BUTTON_3 12
#define PIN_BUTTON_4 13

Button button1(PIN_BUTTON_1),
       button2(PIN_BUTTON_2),
       button3(PIN_BUTTON_3),
       button4(PIN_BUTTON_4);

ToneSynth synth;

#define LOGGING_DELAY 50
int last_log_millis = 0;

IrDistanceSensor_Relative sensor_left(PIN_SENS_L),
                          sensor_right(PIN_SENS_R);

void setup()
{
  synth.setPins(AUDIO_OUT_L, AUDIO_OUT_R);
  synth.setDelay(10);
  synth.setMaxFrequency(1500);
  // TODO: use Kails button library
  // pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  // pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  // pinMode(PIN_BUTTON_3, INPUT_PULLUP);a
  // pinMode(PIN_BUTTON_4, INPUT_PULLUP);

  SerialUSB.begin(9600);

  sensor_left.Init();
  sensor_right.Init();
}

void loop()
{
  sensor_left.sensorLoop();
  sensor_right.sensorLoop();

  // set delay based on left sensor distance
  synth.setDelay(fmap(sensor_left.GetDistance(), 0.0f, 1.0f, 10.0f, 60.0f));
  // set frequency based on left sensor distance
  synth.setFrequency(fmap(sensor_right.GetDistance(), 
                     0.0f, 1.0f, 110.0f, 1760.0f));

  unsigned long curr_millis = millis();
  if (curr_millis - last_log_millis > LOGGING_DELAY) 
  {

    SerialUSB.print("Left Sensor: ");
    //SerialUSB.println(sensor_left.GetDistance());
    SerialUSB.print("Right Sensor: ");
    SerialUSB.println(sensor_right.GetDistance());
  }

  synth.update();

  //delay(10);
}
