/**
 * @file   theremin.cpp
 * @author Kailen Swensen (swensenkailen@gmail.com)
 * @date   2022-07-05
 * @brief  Implementation for a theremin with custom parameters utilizing 
 *         a Daisy board and Daisy audio libraries 
 * 
 *         TODO: implement button debouncing, sensor parsing, test
 */

#include "daisysp.h"
#include "daisy_seed.h"

using namespace daisysp;
using namespace daisy;

/**
 * 
 *  button 1/2 (top): switch between waveform, mode by pressing both
 *  
 *      - waveforms: sin, saw, sqr, tri
 *      - modes: frequency/amp, frequency/trem
 *               frequency/vibrato, midi note out/midi velocity out,
 *               ???
 *  
 *  buttons 3/4 (bottom): other parameters based on mode
 *      - mode params (respective order): octave(?), tremolo depth,
 *                                        fm waveform(?), ???
 * 
 */

// audio output pins
#define AUDIO_OUT_L 18
#define AUDIO_OUT_R 19

// future me: usb output (midi) is on pin 1
#define MIDI_IO 1
// left ired sensor 
#define SENSOR_L 22
// right ired sensor
#define SENSOR_R 23

// Buttons - TL = top right, etc...
#define BUTTON_TL 2
#define BUTTON_TR 3
#define BUTTON_BL 4
#define BUTTON_BR 5

/**
 * @brief Oscillator type enum
 * 
 */
enum OscType
{
    SIN = 0,
    TRI = 1,
    SAW = 2,
    SQR = 3
};

/**
 * @brief System mode enum
 * 
 */
enum SystemMode
{
    WAVEFORM = 0,
    OUTPUT_MODE = 1
};

/**
 * @brief namespace to hold fx classes so not ambiguous w/ daisysp classes
 * 
 */
namespace fx
{
    /**
     * @brief Output mode enum
     * 
     */
    enum OutputMode
    {
        FREQ_AMP = 0,
        FREQ_TREM = 1,
        FREQ_VIB = 2,
        MIDI = 3,
    };

    /**
     * @brief Tremolo effect class
     * 
     */
    class Tremolo
    {
        public:

            /**
             * @brief Sets the speed of the tremolo
             * 
             * @param s new speed (hz)
             */
            void setSpeed(float s)
            {
                speed = s;
            }

            /**
             * @brief Sets the depth of the tremolo 
             * 
             * @param d new depth (amplitude [0-1])
             */
            void setDepth(float d)
            {
                depth = d;
            }

            /**
             * @brief Processes input signal with tremolo
             * 
             * @param input 
             * @return float 
             */
            float process(float input)
            {
                // subtract depth from amp to normalize to 0-1
                float output = input - depth;

                // apply sine wave to amplitude
                output *= (depth * sin(dt * speed));
                
                // update dt for correct sinewave output
                ++dt;

                return output;
            }

        private:
            int dt;
            // frequency of trem (hz), and how much it effects amplitude (linear)
            float speed, depth;
    };

    /**
     * @brief Vibrato effect class
     * 
     */
    class Vibrato
    {
        public:

            /**
             * @brief Sets the speed of the vibrato
             * 
             * @param s new speed (hz)
             */
            void setSpeed(float s)
            {
                speed = s;
            }

            /**
             * @brief Sets the depth of the vibrato
             * 
             * @param d new depth (hz)
             */
            void setDepth(float d)
            {
                depth = d;
            }

            /**
             * @brief Processes input (frequency) with vibrato
             * 
             * @param input input signal
             * @return float 
             */
            float process(float input)
            {
                // apply sine wave to frequency
                float output = input + (depth * sin(dt * speed));
                // i.e. 440hz + (5hz * sin(~)) == 440 +/- 5
                
                // update dt for correct sinewave output
                ++dt;

                return output;
            }

        private:
            int dt;
            // frequency of vib (hz), and how much it effects frequency (hz)
            float speed, depth;
    };
}

/**
 * @brief Theremin class - wrapper for daisysp::Oscillator with additions 
 *        such as a tremolo and vibrato
 * 
 */
class Theremin
{
    public:
        
        /**
         * @brief Default constructor
         * 
         */
        Theremin()
        {
            oscType = SIN;
            sysMode = WAVEFORM;
            outMode = fx::FREQ_AMP;
            frequency = 220.0f;
            amplitude = 0.5f;

            tremActive = false; vibActive = false;
        }

        /**
         * @brief Parameterized constructor
         * 
         * @param osc_type waveform type
         * @param freq     frequency
         * @param amp      amplitude
         */
        Theremin(OscType osc_type, float freq, float amp) :
                oscType(osc_type), frequency(freq), amplitude(amp)
        {
        }

        /**
         * @brief Initializes the theremin with current param values
         * 
         * @param sample_rate sampling rate
         */
        void init(int sample_rate)
        {
            osc.Init(sample_rate);

            setType(oscType);
            osc.SetFreq(frequency);
            osc.SetAmp(amplitude);
        }

        /**
         * @brief Processes output of the theremin with fx
         * 
         * @return float 
         */
        float process()
        {   
            // if fx are active, apply
            if (tremActive)
                setAmplitude(trem.process(amplitude));
            else if (vibActive)
                setFrequency(vib.process(frequency));

            return osc.Process();
        }
        
        /**
         * @brief Sets the waveform of the oscillator
         * 
         * @param t new waveform type
         */
        void setType(OscType t)
        {
            switch(t)
            {
                case SIN:
                    osc.SetWaveform(0);
                    break;
                case TRI:
                    osc.SetWaveform(1);
                    break;
                case SAW:
                    osc.SetWaveform(2);
                    break;
                case SQR:
                    osc.SetWaveform(4);
                    break;
            }
        }

        /**
         * @brief Sets the system mode of the theremin
         * 
         * @param s new system mode
         */
        void setSystemMode(SystemMode s)
        {
            sysMode = s;
        }

        /**
         * @brief Sets the output mode of the theremin
         * 
         * @param m new output mode
         */
        void setOutputMode(fx::OutputMode m)
        {
            outMode = m;
        }

        /**
         * @brief Sets the frequency of the oscillator
         * 
         * @param f new frequency
         */
        void setFrequency(float f)
        {
            frequency = f;
            osc.SetFreq(f);
        }

        /**
         * @brief Sets the amplitude of the oscillator
         * 
         * @param a new amplitude
         */
        void setAmplitude(float a)
        {
            amplitude = a;
            osc.SetAmp(a);
        }

        /**
         * @brief Sets whether tremolo is active or not
         * 
         * @param b is active
         */
        void tremoloActive(bool b)
        {
            tremActive = b;
        }

        /**
         * @brief Sets whether vibrato is active or not
         * 
         * @param b is active
         */
        void vibratoActive(bool b)
        {
            vibActive = b;
        }
        
        OscType oscType;

        SystemMode sysMode;
        fx::OutputMode outMode;

    private:

        // oscillator
        Oscillator osc;

        // fx
        fx::Tremolo trem;
        fx::Vibrato vib;
        
        bool tremActive, vibActive;

        float frequency, amplitude;
};

// hardware object for peripheral initialization and setting
static DaisySeed  hardware;

// output audio source
static Theremin theremin;

// TODO:
// static input handling functions OR put it in Theremin class???

/**
 * @brief Updates status of ir sensors and any parameters associated
 * 
 */
static void updateSensors()
{
    // left sensor (change frequency)

    // right sensor (depends on mode)
    switch(theremin.outMode)
    {
        case fx::FREQ_AMP:
            // change amplitude
            break;
        case fx::FREQ_TREM:
            // change tremolo depth or speed
            break;
        case fx::FREQ_VIB:
            // change vibrato depth or speed
            break;
        case fx::MIDI:
            // ???

            break;
    }
}

/**
 * @brief Updates status of buttons and any parameters associated
 * 
 */
static void updateButtons()
{
    // if both buttons are pressed, change system modes

    // TODO: top l/r buttons should flip between waveforms/output mode here
    if (theremin.sysMode == WAVEFORM)
    {
    }
    else if (theremin.sysMode == OUTPUT_MODE)
    {
    }

    // TODO: bottom l/r buttons should change theremin parameter values,
    // depending on the current output mode
    // check output mode 
    switch(theremin.outMode)
    {
        case fx::FREQ_AMP:
            // change octave (i.e. multiply or divide frequency by 2)
            break;
        case fx::FREQ_TREM:
            // change tremolo depth or speed
            break;
        case fx::FREQ_VIB:
            // change vibrato depth or speed
            break;
        case fx::MIDI:
            // ???

            break;
    }

}

/**
 * @brief Callback function for daisy audio output
 * 
 * @param in   input signal buffer (if any)
 * @param out  output signal buffer
 * @param size size of buffer
 */
static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{
    float signal;

    // retrieve data
    for(size_t i = 0; i < size; i += 2)
    {
        signal = theremin.process();

        // left out
        out[i] = signal;
        // right out
        out[i + 1] = signal;
    }
}

/**
 * @brief Main function with loop housing all theremin updates
 * 
 * @return int 
 */
int main(void)
{
    // initialize seed hardware and oscillator daisysp module
    float sample_rate;

    // configure daisy hardware
    hardware.Configure();
    hardware.Init();
    hardware.SetAudioBlockSize(4);
    sample_rate = hardware.AudioSampleRate();

    // initialize theremin
    theremin.init(sample_rate);

    // ADC configuration
    AdcChannelConfig adcConfig;

    // initialize pins
    adcConfig.InitSingle(hardware.GetPin(SENSOR_L));
    adcConfig.InitSingle(hardware.GetPin(SENSOR_R));
    hardware.adc.Init(&adcConfig, 1);

    // start daisy audio
    hardware.StartAudio(AudioCallback);

    // start reading adc values
    hardware.adc.Start();

    // loop forever
    while (1)
    {
        // update sensors
        updateSensors();

        // update buttons
        updateButtons();

        // wait 1 ms
        System::Delay(1);
    }
}
