//--------------------------------------------------------------------
// Name:            Chris Coulston
// Date:            Fall 2018
// Purp:            inLab09
//
// Assisted:        The entire class of EENG 383
// Assisted by:     Microchips 18F26K22 Tech Docs 
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
#include "mcc_generated_files/mcc.h"
#pragma warning disable 520     
#pragma warning disable 1498

#define EEPROM_I2C_ADDR     0b1010000       // 7-bit address for 24LC256
#define BLOCK_SIZE          64              // 0x40
#define NUM_BLOCKS          512

    uint8_t                 dummy[1000];
    
//----------------------------------------------
// Main "function"
//----------------------------------------------
void main (void) {


    uint16_t                pageAddress = 0x0000, byteAddress=0;    
    uint8_t                 pData[BLOCK_SIZE+2];    // block + address bytes
    uint8_t                 byteData=0;
    uint8_t                 i;
    I2C2_MESSAGE_STATUS     I2C_Wflag;
    char                    cmd;
    uint16_t                block;                  // There are more than uint8_t blocks!
    
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();    
    INTERRUPT_PeripheralInterruptEnable();
    
	printf("Development Board\r\n");
	printf("inLab 08 terminal\r\n");
    printf("EEPROM testing\r\n");
	printf("\r\n> ");                       // print a nice command prompt


	for(;;) {

		if (EUSART1_DataReady) {			// wait for incoming data on USART
            cmd = EUSART1_Read();
			switch (cmd) {		// and do what it tells you to do

			//--------------------------------------------
			// Reply with help menu
			//--------------------------------------------
			case '?':
				printf("\r\n-------------------------------------------------\r\n");
				printf("-------------------------------------------------\r\n");
                printf("?: help menu\r\n");
				printf("o: k\r\n");
                printf("Z: Reset processor\r\n");                     
                printf("z: Clear the terminal\r\n");    
                printf("p/P decrease/increase page address\r\n");
                printf("w: Write a byte to the EEPROM\r\n");
                printf("W: Write entire EEPROM\r\n");
                printf("r: Read a byte from EEPROM\r\n");
                printf("R: Read a page from EEPROM\r\n");
                printf("-------------------------------------------------\r\n");
				break;

			//--------------------------------------------
			// Reply with "k", used for PC to PIC test
			//--------------------------------------------
			case 'o':
				printf("o:	ok\r\n");
				break;

            //--------------------------------------------
            // Reset the processor after clearing the terminal
            //--------------------------------------------                      
            case 'Z':
                for (i=0; i<40; i++) printf("\n");
                RESET();                    
                break;

            //--------------------------------------------
            // Clear the terminal
            //--------------------------------------------                      
            case 'z':
                for (i=0; i<40; i++) printf("\n");                            
                break;  
                
            //--------------------------------------------
            // Decrease 
            //--------------------------------------------                 
            case 'p':
                pageAddress -= BLOCK_SIZE;
                printf("address = %4x\r\n",pageAddress);
                break;

            //--------------------------------------------
            // Increase 
            //--------------------------------------------                
            case 'P':
                pageAddress += BLOCK_SIZE;
                printf("address = %4x\r\n",pageAddress);
                break;


            //--------------------------------------------
			// Write a byte to the 24LC256 EEPROM
			//--------------------------------------------
            case 'w':
                
                // Separate the 16-bit address into a low/high byte
                pData[0] = byteAddress>>8;              // byte 0 = Most Significant Byte address
                pData[1] = (uint8_t) byteAddress;       // byte 1 = Least Significant Byte address   
                    
                pData[2] = byteData;                    // Store some random byte value

                I2C_Wflag = I2C2_MESSAGE_PENDING;                    
                I2C2_MasterWrite(pData, 3, EEPROM_I2C_ADDR, &I2C_Wflag); // write address + data = BLOCK_SIZE + 2                    
                while(I2C_Wflag == I2C2_MESSAGE_PENDING);                // Wait for data to be transfered to EEPROM

                // Note that we are writing a single byte, and then immediately 
                // jumping back to the menu and waiting for user input.  In
                // this specific case, the write to the EEPROM will have
                // plenty of time to complete.  Therefore we are not going
                // to poll the I2C_Wflag as we do in the 'W' case.
                
                printf("Just wrote %x to address %x.\r\n",byteData, byteAddress);  
                byteAddress += 1;
                byteData += 1;
                
                break;

            //--------------------------------------------
			// Read a byte from the 24LC256 EEPROM
			//--------------------------------------------
            case 'r':  
                // Separate the 16-bit address into a low/high byte
                pData[0] = byteAddress>>8;        // byte 0 = MSbyte address
                pData[1] =  (uint8_t) byteAddress;        // byte 1 = LSbyte address
                
                // Writing 0 bytes of data is used to set starting address for read
                I2C_Wflag = I2C2_MESSAGE_PENDING;
                I2C2_MasterWrite(pData, 2, EEPROM_I2C_ADDR, &I2C_Wflag);
                while(I2C_Wflag == I2C2_MESSAGE_PENDING);                       
                
                // Now you can perform the read at the address you just wrote
                I2C_Wflag = I2C2_MESSAGE_PENDING;
                I2C2_MasterRead(pData, 1, EEPROM_I2C_ADDR, &I2C_Wflag);
                while(I2C_Wflag == I2C2_MESSAGE_PENDING);
                
                printf("Just read %x from address %x\r\n",pData[0], byteAddress);                                
                break;
                
                
            //--------------------------------------------
			// Fill the entire 24LC256 EEPROM with data
			//--------------------------------------------
            case 'W':
                pageAddress = 0;                
                for (block=0; block<NUM_BLOCKS; block++) {
                    
                    // Separate the 16-bit address into a low/high byte
                    pData[0] = pageAddress>>8;              // byte 0 = Most Significant Byte address
                    pData[1] = (uint8_t) pageAddress;       // byte 1 = Least Significant Byte address   
                    pageAddress += BLOCK_SIZE;
                    
                    for (i=0; i<BLOCK_SIZE; i++) pData[i+2] = block;  // bytes between 2 and BLOCK_SIZE are data

                    I2C_Wflag = I2C2_MESSAGE_PENDING;                    
                    I2C2_MasterWrite(pData, BLOCK_SIZE+2, EEPROM_I2C_ADDR, &I2C_Wflag); // write address + data = BLOCK_SIZE + 2                    
                    while(I2C_Wflag == I2C2_MESSAGE_PENDING);                           // Wait for data to be transfered to EEPROM
                    
                    
                    // Wait for EEPROM to commit data into its non-volatile memory
                    do {                                    
                        I2C_Wflag = I2C2_MESSAGE_PENDING;                        
                        I2C2_MasterWrite(pData, 0, EEPROM_I2C_ADDR, &I2C_Wflag);                 
                        while(I2C_Wflag == I2C2_MESSAGE_PENDING);
                    } while(I2C_Wflag == I2C2_DATA_NO_ACK);
                } // end blocks                
                pageAddress = 0;

                printf("Just wrote entire EEPROM, each page filled with same value.\r\n");  
                break;                                
             
            //--------------------------------------------
			// Read a page from the 24LC256 EEPROM
			//--------------------------------------------
            case 'R':  
                // Separate the 16-bit address into a low/high byte
                pData[0] = pageAddress>>8;        // byte 0 = MSbyte address
                pData[1] =  (uint8_t) pageAddress;        // byte 1 = LSbyte address
                
                // Writing 0 bytes of data is used to set starting address for read
                I2C_Wflag = I2C2_MESSAGE_PENDING;
                I2C2_MasterWrite(pData, 2, EEPROM_I2C_ADDR, &I2C_Wflag);
                while(I2C_Wflag == I2C2_MESSAGE_PENDING);                       
                
                // Now you can perform the read at the address you just wrote
                I2C_Wflag = I2C2_MESSAGE_PENDING;
                I2C2_MasterRead(pData, BLOCK_SIZE, EEPROM_I2C_ADDR, &I2C_Wflag);
                while(I2C_Wflag == I2C2_MESSAGE_PENDING);
                
                printf("Just read:");                
                for (i=0; i<BLOCK_SIZE; i++) {
                    if ( (i & 0x0F) == 0)
                        printf("\r\nM[%x] = ",pageAddress+i);
                    printf("%3d ",pData[i]);
                }   
                printf("\r\n");
                break;

			//--------------------------------------------
			// If something unknown is hit, tell user
			//--------------------------------------------
			default:
				printf("Unknown key %c\r\n",cmd);
				break;
			} // end switch
            
		}	// end if
	} // end while 
} // end main


//----------------------------------------------
//
// As configured, we are hoping to get a toggle
// every 100us - this will require some work.
//
// You will be starting an ADC conversion here and
// storing the results (when you reenter) into a global
// variable and setting a flag, alerting main that 
// it can read a new value.
//----------------------------------------------
void TMR0_DefaultInterruptHandler(void){

    uint16_t bigOleWasteOfTime;

    TEST_C0_PIN_SetHigh();
    
    for (bigOleWasteOfTime=0; bigOleWasteOfTime<40; bigOleWasteOfTime++);    
    //TMR0_WriteTimer(0xFFFF - 1600);                         // Less accurate
    TMR0_WriteTimer(TMR0_ReadTimer() + (0xFFFF - 1600));  // More accurate
    
    TEST_C0_PIN_SetLow() ;

}


