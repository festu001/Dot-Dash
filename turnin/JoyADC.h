#ifndef __JOYADC_H__
#define __JOYADC_H__

// Global variables related to the joystick ADC
unsigned short x; // Value from ADC, controls x axis.
unsigned short y; // ... controls y axis on joystick
unsigned char joystick; // Controls joystick's output. Uses only b0 and b1. (!b0 && !b1) = no movement, b0 = left, b1 = right.
// End joystick variables

void Read_ADC(unsigned char axis) {
        if (axis == 'x') {
                // Read X-Axis input
                ADMUX = 0xC0;
                _delay_ms(10);
                x = ADC;
        }

        else if (axis == 'y') {
                // Read Y-Axis input
                ADMUX = 0xC1;
                _delay_ms(10);
                y = ADC;
        }
}

void ADC_init(){
        ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADATE);
}

#endif
