//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Sept 2017
// Lecture: 12
// Purp:    duplicate RC2 input on RC1
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

    uint16_t start, delta;    
    
    // ---------------Configure Oscillator------------------
	OSCCONbits.IRCF2 = 1;           // Internal RC Oscillator Frequency Select bits
	OSCCONbits.IRCF1 = 1;           // Set to 16Mhz
	OSCCONbits.IRCF0 = 1;           //
	OSCTUNEbits.PLLEN = 1;          // enable the 4xPLL, wicked fast 64Mhz
    
    // Configure CCP1 and the associated timer
    CCP1CONbits.CCP1M =  0b0100;
    CCPTMRS0bits.C1TSEL = 0b00;     // Associate TMR1 with CCP1
    T1CONbits.T1CKPS = 0b11;        // 1:8 prescale
    T1CONbits.TMR1ON = 1;           // Turn on timer 1
    
    // Configure all the I/O pins
    TRISCbits.TRISC1 = 0;           // MAke RC2 (associated with CCP1) an output    
    TRISCbits.TRISC2 = 1;           // MAke RC2 (associated with CCP1) an output    
    ANSELCbits.ANSC2 = 0;
    
    TRISAbits.TRISA2 = 1;           // Make the upper button an input
    TRISAbits.TRISA3 = 1;           // Make the lower button an input
    ANSELAbits.ANSA2 = 0;           // Upper button is digital input
    ANSELAbits.ANSA3 = 0;           // Lower button is digital input
    
    for(;;) {
      
        CCP1CONbits.CCP1M =  0b0100;        // Capture timer 1 value into         
        PIR1bits.CCP1IF = 0;                // CCPR1 register when a falling 
        while (PIR1bits.CCP1IF == 0);       // edge of RC2 is detected
        start = CCPR1;
        
        CCP1CONbits.CCP1M = 0b0101;         // Capture timer 1 value into  
        PIR1bits.CCP1IF = 0;                // CCPR2 register when a rising
        while (PIR1bits.CCP1IF == 0);       // edge of RC2 is detected
        delta = CCPR1 - start;
        
        // Copy the pulse on RC1
        LATCbits.LATC1 = 1;
        TMR1 = 0xFFFF - delta;
        PIR1bits.TMR1IF = 0;
        while (PIR1bits.TMR1IF == 0);        
        LATCbits.LATC1 = 0;        
    } // end infinite loop    
} // end main

