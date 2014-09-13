#include <LED_Bar.h>

LED_Bar bar(7, 6);                  // config Io here, (clk, dio)


void setup()
{
    bar.ledIndexBit(0b1101010101);
}

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

void loop()
{
    for(int i=0; i<9; i++)
    {
        bar.ledIndexBit(mouse_buf[i]);
        delay(50);
    }
    
    delay(100);
}