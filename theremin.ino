/**
 * @author Kailen Swensen, Will McDonald
 * @date   May 2022
 * @brief  Arduino theremin with MIDI output
 *         NOTE: currently untested, nor debugged
 * 
 */

 #include <math.h> // log()
 #define PI atan(1.0) * 4.0

// While this comment exists, most values will be arbitrary until circuit is set up,
// and pin values are certain
const int ired_sensor1 = 1, 
          ired_sensor2 = 2,
          button1 = 3,
          button2 = 4,
          button3 = 5,
          button4 = 6,
          audio_out_pin = 7;
          
// if not too expensive, i'd like to use doubles, but can probably work with ints
// TODO: set up sinewave lookup table (a bit more efficient than sin())
const double sine_table[100];
double frequency;

void setup() 
{
  // initialize serial communication:
  Serial.begin(9600);
  // set MIDI baud rate
  Serial.begin(31250);
  
  // set up input/output pins
  pinMode(ired_sensor1, INPUT);
  pinMode(ired_sensor2, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(audio_out_pin, OUTPUT);
}

void loop()
{
  // retrieve sensor/button data (will have to convert if data is given as ints)
  double sensor1_data = pulseIn(ired_sensor1, HIGH),
         sensor2_data = pulseIn(ired_sensor2, HIGH),
         button1_data = pulseIn(button1, HIGH),
         button2_data = pulseIn(button2, HIGH),
         button3_data = pulseIn(button3, HIGH),
         button4_data = pulseIn(button4, HIGH);

  // set oscillator frequency, defaulted for now (will use sensor_data)
  int carrier_frequency = 440;    
  int mopdulator_frequency = 220; 

  // this is where we'll put the final calculated output,
  // currently not too sure if this needs to be an int for analog output
  int audio_output = 0;

  // 144 = note on
  int midi_command = 0x90;
  // frequency to midi conversion formula
  int midi_note = (int)((12 * log(frequency/220.0) / log(2.0)) + 57);
  // velocity, could be controlled by button_data or second sensor if wanted
  int midi_velocity = 0;
    
  // play sound
  analogWrite(audio_out_pin, audio_output);
  
  // alternatively, send MIDI
  midiWrite(midi_command, midi_note, midi_velocity);

  // update viewport
  Serial.print("Theremin");
}

void midiWrite(int command, int note, int velocity)
{
  // write the command, note value, and velocity value to MIDI output
  Serial.write(command);
  Serial.write(note);
  Serial.write(velocity);
}
