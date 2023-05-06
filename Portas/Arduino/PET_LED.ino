#include <Adafruit_NeoPixel.h>

#define LED_PIN 2
#define LED_COUNT 100
#define DELAY 500

#define MAGENTA 0xFF00FF
#define WHITE 0xFFFFFF
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define YELLOW 0xFFFF00
#define BLACK 0x000000
#define GREY 0xC0C0C0
#define ORANGE 0xFF7F00
#define PINK 0xFF1CAE

long long int p = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN);

void setup()
{
    strip.begin();
    for (int i = 0; i < LED_COUNT; i++)
        strip.setPixelColor(i, GREY);
    strip.show();
}

void loop()
{
    strip.show();
    
    int color1 = randColor();
    int color2 = randColor();
    int color3 = randColor();
    
    int ledP[LED_COUNT] = {0};
    int ledE[LED_COUNT] = {0};
    int ledT[LED_COUNT] = {0};
    
    for (int i = 20; i < 80; i += 10)
    {
        ledP[i] = 1;
    }
    for (int i = 21; i < 23; i++)
    {
        ledP[i] = 1;
    }
    for (int i = 41; i < 43; i++)
    {
        ledP[i] = 1;
    }
    ledP[32] = 1;
    
    setMultPixelColor(ledP, LED_COUNT, color1);
    
    for (int i = 24; i < 80; i += 10)
    {
        ledE[i] = 1;
    }
    for (int i = 25; i < 27; i++)
    {
        ledE[i] = 1;
    }
    ledE[75] = 1;
    ;
    ledE[76] = 1;
    ledE[55] = 1;
    ;
    
    setMultPixelColor(ledE, LED_COUNT, color2);
    
    for (int i = 28; i < 80; i += 10)
    {
        ledT[i] = 1;
    }
    ledT[27] = 1;
    ledT[29] = 1;
    setMultPixelColor(ledT, LED_COUNT, color3);
    
    delay(DELAY);
}

int randColor()
{
    return random(0, 0xFFFFFF);
}

void setMultPixelColor(int *v, int size, int color)
{
    for (int i = 0; i < size; i++)
        if (v[i] == 1)
            strip.setPixelColor(i, color);
}