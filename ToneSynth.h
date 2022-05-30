/**
 * @file ToneSynth.h
 * @author Kailen Swensen
 * @brief Class to control the tone() function with customizable
 *        interval for time 
 * @version 0.1
 * @date 2022-05-19
 * 
 */

// float interpretation of arduino map() from Common.h
static float fmap (float in, float in_x, float in_y, float out_x, float out_y)
{
  return (in - in_x) * (out_y - out_x)/(in_y - in_x) + out_x;
}

class ToneSynth
{
  public:
    ToneSynth() : isActive(true), pinl(0), pinr(1), numChannels(0), currFrequency(0), 
              maxFrequency (0), delayPeriod(0) { }
    
    ToneSynth(int p1, int p2, int nc, float cf, int mf,int dp) :
              pinl(p1), pinr(p2), numChannels(nc), currFrequency(cf), 
              maxFrequency(mf), delayPeriod(dp) { }

    // mono
    void setPins(int l)
    {
      pinl = l;
    }

    // stereo
    void setPins(int l, int r)
    {
      pinl = l; pinr = r;
    }

    void setFrequency(float frequency)
    {
      currFrequency = (frequency > maxFrequency) ? maxFrequency : frequency;
    }

    void setMaxFrequency(float frequency)
    {
      maxFrequency = frequency;
    }

    void setDelay(int delay)
    {
      delayPeriod = delay;
    }

    void startNote(float frequency) 
    {
      isActive = true;
      currFrequency = frequency;
    }

    void stopNote()
    {
      isActive = false;
    }

    void update()
    {
      if (isActive)
      {
        tone(pinl, currFrequency, delayPeriod);

        delay(delayPeriod);
      }
    }

  private:

    bool isActive;
    int pinl, pinr;
    int numChannels;     
    int delayPeriod; // delay between tone() "samples" (ms)
    int maxFrequency;
    float currFrequency;
};