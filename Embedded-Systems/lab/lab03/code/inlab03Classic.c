//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Spring 2019
// Lab:     03
// Purp:    Music box
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

#define B5      8
#define A5      6 
#define G4      4 

#define WHO     62500
#define HAL     31250
#define QUA     15625
#define EIG     7813
#define REST    1953        // Thirty second note is used for intra-note rest


#define NUM_OCTAVES 2       // Number of stored notes
#define SONG_LENGTH 17
//*****************************************************************

//*****************************************************************
void main(void) {
    
    // The scales are running on a 1:1 prescaled timer 1
    uint16_t    scale[NUM_OCTAVES*12] = {12864,12144,11472,10816,10208,9632,9088,8592,8112,7648,7232,6816,6432,6080,5728,5408,5104,4816,4560,4288,4048,3824,3616,3408};      
    uint16_t    notes[SONG_LENGTH] = {B5, A5, G4, B5, A5, G4, G4, G4, G4, G4, A5, A5, A5, A5, B5, A5, G4}; 
    uint16_t    duration[SONG_LENGTH] = {QUA, QUA, HAL, QUA, QUA, HAL, EIG, EIG, EIG, EIG, EIG, EIG, EIG, EIG, QUA, QUA, HAL};
    uint8_t i;
    
    SYSTEM_Initialize();
        
    while(1) {
  
        if (TOP_BUTTON_GetValue() == 0) {        
            while (TOP_BUTTON_GetValue() == 0);   
            for (i=0; i<SONG_LENGTH; i++) {
                TMR0_WriteTimer(0xFFFF - duration[i]);
                INTCONbits.T0IF = 0;
                while (TMR0_HasOverflowOccured() == false) {                
                    // Toggle the LED at 1Hz while the button is not being pressed  
                    TMR1_WriteTimer(0xFFFF - scale[notes[i]]);
                    PIR1bits.TMR1IF = 0;
                    while(TMR1_HasOverflowOccured() == false); 
                    SPEAKER_PIN_Toggle();                
                } // end that note      
                SPEAKER_PIN_SetLow();
                TMR0_WriteTimer(0xF000);
                INTCONbits.T0IF = 0;
                while(TMR0_HasOverflowOccured() == false);                 
            } // end song            
        } // if button has been pressed
        SPEAKER_PIN_SetLow();
    } // end infinite loop    
} // end main


