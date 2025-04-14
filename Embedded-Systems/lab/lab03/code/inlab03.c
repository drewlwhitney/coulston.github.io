//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Fall 2020
// Lab:     03
// Purp: Play Hot Cross Buns (B5, A5, G4)
//
// Assisted: The entire EENG 383 class
// Assisted by: Technical documents
//
// Academic Integrity Statement: I certify that, while others may have
// assisted me in brain storming, debugging and validating this program,
// the program itself is my own work. I understand that submitting code
// which is the work of other individuals is a violation of the course
// Academic Integrity Policy and may result in a zero credit for the
// assignment, course failure and a report to the Academic Dishonesty
// Board. I also understand that if I knowingly give my original work to
// another individual that it could also result in a zero credit for the
// assignment, course failure and a report to the Academic Dishonesty
// Board.
//*****************************************************************
#include "mcc_generated_files/mcc.h"
#pragma warning disable 520     
#pragma warning disable 1498

#define N       4       // Number of notes
void microSecondDelay(uint16_t us);
void milliSecondDelay(uint16_t ms);

//*****************************************************************

//*****************************************************************

void main(void) {

    uint16_t halfPeriodDelay[N] = {507, 568, 638, 16}; // microseconds
    uint16_t halfPeriodTimer[N] = {8112, 9088, 10208, 256}; // 1:1 timer counts
    uint8_t i = 0;

    SYSTEM_Initialize();

    for (;;) {

        if (TOP_BUTTON_GetValue() == 0) {
            while (TOP_BUTTON_GetValue() == 0) {
                microSecondDelay(halfPeriodDelay[i]);
                SPEAKER_PIN_Toggle();
            } // end pressed button            
            i = i + 1;
            if (i == N) i = 0;
        } // end pressed button   

        if (BOTTOM_BUTTON_GetValue() == 0) {
            while (BOTTOM_BUTTON_GetValue() == 0) {
                TMR1_WriteTimer(0x10000 - halfPeriodTimer[i]);
                PIR1bits.TMR1IF = 0;
                while (TMR1_HasOverflowOccured() == false);
                SPEAKER_PIN_Toggle();
            } // end pressed button            
            i = i + 1;
            if (i == N) i = 0;
        } // end pressed button   


    } // end infinite loop    
} // end main



//*****************************************************************
// Call microseconds(1000) a number of times
//*****************************************************************

void milliSecondDelay(uint16_t ms) {

    uint16_t i;

    for (i = 0; i < ms; i++) microSecondDelay(1000);

} // end millSecond


//*****************************************************************
// Create a delay of 1uS and loop a number of times
//*****************************************************************

void microSecondDelay(uint16_t us) {

    uint16_t i;

    for (i = 0; i < us; i++) {
        asm("NOP"); // 1
        asm("NOP"); // 2
        asm("NOP"); // 3
        asm("NOP"); // 4
        asm("NOP"); // 5
        asm("NOP"); // 6

        i = i;
    } // end for     
} // end microSecond()