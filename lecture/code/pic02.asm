;--------------------------------------------------------------------
;- Name:	Chris Coulston
;- Date:	Fall 2017
;-
;- Purp:	Second PIC program for the class - duplicates lecture 02
;- Assisted:    The entire class of EENG 383
;- Assisted by: Microchips 18F26K22 Tech Docs (page 373)
;-
;- Academic Integrity Statement: I certify that, while others may have
;- assisted me in brain storming, debugging and validating this program,
;- the program itself is my own work. I understand that submitting code
;- which is the work of other individuals is a violation of the course
;- Academic Integrity Policy and may result in a zero credit for the
;- assignment, or course failure and a report to the Academic Dishonesty
;- Board. I also understand that if I knowingly give my original work to
;- another individual that it could also result in a zero credit for the
;- assignment, or course failure and a report to the Academic Dishonesty
;- Board.
;   Setup   Windows -> PIC Memories view -> Program Memory
;	Memory: Progam Memory	Format: Code
;	Pull to right of screen	    
;
;	Windows -> PIC Memories view -> File Registers
;	    Pull to right of screen
;	    Memory: File Regstiers	Format: Symbol	    
;	    
;	Windows -> Debugging -> Variables 
;	    <Enter new watch> => WREG	(case sensitive)	
;	    <Enter new watch> => STATUS	(case sensitive)	
;	    <Enter new watch> => X8	(memory address)
;
;	Debug -> Pause
;	Debug -> Reset
;	Debug -> Step Into F7	    
;	
;------------------------------------------------------------------------
    #include        <p18f26k22.inc>	; processor specific definitions
    radix           decimal		; base 10 as the default


GPR_VAR        UDATA_ACS
X8	       RES        1      ; User variable "1" is number of bytes
Y8	       RES        1      ; Another 8-bit variable 
Z8	       RES        1      ; One mroe for good measure

;**********************************************************************
; The bottom of the PIC's memory contains reset vector
;**********************************************************************
RES_VECT    CODE    0x0000      ; Address 0 is the processor reset vector
	    GOTO    main        ; go to beginning of program

MAIN_PROG CODE			; allows the assembler to put code where it wants
	
main	NOP
 
	;****************************************************
	;  C arithmetic operations - Lecture 02
	;****************************************************
	MOVLW	25
	MOVWF	X8		; M[X8] <- 25	
	MOVLW	15
	MOVWF	Y8		; M[X8] <- 25	
	MOVLW	10
	MOVWF	Z8		; M[X8] <- 25

	;  z = x + y;		yields z = 40
	MOVF	Y8, W		; W <- M[Y8]
	ADDWF	X8, W		; W <- M[X8] + W
	MOVWF	Z8		; M[Z8] <- W
	
	; z = x - y;		yields z = 10
	MOVF	Y8, W		; W <- M[Y8]
	SUBWF	X8, W		; W <- M[X8] - W
	MOVWF	Z8		; M[Z8] <- W
	
	; z += x;		yields z = 35
	MOVF	X8, W		; W <- M[X8]
	ADDWF	Z8, F		; M[Z8] <- M[Z8] + W		
	
	; z = x++;		yields z = 25, x = 26
	MOVF	X8, W
	MOVWF	Z8
	INCF	X8
    
	; z = y - x;	overflow z = 245 (15 - 26 = -11 rolls to 256 - 11 = 245)
	MOVF	X8, W
	SUBWF	Y8, W
	MOVWF	Z8

	;****************************************************
	;  C logic operations - Lecture 02
	;****************************************************	
	MOVLW	B'10101100'
	MOVWF	X8	
	MOVLW	B'00110101'
	MOVWF	Y8
	CLRF	Z8
	
	;  z = x & 0x0F;	z = 0b0000 1100
	MOVLW	0x0F		; W <- 0x0F
	ANDWF	X8, W		; W <- W & M[X8]	
	MOVWF	Z8		; M[Z8] <- W

	;  z = x | y;		z = 0b101 1110
	MOVF	Y8,W		; W <- M[Y8]		
	IORWF	X8, W		; W <- W | M[X8]	
	MOVWF	Z8		; M[Z8] <- W			
	
	; z = x ^ 0x0F;		z = 0b1010 0011
	MOVLW	0x0F		; W <- 0x0F
	XORWF	X8, W		; W <- W ^ M[X8]	
	MOVWF	Z8		; M[Z8] <- W	    
	
	; z = ~x;		z = 0b0101 0011
	COMF	X8, W		; W <- ~M[X8]
	MOVWF	Z8		; M[Z8] <- W

	; z = x<<1;		z = 0b0101 1000
	BCF	STATUS, C	; clear carry bit
	RLCF	X8, W		; rotate X8 left with carry bit into LSB into W
	MOVWF	Z8		; M[Z8] <- W
 
	; z = x>>3;		z = 0b0001 0101	
	MOVF	X8, W
	MOVWF	Z8		; M[Z8] <- M[X8]
	BCF	STATUS, C	; clear carry
	RRCF	Z8		; rotate right with 0 into MSB (logical SR)
	BCF	STATUS, C	; again
	RRCF	Z8
	BCF	STATUS, C	; again
	RRCF	Z8
	
;**********************************************************************
; Its a good idea to give the processor something to do when you program
; terminates.  Most of the time we will stick our programs in a while(1)
; loop and make them do some task forever.
;**********************************************************************
done	GOTO	done
	
        END  			; This must be the last line of your program - move at your peril.	
