#include <Arduino.h>
#include "ym2203c.hpp"

#define HC595_SCK (21)
#define HC595_RCK (20)
#define HC595_SI (28)
#define YM2203_CS1 (27)
#define YM2203_WR (26)
#define YM2203_A0 (22)
YM2203C sound;

void setup_debug_serial_out(void) {
    #define DBG Serial1.printf
    Serial1.setRX(17u);
    Serial1.setTX(16u);
    Serial1.begin();
}

void setup() {
    pinMode(25, OUTPUT);
    sound.init(HC595_SCK, HC595_RCK, HC595_SI, YM2203_CS1, YM2203_WR, YM2203_A0);
    // 22.6757us(44100Hz)
    sound.repeatied_timer_us(22); //interval setting
}

bool loop_started = false;
void loop() {
    loop_started = true;
    static int a;

    // LED blink ctrl
    if(++a == 2) {
        a = 0;
    }
    if(a) {
        sio_hw->gpio_set = (1ul << 25) | 1;
    } else {
        sio_hw->gpio_clr = (1ul << 25) | 1;
    }

    delay(100);
}

void setup1(void) {
}

void loop1(void) {
}
