// esquema aplicado: https://forums.parallax.com/uploads/attachments/43798/82270.jpg
//https://forums.parallax.com/discussion/133296/using-a-74hc165-and-a-74hc595-on-4-pins-spin


//poner en latch resistencia de 10Kohmls contra gnd


// Define Connections to 74HC595 

const int PIN_595_LATCH = A1;
/*
    74595 -> ST_CP pin 12
*/

const int PIN_595_AND_165_CLOCK = A3;
/*
    74595 -> SH_CP pin 11
    74165 -> CP pin 2
*/

const int PIN_595_AND_165_DATA = A2;
/*
    74595 -> DS pin 14 
    74165 -> Q7 pin 7
*/

// Define Connections to 74HC165
const int PIN_165_LOAD = A0;
/* 
    74165 -> PL pin 1
*/

const int PIN_RELES[8] = {2, 3, 4, 5, 6, 7, 8, 0};

byte buffer_leds[] = {0, 0, 0, 0, 0, 0, 0};
#define LAST_BIT_FOR_LEDS 7

unsigned long buffer_before_keys_timer[] = {0, 0, 0, 0, 0, 0, 0, 0};
byte buffer_before_keys[] = {0, 0, 0, 0, 0, 0, 0, 0};
byte buffer_keys[] = {0, 0, 0, 0, 0, 0, 0, 0};
#define LAST_BIT_FOR_KEYS 8

byte group_keys_from_key_0[] = {0, 2, 3}; // Para usar con Pc+Monitor+Altavoces
byte group_keys_from_key_1[] = {1, 2, 3}; // Para usar con Portatil+Monitor+Altavoces

const int PIN_BEEPER = 12;

void setup()
{
    pinMode(PIN_595_LATCH, OUTPUT);
    pinMode(PIN_165_LOAD, OUTPUT);
    pinMode(PIN_595_AND_165_CLOCK, OUTPUT);
    pinMode(PIN_595_AND_165_DATA, INPUT);
  
    digitalWrite(PIN_165_LOAD, HIGH);
    digitalWrite(PIN_595_AND_165_CLOCK, LOW);

    pinMode(PIN_BEEPER, OUTPUT);

    for(byte loop_byte_led = 0; loop_byte_led < LAST_BIT_FOR_LEDS; loop_byte_led++)
    {
        pinMode(PIN_RELES[loop_byte_led], OUTPUT); //Setea el pin de cada relé
        digitalWrite(PIN_RELES[loop_byte_led], 1); //Apaga el estado de cada relé para iniciar con todos apagados
    }

    // Serial.begin(115200);
    ledsTest();
}


void loop() 
{

    readKeys();
    // for (byte loop_byte_led=0; loop_byte_led < LAST_BIT_FOR_LEDS; loop_byte_led++)
    // {
        // buffer_leds[loop_byte_led] = buffer_keys[loop_byte_led];
        // Serial.print(buffer_keys[loop_byte_led]);
    // }
    // Serial.println();

    flushLeds();
    flushReles();

    for (byte loop_byte_key=0; loop_byte_key < LAST_BIT_FOR_KEYS; loop_byte_key++)
    {
        if (buffer_keys[loop_byte_key] == 1)
        {
            if (buffer_before_keys_timer[loop_byte_key] == 0) buffer_before_keys_timer[loop_byte_key] = millis();
        }
        else
        {
            if (buffer_before_keys[loop_byte_key] == 1 && buffer_keys[loop_byte_key] == 0)
            {
                if ((millis()-buffer_before_keys_timer[loop_byte_key]) > 400)
                {
                    if (key_long_action(loop_byte_key) == 1)
                    {
                        beep();
                        delay(100);
                        beep();
                    }
                }
                else
                {
                    if (key_action(loop_byte_key) == 1)
                    {
                        beep();
                    }
                }
                buffer_before_keys_timer[loop_byte_key] = 0;
            }
        }
        buffer_before_keys[loop_byte_key] = buffer_keys[loop_byte_key];
    }
}

void ledsTest()
{
    byte loop_byte_led = 0;
    byte led_byte_clear = 0;
    for(loop_byte_led = 0; loop_byte_led < LAST_BIT_FOR_LEDS; loop_byte_led++)
    {
        for (led_byte_clear = 0; led_byte_clear < LAST_BIT_FOR_LEDS; led_byte_clear++) { buffer_leds[led_byte_clear] = 0; }
        buffer_leds[loop_byte_led] = 1;
        flushLeds();
        beep();
    }
    for (led_byte_clear = 0; led_byte_clear < LAST_BIT_FOR_LEDS; led_byte_clear++) { buffer_leds[led_byte_clear] = 1; }
    for (led_byte_clear = 0; led_byte_clear < LAST_BIT_FOR_LEDS; led_byte_clear++) { buffer_leds[led_byte_clear] = 0; }
}

void readKeys()
{
    byte chip_bit;
    delay(5);
    pinMode(PIN_595_AND_165_DATA, INPUT);
    digitalWrite(PIN_165_LOAD, HIGH);
    delay(5);
    digitalWrite(PIN_165_LOAD, LOW);
    digitalWrite(PIN_165_LOAD, HIGH);
    delay(5);

    byte keys = shiftIn(PIN_595_AND_165_DATA, PIN_595_AND_165_CLOCK, LSBFIRST);
    for(byte loop_byte_key = 0; loop_byte_key < LAST_BIT_FOR_KEYS; loop_byte_key++)
    {
        switch(loop_byte_key)
        {
            case 0: chip_bit = 6; break;
            case 1: chip_bit = 5; break;
            case 2: chip_bit = 4; break;
            case 3: chip_bit = 3; break;
            case 4: chip_bit = 2; break;
            case 5: chip_bit = 1; break;
            case 6: chip_bit = 0; break;
            case 7: chip_bit = 7; break;
            case 8: chip_bit = 8; break;
        }
        buffer_keys[chip_bit] = 1;
        if (bitRead(keys, loop_byte_key)) buffer_keys[chip_bit] = 0;
    }
}

void flushLeds()
{
    byte leds_value = 0;
    byte chip_bit = 0;
    delay(5);
    pinMode(PIN_595_AND_165_DATA, OUTPUT);
    for (byte loop_byte_led = 0; loop_byte_led < LAST_BIT_FOR_LEDS; loop_byte_led++)
    {
        switch(loop_byte_led)
        {
            case 0: chip_bit = 6; break;
            case 1: chip_bit = 5; break;
            case 2: chip_bit = 4; break;
            case 3: chip_bit = 3; break;
            case 4: chip_bit = 2; break;
            case 5: chip_bit = 1; break;
            case 6: chip_bit = 0; break;
        }
        leds_value <<= 1;
        bool real_value = buffer_leds[chip_bit];
        leds_value |= real_value;
    }
    digitalWrite(PIN_595_LATCH, LOW);
    shiftOut(PIN_595_AND_165_DATA, PIN_595_AND_165_CLOCK, MSBFIRST, leds_value);
    digitalWrite(PIN_595_LATCH, HIGH);
}

void flushReles()
{
    // Nos basamos en el buffer de leds para setear cada relé
    for(byte loop_byte_led = 0; loop_byte_led < LAST_BIT_FOR_LEDS; loop_byte_led++)
    {
        digitalWrite(PIN_RELES[loop_byte_led], !buffer_leds[loop_byte_led]);
    }
}

void beep()
{
    digitalWrite(PIN_BEEPER, 1);
    delay(5);
    digitalWrite(PIN_BEEPER, 0);
}

bool key_action(byte byte_key)
{
    byte loop_byte_led = 0;
    if (byte_key == 7)
    {
        for(loop_byte_led = 0; loop_byte_led < LAST_BIT_FOR_LEDS; loop_byte_led++)
        {
            buffer_leds[loop_byte_led] = 0;
        }
        return 1;
    }
    else
    {
        buffer_leds[byte_key] = !buffer_leds[byte_key];
    }
    return 1;
}

bool key_long_action(byte byte_key)
{
    byte *group_keys;
    switch(byte_key)
    {
        case 0:
            group_keys = group_keys_from_key_0;
            break;
        case 1:
            group_keys = group_keys_from_key_1;
            break;
        default:
            return 0;
    }

    for (byte loop = 0; loop < sizeof(group_keys)+1; loop++)
    {
        buffer_leds[group_keys[loop]] = 1;
    }
    return 1;
}
