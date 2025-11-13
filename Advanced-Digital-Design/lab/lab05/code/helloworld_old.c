/******************************************************************************
 * Borrows from: https://github.com/Xilinx/embeddedsw/blob/master/XilinxProcessorIPLib/drivers/ttcps/examples/xttcps_intr_example.c
 ******************************************************************************/
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "enhancedPwm.h"
#include "xuartps_hw.h"

#include "xil_exception.h"
#include "xttcps.h"
#include "xscugic.h"

#define ENHANCED_PWM_BASEADDR	XPAR_ENHANCEDPWM_0_S00_AXI_BASEADDR
#define USART_BASEADDR			XPAR_PS7_UART_1_BASEADDR
#define DUTY_CYCLE_OFFSET		0
#define PWM_COUNT_OFFSET		4

#define TTC0_0_DEVICE_ID	XPAR_XTTCPS_0_DEVICE_ID
#define TTC0_0_INTR_ID		XPAR_XTTCPS_0_INTR
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

typedef struct {
	u32 OutputHz;			/* Output frequency */
	XInterval Interval;		/* Interval value */
	u8 Prescaler;			/* Prescaler value */
	u16 Options;			/* Option settings */
} TmrCntrSetup;


#define NUM_TTC0_INDEX	1


/* Set up routines for timer counters */
int SetupIntervalTimerWithInterrupt(void);
static int SetupInterruptSystem(u16 IntcDeviceID, XScuGic *IntcInstancePtr);
static void Ttc0IsrHander(void *CallBackRef, u32 StatusEvent);


XScuGic InterruptController;  /* Interrupt controller instance */
XTtcPs  TtcPsInst[NUM_TTC0_INDEX];	/* Number of available timer counters */

TmrCntrSetup SettingsTable[NUM_TTC0_INDEX] = {
		{10000, 0, 0, 0}
};

#define SIN_LUT_LENGTH 64

u16 phaseIncrement = 0;
u16 dutyCycle = 0;
u8 generateWave = FALSE;
u8 sinLut[SIN_LUT_LENGTH + 1] = {128, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 240, 245, 250, 253, 254, 255, 254, 253, 250, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140, 128, 115, 103, 90, 79, 67, 57, 47, 37, 29, 21, 15, 10, 5, 2, 1, 0, 1, 2, 5, 10, 15, 21, 29, 37, 47, 57, 67, 79, 90, 103, 115, 128};

int main()
{
	u8 c;

	int Status;


	init_platform();

	Status = SetupInterruptSystem(INTC_DEVICE_ID, &InterruptController);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = SetupIntervalTimerWithInterrupt();
	if (Status != XST_SUCCESS) {
		return Status;
	}

	printf("Welcome to the Enhanced PWM interface\n\r");

	while(1) {

		c=XUartPs_RecvByte(USART_BASEADDR);

		switch(c) {

		/*-------------------------------------------------
		 * Reply with the help menu
		 *-------------------------------------------------
		 */
		case '?':
			printf("--------------------------\r\n");
			printf("PL LED4 displays the PWM output \r\n");
			printf("Disable the Enhanced PWM module by pressing PL_KEY4\r\n");
			printf("PWM Counter 	%ld \r\n", ENHANCEDPWM_mReadReg(ENHANCED_PWM_BASEADDR , PWM_COUNT_OFFSET));
			printf("Duty Cycle = %d\r\n", dutyCycle);
			printf("--------------------------\r\n");
			printf("?: help menu\r\n");
			printf("d: Enter a duty cycle.\r\n");
			printf("f: Flush terminal\r\n");
			printf("0: read enhanced PWM registers\r\n");
			printf("1: read ttc0 index 0 registers\r\n");
			printf("s: Sin Wave\r\n");
			printf("S: Serial Information\r\n");
			break;


			/*-------------------------------------------------
			 * Tell the counter to count up once
			 *-------------------------------------------------
			 */
		case 'd':
			dutyCycle = 0;
			printf("Enter a decimal value between 0 and 256:\r\n");
			do {
				c=XUartPs_RecvByte(USART_BASEADDR);
				if ( (c >= '0') && (c <= '9') ) {
					dutyCycle = dutyCycle * 10 + (c-'0');
					putchar(c);
				}

			} while (c != '\r');

			printf("\r\n");
			ENHANCEDPWM_mWriteReg(ENHANCED_PWM_BASEADDR , DUTY_CYCLE_OFFSET, dutyCycle);					// put value into slv_reg1

			printf("loaded: %d\r\n",dutyCycle);
			break;


			/*-------------------------------------------------
			 * Toggle wave generation
			 *-------------------------------------------------
			 */
		case 's':

			if (generateWave == TRUE) {
				generateWave = FALSE;
				printf("Wave off\r\n");
			} else {
				phaseIncrement += 10;
				generateWave = TRUE;
				printf("Wave on, phase accumulator = %d\r\n",phaseIncrement);
			}

			break;

			/*-------------------------------------------------
			 * Read the AXI register associated with the enhancedPwm component
			 *-------------------------------------------------
			 */
		case '0':
			printf("ENHANCED_PWM_BASEADDR registers \r\n");
			for (c=0; c<4; c++) {
				printf("M[BASEADDR + %d] = %ld\r\n",4*c,ENHANCEDPWM_mReadReg(ENHANCED_PWM_BASEADDR , 4*c));
			}
			break;

			/*-------------------------------------------------
			 * Read the AXI register associated with the TTC0_0 component
			 *-------------------------------------------------
			 */
		case '1':
			printf("XTtcPs_GetCounterValue	= %04x\r\n", XTtcPs_GetCounterValue(& TtcPsInst[TTC0_0_DEVICE_ID])  );
			printf("XTtcPs_GetInterval 	= %04x\r\n", XTtcPs_GetInterval(& TtcPsInst[TTC0_0_DEVICE_ID]) );
			printf("XTtcPs_GetPrescaler 	= %04x\r\n", XTtcPs_GetPrescaler(& TtcPsInst[TTC0_0_DEVICE_ID]) );
			printf("XTtcPs_GetOptions 	= %04lx\r\n", XTtcPs_GetOptions(& TtcPsInst[TTC0_0_DEVICE_ID]) );
			printf("TtcPsInst[TTC_TICK_DEVICE_ID].Config.InputClockHz = %lu\r\n", TtcPsInst[TTC0_0_DEVICE_ID].Config.InputClockHz);
			printf("SettingsTable[0][%04lu, %04x, %04x, %04x]\r\n",SettingsTable[0].OutputHz, SettingsTable[0].Interval, SettingsTable[0].Prescaler, SettingsTable[0].Options);
			break;



			/*-------------------------------------------------
			 * Clear the terminal window
			 *-------------------------------------------------
			 */
		case 'f':
			for (c=0; c<40; c++) printf("\r\n");
			break;

		case 'S':
			printf("Serial registers\r\n");
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_CR_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_MR_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_IER_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_IDR_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_IMR_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_ISR_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_BAUDGEN_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_RXTOUT_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_RXWM_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_MODEMCR_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_MODEMSR_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_SR_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_FIFO_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_BAUDDIV_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_FLOWDEL_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_TXWM_OFFSET));
			printf("%ld ",XUartPs_ReadReg(USART_BASEADDR, XUARTPS_RXBS_OFFSET));
			printf("\r\n");
			break;
			/*-------------------------------------------------
			 * Unknown character was
			 *-------------------------------------------------
			 */
		default:
			printf("unrecognized character: %c\r\n",c);
			break;
		} // end case
	} // end while

	cleanup_platform();

	return 0;

} // end main



/****************************************************************************/
/**
 *
 * This function sets up the TTC0 timer with an associated ISR
 *
 * @param	None
 *
 * @return	XST_SUCCESS if everything sets up well, XST_FAILURE otherwise.
 *
 * @note		None
 *
 *****************************************************************************/
int SetupIntervalTimerWithInterrupt(void)
{
	int Status;
	TmrCntrSetup 	*TimerSetup;
	XTtcPs_Config 	*Config;
	XTtcPs 			*TtcTimerInstPtr;


	TtcTimerInstPtr = &(TtcPsInst[TTC0_0_DEVICE_ID]);
	TimerSetup = &SettingsTable[TTC0_0_DEVICE_ID];

	/*
	 * Set up appropriate options for Ticker: interval mode without
	 * waveform output.
	 */
	TimerSetup->Options |= (XTTCPS_OPTION_INTERVAL_MODE |
					      XTTCPS_OPTION_WAVE_DISABLE);


	Config = XTtcPs_LookupConfig(TTC0_0_DEVICE_ID);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	/*
	 * Initialize the device
	 */
	Status = XTtcPs_CfgInitialize(TtcTimerInstPtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the options
	 */
	XTtcPs_SetOptions(TtcTimerInstPtr, TimerSetup->Options);

	/*
	 * Timer frequency is preset in the TimerSetup structure,
	 * however, the value is not reflected in its other fields, such as
	 * IntervalValue and PrescalerValue. The following call will map the
	 * frequency to the interval and prescaler values.
	 */
	XTtcPs_CalcIntervalFromFreq(TtcTimerInstPtr, TimerSetup->OutputHz,
		&(TimerSetup->Interval), &(TimerSetup->Prescaler));

	/*
	 * Set the interval and prescaler
	 */
	XTtcPs_SetInterval(TtcTimerInstPtr, TimerSetup->Interval);
	XTtcPs_SetPrescaler(TtcTimerInstPtr, TimerSetup->Prescaler);


	/*
	 * Connect to the interrupt controller
	 */
	Status = XScuGic_Connect(&InterruptController, TTC0_0_INTR_ID,
		(Xil_ExceptionHandler)XTtcPs_InterruptHandler, (void *)TtcTimerInstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XTtcPs_SetStatusHandler(&(TtcPsInst[TTC0_0_DEVICE_ID]), &(TtcPsInst[TTC0_0_DEVICE_ID]),
		              (XTtcPs_StatusHandler)Ttc0IsrHander);

	/*
	 * Enable the interrupt for the Timer counter
	 */
	XScuGic_Enable(&InterruptController, TTC0_0_INTR_ID);

	/*
	 * Enable the interrupts for the tick timer/counter
	 * We only care about the interval timeout.
	 */
	XTtcPs_EnableInterrupts(TtcTimerInstPtr, XTTCPS_IXR_INTERVAL_MASK);

	/*
	 * Start the tick timer/counter
	 */
	XTtcPs_Start(TtcTimerInstPtr);

	return Status;
}



/****************************************************************************/
/**
 *
 * This function setups the interrupt system such that interrupts can occur.
 * This function is application specific since the actual system may or may not
 * have an interrupt controller.  The TTC could be directly connected to a
 * processor without an interrupt controller.  The user should modify this
 * function to fit the application.
 *
 * @param	IntcDeviceID is the unique ID of the interrupt controller
 * @param	IntcInstacePtr is a pointer to the interrupt controller
 *		instance.
 *
 * @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
 *
 * @note		None.
 *
 *****************************************************************************/
static int SetupInterruptSystem(u16 IntcDeviceID,
		XScuGic *IntcInstancePtr)
{
	int Status;
	XScuGic_Config *IntcConfig; /* The configuration parameters of the
					interrupt controller */

	/*
	 * Initialize the interrupt controller driver
	 */
	IntcConfig = XScuGic_LookupConfig(IntcDeviceID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
			IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			IntcInstancePtr);

	/*
	 * Enable interrupts in the ARM
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

/***************************************************************************/
/**
 *
 * This function is the handler which handles the periodic TTC0 interrupt.
 *
 * @param	CallBackRef contains a callback reference from the driver, in
 *		this case it is the instance pointer for the TTC driver.
 *
 * @return	None.
 *
 * @note	None.
 *
 *****************************************************************************/
static void Ttc0IsrHander(void *CallBackRef, u32 StatusEvent)
{

	static u16 phaseAccumulator = 0;
	u16 sinLutIndex = 0;

	// Do ISR stuff here
	if (generateWave == TRUE) {
		phaseAccumulator += phaseIncrement;
		sinLutIndex = (phaseAccumulator >> 10);
		ENHANCEDPWM_mWriteReg(ENHANCED_PWM_BASEADDR , DUTY_CYCLE_OFFSET, sinLut[sinLutIndex]);					// put value into slv_reg1
	}
}

