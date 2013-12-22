#include "aduc812.h"
#include "interrupt.h"
#include "max.h"

static uint8_t half_periods=0;
static uint8_t state=0;

static void sound_isr() __interrupt (TF2_VECTOR){
    static uint8_t ENA_Value;
    
    if(half_periods>40){
        state=0;
        half_periods=0;
    }
    
    half_periods++;

    if (state == 1){
        ENA_Value=(read_max(ENA)|ENA_Value^0x1C|0x60);
        write_max(ENA,ENA_Value);
    }

    TH2 = 0xF9;
    TL2 = 0x1E;
}
void make_beep(){
    half_periods = 0;
    state = 1;
    TH2 = 0xF9;
    TL2 = 0x1E;
}

void init_sound(){
    set_vector(TF2_ADDRESS, (void*)sound_isr);
    TR2 = 1;
    ET2 = 1;        
}

