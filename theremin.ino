/**
 * @author Kailen Swensen, Will McDonald
 * @date   May 2022
 * @brief  Arduino theremin with MIDI output
 * Written for RP2040
 */

// #include <Button.h>
#include "IrDistanceSensor.h"

#define PIN_SENS_L A0
#define PIN_SENS_R A1

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

IrDistanceSensor sensor_left(PIN_SENS_L),
                 sensor_right(PIN_SENS_R);

void setup()
{
  // pinMode(PIN_SENS_L, INPUT);
  // pinMode(PIN_SENS_R, INPUT);


  // TODO: use Kails button library
  // pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  // pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  // pinMode(PIN_BUTTON_3, INPUT_PULLUP);
  // pinMode(PIN_BUTTON_4, INPUT_PULLUP);

  SerialUSB.begin(9600);

  sensor_left.Init();
  sensor_right.Init();
}

void loop()
{
  sensor_left.sensorLoop();
  sensor_right.sensorLoop();

  unsigned long curr_millis = millis();
  if (curr_millis - last_log_millis > LOGGING_DELAY) {
    SerialUSB.print("Left Sensor: ");
    SerialUSB.println(sensor_left.GetDistanceCM());
    SerialUSB.print("Right Sensor: ");
    SerialUSB.println(sensor_right.GetDistanceCM());
  }

  delay(10);
}
