/**
 * @author Kailen Swensen, Will McDonald
 * @date   May 2022
 * @brief  Arduino theremin with MIDI output
 * Written for RP2040
 */

#include "IrDistanceSensor.h"

#define PIN_SENS_L A1
#define PIN_SENS_R A0

#define PIN_BUTTON_1 10
#define PIN_BUTTON_2 11
#define PIN_BUTTON_3 12
#define PIN_BUTTON_4 13

// Button button1(PIN_BUTTON_1),
//        button2(PIN_BUTTON_2),
//        button3(PIN_BUTTON_3),
//        button4(PIN_BUTTON_4);


#define LOGGING_DELAY 50
int last_log_millis = 0;

IrDistanceSensor_Relative sensor_left(PIN_SENS_L),
                          sensor_right(PIN_SENS_R);

void setup()
{
  // TODO: use Kails button library
  // pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  // pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  // pinMode(PIN_BUTTON_3, INPUT_PULLUP);
  // pinMode(PIN_BUTTON_4, INPUT_PULLUP);

  SerialUSB.begin(11520);

  sensor_left.Init();
  sensor_right.Init();
}

void loop()
{
  sensor_left.sensorLoop();
  sensor_right.sensorLoop();

  float left_val = sensor_left.GetDistance();
  float right_val = sensor_right.GetDistance();

  unsigned long curr_millis = millis();
  if (curr_millis - last_log_millis > LOGGING_DELAY) {
    SerialUSB.print("Left: ");
    SerialUSB.println(left_val);
    SerialUSB.print("Right: ");
    SerialUSB.println(right_val);
  }

  delay(10);
}
