/**
 * @file Buttons.h
 * @author Kailen Swensen
 * @brief Class to house Button class, modified for our purposes
 * @version 0.1
 * @date 2022-05-19
 * 
 */

#include "Button.h"

// basic print function for default callback functions
void button_pressed()
{
  Serial.println("Button Pressed");
}

class Buttons
{
  public:

  // defaulted to demo circuit values
  Buttons() : gap(500), pin1(10), pin2(11), pin3(12), pin4(13)
  {
  }

  void Init()

  {
    // default settings
    initButtons();
    setDebounceGap(gap);
    for (int i = 0; i < 4; ++i)
      setCallbackFunction(i, &button_pressed);
  }

  void setPins(int a, int b, int c, int d)
  {
    pin1 = a; pin2 = b; pin3 = c; pin4 = d;
  }

  void initButtons()
  {
    buttons[0] = Button(pin1);
    buttons[1] = Button(pin2);
    buttons[2] = Button(pin3);
    buttons[3] = Button(pin4);
  }

  void setDebounceGap(unsigned ms)
  {
    for (int i = 0; i < 4; ++i)
      buttons[i].set_maximum_gap(ms);
  }

  void setCallbackFunction(int button, void * fcn)
  {
    buttons[i].on_press(fcn);
  }

  // their "update" function is called "init"
  // for readability i'll enclose it in a function
  void update()
  {
    for (int i = 0; i < 4; ++i)
      buttons[i].init();
  }

  private:

  Button buttons[4];
  unsigned gap;
  int pin1, pin2, pin3, pin4;

}