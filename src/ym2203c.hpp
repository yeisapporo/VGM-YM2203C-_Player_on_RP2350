#ifndef _HPP_YM2203C_HPP_
#define _HPP_YM2203C_HPP_

#include "banoashouse.hpp"

typedef struct {
    uint8_t vgm_ident[4];
    uint32_t eof_offset;
    uint32_t version;
    uint32_t sn76489_clock;
    uint32_t ym2413_clock;
    uint32_t gd3_offset;
    uint32_t total_samples_num;
    uint32_t loop_offset;
    uint32_t loop_samples_num;
    uint32_t rate;
    uint16_t sn_fb;
    uint8_t snw;
    uint8_t sf;
    uint32_t ym2612_clock;
    uint32_t ym2151_clock;
    uint32_t vgm_data_offset;
    uint32_t sega_pcm_clock;
    uint32_t spcm_interface;
    uint32_t rf5c68_clock;
    uint32_t ym2203_clock;
    uint32_t ym2608_clock;
    uint32_t ym2610_b_clock;
    uint32_t ym3812_clock;
    uint32_t ym3526_clock;
    uint32_t y8950_clock;
    uint32_t ymf262_clock;
    uint32_t ymf278b_clock;
    uint32_t ymf271_clock;
    uint32_t ymz280b_clock;
    uint32_t rf5c164_clock;
    uint32_t pwm_clock;
    uint32_t ay8910_clock;
    uint8_t ayt;
    uint8_t ay_flags[3];
    uint8_t vm;
    uint8_t reserved_7d;
    uint8_t lb;
    uint8_t lm;
    uint32_t bg_dmg_clock;
    uint32_t nes_apu_clock;
    uint32_t multipcm_clock;
    uint32_t upd7759_clock;
    uint32_t okim6258_clock;    
    uint8_t of;
    uint8_t kf;
    uint8_t cf;
    uint8_t reserved_97;
    uint32_t okim6295_clock;
    uint32_t k051649_clock;
    uint32_t k054539_clock;
    uint32_t huc6280_clock;
    uint32_t c140_clock;
    uint32_t k053260_clock;
    uint32_t pokey_clock;
    uint32_t qsound_clock;
    uint32_t scsp_clock;
    uint32_t extra_hdr_ofs;
    uint32_t wonderswan_clock;
    uint32_t vsu_clock;
    uint32_t saa1099_clock;
    uint32_t es5503_clock;
    uint32_t es5506_clock;
    uint16_t es_chns;
    uint8_t cd;
    uint8_t reserved_d7;
    uint32_t x1_010_clock;
    uint32_t c352_clock;
// actual vgm body starts at 0xe0, so delete #if block below (PC-88/98 data only?).
#if 0
    uint32_t ga20_clock;
    
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t reserved_fc;
#endif
    #define VGM_DATA_DUMMY_SIZE (1)
    uint8_t data[VGM_DATA_DUMMY_SIZE];
} VGM_DATA;

class YM2203C {
    private:
    static pin_size_t _hc595_sck;
    static pin_size_t _hc595_rck;
    static pin_size_t _hc595_si;
    static pin_size_t _ym2203_cs1;
    static pin_size_t _ym2203_wr;
    static pin_size_t _ym2203_a0;

    public:    
    static const VGM_DATA *pvgm;
    static uint16_t sample_wait;
    static uint32_t datget_cnt;

    static void init(
        pin_size_t hc595_sck,
        pin_size_t hc595_rck,
        pin_size_t hc595_si,
        pin_size_t ym2203_cs1,
        pin_size_t ym2203_wr,
        pin_size_t ym2203_a0
    ) {
        _hc595_sck = hc595_sck;
        _hc595_rck = hc595_rck;
        _hc595_si = hc595_si;
        _ym2203_cs1 = ym2203_cs1;
        _ym2203_wr = ym2203_wr;
        _ym2203_a0 = ym2203_a0;        

        pinMode(_hc595_sck, OUTPUT);
        pinMode(_hc595_rck, OUTPUT);
        pinMode(_hc595_si, OUTPUT);
        pinMode(_ym2203_cs1, OUTPUT);
        pinMode(_ym2203_wr, OUTPUT);
        pinMode(_ym2203_a0, OUTPUT);
    
        digitalWrite(_hc595_sck, LOW);
        digitalWrite(_hc595_rck, LOW);
        digitalWrite(_hc595_si, LOW);
        digitalWrite(_ym2203_wr, LOW);
        digitalWrite(_ym2203_a0, LOW);
        
        digitalWrite(_ym2203_cs1, LOW);    
    }

    static uint8_t datget(void) {
        return pvgm->data[datget_cnt++];
    }

    static unsigned int wait_ym2203c[2];
    static void write_ym2203(uint8_t bus_select, uint8_t dat) {
        #define WRITE_ENABLE    (0)
        #define WRITE_DISABLE    (1)
        // wait (us)
        #define ADDR_WAIT (5)
        #define DATA_WAIT (22)
        // bus selection
        #define ADDR_BUS (0)
        #define DATA_BUS (1)

        digitalWrite(_ym2203_wr, WRITE_DISABLE);
        digitalWrite(_ym2203_a0, bus_select);
        delayMicroseconds(DATA_WAIT);
        digitalWrite(_hc595_rck, LOW);
        digitalWrite(_hc595_sck, LOW);
        shiftOut(_hc595_si, _hc595_sck, MSBFIRST, dat);
        // ***** needed if cascading 74HC595. *****
        //shiftOut(_hc595_si, _hc595_sck, MSBFIRST, 0);
        digitalWrite(_hc595_rck, HIGH);
        digitalWrite(_ym2203_wr, WRITE_ENABLE);

        delayMicroseconds(wait_ym2203c[bus_select]);

        digitalWrite(_ym2203_wr, WRITE_DISABLE);
    }

    static void vgm_play(void) {
        uint8_t cmd;
        uint8_t aa;
        uint8_t dd;
        static bool started = false;
        
        if(!started) {
            // set prescaler.
            write_ym2203(ADDR_BUS, 0x2d);
            write_ym2203(DATA_BUS, 0xff);
            started = true;
        }

        if(sample_wait != 0) {
            sample_wait--;
            return;
        }

        cmd = datget();
        switch (cmd) {
        case 0x55:
            aa = datget();
            dd = datget();
            write_ym2203(ADDR_BUS, aa);
            write_ym2203(DATA_BUS, dd);
            break;
        case 0x61:
            sample_wait = datget() + (datget() << 8);
            break;
        case 0x62:
            sample_wait = 735;
            break;
        case 0x63:
            sample_wait = 882;
            break;
        case 0x66:
            break;
        case 0x70: case 0x71: case 0x72: case 0x73:
        case 0x74: case 0x75: case 0x76: case 0x77:
        case 0x78: case 0x79: case 0x7a: case 0x7b:
        case 0x7c: case 0x7d: case 0x7e: case 0x7f:
            sample_wait = cmd - 0x6f;
            break;
        default:
            //DBG("***** encountered unknown command for YM2203. quit. *****\r\n");
            datget_cnt = 0;
            break;
        }

    }

    static uint32_t timeout_us;
    static void timer_handler(void) {
        hw_clear_bits(&timer_hw->intr, 1u << TIMER0_IRQ_0);
        timer_hw->alarm[TIMER0_IRQ_0] = (uint32_t)(timer_hw->timerawl + timeout_us);    
        vgm_play();
    }
    
    void repeatied_timer_us(uint32_t delay_us) {
        timeout_us = delay_us;
        hw_set_bits(&timer_hw->inte, 1u << TIMER0_IRQ_0);
        irq_set_exclusive_handler(TIMER0_IRQ_0, timer_handler);
        irq_set_enabled(TIMER0_IRQ_0, true);
        timer_hw->alarm[TIMER0_IRQ_0] = (uint32_t)(timer_hw->timerawl + delay_us);
    }
};

// vgm data body:
const VGM_DATA *YM2203C::pvgm = (VGM_DATA *)&vgm_data[0];
// initial values:
uint16_t YM2203C::sample_wait = 0;
uint32_t YM2203C::datget_cnt = 0;
uint32_t YM2203C::timeout_us = 0;
pin_size_t YM2203C::_hc595_sck = 21;
pin_size_t YM2203C::_hc595_rck = 20;
pin_size_t YM2203C::_hc595_si = 28;
pin_size_t YM2203C::_ym2203_cs1 = 27;
pin_size_t YM2203C::_ym2203_wr = 26;
pin_size_t YM2203C::_ym2203_a0 = 22;
unsigned int YM2203C::wait_ym2203c[] = {ADDR_WAIT, DATA_WAIT};

#endif // _HPP_YM2203C_HPP_
