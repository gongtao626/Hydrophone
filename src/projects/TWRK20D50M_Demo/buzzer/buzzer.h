


void  buzzer_on(void);
void  buzzer_off(void);

void tone_sweep_init(void);



void  pit1_isrv(void);
void  PIT_init(void);
void  buzzer_ftm_init(void);
void set_out_module(uint16 new_module);

extern uint32 ti_delay;
extern uint32 ti_accel_sampling;
extern uint16 ti_echo;
extern uint16 ti_fall;
extern uint16 ti_tone_wait;
extern uint16 ti_test_leds;
extern uint16 ti_test_adc;


extern char echo_flag;
extern uint8  tone_enable; 
extern uint8 base_time125m;

/*
 musical tones
_S for SHARP TONES
*/

extern const uint16 C_MAJ_SCALE[];

typedef enum _TONES
{
     _G4  =   0 ,
     _G4_S  =   1 ,
     _A4  =   2 ,
     _A4_S  =   3 ,
     _B4  =   4 ,
     _C5  =   5 ,
     _C5_S  =   6 ,
     _D5  =   7 ,
     _D5_S  =   8 ,
     _E5  =   9 ,
     _F5  =   10 ,
     _F5_S  =   11 ,
     _G5  =   12 ,
     _G5_S  =   13 ,
     _A5  =   14 ,
     _A5_S  =   15 ,
     _B5  =   16 ,
     _C6  =   17 ,
     _C6_S  =   18 ,
     _D6  =   19 ,
     _D6_S  =   20 ,
     _E6  =   21 ,
     _F6  =   22 ,
     _F6_S  =   23 ,
     _G6  =   24 ,
     _G6_S  =   25 ,
     _A6  =   26 ,
     _A6_S  =   27 ,
     _B6  =   28 ,
     _C7  =   29 ,
     _C7_S  =   30 ,
     _D7  =   31 ,
     _D7_S  =   32 ,
     _E7  =   33 ,
     _F7  =   34 ,
     _F7_S  =   35 ,
     _G7  =   36 ,
     _G7_S  =   37 ,
     _A7  =   38 ,

} TONES;

void tone(TONES tone,uint16 time,uint16 pause);