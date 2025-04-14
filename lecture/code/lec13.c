//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Sept 2017
// Lecture: 13
// Purp:    generate a PWM waveform on RC2
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
#include <stdint.h>

// Configuration bits
#pragma config FOSC = INTIO67    // Oscillator Selection bits->Internal oscillator block
#pragma config PLLCFG = OFF    // 4X PLL Enable->Oscillator used directly
#pragma config PRICLKEN = ON    // Primary clock enable bit->Primary clock enabled

//*****************************************************************
//*****************************************************************
void main(void) {

    
    // ---------------Configure Oscillator------------------
	OSCCONbits.IRCF2 = 1;           // Internal RC Oscillator Frequency Select bits
	OSCCONbits.IRCF1 = 1;           // Set to 16Mhz
	OSCCONbits.IRCF0 = 1;           //
	OSCTUNEbits.PLLEN = 1;          // enable the 4xPLL, wicked fast 64Mhz
    
    // Configure CCP1 and the associated timer
    CCP1CONbits.CCP1M = 0b1100;     // PWM Mode
    CCPTMRS0bits.C1TSEL = 0b00;     // Associate TMR2 with CCP1 in PWM mode
    PR2 = 0xFF;
    CCPR1 = 0x80;
    
    T2CONbits.T2CKPS = 0b11;        // 1:16 prescaler to slow things down
    T2CONbits.TMR2ON = 1;           // Turn on timer 2
    
    // Configure all the I/O pins
    TRISCbits.TRISC2 = 0;           // MAke RC2 (associated with CCP1) an output
    
    TRISAbits.TRISA2 = 1;           // Make the upper button an input
    TRISAbits.TRISA3 = 1;           // Make the lower button an input
    ANSELAbits.ANSA2 = 0;           // Upper button is digital input
    ANSELAbits.ANSA3 = 0;           // Lower button is digital input
    
    for(;;) {

        if (PORTAbits.RA2 == 0) {
            while (PORTAbits.RA2 == 0);
            CCPR1 += 16;
        } // end if upper button is pressed        
        
        if (PORTAbits.RA3 == 0) {
            while (PORTAbits.RA3 == 0);
            CCPR1 -= 16;
        } // end if upper button is pressed        
        
        
    } // end infinite loop    
} // end main



