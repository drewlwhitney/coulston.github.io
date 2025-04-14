//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Sept 2017
// Lecture: 11
// Purp:    generate a waveform on RC2
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
#include <xc.h>

// Configuration bits
#pragma config FOSC = INTIO67    // Oscillator Selection bits->Internal oscillator block
#pragma config PLLCFG = OFF    // 4X PLL Enable->Oscillator used directly
#pragma config PRICLKEN = ON    // Primary clock enable bit->Primary clock enabled

//*****************************************************************
//*****************************************************************
void main(void) {

    // ---------------Configure Oscillator------------------
	OSCCONbits.IRCF2 = 1;		// Internal RC Oscillator Frequency Select bits
	OSCCONbits.IRCF1 = 1;		// Set to 16Mhz
	OSCCONbits.IRCF0 = 1;		//
	OSCTUNEbits.PLLEN = 1;		// enable the 4xPLL, wicked fast 64Mhz
    
    CCP1CONbits.CCP1M = 0b0010;       // Toggle on match    
    CCPTMRS0bits.C1TSEL = 0b00;     // Associate TMR1 with CCP1
    T1CONbits.TMR1ON = 1;           // Turn on timer 1
    TRISCbits.TRISC2 = 0;
    
    for(;;) {
        CCPR1 = TMR1 + 8112;
        PIR1bits.CCP1IF = 0;
        while (PIR1bits.CCP1IF == 0);
        // Do stuff
    }
    
} // end main

