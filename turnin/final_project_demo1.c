/*	Author: Fio Estuar
 *  Partner(s) Name: 
 *	Lab Section: 23
 *	Assignment: Final Project, Demo 1
 *	Exercise Description: For this part of my final project, I will be setting up my joystick using ADC in the following way...
 *	
 *		|| INPUTS ||
 *		A0: The ADC result for the left/right axis
 *		A1: The ADC result for the up/down axis.
 *
 * 		|| Outputs ||
 *		B0...B3: A 4-bit output (currently wired to LED's, but intended to be sent to the LED matrix once that is set up) 
 *			with the following meanings. Each light being on represents that the direction represented by that light 
 *			(up, down, left, right) is currently being read from the joystick. All lights off represents zero. The Up/Down 
 *			and Left/Right axes are independent from each other, meaning combinations such as Up-Left, Down-Right, Up-Right, Down-Left
 *			are possible. However, the pairs B0-B1 and B2-B3 are mutually exclusive, meaning a combination such as Up-Down is not possible.
 *
 *			B0: Up
 *			B1: Down
 *			B2: Left
 *			B3: Right
 *	
 *	Please read the README in my submission zip too! :)
 *	Demo Link: https://drive.google.com/file/d/1_5A8uceS-aWZt-gQFKODshflqrECMqGm/view?usp=sharing
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>

#include <avr/interrupt.h>

#define F_CPU 1000000UL  // 1 MHz
#include <util/delay.h>

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"

#endif

unsigned short x; // Value from ADC, controls x axis.
unsigned short y; // ... controls y axis on joystick
unsigned char joystick; // Controls joystick's output. Only writes to B0...B3

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
enum joy_states {joy_start, joy_run} joy_state;
void Tick_Joystick() {
	switch(joy_state) { // Transitions
		case joy_start:
			joy_state = joy_run;
			break;

		case joy_run:
			joy_state = joy_run;
			break;

		default: break;
	}

	switch(joy_state) { // State actions
		case joy_start:
			joystick = 0x00;
			break;

		case joy_run: 
			joystick = 0x00; // Initialize to zero at every tick to allow a clean value for masking.

			// Check for neutral state (no movement on joystick).
			if ( (x > 464 && x < 560) && (y > 464 && y < 560)) { // Neutral. Center point is 512, with a deadzone of +/-48 for both axes
				joystick = 0x00;
			}


			else { // Some movement detected.
				// Check for x-axis movement.
				if (x <= 464) { // Left should be x = [0, 464]. We have already checked for the neutral position in the first if statement, so no need to check again.
					joystick = joystick || (0x04); // Set B2 = 1, which should be left.
				}

				else if (x >= 560) { // Right should be x = [560, 1024].
					joystick = joystick || (0x08); // B3 = 1, which is right.
				} 

				// Check for y-axis movement.
				if (y <= 464) {
					joystick = joystick || (0x01); // B0 = 1, up 
				}

				else if (y >= 560) {
					joystick = joystick || (0x02); // B1 = 1, down
				}
			}
			break;
	}
}

enum output_states {output_start, output_write} output_state;
void Tick_Output() {
	switch(output_state) { // Transitions
		case output_start:
			output_state = output_write;	
			break;

		case output_write:
			output_state = output_write;
			break;

		default: break;
	}

	switch(output_state) { // State actions
		case output_start:
			break;

		case output_write:
			PORTB = joystick;
			break;
	}
}

void ADC_init(){
	ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADATE);
}

int main(void) {
    	DDRA = 0x00; PORTA = 0xFF; // PINA is input.
    	DDRB = 0xFF; PORTB = 0x00; // PORTB is output.

    	ADC_init(); // Initialize ADC.

	/* Timer stuff for the future since I know I'll need it later.
	Timer_Set(TODO);
	Timer_On();
	*/ 

	/* Insert your solution below */
    	while (1) {
		Read_ADC('x');
		Read_ADC('y');
		Tick_Joystick();
		Tick_Output();
    	}
    
	return 1;
}
