/*
 * ____                     _      ______ _____    _____
  / __ \                   | |    |  ____|  __ \  |  __ \
 | |  | |_ __   ___ _ __   | |    | |__  | |  | | | |__) |__ _  ___ ___
 | |  | | '_ \ / _ \ '_ \  | |    |  __| | |  | | |  _  // _` |/ __/ _ \
 | |__| | |_) |  __/ | | | | |____| |____| |__| | | | \ \ (_| | (_|  __/
  \____/| .__/ \___|_| |_| |______|______|_____/  |_|  \_\__,_|\___\___|
        | |
        |_|
 Open LED Race
 Uma corrida de carro minimalista para fitas LED

 Este é um programa grátis; você pode distribuir e/ou modificar
 isso pelos termos do GNU Gerneral Public License (Licença Pública Geral) publicando pela
 Free Software Foundation (Fundação de Software Gratuitos); e também pela versão 3 da Licença, ou
 (à sua escolha) qualquer versão recente.
 por gbarbarov@singulardevices.com para Arduino day Seville 2019
 https://www.hackster.io/gbarbarov/open-led-race-a0331a
 https://twitter.com/openledrace

 Próximos melhoramentos em:
 https://gitlab.com/open-led-race
 https://openledrace.net/open-software/

*/

#include <Adafruit_NeoPixel.h>
#define MAXLED 300 // Máximo de leds da pista

// Pins Arduino Day 19 version
#define PIN_LED A0  // R 470 ohms para o pino DI da fita WS2812 e WS2813,  CAP 1000 uF, VCC 5v/GND, fonte de energia de 5V 2A.
#define PIN_P1 7    // botão do player 1: PIN e GND
#define PIN_P2 6    // botão do player 2: PIN e GND
#define PIN_AUDIO 3 // Ligar um falante de 4 a 8 ohms, mas passando por um capacitor de 1uf

int NPIXELS = MAXLED;

#define COLOR1 track.Color(255, 0, 0) // Cor carrinho 1
#define COLOR2 track.Color(0, 255, 0) // Cor carrinho 2

int win_music[] = { // Música de vitória
    2093, 2637, 3136, 2093,
    2637, 3136, 2093, 2637,
    3136};

byte gravity_map[MAXLED];

int TBEEP = 3;

float speed1 = 0;
float speed2 = 0;
float dist1 = 0;
float dist2 = 0;

byte loop1 = 0;
byte loop2 = 0;

byte leader = 0;
byte loop_max = 5; // total de voltas da corrida

float ACEL = 0.2;
float kf = 0.015; // constante de atrito
float kg = 0.003; // constante de gravidade

byte flag_sw1 = 0;
byte flag_sw2 = 0;
byte draworder = 0;

unsigned long timestamp = 0;

Adafruit_NeoPixel track = Adafruit_NeoPixel(MAXLED, PIN_LED, NEO_GRB + NEO_KHZ800);

int tdelay = 5;

void set_ramp(byte H, byte a, byte b, byte c)
{
    for (int i = 0; i < (b - a); i++)
    {
        gravity_map[a + i] = 127 - i * ((float)H / (b - a));
    };
    gravity_map[b] = 127;
    for (int i = 0; i < (c - b); i++)
    {
        gravity_map[b + i + 1] = 127 + H - i * ((float)H / (c - b));
    };
}

void set_loop(byte H, byte a, byte b, byte c)
{
    for (int i = 0; i < (b - a); i++)
    {
        gravity_map[a + i] = 127 - i * ((float)H / (b - a));
    };
    gravity_map[b] = 255;
    for (int i = 0; i < (c - b); i++)
    {
        gravity_map[b + i + 1] = 127 + H - i * ((float)H / (c - b));
    };
}

void setup()
{
    for (int i = 0; i < NPIXELS; i++)
    {
        gravity_map[i] = 127;
    };
    track.begin();
    pinMode(PIN_P1, INPUT_PULLUP);
    pinMode(PIN_P2, INPUT_PULLUP);

    if ((digitalRead(PIN_P1) == 0)) // apertar o player 1 na incialização para ativar a física
    {

        // RAMPA

        set_ramp(12, 90, 100, 110); // rampa centrada no LED 100, com 10 LEDs de subida e 10 LEDs de decida
        for (int i = 0; i < NPIXELS; i++)
        {
            track.setPixelColor(i, track.Color(0, 0, (127 - gravity_map[i]) / 8));
        };
        track.show();

        // Para fazer novas rampas, basta copiar o set_ramp e atribuir novos valores: set_ramp(12, x, y, z)
        //  x = Início da rampa
        //  y = Pico da rampa
        //  z = final da rampa
    };
    start_race();
}

void start_race()
{
    for (int i = 0; i < NPIXELS; i++)
    {
        track.setPixelColor(i, track.Color(0, 0, 0));
    };
    track.show();
    delay(2000);
    track.setPixelColor(12, track.Color(0, 255, 0));
    track.setPixelColor(11, track.Color(0, 255, 0));
    track.show();
    tone(PIN_AUDIO, 400);
    delay(2000);
    noTone(PIN_AUDIO);
    track.setPixelColor(12, track.Color(0, 0, 0));
    track.setPixelColor(11, track.Color(0, 0, 0));
    track.setPixelColor(10, track.Color(255, 255, 0));
    track.setPixelColor(9, track.Color(255, 255, 0));
    track.show();
    tone(PIN_AUDIO, 600);
    delay(2000);
    noTone(PIN_AUDIO);
    track.setPixelColor(9, track.Color(0, 0, 0));
    track.setPixelColor(10, track.Color(0, 0, 0));
    track.setPixelColor(8, track.Color(255, 0, 0));
    track.setPixelColor(7, track.Color(255, 0, 0));
    track.show();
    tone(PIN_AUDIO, 1200);
    delay(2000);
    noTone(PIN_AUDIO);
    timestamp = 0;
};

void winner_fx()
{
    int msize = sizeof(win_music) / sizeof(int);
    for (int note = 0; note < msize; note++)
    {
        tone(PIN_AUDIO, win_music[note], 200);
        delay(230);
        noTone(PIN_AUDIO);
    }
};

void burning1()
{
    // to do
}

void burning2()
{
    // to do
}

void track_rain_fx()
{
    // to do
}

void track_oil_fx()
{
    // to do
}

void track_snow_fx()
{
    // to do
}

void fuel_empty()
{
    // to do
}

void fill_fuel_fx()
{
    // to do
}

void in_track_boxs_fx()
{
    // to do
}

void pause_track_boxs_fx()
{
    // to do
}

void flag_boxs_stop()
{
    // to do
}

void flag_boxs_ready()
{
    // to do
}

void draw_safety_car()
{
    // to do
}

void telemetry_rx()
{
    // to do
}

void telemetry_tx()
{
    // to do
}

void telemetry_lap_time_car1()
{
    // to do
}

void telemetry_lap_time_car2()
{
    // to do
}

void telemetry_record_lap()
{
    // to do
}

void telemetry_total_time()
{
    // to do
}

int read_sensor(byte player)
{
    // to do
}

int calibration_sensor(byte player)
{
    // to do
}

int display_lcd_laps()
{
    // to do
}

int display_lcd_time()
{
    // to do
}

void draw_car1(void)
{
    for (int i = 0; i <= loop1; i++)
    {
        track.setPixelColor(((word)dist1 % NPIXELS) + i, track.Color(0, 255 - i * 20, 0));
    };
}

void draw_car2(void)
{
    for (int i = 0; i <= loop2; i++)
    {
        track.setPixelColor(((word)dist2 % NPIXELS) + i, track.Color(255 - i * 20, 0, 0));
    };
}

void loop()
{
    // for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,0,0));};
    for (int i = 0; i < NPIXELS; i++)
    {
        track.setPixelColor(i, track.Color(0, 0, (127 - gravity_map[i]) / 8));
    };

    if ((flag_sw1 == 1) && (digitalRead(PIN_P1) == 0))
    {
        flag_sw1 = 0;
        speed1 += ACEL;
    };
    if ((flag_sw1 == 0) && (digitalRead(PIN_P1) == 1))
    {
        flag_sw1 = 1;
    };

    if ((gravity_map[(word)dist1 % NPIXELS]) < 127)
        speed1 -= kg * (127 - (gravity_map[(word)dist1 % NPIXELS]));
    if ((gravity_map[(word)dist1 % NPIXELS]) > 127)
        speed1 += kg * ((gravity_map[(word)dist1 % NPIXELS]) - 127);

    speed1 -= speed1 * kf;

    if ((flag_sw2 == 1) && (digitalRead(PIN_P2) == 0))
    {
        flag_sw2 = 0;
        speed2 += ACEL;
    };
    if ((flag_sw2 == 0) && (digitalRead(PIN_P2) == 1))
    {
        flag_sw2 = 1;
    };

    if ((gravity_map[(word)dist2 % NPIXELS]) < 127)
        speed2 -= kg * (127 - (gravity_map[(word)dist2 % NPIXELS]));
    if ((gravity_map[(word)dist2 % NPIXELS]) > 127)
        speed2 += kg * ((gravity_map[(word)dist2 % NPIXELS]) - 127);

    speed2 -= speed2 * kf;

    dist1 += speed1;
    dist2 += speed2;

    if (dist1 > dist2)
    {
        leader = 1;
    }
    if (dist2 > dist1)
    {
        leader = 2;
    };

    if (dist1 > NPIXELS * loop1)
    {
        loop1++;
        tone(PIN_AUDIO, 600);
        TBEEP = 2;
    };
    if (dist2 > NPIXELS * loop2)
    {
        loop2++;
        tone(PIN_AUDIO, 700);
        TBEEP = 2;
    };

    if (loop1 > loop_max)
    {
        for (int i = 0; i < NPIXELS; i++)
        {
            track.setPixelColor(i, track.Color(0, 255, 0));
        };
        track.show();
        winner_fx();
        loop1 = 0;
        loop2 = 0;
        dist1 = 0;
        dist2 = 0;
        speed1 = 0;
        speed2 = 0;
        timestamp = 0;
        start_race();
    }
    if (loop2 > loop_max)
    {
        for (int i = 0; i < NPIXELS; i++)
        {
            track.setPixelColor(i, track.Color(255, 0, 0));
        };
        track.show();
        winner_fx();
        loop1 = 0;
        loop2 = 0;
        dist1 = 0;
        dist2 = 0;
        speed1 = 0;
        speed2 = 0;
        timestamp = 0;
        start_race();
    }
    if ((millis() & 512) == (512 * draworder))
    {
        if (draworder == 0)
        {
            draworder = 1;
        }
        else
        {
            draworder = 0;
        }
    };

    if (draworder == 0)
    {
        draw_car1();
        draw_car2();
    }
    else
    {
        draw_car2();
        draw_car1();
    }

    track.show();
    delay(tdelay);

    if (TBEEP > 0)
    {
        TBEEP -= 1;
        if (TBEEP == 0)
        {
            noTone(PIN_AUDIO);
        };
    };
}