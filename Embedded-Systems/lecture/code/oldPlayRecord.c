//---------------------------------------------------------------
// Use these functions to build an RTOS for the playback system
// Delay(x) - creates task to delay x mS
// SignalEvent(messageName, messageValue) - send message 
// GetMessageValue(messageName) - gets the messages value
// WaitForEvent(messageName) - causes task to wait on message
// CheckForEvent(messageName) - Equal 1 if there is a waiting message
//---------------------------------------------------------------

//---------------------------------------------------------------
// messages that are used in this program
// messageStartRecord		messageEndRecord
// messageStartPlay		messageEndPlay
// messageNewMic	
// messageFullPage		messageWriteComplete
// messageNextPage		messageValidPage
// messageNeedNewNote		messageNoteValid
// messageStoreLCD		messagePlayLCD
// messageADIF			messageSSPIF
//---------------------------------------------------------------

// Globals
uint8_t red[64], black[64];
uint32_t RecordedAddress;

//---------------------------------------------------------------
// task:	ReadJoy
// purp:	Reads the joystick every 100mS and
//		starts recording on up 
//		stops recording on down
//		starts playback on left
//		stops recording on right
//---------------------------------------------------------------
void taskReadJoy() {
    while (1) {
	Delay(100);
	if (up_pin)	SignalEvent(___________________,1);
	if (down_pin)	SignalEvent(___________________,1);
	if (left_pin)	SignalEvent(___________________,1);
	if (right_pin)	SignalEvent(___________________,1);
}   }

//---------------------------------------------------------------
// task:	ReadMic
// purp:	Reads the microphone every 1mS and signals 
//		StoreMic with this new sample
//---------------------------------------------------------------
void taskReadMic() {
    while (1) {
	WaitForEvent(____________________);
	// skipping code for initialization of the ADC subsystem	
	while(!CheckForEvent(______________________)) {
	    ADCCONbits.GO = 1;
	    WaitForEvent(messageADIF);
	    SignalEvent(____________________);
	    Delay(_______);
}   }


//---------------------------------------------------------------
// task:	StoreMic
// purp:	acquires samples from ReadMic, puts them into
//		the red or black buffer.  When a buffer is full
//		writing is switched to the other buffer and the
//		full buffer is stored in the SDCARD via the
//		WritePage task.
//---------------------------------------------------------------
void taskStoreMic() {
    uint32_t address;
    uint8_t *buffer;
    while (1) {
	WaitForEvent(_________________);
	address=0;
	buffer=red;
	while(!CheckForEvent(______________________)) {
	    i=0;
	    while(!CheckForEvent(_________________) && i<____) {
		WaitForEvent(___________________);
		buffer[i++]=ValueOfMessage(___________________);
	    }
	    SignalEvent(_______________,_______________);
	    if (buffer==red) buffer=black; else buffer=red;
	    address += 64;
	}
	RecordedAddress=address;
}   }	

//---------------------------------------------------------------
// task:	WritePage
// purp:	Writes a full buffer to the SDCARD and signals
//		the StoreMic task when complete. 
//---------------------------------------------------------------
void taskWritePage() {
    uint32_t address;
    while (1) {
	WaitForEvent(___________________);
	address = GetMessageValue(_________________);
	SSPCON2bits.SEN=1;		WaitForEvent(messageSSPIF);
	SSPBUF=0xA0;			WaitForEvent(messageSSPIF);
	if (GetMessageValue(messageSSPIF)) {
	    SignalEvent(messageError, typeEepromNotFound);
	    KillTask();
	}
	SSPBUF=address>>8;		WaitForEvent(messageSSPIF);
	SSPBUF=address;			WaitForEvent(messageSSPIF);
	for(i=0; i<64; i++) {
	    SSPBUF=buf[i]);		WaitForEvent(messageSSPIF);
	} // end for
	SSPCON2bits.PEN=1;		WaitForEvent(messageSSPIF);

	I2CACK = 1;
	while(I2CACK) {
	    Delay(1);
	    SSPCON2bits.SEN=1;		WaitForEvent(messageSSPIF);
	    SSPBUF=0xA0;		WaitForEvent(messageSSPIF);
	    I2CACK = GetMessageValue(messageSSPIF);
	}
	SSPCON2bits.PEN=1;		WaitForEvent(messageSSPIF);
	SignalEvent(__________________________,1)
}   }

//---------------------------------------------------------------
// task:	PlayPage
// purp:	Reads out pages from the SDCARD into one of two
//		buffers and informs the PlayNote task when a 
//		new CCPR1L value is ready.
//---------------------------------------------------------------
void taskPlayPage() {
    uint32_t address=0;
    uint8_t *buffer;

    while (1) {
	WaitForEvent(_________________________);	
	SignalEvent(__________________________,_____________);
	address += 64;
	WaitForEvent(___________________________);
	buffer=ValueOfMessage(________________________);
	while(address < RecordedAddress) {
	    SignalEvent(___________________,_____________);
	    address += 64;
	    for(i=0; i<64; i++) {
		SignalEvent(_________________________,______________);
		WaitForEvent(________________);
	    }
	    WaitForEvent(_________________________);
	    buffer=ValueOfMessage(____________________);
}   }	}

//---------------------------------------------------------------
// task:	ReadSDCARD
// purp:	read the SDCARD and toggle buffers.
//---------------------------------------------------------------
void taskReadSDCARD() {
    uint32_t address=0;
    uint8_t *buffer = red;

    while (1) {
	WaitForEvent(______________________);
	address=ValueOfMessage(________________________);
	// I2C SDCARD stuff...
	SignalEvent(___________________________,_______________);
	if (buffer == red) buffer=black; else buffer=red;
}   }

//---------------------------------------------------------------
// task:	PlayNote
// purp:	Play the PWM frequency for 1ms
//---------------------------------------------------------------
void taskPlayNote() {
    while(1) {
	WaitForEvent(_________________________);
	CCPR1L = ValueOfMessage(_________________________);
	Delay(__);
	SignalEvent(________________________,1);
}   }


//---------------------------------------------------------------
// task:	I2C_ISR
// purp:	Anytime anyone writes to the SSPBUF this ISR
//		will send a message bac
//---------------------------------------------------------------
void I2C_ISR() {
    PIR1bits.SSPIF = 0;
    SignalEvent(messageSSPIF,SSPCON2bits.ACKSTAT);
}
