
/**
 * @file audio-out-test.ino
 * @author Kailen Swensen
 * @brief Test for audio output with basic sweeping tone
 * @version 0.1
 * @date 2022-05-19
 */

// simple class for testing different rates of using the tone() function
class SweepFreq
{
  public:

    SweepFreq() : numChannels(2), startFrequency(0), 
                  maxFrequency(2000), delayPeriod(40), frate(0.25) { }

    void setup(int nc, float sf, int mf, int dp, float fr) 
    {
      numChannels = nc; startFrequency = sf; maxFrequency = mf; delayPeriod = dp;
    }

    // increase tone 
    void update()
    {
      // reset at max
      if (startFrequency > maxFrequency)
        startFrequency = 0;

      // tone out on pins 2 and 3 (L/R of audio jack)
      tone(2, startFrequency, delayPeriod);
      delay(delayPeriod);
      startFrequency += frate;
    }

  private:

    int numChannels;     
    float startFrequency;
    int maxFrequency;
    int delayPeriod; // ms
    float frate; // hz
     
};

SweepFreq test;

void setup() 
{
  // A2 - A6, 40ms delay between "samples"
  test.setup(2, 110, 1760, 40, 0.2 );
}

void loop() 
{
  // increases frequency each period
  test.update();
}
