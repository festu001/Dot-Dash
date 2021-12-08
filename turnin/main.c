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
 *		C0..C3
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
#endif

#include "timer.h"
#include "Matrix.h" // Custom header file I wrote to drive the LED matrix.
#include "JoyADC.h" // Custom header file with relevant ADC drivers for the joystick.

//#include "scheduler.h"

// From Matrix.h
// 	unsigned char playerPosition;
// 	unsigned char playerCollision; // 1 if the player collides with a wall, 0 otherwise.
//	unsigned char dangerous; // 8-bit variable listing currently dangerous spots in row 1 (bottom). if a bit is 1, that column is dangerous to be in.
//	const unsigned char refreshRate = 10; // value in ms for how fast the LED matrix scans.

// From JoyADC.h
// 	unsigned short x; // Value from ADC, controls x axis.
//	unsigned short y; // ... controls y axis on joystick
//	unsigned char joystick; // Controls joystick's output. Uses only b0 and b1. (!b0 && !b1) = no movement, b0 = left, b1 = right.
// 	

// Game logic variables
unsigned char level = 1;
unsigned char reset = 0; // Reset button for game, will be wired to A2.
unsigned char score = 0; // 2 bit value that counts up to 3.
unsigned char win = 0; // 
unsigned char playerDead = 0; // Boolean

enum matrix_states {matrix_start, matrix1, matrix2, matrix3, matrix4, matrix5, matrix6, matrix7,matrix8, matrix9, matrix10, matrix_wait, matrix_stop} matrix_state;
void Tick_DrawMatrix() {
	static unsigned short i; // counter
	unsigned short numCycles = 100;
	const unsigned char walls[4] = {0xDB, 0xFC, 0xE7, 0x3F};

	switch(matrix_state) { // Transitions
		if (playerDead) {
			matrix_state = matrix_stop;
			break;
		}

		case matrix_start:
			matrix_state = matrix1;
			break;

		case matrix1: 
			if (playerDead) {
				matrix_state = matrix_stop;
			}

			if (i <= numCycles) {
				matrix_state = matrix1;
				i++;
			}

			else if (i > numCycles) { 
				matrix_state = matrix2; 
				i = 0;
			}
			break;
			
		case matrix2:
                        if (i <= numCycles) {
                                matrix_state = matrix2;
                                i++;
                        }

                        else if (i > numCycles) {
                                matrix_state = matrix3;
                                i = 0;
                        }
                        break;
		
		case matrix3:
                        if (i <= numCycles) {
                                matrix_state = matrix3;
                                i++;
                        }

                        else if (i > numCycles) {
                                matrix_state = matrix4;
                                i = 0;
                        }
                        break;

		case matrix4:
                        if (i <= numCycles) {
                                matrix_state = matrix4;
                                i++;
                        }

                        else if (i > numCycles) {
                                matrix_state = matrix5;
                                i = 0;
                        }
                        break;

		case matrix5:
                        if (i <= numCycles) {
                                matrix_state = matrix5;
                                i++;
                        }

                        else if (i > numCycles) {
                                matrix_state = matrix6;
                                i = 0;
                        }
                        break;

		case matrix6:
                        if (i <= numCycles) {
                                matrix_state = matrix6;
                                i++;
                        }

                        else if (i > numCycles) {
                                matrix_state = matrix7;
                                i = 0;
                        }
                        break;

		case matrix7:
                        if (i <= numCycles) {
                                matrix_state = matrix7;
                                i++;
                        }

                        else if (i > numCycles) {
                                matrix_state = matrix8;
                                i = 0;
                        }
                        break;

		case matrix8:
                        if (i <= numCycles) {
                                matrix_state = matrix8;
                                i++;
                        }

                        else if (i > numCycles) {
                                matrix_state = matrix9;
                                i = 0;
                        }
                        break;

		case matrix9:
                        if (i <= numCycles) {
                                matrix_state = matrix9;
                                i++;
                        }

                        else if (i > numCycles) {
                                matrix_state = matrix10;
                                i = 0;
                        }
                        break;
		
		case matrix10:
                        if (i <= numCycles) {
                                matrix_state = matrix10;
                                i++;
                        }

                        else if (i > numCycles) {
                                matrix_state = matrix_wait;
                                i = 0;

				// Adjust global game variables while waiting to start next level and speed up by 15%.
				score++;
				level++;
				numCycles -= 15;
                        }
                        break;

		case matrix_wait:
			if (i <= 200) { 
				matrix_state = matrix_wait;
				i++;
			}

			else {
				matrix_state = matrix1;
				i = 0;
			}
			break;

		case matrix_stop:
			matrix_state = matrix_stop;
			break;
	}

	switch(matrix_state) { // State actions

		case matrix_start:
			i = 0;
			numCycles = 10; // Level 1, the walls update move every 1000 ms, while the function ticks every 10ms.
			break;

		case matrix1:
			// Draw the currently active walls
			Scan_Matrix(8, walls[2]);
			Scan_Matrix(6, walls[1]);
			Scan_Matrix(4, walls[0]);

			// Mark which tiles are dangerous
			Set_Dangerous(0x00);

			// Draw the player.
			Draw_Player_Position(playerPosition);
			break;

		case matrix2:
			Scan_Matrix(7, walls[2]);
			Scan_Matrix(5, walls[1]);
			Scan_Matrix(3, walls[0]);
			Set_Dangerous(0x00);

			Draw_Player_Position(playerPosition);

			break;

                case matrix3:
			Scan_Matrix(8, walls[3]);
			Scan_Matrix(6, walls[2]);
			Scan_Matrix(4, walls[1]);
                        Scan_Matrix(2, walls[0]);
			Set_Dangerous(0x00);

                        Draw_Player_Position(playerPosition);

                        break;

                case matrix4:
                        Scan_Matrix(7, walls[3]);
			Scan_Matrix(5, walls[2]);
			Scan_Matrix(3, walls[1]);
			Scan_Matrix(1, walls[0]);
			Set_Dangerous(walls[0]);

                        Draw_Player_Position(playerPosition);
                        break;

                case matrix5:
                        Scan_Matrix(8, walls[0]);
			Scan_Matrix(6, walls[3]);
			Scan_Matrix(4, walls[2]);
			Scan_Matrix(2, walls[1]);
			Set_Dangerous(0x00);

                        Draw_Player_Position(playerPosition);
                        break;

		case matrix6:
                        Scan_Matrix(7, walls[0]);
			Scan_Matrix(5, walls[3]);
			Scan_Matrix(3, walls[2]);
			Scan_Matrix(1, walls[1]);
			Set_Dangerous(walls[1]);

                        Draw_Player_Position(playerPosition);
                        break;

		case matrix7:
                        Scan_Matrix(8, walls[1]);
			Scan_Matrix(6, walls[0]);
			Scan_Matrix(4, walls[3]);
			Scan_Matrix(2, walls[2]);
			Set_Dangerous(0x00);

                        Draw_Player_Position(playerPosition);
                        break;

		case matrix8:
                        Scan_Matrix(7, walls[1]);
			Scan_Matrix(5, walls[0]);
			Scan_Matrix(3, walls[3]);
			Scan_Matrix(1, walls[2]);
			Set_Dangerous(walls[2]);

                        Draw_Player_Position(playerPosition);
                        break;
	
		case matrix9:
                        Scan_Matrix(8, walls[2]);
			Scan_Matrix(6, walls[1]);
			Scan_Matrix(4, walls[0]);
			Scan_Matrix(2, walls[3]);
			Set_Dangerous(0x00);

                        Draw_Player_Position(playerPosition);
                        break;
		
		case matrix10:
                        Scan_Matrix(8, walls[2]);
			Scan_Matrix(6, walls[1]);
			Scan_Matrix(4, walls[0]);
			Scan_Matrix(1, walls[3]);
			Set_Dangerous(walls[3]);

                        Draw_Player_Position(playerPosition);
                        break;

		case matrix_wait:
			Scan_Matrix(8, walls[2]);
			Scan_Matrix(6, walls[1]);
			Scan_Matrix(4, walls[0]);
			Set_Dangerous(0x00);

                        Draw_Player_Position(playerPosition);
			break;

		case matrix_stop:
			Scan_Matrix(1, playerPosition); // freeze player in place.
			break;
	}
}

// Controls the 'joystick' variable, which tells the game logic if the player wants to move and which way.
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
			Read_ADC('x');

			// Check for neutral state (no movement on joystick).
			if ( (x > 464 && x < 624) && (y > 464 && y < 560)) { // Neutral. Center point is 512, with a deadzone of +/-48 for both axes
				joystick = 0x00;
			}


			else { // Some movement detected.
				// Check for x-axis movement.
				if (x <= 464) { // Left should be x = [0, 464]. We have already checked for the neutral position in the first if statement, so no need to check again.
					joystick = joystick | (0x01); // Set B0 = 1, which should be left.
				}

				else if (x >= 624) { // Right should be x = [624, 1024]. Extra deadzone on right is to account for my particular joystick being very sensitive to right side movement.
					joystick = joystick | (0x02); // B1 = 1, which is right.
				} 

				/*
				// Check for y-axis movement.
				if (y <= 464) {
					joystick = joystick | (0x04); // B0 = 1, up 
				}

				else if (y >= 560) {
					joystick = joystick | (0x08); // B1 = 1, down
				}
				*/
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
			PORTB = score;
			break;
	}
}

enum player_states {player_start, player_wait, player_move, player_dead} player_state;
void Tick_Player() {
// period = 350 ms (tentative)
	switch(player_state) {
		case player_start:
			player_state = player_wait;
			break;

		case player_wait:
			if (joystick && !playerDead) { 
				player_state = player_move;
		       	}

			else { 
				player_state = player_wait;
		       	}

			break;

		case player_move:
			if (joystick && !playerDead) {
				player_state = player_move;
			}

			else {
				player_state = player_wait;
			}
			
			break;

		case player_dead:
				player_state = player_dead; 
	}

	switch(player_state) { // State actions
		case player_start:
			playerPosition = 0x08; // Start in the fourth column from the right.
			break;

		case player_wait:
			Check_Collision(dangerous, playerPosition);

			if (playerCollision) {
				player_state = player_dead;
				break;
			}

			break;

		case player_move:
			Check_Collision(dangerous, playerPosition);
			
			if (playerCollision) {
				player_state = player_dead;
				break;
			}
			

			// B0 is high and player is not in leftmost spot, move left.
			if ( (joystick & 0x01) && (playerPosition != 0x80) ) { 
				playerPosition = playerPosition << 1; // shift left 1 space.
			}

			// B1 is high and player is not in rightmmost spot, move right.
			else if ((joystick & 0x02) && (playerPosition != 0x01) ) { // B1 is high, move right.
				playerPosition = playerPosition >> 1; // shift right one space.
			}
			break;

		case player_dead:
			playerDead = 1;
			break;
	}		
}

// enum matrix_states { matrix_start, matrix_draw }

int main(void) {
    	DDRA = 0x00; PORTA = 0xFF; // PINA is input.
    	DDRB = 0xFF; PORTB = 0x00; // PORTB is output.
	DDRC = 0xFF; PORTC = 0x00;

    	ADC_init(); // Initialize ADC.

	// Timer stuff
	unsigned long JS_period = 100, JS_elapsedTime = JS_period;
   	unsigned long PL_period = 250, PL_elapsedTime = PL_period;
	unsigned long OUT_period = JS_period, OUT_elapsedTime = OUT_period;
	unsigned long DM_period = refreshRate, DM_elapsedTime = DM_period;
 	const unsigned long timerPeriod = 10; 

	TimerSet(timerPeriod);
	TimerOn();
	
	joy_state = joy_start;
	player_state = player_start;
	output_state = output_start;
	matrix_state = matrix_start;

	/* Insert your solution below */
    	while (1) {
		if (JS_elapsedTime >= JS_period) { // 100 ms period
			Tick_Joystick();
			JS_elapsedTime = 0;
		}

		if (PL_elapsedTime >= PL_period) { // 500 ms period
			Tick_Player();
			PL_elapsedTime = 0;
		}

		if (DM_elapsedTime >= DM_period) { // 10 ms period.
			Tick_DrawMatrix();
			DM_elapsedTime = 0;
		}

		if (OUT_elapsedTime >= OUT_period) { // 500 ms period (equal to the player's period).	
			Tick_Output();
			OUT_elapsedTime = 0;
		}

		while(!TimerFlag) {}
		TimerFlag = 0;

		JS_elapsedTime += timerPeriod;
		PL_elapsedTime += timerPeriod;
		OUT_elapsedTime += timerPeriod;
		DM_elapsedTime += timerPeriod;
	/*	
	// MATRIX TESTING
	// An example 'frame'...
        Scan_Matrix(8, 0xE7);
        Scan_Matrix(7, 0x00);
        Scan_Matrix(6, 0xFE);
        Scan_Matrix(5, 0x00);
        Scan_Matrix(4, 0xDB);
        Scan_Matrix(3, 0x00);
        Scan_Matrix(2, 0x00);

        // Special case for bottom row, we need to also set the dangerous tiles as well
        // as player position.
        Scan_Matrix(1, 0x00);
        Set_Dangerous(0x00);
        Draw_Player_Position(playerPosition);
	*/

        // Testing collision
	/*
        if (playerCollision) {
                PORTB = 0x01;
        }

        else { PORTB = 0x00; }
	*/

	// END MATRIX TEST
    	}
	
    	
	return 1;
}
