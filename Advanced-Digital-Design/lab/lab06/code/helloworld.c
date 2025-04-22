#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "my_counter_ip.h"
#include "xuartps_hw.h"

#define COUNTER_BASEADDR	XPAR_MY_COUNTER_IP_0_S00_AXI_BASEADDR
#define USART_BASEADDR		XPAR_PS7_UART_1_BASEADDR

#define COUNTER_DREG_OFFSET		0
#define COUNTER_CTRL_OFFSET		4
#define COUNTER_QREG_OFFSET		8


// The following constants define the Counter commands
#define COUNTER_HOLD_CMD		0x00		// The control bits are defined in the VHDL
#define	COUNTER_LOAD_CMD		0x01		// code contained in lec19.vhdl.
#define	COUNTER_COUNT_CMD		0x02		// They are added here to centralize the bit values in
#define COUNTER_RESET_CMD		0x03		// a single place.


int main()
{
	unsigned char c;

	init_platform();

	printf("Welcome to the Counter IP\n\r");

	while(1) {

    	c=XUartPs_RecvByte(USART_BASEADDR);

    	switch(c) {

    		/*-------------------------------------------------
    		 * Reply with the help menu
    		 *-------------------------------------------------
			 */
    		case '?':
    			printf("--------------------------\r\n");
    			printf("PL LED1 = MSB	PL LED4 = LSB\r\n\n");
    			printf("Press PL Key4	counter_Q[3:0]\r\n");
    			printf("Release PL Key4	counter_Q[7:4]\r\n\n");
    			printf("Counter D: 	0x%04x \r\n", MY_COUNTER_IP_mReadReg(COUNTER_BASEADDR , COUNTER_DREG_OFFSET));
    			printf("Counter Ctrl: 	0x%04x \r\n", MY_COUNTER_IP_mReadReg(COUNTER_BASEADDR, COUNTER_CTRL_OFFSET));
    			printf("Counter Q: 	0x%04x \r\n",MY_COUNTER_IP_mReadReg(COUNTER_BASEADDR, COUNTER_QREG_OFFSET));
    			printf("--------------------------\r\n");
    			printf("?: help menu\r\n");
    			printf("c: count up once \r\n");
    			printf("C: Count up continuously\r\n");
    			printf("l: Load counter\r\n");
    			printf("r: Reset counter\r\n");
    			printf("f: Flush terminal\r\n");
    			break;

			/*-------------------------------------------------
			 * Tell the counter to count up once
			 *-------------------------------------------------
			 */
    		case 'c':
    			MY_COUNTER_IP_mWriteReg(COUNTER_BASEADDR, COUNTER_CTRL_OFFSET,COUNTER_COUNT_CMD);
    			MY_COUNTER_IP_mWriteReg(COUNTER_BASEADDR, COUNTER_CTRL_OFFSET,COUNTER_HOLD_CMD);
    			printf("count up\r\n");
    			break;

    			/*-------------------------------------------------
    			 * Tell the counter to count up until key press
    			 *-------------------------------------------------
    			 */
    		case 'C':
    			printf("Press any key to stop counting\r\n");
    			MY_COUNTER_IP_mWriteReg(COUNTER_BASEADDR, COUNTER_CTRL_OFFSET,COUNTER_COUNT_CMD);
    			c=XUartPs_RecvByte(USART_BASEADDR);
    			MY_COUNTER_IP_mWriteReg(COUNTER_BASEADDR, COUNTER_CTRL_OFFSET,COUNTER_HOLD_CMD);
    			break;

			/*-------------------------------------------------
			 * Tell the counter to load a one digit value
			 *-------------------------------------------------
			 */
        	case 'l':
        		printf("Enter a 0-9 value to store in the counter: \r\n");
            	c=XUartPs_RecvByte(USART_BASEADDR);
            	printf("%c\r\n",c);
            	MY_COUNTER_IP_mWriteReg(COUNTER_BASEADDR, COUNTER_DREG_OFFSET,c-'0');					// put value into slv_reg1
            	MY_COUNTER_IP_mWriteReg(COUNTER_BASEADDR, COUNTER_CTRL_OFFSET,COUNTER_LOAD_CMD);				// load command
            	MY_COUNTER_IP_mWriteReg(COUNTER_BASEADDR, COUNTER_CTRL_OFFSET,COUNTER_HOLD_CMD);
    			printf("loaded: %c\r\n",c);
        		break;

			/*-------------------------------------------------
			 * Reset the counter
			 *-------------------------------------------------
			 */
            case 'r':
            	MY_COUNTER_IP_mWriteReg(COUNTER_BASEADDR, COUNTER_CTRL_OFFSET,COUNTER_RESET_CMD);				// reset command
            	MY_COUNTER_IP_mWriteReg(COUNTER_BASEADDR, COUNTER_CTRL_OFFSET,COUNTER_HOLD_CMD);
            	printf("reset\r\n");
            	break;

			/*-------------------------------------------------
			 * Clear the terminal window
			 *-------------------------------------------------
			 */
            case 'f':
            	for (c=0; c<40; c++) printf("\r\n");
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
