#include <Ultrasonic.h>
#include <Streaming.h>
#include <LED_Bar.h>
#include <ChainableLED.h>
 #include "pitches.h"

#define NUM_LEDS  1

ChainableLED leds(A2, A3, NUM_LEDS);

#define ULTRA_TH            99


LED_Bar bar(7, 6);                  // config Io here, (clk, dio)
Ultrasonic ultrasonic(3);

const int pinFun    = A5;
const int pinPlay   = A0;

#define FUNON()     digitalWrite(pinFun, HIGH)
#define FUNOFF()    digitalWrite(pinFun, LOW)

void playRec()
{
    digitalWrite(pinPlay, HIGH);
    delay(20);
    digitalWrite(pinPlay, LOW);
}

int getUltra()
{
    int value = 0;
    ultrasonic.MeasureInCentimeters();
    value = ultrasonic.RangeInCentimeters;

    return value;
}

bool isUltraNear()
{
    int value = getUltra();
    if(value < ULTRA_TH)return 1;
    return 0;
}


void setup()
{
    Serial.begin(115200);
    pinMode(A5, OUTPUT);
    pinMode(pinPlay, OUTPUT);
    FUNOFF();

    delay(1000);
}


void loop()
{
    if(isUltraNear())
    {
        moveMode();
    }

    breath_item(1, 1);
    delay(10);
}


/*************************************************************************************
 * MOVE MODE
 *************************************************************************************/
void moveMode()
{
    unsigned long timer1 = millis();

    cout << "GET IN MOVE MODE" << endl;

    FUNON();
    while(1)
    {
        int cm_val = getUltra();

        if(cm_val<ULTRA_TH)timer1 = millis();

        if((millis() - timer1) > 1000)
        {
            cout << "OUT MOVE MDOE NOW" << endl;
            break;
        }

        int mouth_delay = 0;
        if(cm_val < ULTRA_TH)
        {
            mouth_delay = cm_val/2;
        }

        mouth(mouth_delay, 1);
        color_change(1, 1);
        play();
        delay(10);                  //detect every 10ms
    }

    FUNOFF();
    mouth(0, 0);
    color_change(0, 0);


}


/*************************************************************************************
 * MOUSE FUNCTION
 *************************************************************************************/
unsigned int mouse_buf[9] =
{
    0b0000110000,
    0b0001111000,
    0b0011111100,
    0b0111111110,
    0b1111111111,
    0b0111111110,
    0b0011111100,
    0b0001111000,
    0b0000110000,
};

// freq: how many times run once, freq x 10ms
// mode = 0: exit mode
// mode = 1: normal mode
void mouth(int freq, int mode)
{

    static unsigned long timer_m = millis();
    static int bit_ctrl = 0;

    if(0 == mode)
    {
        bar.ledIndexBit(mouse_buf[0]);
        return;
    }

    if((millis() - timer_m) > freq)
    {
        timer_m = millis();
        bit_ctrl++;
        if(bit_ctrl>8)bit_ctrl = 0;
        bar.ledIndexBit(mouse_buf[bit_ctrl]);
    }
}


/*************************************************************************************
 * BREATH FUNCTION
 *************************************************************************************/
void setRGB(int r, int g, int b)
{
    leds.setColorRGB(0, r, g, b);
}


// cm: 100-0
void color_change(int freq, int mode)
{
    static int bit_ctrl = 0;
    static unsigned long timer_c = millis();

    if(0 == mode)
    {
        bit_ctrl = 0;
        return;
    }

    if(millis() - timer_c > freq)
    {
        timer_c = millis();

        if(bit_ctrl < 250)
        {
            setRGB(bit_ctrl, 0, 249-bit_ctrl);
        }
        else if(bit_ctrl < 499)
        {
            setRGB(499 - bit_ctrl, bit_ctrl - 250, 0);
        }
        else
        {
            setRGB(0, 749-bit_ctrl, bit_ctrl-500);
        }

        bit_ctrl+=3;
        bit_ctrl = bit_ctrl>749 ? 0 : bit_ctrl;

        delay(freq);

    }


}

void breath_item(int freq, int mode)
{
    static int bit_ctrl = 0;
    static unsigned long timer_b = millis();

    if(0 == mode)
    {
        bit_ctrl = 0;
        setRGB(0, 0, 0);
        return;
    }

    if(millis()-timer_b > freq)
    {
        timer_b = millis();

        if(bit_ctrl<=254)
        {
            setRGB(0, 0, bit_ctrl);
        }
        else if(bit_ctrl<=509)
        {
            setRGB(0, 0, 509-bit_ctrl);
        }

        bit_ctrl+=5;
        if(bit_ctrl>509)bit_ctrl = 0;
    }
}


/*************************************************************************************
 * MUSIC FUNCTION
 *************************************************************************************/
 
int melody[] = {NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4,4,4,4,4 };


// play music here
void play()
{
/*
    for (int thisNote = 0; thisNote < 8; thisNote++) 
    {
        // to calculate the note duration, take one second
        // divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000/noteDurations[thisNote];
        tone(2, melody[thisNote],noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(2);
    }
*/
    static int thisNote = 0;
    static unsigned long timer_p = millis();
    static int pauseBetweenNotes = 0;
    
    if(millis()-timer_p > pauseBetweenNotes)
    {
        timer_p = millis();
        noTone(2);
        int noteDuration = 1000/noteDurations[thisNote];
        tone(2, melody[thisNote],noteDuration);
        
        pauseBetweenNotes = noteDuration * 1.30;
        // delay(pauseBetweenNotes);
        
        thisNote++;
        if(thisNote > 7)thisNote = 0;
    }
}