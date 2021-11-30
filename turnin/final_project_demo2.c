/*	Author: Fio Estuar
 *  Partner(s) Name: 
 *	Lab Section: 23
 *	Assignment: Final Project, Demo 2
 *	Exercise Description: For this part of my final project, I will be adding functionality to control an LED matrix using shift registers....
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
 *		C0..C3 output to the shift registers, which will control the LCD matrix and drive both the obstacles and player's movement.
 *	
 *	
 *	Demo: https://drive.google.com/file/d/1_5A8uceS-aWZt-gQFKODshflqrECMqGm/view?usp=sharing
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

// Global variables related to the joystick
unsigned short x; // Value from ADC, controls x axis.
unsigned short y; // ... controls y axis on joystick
unsigned char joystick; // Controls joystick's output. Only writes to B0...B3
// End joystick variables

// Global variables for game state.
unsigned char gameOver;

// Global control variables
unsigned char restartGame; // Reset button for game, will be wired to A2.
unsigned char score; 

// NEW FOR DEMO 2
// Shift Register
void transmit_data(unsigned char data) {
	unsigned char i;
	for (i = 7; i >= 0 ; --i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x04;
	}

	// set RCLK = 1. Rising edge copies data from the “Shift” register to the “Storage” register
	PORTC |= 0x02;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

enum Player_states {Start_Player_start, Player_start, joystick_read, main_state, intialization, right, left, player_off} Player_state;
void Player() {
	unsigned char Right_Pressed = 0;
	unsigned char Left_Pressed = 0;

	if (joystick == 0x04) {Right_Pressed = 1;}
        else if (joystick == 0x08) {Left_Pressed = 1;}	

	switch (Player_state) // State
		case Start_Player_start:
		if(startGame == 1 && gameOver == 0)
		{
			Player_state = Player_start;
		}
		else
		{
			Player_state = Start_Player_start;
		}
		break;

		case Player_start:
		Player_state = intialization;
		break;

		case intialization:
		Player_state = main_state;
		break;

		case main_state:
		if (Left_Pressed)
		{
			Player_state == left;
			_delay_ms(100);
		}
		if(Right_Pressed)
		{
			Player_state = right;
			_delay_ms(100);
		}
		break;

		case joystick_read:
		if(Right_Pressed == 1 && Left_Pressed == 0) //If joystick reads right side movement,  increment
		{
			Player_state = right;
		}
		else if(Left_Pressed == 1 && Right_Pressed == 0) //If joystick reads left side movement,  increment
		{
			Player_state = left;
		}
		break;

		case left:
		if(gameOver == 1)
		{
			Player_state = player_off;
		}
		else
		{
			Player_state = joystick_read;
		}
		break;

		case right:
		if(gameOver == 1)
		{
			CLEAR_player();
			Player_state = Start_Player_start;
		}
		else
		{
			Player_state = joystick_read;
		}
		break;

		default:
		Player_state = Player_start;
		break;
	}

	switch (Player_state) //State actions
	{
		case Player_start:
		RED_player_start();
		break;

		case intialization:
		RED_player_start();
		bounds = 0x04;
		break;

		case main_state:
		break;

		case left:
		if (bounds > 0x01)
		{
			if(debuff != 1)
			{
				bounds--;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character >> 1) ^ 0x80);
				_delay_ms(10);
			}
			else if (debuff == 1)
			{
				bounds--;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character >> 1) ^ 0x80);
				_delay_ms(50);
			}
		}
		break;

		case right:
		if (bounds < 0x08)
		{
			if(debuff != 1)
			{
				bounds++;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character << 1) ^ 0x01);
				_delay_ms(10);
			}
			else if (debuff == 1)
			{
				bounds++;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character << 1) ^ 0x01);
				_delay_ms(50);
			}
		}
		else if (bounds < 0x08)
		{
			if(debuff != 1)
			{
				bounds++;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character << 1) ^ 0x01);
				_delay_ms(10);
			}
			else if (debuff == 1)
			{
				bounds++;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character << 1) ^ 0x01);
				_delay_ms(50);
			}
		}

		break;

		default:
		break;
	}
}

void RED_player_start()
{
	X_Coordinate_Power_player =	  0b00000001; // POWER X-coordinate
	Y_Coordinate_Green_player = ~(0b00000000); //Green y-coordinate
	Y_Coordinate_Red_player = ~(0b00001000); // RED y-coordinate
}

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
