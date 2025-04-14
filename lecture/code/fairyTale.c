//---------------------------------------------------------------
// Filename: fairy tale RTOS application
// Author: Chris Coulston
//---------------------------------------------------------------

// Create the eventIDs 
// These must be give sequential numerical indicies
unsigned uint8_t semaphoreFullMoon = 0;
unsigned uint8_t semaphoreClockStrikeOne = 1;
unsigned uint8_t semaphoreGreenCheese = 2;

char UserChar;
uint8_t NumMice = 12;
uint8_t hour=0;
uint8_t day=0;
uint8_t month=0;

//---------------------------------------------------
//---------------------------------------------------
main() {

	INIT();
	InitializeMultitasking(5, 3);

	CreateTask(taskCow,5);
	CreateTask(taskMouse,4);
	CreateTask(taskTicToc,3);
	CreateTask(taskAddMice,2);
	CreateTask(taskAnnounceTime,1);

	CreateEvent(semaphoreFullMoon,0);
	CreateEvent(semaphoreClockStrikeOne,0);
	CreateEvent(semaphoreGreenCheese,0);

	StartMultitasking();
}


//---------------------------------------------------------------
//---------------------------------------------------------------
void taskCow() {

    for(;;) {
		WaitForEvent(semaphoreFullMoon);
		JUMP_OVER_MOON();
		SignalEvent(semaphoreGreenCheese,1);
}   }

//---------------------------------------------------
//-------------------------------------------------
void taskMouse() {

    for(;;) {
		WaitForEvent(semaphoreGreenCheese);
		if (NumMice > 0) {
			RUN_UP_THE_CLOCK();
			WaitForEvent(semaphoreClockStrikeOne);
			NumMice -= 1;
			RUN_DOWN_THE_CLOCK();
}   }	}



//---------------------------------------------------
//-------------------------------------------------
void taskAnnounceTime() {

    for(;;) {
		WaitForEvent(semaphoreClockStrikeOne);
		printf("The clock struck one\r\n");
}   }



//---------------------------------------------------
//-------------------------------------------------
void taskTicToc() {

    for(;;) {
		delayMS(1000);
		printf("		tic toc\r\n");
}   }

//---------------------------------------------------
//-------------------------------------------------
void taskAddMice() {

    for(;;) {
		delayMS(200);
		ADD_MICE();
}   }


//---------------------------------------------------
// INIT() 
// This is a generic initialization function.  We are setting
// up timer0 to interrupt once every second.
//---------------------------------------------------
void INIT() {
	Enable_Interrupts(INT_RDA);
	SET_TIMER1(0x0000);
	SETUP_TIMER_1(T1_INTERNAL | T1_DIV_BY_8);
	Enable_Interrupts(INT_TIMER1);
} // end init


//---------------------------------------------------
// JUMP_OVER_MOON
// This function informs everyone when the cow jumps
// over the moon.
//---------------------------------------------------
void JUMP_OVER_MOON() {
	printf("The cow is jumping over the moon\r\n");
}

//---------------------------------------------------
// RUN_UP_THE_CLOCK
// This function informs everyone how many mice are
// running up the clock.
//---------------------------------------------------
void RUN_UP_THE_CLOCK() {
	printf("%d mice are running up the clock\r\n",NumMice);
}


//---------------------------------------------------
// RUN_DOWN_THE_CLOCK
// This function informs everyone how many mice are
// running down the clock.
//---------------------------------------------------
void RUN_DOWN_THE_CLOCK() {
	printf("%d mice are running down the clock\r\n",NumMice);
}

//---------------------------------------------------
// ADD_MICE
// This fucntion checks for a keypress between 0 and
// 9.  It adds that many mice and clears the character.
//---------------------------------------------------
void ADD_MICE() {

	if ((UserChar>='0') && (UserChar<='9')) NumMice += UserChar-'0';
	UserChar = '\0';		// consume the character
}


//---------------------------------------------------
// This ISR help keep track of time.
//---------------------------------------------------
#int_timer1
void TMR1_ISR() {

	hour = hour + 1;

	if (hour == 1) SignalEvent(semaphoreClockStrikeOne,1);

	if (hour == 24) {
		hour = 0;
		day += 1; 
	}

	if (day == 28) {
		SignalEvent(semaphoreFullMoon,1);
		day = 0;
	}
	SET_TIMER1(0x8000);
}



//---------------------------------------------------
// This ISR allows users to enter data into the RTOS
// through the global variable UsrChar.
//---------------------------------------------------
#int_rda
void RDA_ISR() {
	UserChar = getc();	
}
