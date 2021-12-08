#ifndef __MATRIX_H__
#define __MATRIX_H__


/* This code controls the LED Matrix. 
 *      We scan row by row, lighting each individual LED needed per row by activating its relevant column.
 *      FOr example, to light R1C1 (row 1, column 1, aka the LED in the bottom-right corner, we would use the following code.
 *              
 *              Set_Rows(0xFE); // Enable the bottom row.
 *              Set_Columns(0x01); // Enable the first LED (from right to left).
 *
 *      If we wanted to set several rows, we would then do (from top down)...
 *              Set_Rows(...);
 *              Set_Columns(...);
 *		_delay_ms(20) ms (using some external timer function, not handled here).
 *
 *              Set_Rows(0xFE);
 *              Set_Columns(0x01);
 *
 *      When we set the bottom row, we should also make sure to set which columns are dangerous. To do this, you would
 *      set unsigned char 'dangerous' = (the same argument you gave to Set_Columns).
 *
 *      REFERENCE: Each row is as follows...
 *              Row 1: 0xFE (bottom)
 *              Row 2: 0xFD
 *              Row 3: 0xFB
 *              Row 4: 0xF7
 *              Row 5: 0xEF
 *              Row 6: 0xDF
 *              Row 7: 0xBF
 *              Row 8: 0x7F
*/

// Global variables related to controlling the matrix.
unsigned char playerPosition = 0x08;
unsigned char playerCollision; // 1 if the player collides with a wall, 0 otherwise.
unsigned char dangerous; // 8-bit variable listing currently dangerous spots in row 1 (bottom). if a bit is 1, that column is dangerous to be in.
const unsigned char refreshRate = 10; // value in ms for how fast the LED matrix scans.


void Set_Rows(unsigned char data) {
        /* Controls which rows are lit, bits that are low/0 are lit. 
                b0 of data controls the bottom row, going up from b0...b7
                rows are controlled by PORTC bits C0...C3
        */
        int i;
        for (i = 0; i < 8 ; ++i) {
                // Sets SRCLR on the shift register to 1 (high)  allowing data to be sent. 
                // This also clears SRCLK to prepare for data to be sent.
                PORTC = 0x08;
                // set SER = next bit of data to be sent.
                PORTC |= ((data >> i) & 0x01);
                // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
                PORTC |= 0x02;
        }
        // RCLK = 1, because rising edge copies data from “Shift” register to “Storage” register
        PORTC |= 0x04;
        // clears all lines in preparation of a new transmission
        PORTC = 0x00;
        return;
}

void Set_Columns(unsigned char data) {
        /* Controls which columns are lit, bits that are high/1 are lit.
                b0 of data controls rightmost column, going from right to left from b0...b7
                columns controlled by PORTC bits C4...C7
        */
        int i;
        for (i = 0; i < 8 ; ++i) {
                // Sets SRCLR on the shift register to 1 (high)  allowing data to be sent.
                // This also clears SRCLK to prepare for data to be sent.
                PORTC = 0x80;
                // set SER = next bit of data to be sent.
                PORTC |= ((data >> i) & 0x01) << 4;
                // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
                PORTC |= 0x20;
        }
        // RCLK = 1, because rising edge copies data from “Shift” register to “Storage” register
        PORTC |= 0x40;
        // clears all lines in preparation of a new transmission
        PORTC = 0x00;
        return;
}

// row should be a number from 1-8.
// col should be an 8-bit binary number listing which rows should be on.
// col_b0 = row 1, col_b1 = row 2, and so on from right to left.
void Scan_Matrix(unsigned char row, unsigned char col) {
        // Array to hold the hex values representing each row for scanning purposes.
        unsigned char rows[8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};

        // row should only ever be 1 to 8.
        Set_Rows(rows[row-1]);
        Set_Columns(col);
        _delay_ms(refreshRate);
}

// Used when rendering the bottom row where the player is. 
// Takes in the columns to be lit, and checks if the player is currently in any of them.
void Check_Collision(unsigned char danger_col, unsigned char player_pos) {

        // If the player is in a spot a wall is also in, there will be two 1's being AND'ed, and the result is zero.
        // Otherwise, no 1's will match up, meaning the result is 0 if there is no collision.
        if (player_pos & danger_col) {
                playerCollision = 1;
        }
        else { playerCollision = 0; }

        return;
}

// Sets which columns in the bottom row the player will die in.
void Set_Dangerous(unsigned char new_dangerous) {
        dangerous = new_dangerous;
        return;
}

// Empty_Evens and Empty_Odds are helper functions to make the alternating patterns easier to draw.
void Matrix_Empty_Evens() {
	Scan_Matrix(2, 0x00);
	Scan_Matrix(4, 0x00);
	Scan_Matrix(6, 0x00);
	Scan_Matrix(8, 0x00);
	return;
}

void Matrix_Empty_Odds() {
	Scan_Matrix(1, 0x00);
	Scan_Matrix(3, 0x00);
	Scan_Matrix(5, 0x00);
	Scan_Matrix(7, 0x00);
	return;
}	

// Lights the LED on the matrix that the player occupies, and sends that to be checked for a collision.
// Only one bit of player_pos should be 1.
void Draw_Player_Position(unsigned char player_pos) {
	Scan_Matrix(1, player_pos);
        return;
}


#endif
