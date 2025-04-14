//---------------------------------------------------------------
// Filename: Vending Machine RTOS application
// Purpose: To offer a multitasking programming construct
//---------------------------------------------------------------


// Create the eventIDs
unsigned uint8_t messageSelection = 0;
unsigned uint8_t semaphoreItemReleased = 1;
unsigned uint8_t messageCentsLeftOver = 2;

unsigned uint8_t* item = 0;
unsigned uint8_t  c;
unsigned uint8_t  money = 0;

void main() {

	Enable_Interrupts(INT_RDA);
	
	// Initialize the multitasking sub system
	InitializeMultitasking(7, 3);

	// Initiate each of the users tasks
	CreateTask(taskControlTemps,7);	
	CreateTask(taskShowEmpties,6);
	CreateTask(taskAcceptCurrency,5);	
	CreateTask(taskReadButtons,4);
	CreateTask(taskMakeChange,3);	
	CreateTask(taskReleaseItem,2);
	CreateTask(taskCallPolice,1);

	
	CreateEvent(messageSelection, 0);
	CreateEvent(semaphoreItemReleased, 0);
	CreateEvent(messageCentsLeftOver, 0);
	
	StartMultitasking();
}

//---------------------------------------------------------------
//---------------------------------------------------------------
void taskCallPolice() {

	for(;;) {
		delayMS(100);
		if (funcTilt()) {
			printf("Vandal! Alerting Police HQ...\n\r");
			printf("Chief Wigam is on his way, better run ;-)\n\n\r");
		}
	}
}

//---------------------------------------------------------------
//---------------------------------------------------------------
void taskAcceptCurrency() {
	unsigned uint8_t selected;
	
	for(;;) {
		Delay(10);
		money += funcNewCoinsOrBills();		
	}
}

//---------------------------------------------------------------
//---------------------------------------------------------------
void taskReadButtons() {
	unsigned uint8_t selection = 1;

	for(;;) {
		Delay(20);
		if (c >= '1' && c <= '9') {
			item = c - '0';
			SignalEvent(messageSelection, item);
			c = '\0';
		}
	}
}

//---------------------------------------------------------------
//---------------------------------------------------------------
void taskReleaseItem() {
	
	for(;;) {
		WaitForEvent(messageSelection);
		if (money >= funcPriceOf(item)) {
			funcPrintSelection(item);
			SignalEvent(semaphoreItemReleased, 1);
			SignalEvent(messageCentsLeftOver, money - funcPriceOf(item));
			money = 0;
		}
	}
}

//---------------------------------------------------------------
//---------------------------------------------------------------
void taskMakeChange() {
	
	for(;;) {
		WaitForEvent(messageCentsLeftOver);
		funcRefund(GetMessageValue(messageCentsLeftOver));
	}
}	

//---------------------------------------------------------------
//---------------------------------------------------------------
void taskControlTemps() {
	
	for(;;) {
		delayMS(500);
		printf("Checking Temperature\n\r");	
	}	
}

//---------------------------------------------------------------
//---------------------------------------------------------------
void taskShowEmpties() {
	
	for(;;) {
		WaitForEvent(semaphoreItemReleased);
		printf("Checking Item inventory...\n\r");
		printf("We're still good, not empty yet.\n\r");
	}	
}

//---------------------------------------------------------------
//---------------------------------------------------------------
void funcPrintSelection(unsigned uint8_t selection) {
	printf("\n\rYou selected: ");
	switch (selection) {
		case 1:
			printf("Pepsi\n\r");
			break;
		case 2:
			printf("Mountain Dew\n\r");
			break;
		case 3:
			printf("Dr. Pepper\n\r");
			break;
		case 4:
			printf("Diet Pepsi\n\r");
			break;
		case 5:
			printf("Lipton Ice Tea\n\r");
			break;
		case 6:	
			printf("Gatorade\n\r");
			break;
		case 7:
			printf("5Alive\n\r");
			break;
		case 8:
			printf("Jolt Cola\n\r");
			break;
		case 9:
			printf("Pepperoni Pizza\n\r");
			break;
		default:
			printf("No selection made");
			break;
	}	
}

//---------------------------------------------------------------
//---------------------------------------------------------------
unsigned int1 funcTilt() {
	if (c == 't' || c == 'T') {
		c = '\0';
		return 1;	
	}
	return 0;
}

//---------------------------------------------------------------
//---------------------------------------------------------------
unsigned uint8_t funcNewCoinsOrBills() {
	unsigned uint8_t i = 0;
	
	switch (c) {
		case 'd':
		case 'D':
			i = 10;
			printf("Total Deposited: %u\n\r", money + i);
			break;
		case 'n':
		case 'N':
			i = 5;
			printf("Total Deposited: %u\n\r", money + i);
			break;
		case 'q':
		case 'Q':
			i = 25;
			printf("Total Deposited: %u\n\r", money + i);
			break;
	}	
	c = '\0';	
	return i;
}

//---------------------------------------------------------------
//---------------------------------------------------------------
unsigned uint8_t funcPriceOf(unsigned uint8_t item) {
	unsigned uint8_t price;
	
	switch (item) {
		case 1: case 2: case 3: case 4: case 5: case 6:
			price = 75;
			break;
		case 7: case 8:
			price = 125;
			break;
		case 9:
			price = 175;
			break;
		default:
			price = 0;
			break;		
	}	
	return price;
}

//---------------------------------------------------------------
//---------------------------------------------------------------
void funcRefund(unsigned uint8_t change) {
	printf("\n\rDispensing Change...\n\r");
	printf("%d cents returned.\n\r", change);	
}

//---------------------------------------------------------------
//---------------------------------------------------------------
#int_rda
void RDA_ISR() {
	c = getc();	
}

