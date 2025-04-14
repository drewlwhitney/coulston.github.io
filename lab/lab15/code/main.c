//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Fall 2020
// Purp:    Accelerometer data logger
// MCC:     edit static inline void I2C1_MasterWaitForEvent(void)
//          and add an escape clause
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
#include "./mcc_generated_files/i2c1_master.h"
#include "./mcc_generated_files/examples/i2c1_master_example.h"
#include <ctype.h>
#include "./mma8452.h"
#include "./mpu6050.h"
#include "./sdCard.h"

#pragma warning disable 520     
#pragma warning disable 1498
#pragma warning disable 1510
#pragma warning disable 2053

#define BLOCK_SIZE              512
#define RED                     0
#define BLUE                    1

// Assumes 1:16 prescalar on TMR0
#define TMR0_FIFTY_MILLISECONDS     50000
#define TMR0_FIVE_MILLISECONDS      5000
#define TMR0_FIFTH_MILLISECONDS     500

#define LED_OFF              1024
#define LED_ON               0

#define EEPROM_AFSR_ADDRESS             0x0B
#define EEPROM_GFSR_ADDRESS             0x0C
#define EEPROM_AXIS_ADDRESS             0x0D
#define EEPROM_DLPF_ADDRESS             0x0E
#define EEPROM_SAMPLE_RATE_ADDRESS      0x0F

#define EEPROM_ACC_X                    0x00
#define EEPROM_ACC_Y                    0x01
#define EEPROM_ACC_Z                    0x02

#define EEPROM_GYRO_X                   0x03
#define EEPROM_GYRO_Y                   0x04
#define EEPROM_GYRO_Z                   0x05

#define LOW_SAMPLE_RATE                 0x00
#define MED_SAMPLE_RATE                 0x01
#define HIGH_SAMPLE_RATE                0x02


uint8_t userEnter2hex(void);
void print8dot8(uint16_t value);
void myTMR0ISR(void);


//----------------------------------------------
// Creating a data structure for the buffers
//----------------------------------------------

typedef enum {
    BEING_FILLED, FILLED, BEING_EMPTIED, EMPTY
} myBufferState_t;

typedef struct {
    myBufferState_t state;
    uint8_t buffer[BLOCK_SIZE];
} sdCardBlock_t;

sdCardBlock_t redBuffer, blueBuffer;
uint8_t readingFromSDcardToPWM = false; // main controls this variable
uint8_t writingAccToSDcard = false; // main controls this variable
uint16_t    sampleRate = TMR0_FIVE_MILLISECONDS;



//*****************************************************************
//*****************************************************************

void main(void) {

    uint8_t acquiringData = false;
    uint8_t endOfFile = false;

    uint16_t    i, j;
    char        cmd;
    uint8_t     status, cfg;
    uint32_t    sdCardAddress = 0x00000000;
    uint32_t    loopSDcardAddress;
    uint16_t    playLengthInBlocks = 0;
    uint16_t    blockCount;
    uint8_t     i2cAddress, i2cData;
    int16_t     x16, y16, z16;

    SYSTEM_Initialize();

    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while (INTCONbits.TMR0IF == 0);

    CS_SetHigh();
    READY_LED_SetHigh(); // Turn REDY LED on
    RECORD_LED_SetLow(); // Turn RECORD LED off
    FINISH_LED_SetLow(); // Turn FINISH LED on

    I2C1_Initialize();
    TMR0_SetInterruptHandler(myTMR0ISR);
    INTERRUPT_GlobalInterruptEnable(); // ISR not working? - you probably 
    INTERRUPT_PeripheralInterruptEnable(); // forgot to add these 2 lines

    // Initialize SD card
    for (i = 0; i < 20; i++) printf("\r\n");

    printf("Initializing SD card\r\n");
    readingFromSDcardToPWM = false;
    writingAccToSDcard = false;
    SPI2_Close();
    SPI2_Open(SPI2_DEFAULT);
    SDCARD_Initialize(true);


    printf("Initializing: acceleration scale, gyroscope scale, dlpf scale\r\n");
    
    INTERRUPT_GlobalInterruptDisable();
    MPU6050_setAccelerationScale(DATAEE_ReadByte(EEPROM_AFSR_ADDRESS));
    MPU6050_setGyroscopeScale(DATAEE_ReadByte(EEPROM_GFSR_ADDRESS)); 
    MPU6050_setDlpfScale(DATAEE_ReadByte(EEPROM_DLPF_ADDRESS));    
    MPU6050_active();                           // Set to active to start reading
    switch(DATAEE_ReadByte(EEPROM_SAMPLE_RATE_ADDRESS)) {
        case LOW_SAMPLE_RATE:   sampleRate = TMR0_FIFTY_MILLISECONDS;       break;
        case MED_SAMPLE_RATE:   sampleRate = TMR0_FIVE_MILLISECONDS;        break;
        case HIGH_SAMPLE_RATE:  sampleRate = TMR0_FIFTH_MILLISECONDS;       break;
        default:                sampleRate = TMR0_FIVE_MILLISECONDS;        break;
    }    
    INTERRUPT_GlobalInterruptEnable();

    printf("Finished initialization of accelerometer\r\n");


    if (TOP_BUTTON_GetValue() == 0) {

        printf("In autonomous record mode - press bottom button to start data logging.\r\n");
        // Wait for top button to be pushed, then record
        while (BOTTOM_BUTTON_GetValue() == 1);
        while (BOTTOM_BUTTON_GetValue() == 0);
        printf("Data logging started.  Press top button to stop.\r\n");

        READY_LED_SetLow(); // Turn REDY LED on
        FINISH_LED_SetLow(); // Turn FINISH LED on

        loopSDcardAddress = 0;
        redBuffer.state = BEING_FILLED;
        blueBuffer.state = BEING_FILLED;
        writingAccToSDcard = true;

        acquiringData = true;

        while (acquiringData == true) {

            while (redBuffer.state == BEING_FILLED) {
                if (TOP_BUTTON_GetValue() == 0) {
                    acquiringData = false;
                }
            }
            SDCARD_WriteBlock(loopSDcardAddress, redBuffer.buffer);
            while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);
            redBuffer.state = BEING_FILLED;
            loopSDcardAddress += BLOCK_SIZE;

            while (blueBuffer.state == BEING_FILLED) {
                if (TOP_BUTTON_GetValue() == 0) {
                    acquiringData = false;
                }
            }
            SDCARD_WriteBlock(loopSDcardAddress, blueBuffer.buffer);
            while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);
            blueBuffer.state = BEING_FILLED;
            loopSDcardAddress += BLOCK_SIZE;
        }
        
        // Provide ime for previous commit to finish
        INTCONbits.TMR0IE = 0;
        TMR0_WriteTimer(0x0000);
        INTCONbits.TMR0IF = 0;
        while (INTCONbits.TMR0IF == 0);
        INTCONbits.TMR0IE = 1;
        
        // Write a a STOP indicator on the SD card in block+1
        for (i = 0; i < BLOCK_SIZE; i++) blueBuffer.buffer[i] = 0xFF;
        SDCARD_WriteBlock(loopSDcardAddress, blueBuffer.buffer);
        while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);
                 
        printf("\r\nData logging stopped.  Terminal Ready.\r\n");
    } else {
        printf("Bypassed autonomous record mode. Terminal Ready.\r\n");
    }

    writingAccToSDcard = false;
    READY_LED_SetLow(); // Turn REDY LED on      
    FINISH_LED_SetHigh(); // Turn FINISH LED on

    printf("\r\n> "); // print a nice command prompt

    for (;;) {
        if (EUSART1_DataReady) { // wait for incoming data on USART
            cmd = EUSART1_Read();
            printf("%c\r\n", cmd); // print a nice command prompt     
            switch (cmd) { // and do what it tells you to do
                case '?':
                    printf("-------------------------------------------------\r\n");
                    printf("MPU6050 configuration \r\n");

                    
                    switch(DATAEE_ReadByte(EEPROM_AFSR_ADDRESS)) {
                        case MPU6050_SCALE_2G: printf("Accelerometer FSR is +/- 2 g's\r\n");  break;
                        case MPU6050_SCALE_4G: printf("Accelerometer FSR is +/- 4 g's\r\n");  break;
                        case MPU6050_SCALE_8G: printf("Accelerometer FSR is +/- 8 g's\r\n");  break;
                        case MPU6050_SCALE_16G: printf("Accelerometer FSR is +/- 16 g's\r\n");  break;
                        default:               printf("Error with stored accelerometer FSR value = %02X\r\n",DATAEE_ReadByte(EEPROM_AFSR_ADDRESS)); break;
                    }

                    switch(DATAEE_ReadByte(EEPROM_GFSR_ADDRESS)) {
                        case MPU6050_SCALE_250D:  printf("Gyroscope FSR is +/- 250 degree/sec\r\n");  break;
                        case MPU6050_SCALE_500D:  printf("Gyroscope FSR is +/- 500 degree/sec\r\n");  break;
                        case MPU6050_SCALE_1000D: printf("Gyroscope FSR is +/- 100 degree/sec\r\n");  break;
                        case MPU6050_SCALE_2000D: printf("Gyroscope FSR is +/- 2000 degree/sec\r\n");  break;
                        default:                  printf("Error with stored gyroscope FSR value = %02X\r\n",DATAEE_ReadByte(EEPROM_GFSR_ADDRESS)); break;
                    }
                    
                    switch(DATAEE_ReadByte(EEPROM_DLPF_ADDRESS)) {
                        case 0:     printf("DLPF set to 260 Hz bandwidth\r\n"); break;
                        case 1:     printf("DLPF set to 184 Hz bandwidth\r\n"); break;
                        case 2:     printf("DLPF set to 94 Hz bandwidth\r\n");  break;
                        case 3:     printf("DLPF set to 44 Hz bandwidth\r\n");  break;
                        case 4:     printf("DLPF set to 21 Hz bandwidth\r\n");  break;
                        case 5:     printf("DLPF set to 10 Hz bandwidth\r\n");  break;
                        case 6:     printf("DLPF set to 5 Hz bandwidth\r\n");   break;
                        default:    printf("Error with stored accelerometer DLPF value = %02X\r\n",DATAEE_ReadByte(EEPROM_DLPF_ADDRESS)); break;
                    }
                    
                    switch(DATAEE_ReadByte(EEPROM_AXIS_ADDRESS)) {
                        case EEPROM_GYRO_X:    printf("gyroscope X-axis\r\n"); break;
                        case EEPROM_GYRO_Y:    printf("gyroscope Y-axis\r\n"); break;
                        case EEPROM_GYRO_Z:    printf("gyroscope Z-axis\r\n"); break;                       
                        case EEPROM_ACC_X:     printf("accelerometer X-axis\r\n"); break;                       
                        case EEPROM_ACC_Y:     printf("accelerometer Y-axis\r\n"); break;                       
                        case EEPROM_ACC_Z:     printf("accelerometer Z-axis\r\n"); break;  
                        default:               printf("Unknown AXIS being read = %02X\r\n",DATAEE_ReadByte(EEPROM_AXIS_ADDRESS)); break;
                    }

                    switch(DATAEE_ReadByte(EEPROM_SAMPLE_RATE_ADDRESS)) {
                        case LOW_SAMPLE_RATE:   printf("sample every %u us\r\n", TMR0_FIFTY_MILLISECONDS);
                        case MED_SAMPLE_RATE:   printf("sample every %u us\r\n", TMR0_FIVE_MILLISECONDS);        break;
                        case HIGH_SAMPLE_RATE:  printf("sample every %u us\r\n", TMR0_FIFTH_MILLISECONDS);       break;
                        default:                printf("Unknown sampling rate = %02X\r\n",DATAEE_ReadByte(EEPROM_AXIS_ADDRESS)); break;
                    }    
                        
                    printf("-------------------------------------------------\r\n");
                    printf("?: Help menu\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("u: autonomous Usage instructions\r\n");
                    printf("-------------------------------------------------\r\n");
                    printf("i: Initialize SD card\r\n");
                    printf("-/+: decrease/increase read address\r\n");
                    printf("b: read a Block of %d bytes from SD card\r\n", BLOCK_SIZE);
                    printf("-------------------------------------------------\r\n");
                    printf("d: set digital low pass filter scale and store in PIC EEPROM\r\n");
                    printf("f: set accelerometer FSR scale and store in PIC EEPROM\r\n");
                    printf("F: set gyroscope FSR scale and store in PIC EEPROM\r\n");
                    printf("E: Set the type of data that is logged and store in PIC EEPROM\r\n");     
                    printf("S: Set the sample rate of the MPU6050 and store in PIC EEPROM\r\n");                         
                    printf("-------------------------------------------------\r\n");
                    printf("o: reset accelerometer\r\n");
                    printf("w: Write an arbitrary MPU6050 register and value\r\n");
                    printf("r: Read an arbitrary MPU6050 register\r\n");
                    printf("A: read all the registers from the MPU6050\r\n");
                    printf("a: read acceleration data registers from the MPU6050\r\n");
                    printf("g: read gyroscope data registers from the MPU6050\r\n");
                    printf("n: collect and spool 512 samples\r\n");
                    printf("-------------------------------------------------\r\n");
                    printf("R: Record data from MPU6050 (according to configuration) to SD card\r\n");
                    printf("s: spool data on SD card data to terminal\r\n");
                    printf("-------------------------------------------------\r\n");             
                    printf("\r\n> "); // print a nice command prompt                    
                    break;

                    //--------------------------------------------
                    // Reset the processor after clearing the terminal
                    //--------------------------------------------                      
                case 'Z':
                    for (i = 0; i < 40; i++) printf("\r\n");
                    printf("\r\n> "); // print a nice command prompt
                    RESET();
                    break;

                    //--------------------------------------------
                    // Clear the terminal
                    //--------------------------------------------                      
                case 'z':
                    for (i = 0; i < 40; i++) printf("\r\n");
                    printf("\r\n> "); // print a nice command prompt
                    break;

                    //--------------------------------------------
                    // Print instructions
                    //--------------------------------------------     
                case 'u':
                    printf("To run in autonomous mode:\r\n");
                    printf("    Step 1: Reset + top button pressed                  Yellow LED\r\n");
                    printf("    Step 2: Press bottom button to start recording      Flashing 0.5Hz green LED \r\n");
                    printf("    Step 3: Press upper button to stop logging          Red LED\r\n");
                    printf("    Step 4: Use terminal\r\n");
                    printf("\r\n> "); // print a nice command prompt
                    break;

                    //-------------------------------------------- 
                    // Init SD card to get it read to perform read/write
                    //--------------------------------------------    
                case 'i':
                    readingFromSDcardToPWM = false;
                    writingAccToSDcard = false;
                    SPI2_Close();
                    SPI2_Open(SPI2_DEFAULT);
                    SDCARD_Initialize(true);
                    printf("SD card initalized.\r\n");
                    printf("\r\n> "); // print a nice command prompt
                    break;

                    //--------------------------------------------
                    // -/+ decrease/increase read address
                    //--------------------------------------------                 
                case '-':
                case '+':
                    readingFromSDcardToPWM = false;
                    writingAccToSDcard = false;
                    if (cmd == '-') {
                        sdCardAddress -= BLOCK_SIZE;
                        printf("Decreased ");
                    } else {
                        sdCardAddress += BLOCK_SIZE;
                        printf("Increased ");
                    }
                    printf("SD card address:  ");
                    printf("%04x", sdCardAddress >> 16);
                    printf(":");
                    printf("%04x", sdCardAddress & 0X0000FFFF);
                    printf("\r\n> "); // print a nice command prompt
                    break;

                    //--------------------------------------------
                    // b: read a Block of BLOCK_SIZE bytes from SD card
                    //--------------------------------------------
                case 'b':
                    readingFromSDcardToPWM = false;
                    writingAccToSDcard = false;

                    // Separate the 16-bit address into a low/high byte
                    SDCARD_ReadBlock(sdCardAddress, redBuffer.buffer);
                    printf("Read from address: ");
                    printf("%04x", sdCardAddress >> 16);
                    printf(":");
                    printf("%04x", sdCardAddress & 0X0000FFFF);
                    printf("\r\n");
                    hexDumpBuffer(redBuffer.buffer);
                    printf("\r\n> "); // print a nice command prompt                    
                    break;

                    //--------------------------------------------
                    // Simple reset the MPU6050
                    //--------------------------------------------                                          
                case 'o':
                    // https://courses.cs.washington.edu/courses/cse474/17wi/labs/l4/MPU6050BasicExample.ino
                    INTERRUPT_GlobalInterruptDisable();
                    MPU6050_deviceReset();
                    MPU6050_setAccelerationScale(DATAEE_ReadByte(EEPROM_AFSR_ADDRESS));
                    MPU6050_setGyroscopeScale(DATAEE_ReadByte(EEPROM_GFSR_ADDRESS)); 
                    MPU6050_setDlpfScale(DATAEE_ReadByte(EEPROM_DLPF_ADDRESS));    
                    MPU6050_active();                           // Set to active to start reading                                        
                    INTERRUPT_GlobalInterruptEnable();
                    printf("MPU6050 enabled.\r\n");

                    printf("\r\n> "); // print a nice command prompt                    
                    break;

                    //--------------------------------------------
                    // d: change digital low pass filter
                    //--------------------------------------------                       
                case 'd':                    
                    printf("0:  260 Hz bandwidth\r\n");
                    printf("1:  184 Hz bandwidth\r\n");
                    printf("2:  94 Hz bandwidth\r\n");
                    printf("3:  44 Hz bandwidth\r\n");
                    printf("4:  21 Hz bandwidth\r\n");
                    printf("5:  10 Hz bandwidth\r\n");
                    printf("6:  5 Hz bandwidth\r\n");

                    while (EUSART1_DataReady);
                    cmd = EUSART1_Read();
                    
                    MPU6050_setDlpfScale(cmd-'0');
                    DATAEE_WriteByte(EEPROM_DLPF_ADDRESS,cmd - '0');
                    printf("%c\r\n> ",cmd);                    
                    break;         

                    //--------------------------------------------
                    // f: change full scale range of accelerometer
                    //--------------------------------------------                       
                case 'f':                    
                    printf("0: set accelerometer fsr to +/- 2g\r\n");
                    printf("1: set accelerometer fsr to +/- 4g\r\n");
                    printf("2: set accelerometer fsr to +/- 8g\r\n");                    
                    printf("3: set accelerometer fsr to +/- 16g\r\n");        

                    while (EUSART1_DataReady);
                    cmd = EUSART1_Read();
                    MPU6050_setAccelerationScale(cmd-'0');
                    DATAEE_WriteByte(EEPROM_AFSR_ADDRESS,cmd-'0');
                    printf("%c\r\n> ", cmd);             // print a nice command prompt                                        
                    break;    

                    
                    //--------------------------------------------
                    // F: change full scale range of gyroscope
                    //--------------------------------------------                       
                case 'F':                    
                    printf("0: set gyroscope fsr to +/- 250 degrees/sec\r\n");
                    printf("1: set gyroscope fsr to +/- 500 degrees/sec\r\n");
                    printf("2: set gyroscope fsr to +/- 1000 degrees/sec\r\n");
                    printf("3: set gyroscope fsr to +/- 2000 degrees/sec\r\n");     

                    while (EUSART1_DataReady);
                    cmd = EUSART1_Read();
                    MPU6050_setGyroscopeScale(cmd-'0');
                    DATAEE_WriteByte(EEPROM_GFSR_ADDRESS,cmd-'0');
                    printf("%c\r\n> ", cmd);             // print a nice command prompt                                        
                    break;    

                    //--------------------------------------------
                    // E: Set mode that data is recorded
                    //--------------------------------------------                    
                case 'E':
                    printf("%d:      EEPROM_ACC_X \r\n",EEPROM_ACC_X  );
                    printf("%d:      EEPROM_ACC_Y \r\n",EEPROM_ACC_Y);
                    printf("%d:      EEPROM_ACC_Z \r\n",EEPROM_ACC_Z);                    
                    printf("%d:      EEPROM_GYRO_X \r\n",EEPROM_GYRO_X);
                    printf("%d:      EEPROM_GYRO_Y \r\n",EEPROM_GYRO_Y);
                    printf("%d:      EEPROM_GYRO_Z \r\n",EEPROM_GYRO_Z);                    

                    while (EUSART1_DataReady);
                    cmd = EUSART1_Read();
                    DATAEE_WriteByte(EEPROM_AXIS_ADDRESS, cmd - '0');
                    printf("%c\r\n> ",cmd);                    
                    break;                    

                    //--------------------------------------------
                    // R: Set sample rate of the MPU 6050
                    //--------------------------------------------     
                case 'S':
                    printf("%d: set sample rate to %u us\r\n", LOW_SAMPLE_RATE,  TMR0_FIFTY_MILLISECONDS);
                    printf("%d: set sample rate to %u us\r\n", MED_SAMPLE_RATE,  TMR0_FIVE_MILLISECONDS);
                    printf("%d: set sample rate to %u us\r\n", HIGH_SAMPLE_RATE, TMR0_FIFTH_MILLISECONDS);
                    
                    while (EUSART1_DataReady);
                    cmd = EUSART1_Read();
                    DATAEE_WriteByte(EEPROM_SAMPLE_RATE_ADDRESS, cmd - '0');
                    printf("%c\r\n> ",cmd);                    
                    
                    switch(cmd - '0') {
                        case LOW_SAMPLE_RATE:   sampleRate = TMR0_FIFTY_MILLISECONDS;       break;
                        case MED_SAMPLE_RATE:   sampleRate = TMR0_FIVE_MILLISECONDS;        break;
                        case HIGH_SAMPLE_RATE:  sampleRate = TMR0_FIFTH_MILLISECONDS;       break;
                        default:                sampleRate = TMR0_FIVE_MILLISECONDS;        break;
                    }   
                                            
                    break;                      
                    
                    
                    
                    //--------------------------------------------
                    // Write register and value
                    //--------------------------------------------                       
                case 'w':
                    printf("Enter 1 or 2 digit address then hit Enter\r\n");
                    i2cAddress = userEnter2hex();
                    printf("\r\n");

                    printf("Enter 1 or 2 digit value to write to address 0x%02x\r\n", i2cAddress);
                    i2cData = userEnter2hex();
                    printf("\r\n");

                    INTERRUPT_GlobalInterruptDisable();                 
                    I2C1_Write1ByteRegister(MPU6050_DEVICE_ADDRESS, i2cAddress, i2cData);
                    printf("I2C1_Write1ByteRegister(MPU6050_DEVICE_ADDRESS, 0x%02x, 0x%02x) complete\r\n", i2cAddress, i2cData);
                    INTERRUPT_GlobalInterruptEnable();        
                    
                    printf("\r\n> "); // print a nice command prompt                    
                    break;
                                        
                    //--------------------------------------------
                    // Read MPU6050 register
                    //--------------------------------------------                       
                case 'r':
                    printf("Enter 1 or 2 digit address then hit Enter\r\n");
                    i2cAddress = userEnter2hex();
                    printf("\r\n");

                    INTERRUPT_GlobalInterruptDisable();                                     
                    printf("I2C1_Read1ByteRegister(0x%02x, 0x%02x) = 0x%02x\r\n", MPU6050_DEVICE_ADDRESS, i2cAddress, I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, i2cAddress));
                    INTERRUPT_GlobalInterruptEnable();        
                    
                    printf("\r\n> "); // print a nice command prompt                    
                    break;                 
                    
                    
                    
                    //--------------------------------------------
                    // read All register in the MPU6050
                    //--------------------------------------------                       
                case 'A':
                    INTERRUPT_GlobalInterruptDisable();
                    for (i = 0; i < MPU6050_NUM_REGISTERS; i++) {
                        printf("%02x    %s  \t  %02x\r\n", MPU6050RegisterAddress[i], MPU6050RegisterNames[i], I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, MPU6050RegisterAddress[i]));
                    }                 
                    INTERRUPT_GlobalInterruptEnable();
                    printf("\r\n> "); // print a nice command prompt                    
                    break;

                    //--------------------------------------------
                    // Read acceleration data from the MPU6050
                    //--------------------------------------------                       
                case 'a':
                    INTERRUPT_GlobalInterruptDisable();
                    printf("Accelerometer values\r\n");
                    x16 = (I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, ACCEL_XOUT_H) << 8) | I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, ACCEL_XOUT_L);
                    y16 = (I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, ACCEL_YOUT_H) << 8) | I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, ACCEL_YOUT_L);
                    z16 = (I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, ACCEL_ZOUT_H) << 8) | I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, ACCEL_ZOUT_L);
                    printf("X:  %04x    %d\r\n", x16, x16);
                    printf("Y:  %04x    %d\r\n", y16, y16);
                    printf("Z:  %04x    %d\r\n", z16, z16);            
                    INTERRUPT_GlobalInterruptEnable();
                    printf("\r\n> "); // print a nice command prompt                    
                    break;

                    //--------------------------------------------
                    // Read gyroscope data from the MPU6050
                    //-------------------------------------------                    
                case 'g':
                    printf("Gyroscope values\r\n");
                    x16 = (I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, GYRO_XOUT_H) << 8) | I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, GYRO_XOUT_L);
                    y16 = (I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, GYRO_YOUT_H) << 8) | I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, GYRO_YOUT_L);
                    z16 = (I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, GYRO_ZOUT_H) << 8) | I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, GYRO_ZOUT_L);
                    printf("X:  %04x    %d\r\n", x16, x16);
                    printf("Y:  %04x    %d\r\n", y16, y16);
                    printf("Z:  %04x    %d\r\n", z16, z16);
                    printf("\r\n> "); // print a nice command prompt    
                    break;

                    //--------------------------------------------
                    // n: collect 512 samples for Noise analysis
                    //--------------------------------------------                       
                case 'n':                 
                    printf("Recording %d samples for noise analysis.\r\n",BLOCK_SIZE >> 1);      
                    
                    redBuffer.state = BEING_FILLED;                    
                    writingAccToSDcard = true;
                    INTERRUPT_GlobalInterruptEnable();
                    
                    while (redBuffer.state == BEING_FILLED);
                    writingAccToSDcard = false;
                    
                    printf("Done recording.\r\n");                    
                    
                    for (i = 0; i < BLOCK_SIZE; i += 2) {
                        z16 = (redBuffer.buffer[i] << 8) | redBuffer.buffer[i + 1];
                        printf("%d\r\n", z16);
                    }
                    printf("\r\n> "); // print a nice command prompt    
                    break;
                    
                    //--------------------------------------------
                    // R: Record accelerometer store on SD card
                    //--------------------------------------------
                case 'R':

                    printf("Recording in progress....\r\n");
                    printf("Press any key to stop recording MPU6050 data.\r\n");

                    loopSDcardAddress = 0;
                    redBuffer.state = BEING_FILLED;
                    blueBuffer.state = BEING_FILLED;
                    writingAccToSDcard = true;

                    while (EUSART1_DataReady == false) {
                        while (redBuffer.state == BEING_FILLED);
                        SDCARD_WriteBlock(loopSDcardAddress, redBuffer.buffer);
                        while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);
                        redBuffer.state = BEING_FILLED;
                        loopSDcardAddress += BLOCK_SIZE;

                        while (blueBuffer.state == BEING_FILLED);
                        SDCARD_WriteBlock(loopSDcardAddress, blueBuffer.buffer);
                        while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);
                        blueBuffer.state = BEING_FILLED;
                        loopSDcardAddress += BLOCK_SIZE;

                    }
                    
                    // Disable ISR from collect any further data
                    writingAccToSDcard = false;
                    
                    // Provide ime for previous commit to finish
                    INTCONbits.TMR0IE = 0;
                    TMR0_WriteTimer(0x0000);
                    INTCONbits.TMR0IF = 0;
                    while (INTCONbits.TMR0IF == 0);
                    INTCONbits.TMR0IE = 1;
                    
                    // Write a a STOP indicator on the SD card in block+1
                    for (i = 0; i < BLOCK_SIZE; i++) blueBuffer.buffer[i] = 0xFF;
                    SDCARD_WriteBlock(loopSDcardAddress, blueBuffer.buffer);
                    while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);                    

                    (void) EUSART1_Read();
                    playLengthInBlocks = loopSDcardAddress >> 9;
                    printf("\r\nJust wrote %u blocks to SD card. Each block has %u bytes.\r\n", playLengthInBlocks, BLOCK_SIZE);
                    printf("\r\n> "); // print a nice command prompt                    
                    break;
                    
                    //--------------------------------------------
                    // s: spool memory to a csv file
                    //--------------------------------------------
                case 's':
                    readingFromSDcardToPWM = false;
                    writingAccToSDcard = false;

                    printf("Spooling the SD card will automatically terminate when EOF is encountered.\r\n");
                    printf("You may terminate spooling at anytime with a keypress.\r\n");
                    printf("To spool terminal contents into a file follow these instructions:\r\n\n");
                    printf("Right mouse click on the upper left of the PuTTY window\r\n");
                    printf("Select:     Change settings...\r\n");
                    printf("Select:     Logging\r\n");
                    printf("Select:     Session logging: All session output\r\n");
                    printf("Log file name:  Browse and provide a .csv extension to your file name\r\n");
                    printf("Select:     Apply\r\n");
                    printf("\r\n\nPress any key to start\r\n");

                    while (EUSART1_DataReady == false);
                    (void) EUSART1_Read();
                    blockCount = 0;
                    loopSDcardAddress = 0;
                    endOfFile = false;

                    while ((endOfFile == false) && (EUSART1_DataReady == false)) {
                        SDCARD_ReadBlock(loopSDcardAddress, redBuffer.buffer);
                        j = 0;
                        for (i = 0; i < BLOCK_SIZE; i++)
                            if ((redBuffer.buffer[i] == 255) || (redBuffer.buffer[i] == 0)) j += 1;

                        if (j == BLOCK_SIZE) {
                            endOfFile = true;
                        } else {
                            loopSDcardAddress += BLOCK_SIZE;
                            blockCount += 1;
                            for (i = 0; i < BLOCK_SIZE; i += 2) {
                                z16 = (redBuffer.buffer[i] << 8) | redBuffer.buffer[i + 1];
                                printf("%d\r\n", z16);
                            }
                        }
                    } // end spooling due to keypress or EOF

                    (void) EUSART1_Read();
                    printf("Spooled %d blocks.\r\n", blockCount);
                    printf("To close the file follow these instructions:\r\n\n");
                    printf("Right mouse click on the upper left of the PuTTY window\r\n");
                    printf("Select:     Change settings...\r\n");
                    printf("Select:     Logging\r\n");
                    printf("Select:     Session  logging: None\r\n");
                    printf("Select:     Apply\r\n");
                    printf("\r\n> "); // print a nice command prompt
                    break;

                    //--------------------------------------------
                    // If something unknown is hit, tell user
                    //--------------------------------------------
                default:
                    printf("Unknown key %c\r\n", cmd);
                    printf("\r\n> "); // print a nice command prompt                    
                    break;
            } // end switch
        } // end if        
    } // end infinite loop    
} // end main



//*****************************************************************
// Stupid MPlab default ISR RELOADS TMR initial value so you need
// to be very careful when setting up the ISR in MCC to make the
// The initial value equal to actual 0 which still causes some 
// inaccuracies but is better than screwing things up totally.
//*****************************************************************
#define SDCARD_NUM_BYTES_WRITTEN    2
#define LED_UP                      1
#define LED_DOWN                    0

void myTMR0ISR(void) {

    static uint16_t index = 0;
    static uint8_t color = RED;
    static uint8_t ledCounter = 0;
    static uint8_t ledCounterDirection = LED_UP;
    uint8_t mpu6050_addr = 0;
    
    // Provide an external timing reference
    SAMPLE_FREQ_Toggle();      
    
    if (writingAccToSDcard == true) {

        switch(DATAEE_ReadByte(EEPROM_AXIS_ADDRESS)) {                      
            case EEPROM_ACC_X:     mpu6050_addr = ACCEL_XOUT_H;      break;                      
            case EEPROM_ACC_Y:     mpu6050_addr = ACCEL_YOUT_H;      break;                     
            case EEPROM_ACC_Z:     mpu6050_addr = ACCEL_ZOUT_H;      break; 
            case EEPROM_GYRO_X:    mpu6050_addr = GYRO_XOUT_H;     break;
            case EEPROM_GYRO_Y:    mpu6050_addr = GYRO_YOUT_H;     break;
            case EEPROM_GYRO_Z:    mpu6050_addr = GYRO_ZOUT_H;     break;  
            default:               mpu6050_addr = ACCEL_ZOUT_H;     break;
        }
                        
                        
        if (ledCounterDirection == LED_UP) { // LED on
            ledCounter += 1;
            if (ledCounter >= 100) ledCounterDirection = LED_DOWN;
            RECORD_LED_SetLow();
        } else {
            ledCounter -= 1;
            if (ledCounter == 0) ledCounterDirection = LED_UP;
            RECORD_LED_SetHigh();
        }

        if ((color == RED) && (redBuffer.state == BEING_FILLED)) {
      
            redBuffer.buffer[index + 0] = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, mpu6050_addr);
            redBuffer.buffer[index + 1] = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, mpu6050_addr + 1);
       
            index += SDCARD_NUM_BYTES_WRITTEN;
            if (index >= BLOCK_SIZE) {
                redBuffer.state = FILLED;
                color = BLUE;
                index = 0;
                putchar('r');
            }
        } else if ((color == BLUE) && (blueBuffer.state == BEING_FILLED)) {
  
            blueBuffer.buffer[index + 0] = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, mpu6050_addr);
            blueBuffer.buffer[index + 1] = I2C1_Read1ByteRegister(MPU6050_DEVICE_ADDRESS, mpu6050_addr + 1);
    
            index += SDCARD_NUM_BYTES_WRITTEN;
            if (index >= BLOCK_SIZE) {
                blueBuffer.state = FILLED;
                color = RED;
                index = 0;
                putchar('b');
            }
        }
    }

    if (writingAccToSDcard == false) {
        color = RED;
        index = 0;
        ledCounter = 0;
        ledCounterDirection = LED_UP;
        RECORD_LED_SetLow();
        
    }

    // Set time between conversions
    TMR0_WriteTimer(0x10000 - (sampleRate - TMR0_ReadTimer()));
    INTCONbits.TMR0IF = 0;

}



//----------------------------------------------
// printf 8.8 fixed point format number
//----------------------------------------------

void print8dot8(uint16_t value) {

    uint8_t i;
    uint16_t fracTimes1000 = 0;
    uint16_t powerOf2Times1000 = 5000;

    if (value & 0x8000) {
        printf("-");
        value = ~value + 1;
    }

    printf("%d.", value >> 14);

    powerOf2Times1000 = 5000;
    for (i = 0; i < 14; i++) {
        if (value & (1 << (13 - i))) fracTimes1000 += powerOf2Times1000;
        powerOf2Times1000 = powerOf2Times1000 >> 1;
    }
    printf("%04d", fracTimes1000);

} // end print8dot8



//----------------------------------------------
// prompts user to enter 8-bit value
//----------------------------------------------

uint8_t userEnter2hex(void) {

    uint8_t value = 0;
    char cmd;

    printf(">0x");
    while (EUSART1_DataReady);
    value = 0;
    while ((cmd = EUSART1_Read()) != '\r') {
        if ((cmd >= '0') && (cmd <= '9')) {
            printf("%c", cmd);
            value = (value << 4) + (cmd - '0');
        }

        if ((toupper(cmd) >= 'A') && (toupper(cmd) <= 'F')) {
            printf("%c", cmd);
            value = (value << 4) + (toupper(cmd) - 'A' + 10);
        }
    }
    return (value);
}

