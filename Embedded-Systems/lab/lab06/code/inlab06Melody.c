//--------------------------------------------------------------------
// Name:            Chris Coulston
// Date:            Fall 2020
// Purp:            inLab 6
//
// Assisted:        The entire class of EENG 383
// Assisted by:     Microchips 18F26K22 Tech Docs 
// Conversion to MCC Melody: Nolan Pratt (nolanpratt@mines.edu), Fall 2025
//-
//- Academic Integrity Statement: I certify that, while others may have
//- assisted me in brain storming, debugging and validating this program,
//- the program itself is my own work. I understand that submitting code
//- which is the work of other individuals is a violation of the course
//- Academic Integrity Policy and may result in a zero credit for the
//- assignment, or course failure and a report to the Academic Dishonesty
//- Board. I also understand that if I knowingly give my original work to
//- another individual that it could also result in a zero credit for the
//- assignment, or course failure and a report to the Academic Dishonesty
//- Board.
//------------------------------------------------------------------------
#include "mcc_generated_files/system/system.h"
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

#define LED_ON      25 
#define LED_OFF     0

//----------------------------------------------
// Main "function"
//----------------------------------------------

void main(void) {

    uint8_t mask;
    uint8_t i;
    uint8_t baudRateSelected = 1;
    char cmd;
    char letter = '0';
    uint16_t bitPeriod[6] = {53333, 13333, 6666, 3333, 1666, 833};

    SYSTEM_Initialize();
    EPWM2_LoadDutyValue(LED_OFF);

    // Delay so the Baud rate generator is stable and prints the splash screen correctly
    TMR1_CounterSet(0);
    PIR1bits.TMR1IF = 0;
    while (TMR1_HasOverflowOccured() == false);

    // Since EUSART2 is double buffered, clear out any garbage with two reads from those buffers
    if (EUSART2_IsRxReady()) (void) EUSART2_Read();
    if (EUSART2_IsRxReady()) (void) EUSART2_Read();

    printf("inLab 6\r\n");
    printf("Receive and decode an IR packet\r\n");
    printf("Dev'21 Board wiring\r\n");
    printf("Install a jumper wire from RC0 to RB7 ONLY AFTER unplugging PICKit3\r\n");
    printf("Install a jumper over IR_TX header pins\r\n");
    printf("\r\n> "); // print a nice command prompt


    for (;;) {

        if (EUSART1_IsRxReady()) { // wait for incoming data on USART
            cmd = EUSART1_Read();
            switch (cmd) { // and do what it tells you to do

                    //--------------------------------------------
                    // Reply with help menu
                    //--------------------------------------------
                case '?':
                    printf("-------------------------------------------------\r\n");
                    switch (baudRateSelected) {
                        case 0: printf("300 Baud\r\n");
                            break;
                        case 1: printf("1200 Baud\r\n");
                            break;
                        case 2: printf("2400 Baud\r\n");
                            break;
                        case 3: printf("4800 Baud\r\n");
                            break;                                           
                        case 4: printf("9600 Baud\r\n");
                            break;
                        case 5: printf("19200 Baud\r\n");
                            break;
                        default: printf("1200 Baud\r\n");
                            break;
                    }
                    printf("-------------------------------------------------\r\n");
                    printf("?: help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("b: set the Baud rate of the sent characters\r\n");
                    printf("p: send 1 pulse of 38kHz IR illumination with a duration of %d 1:1 prescaled TMR1 counts\r\n", bitPeriod[baudRateSelected]);
                    printf("S: Send ""%c"" using IR transmitter\r\n", letter);
                    printf("R: use EUSART2 to decode character\r\n");
                    printf("r: reset EUSART2\r\n");
                    printf("-------------------------------------------------\r\n");
                    break;

                    //--------------------------------------------
                    // Reply with "ok", used for PC to PIC test
                    //--------------------------------------------
                case 'o':
                    printf("o:	ok\r\n");
                    break;

                    //--------------------------------------------
                    // Reset the processor after clearing the terminal
                    //--------------------------------------------                      
                case 'Z':
                    for (i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;

                    //--------------------------------------------
                    // Clear the terminal
                    //--------------------------------------------                      
                case 'z':
                    for (i = 0; i < 40; i++) printf("\n");
                    break;

                    //--------------------------------------------
                    // Set the Baud rate - use MCC EUSART2 configuration register tab            
                    //--------------------------------------------                    
                case 'b':
                    printf("Choose the index of the target baud rate\r\n");
                    printf("0: 300 baud\r\n");
                    printf("1: 1200 baud\r\n");
                    printf("2: 2400 baud\r\n");
                    printf("3: 4800 baud\r\n");                    
                    printf("4: 9600 baud\r\n");
                    printf("5: 19200 baud\r\n");
                    while (EUSART1_IsRxReady());
                    while (!EUSART1_IsRxReady());
                    baudRateSelected = EUSART1_Read() - '0';
                    switch (baudRateSelected) {
                        case 0: SPBRGH2 = 0xD0;
                            SPBRG2 = 0x54;
                            break;
                        case 1: SPBRGH2 = 0x34;
                            SPBRG2 = 0x14;
                            break;
                        case 2: SPBRGH2 = 0x1A;
                            SPBRG2 = 0x0A;
                            break;
                        case 3: SPBRGH2 = 0x0D;
                            SPBRG2 = 0x05;
                            break;                            
                        case 4: SPBRGH2 = 0x06;
                            SPBRG2 = 0x82;
                            break;
                        case 5: SPBRGH2 = 0x03;
                            SPBRG2 = 0x40;
                            break;
                        default: SPBRGH2 = 0x34;
                            SPBRG2 = 0x14;
                            break;
                    }
                    printf("Baud rate assigned %02x:%02x\r\n", SPBRGH2, SPBRG2);
                    break;

                    //--------------------------------------------
                    // Turn on the IR LED for bitPeriod[baudRateSelected] TMR1 cnts
                    // This function is inside the for-loop of the 'S' function
                    //--------------------------------------------  
                case 'p':
                    EPWM2_LoadDutyValue(LED_ON);
                    TMR1_CounterSet(0x10000 - bitPeriod[baudRateSelected]);
                    PIR1bits.TMR1IF = 0;
                    while (TMR1_HasOverflowOccured() == false);
                    EPWM2_LoadDutyValue(LED_OFF);
                    printf("Pulse sent\r\n");
                    break;

                    //--------------------------------------------
                    // Transmit the bits of a ASCII character, LSB first including
                    // a start and stop bit.
                    //--------------------------------------------  
                case 'S':
                    // Preface character with a '0' bit
                    EPWM2_LoadDutyValue(LED_ON);
                    TMR1_CounterSet(0x10000 - bitPeriod[baudRateSelected]);
                    PIR1bits.TMR1IF = 0;
                    while (TMR1_HasOverflowOccured() == false);

                    // LSB first
                    mask = 0b00000001;
                    while (mask != 0) {
                        if ((letter & mask) != 0) EPWM2_LoadDutyValue(LED_OFF);
                        else EPWM2_LoadDutyValue(LED_ON);
                        mask = mask << 1;
                        TMR1_CounterSet(0x10000 - bitPeriod[baudRateSelected]);
                        PIR1bits.TMR1IF = 0;
                        while (TMR1_HasOverflowOccured() == false);
                    }

                    // Need a stop bit to break up successive bytes
                    EPWM2_LoadDutyValue(LED_OFF);
                    TMR1_CounterSet(0x10000 - bitPeriod[baudRateSelected]);
                    PIR1bits.TMR1IF = 0;
                    while (TMR1_HasOverflowOccured() == false);

                    printf("just sent %c    %x\r\n", letter, letter);
                    letter += 1;
                    break;

                    //--------------------------------------------
                    // Read the a character from EUSART2 FIFO
                    // It's safe to use EUSART2_Read because RC2IF = 1
                    //--------------------------------------------
                case 'R':
                    if (PIR3bits.RC2IF == 1)
                        printf("Just read in %c from EUSART2\r\n", RCREG2);
                    else
                        printf("Nothing received from EUSART2\r\n");
                    break;

                    //--------------------------------------------
                    // reset EUSART2 in case it needs doing
                    //--------------------------------------------                
                case 'r':
                    RCSTA2bits.CREN = 0; // Try restarting EUSART2
                    RCSTA2bits.CREN = 1;
                    printf("Just reset EUSART2\r\n");
                    break;

                    //--------------------------------------------
                    // If something unknown is hit, tell user
                    //--------------------------------------------
                default:
                    printf("Unknown key %c\r\n", cmd);
                    break;

            } // end switch            
        } // end if
    } // end while 
} // end main