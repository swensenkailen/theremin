/**
 * @file button-test.ino
 * @author Kailen Swensen, Will McDonald
 * @brief Test for button functionality
 * @version 0.1
 * @date 2022-05-17
 */

#include <Button.h>

// buttons connected to pins 10-13
Button button1(10);
Button button2(11);
Button button3(12);
Button button4(13);

void on_press()
{
  Serial.println("Button was pressed");
}

void setupButtons()
{
  // set callback functions
  button1.on_press(on_press);
  button2.on_press(on_press);
  button3.on_press(on_press);
  button4.on_press(on_press);
  
  // lowering bounce max gap for quicker button presses
  button1.set_maximum_gap((unsigned)500);
  button2.set_maximum_gap((unsigned)500);
  button3.set_maximum_gap((unsigned)500);
  button4.set_maximum_gap((unsigned)500);
}

void updateButtons()
{
  button1.init();
  button2.init();
  button3.init();
  button4.init();
}

// sets up board/input on reset or power
void setup() 
{
  setupButtons();
  Serial.begin(9600);
  Serial.println("button_test.ino serial init");
}

// simple loop checking for button presses
void loop() 
{
  updateButtons();
}
