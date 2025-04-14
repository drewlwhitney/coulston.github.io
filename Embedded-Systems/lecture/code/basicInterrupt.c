//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Sept 2017
// Lecture: 13
// Purp:    generate a 50% duty cycle square wave on RC0 using interrupts
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

void INIT_PIC (void);
void __interrupt(high_priority) tmr0_isr(void);

//----------------------------------------------
// Main "function"
//----------------------------------------------
void main (void) {

    INIT_PIC();
    
    for (;;) {
        
	} // end while 
} // end main


//----------------------------------------------
// INIT_PIC
// Initializes the PIC to run at 64MHz
// Setup TMR0 to run 16-bit with no prescalar
//----------------------------------------------
void INIT_PIC (void) {

	OSCCONbits.IRCF2 = 1;           // Internal RC Oscillator
	OSCCONbits.IRCF1 = 1;           //      set to 16Mhz
	OSCCONbits.IRCF0 = 1;           // and enable the 4xPLL
	OSCTUNEbits.PLLEN = 1;          // making a wicked fast 64Mhz Fosc

	TRISCbits.TRISC0 = 0;           // Make RC0 an output
            
	TMR0 = 0xA739;
	INTCONbits.TMR0IF = 0;		// Clear interrupt flag
	T0CON = 0x93;			// T0PS 1:16; T08BIT 16-bit; T0SE Increment_hi_lo; T0CS FOSC/4; TMR0ON enabled; PSA assigned; 
	INTCONbits.TMR0IE = 1;          // Enable TMR0 interrupt
  
	RCONbits.IPEN = 0;		// Disable interrupt priority
	//INTCONbits.PEIE = 1;		// Any interrupt with a flag in PIR1, PIR2, PIR3 PIR4, PIR5 need this set
	INTCONbits.GIE = 1;             // Enable global interrupts    
}

//-----------------------------------------------------------------------------
// The more verbose function declaration for this ISR:
//       void interrupt high_priority tmr_isr(void) @0x08
// tells us that the the high priority interrupt address resides at address
// 0x08 in the PIC instruction memory. 
//
// From page 106 of the technical documents, section 9.1
// When the IPEN bit is cleared (default state), the interrupt
// priority feature is disabled... All interrupts branch to
// address 0008h in Compatibility mode.
//-----------------------------------------------------------------------------
void __interrupt(high_priority) tmr0_isr(void) {

    INTCONbits.TMR0IF=0;        // always clear the interrupt flag
    TMR0 = 0xA739;              // Set a new timer interval and
    LATCbits.LATC0 ^= 1;        // toggle a pin so someone knows we were here

} // end tmr0_isr
