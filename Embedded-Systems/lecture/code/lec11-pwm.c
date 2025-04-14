//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Sept 2017
// Lecture: 11
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

    uint16_t period = 16000;
    uint16_t duty = 8000;
    
    // ---------------Configure Oscillator------------------
	OSCCONbits.IRCF2 = 1;           // Internal RC Oscillator Frequency Select bits
	OSCCONbits.IRCF1 = 1;           // Set to 16Mhz
	OSCCONbits.IRCF0 = 1;           //
	OSCTUNEbits.PLLEN = 1;          // enable the 4xPLL, wicked fast 64Mhz
    
    // Configure CCP1 and the associated timer
    CCP1CONbits.CCP1M = 0b1000;     // Set on match    
    CCPTMRS0bits.C1TSEL = 0b00;     // Associate TMR1 with CCP1      
    T1CONbits.TMR1ON = 1;           // Turn on timer 1
    
    // Configure all the I/O pins
    TRISCbits.TRISC2 = 0;           // MAke RC2 (associated with CCP1) an output    
    TRISAbits.TRISA2 = 1;           // Make the upper button an input
    TRISAbits.TRISA3 = 1;           // Make the lower button an input
    ANSELAbits.ANSA2 = 0;           // Upper button is digital input
    ANSELAbits.ANSA3 = 0;           // Lower button is digital input
    
    for(;;) {
        
        // If CCP1 was configured to set RC2 then lets change it to clear
        if (CCP1CONbits.CCP1M == 0b1000) {
            CCP1CONbits.CCP1M = 0b1001;         // Clear when TMR1 == CCPR1
            CCPR1 = TMR1 + duty;       // Set CCPR1 ahead of TMR1
            PIR1bits.CCP1IF = 0;                // Clear the flag the indicates a match
            while (PIR1bits.CCP1IF == 0);       // Then wait for TMR1 to catch-up with CCPR1
            
        // If CCP1 was configured to clear RC2 then lets change it to set
        } else if (CCP1CONbits.CCP1M == 0b1001) {
            CCP1CONbits.CCP1M = 0b1000;         // Set when TMR1 == CCPR1
            CCPR1 = TMR1 + (period-duty);                // Set CCPR1 ahead of TMR1
            PIR1bits.CCP1IF = 0;                // Clear the flag the indicates a match
            while (PIR1bits.CCP1IF == 0);       // Wait 
        }
        
        if (PORTAbits.RA2 == 0) {
            while (PORTAbits.RA2 == 0);
            duty += 1600;
        } // end if upper button is pressed        
        
        if (PORTAbits.RA3 == 0) {
            while (PORTAbits.RA3 == 0);
            duty -= 1600;
        } // end if upper button is pressed        
        
        
    } // end infinite loop    
} // end main

