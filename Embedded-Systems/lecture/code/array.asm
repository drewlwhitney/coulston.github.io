;--------------------------------------------------------------------
;- Name:	Chris Coulston
;- Date:	Fall 2017
;-
;- Purp:	assembly version of inlab03.c
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
;------------------------------------------------------------------------
    #include        <p18f26k22.inc>	; processor specific definitions
    radix           decimal		; base 10 as the default
    
    ; Configure the PIC - look up in the PIC18F26K22 Users Manual
    CONFIG FOSC = INTIO67		; Internal oscillator block, port function on RA6 and RA7
    CONFIG PLLCFG = ON			; Oscillator multiplied by 4
    CONFIG PRICLKEN = ON		; Primary clock can be disabled by software
    
;**********************************************************************
; These are constants and pointers used in the program.
;**********************************************************************
LOOP_CNT	EQU	0x08		; The number of loop iterations 

GPR_VAR		UDATA_ACS
A_VAR		RES	    1
B_VAR		RES	    1
ARRAY		RES	    LOOP_CNT
	
;**********************************************************************
; The bottom of the PIC's memory contains reset vector
;**********************************************************************
RES_VECT    CODE    0x0000		; Address 0 is the processor reset vector
	GOTO    main                ; go to beginning of program

MAIN_PROG   CODE

;------------------------------------------------------------------------ 
; Call the initilization
;------------------------------------------------------------------------ 
main	    CALL INIT
   
;------------------------------------------------------------------------ 
; XOR SWAP algorithm = swap two variables without using a third
;------------------------------------------------------------------------ 
	MOVLW	0xCC		; Move a constant into the memory location
	MOVWF	A_VAR		; pointed at by A_VAR
	MOVLW	0x55		; Move a constant into the memory location
	MOVWF	B_VAR		; pointed at by B_VAR

	MOVF	B_VAR, W
	XORWF	A_VAR, F    ; complete the first XOR
	MOVF	A_VAR, W
	XORWF	B_VAR, F    ; complete the second XOR
	MOVF	B_VAR, W
	XORWF	A_VAR, F    ; complete the third XOR

;**********************************************************************
; This will create a loop that iterates 0x08 times.  Assembly loops
; are general count-down loops.  The DFSZ instruction makes this 
; the more natural construct in the PIC assembly.
;**********************************************************************
	CLRF	B_VAR		; Start the number of iterations at zero - obviously
	MOVLW	LOOP_CNT	; load the length of a loop into the W register
	MOVWF	A_VAR		; and then into the file register pointed to by CNT

loop
    INCF	B_VAR		; this counts as one more loop iteration
	DECFSZ	A_VAR		; decrement the loop counter and skip
	GOTO	loop		; this goto instruction when its zero

;------------------------------------------------------------------------
; CPFSEQ F_REG		skip next instruction if F_REG = W
; CPFSGT F_REG		skip next instruction if F_REG > W (unsigned compare)
; CPFSLT F_REG		skip next instruction if F_REG < W (unsigned compare)
;------------------------------------------------------------------------ 
	MOVLW	0x40		; the constant in A_VAR will not change
	MOVWF	A_VAR		; for the following three examples

	MOVLW	0x50		; In the first case make W larger than A
	CPFSGT	A_VAR		; skip if A_VAR > W

p1	MOVLW	0x40
	CPFSEQ	A_VAR		; skip if A_VAR = W

p2	MOVLW	0x30
	CPFSLT	A_VAR		; skip if A_VAR < W

p3	NOP

;------------------------------------------------------------------------
;	ARRAY
;------------------------------------------------------------------------ 
	MOVLW	LOOP_CNT	; look at this number of locations	
	MOVWF	A_VAR		; and put the counter in this data location
	MOVLW	ARRAY		; move the initial array address into FSR0
	CLRF	FSR0H		; Since there are 4k data address, the data
	MOVWF	FSR0L		; address is 12-bits (store in a 16-bit "register"
	
write	
    MOVF	A_VAR, W    ; move the count index into the W register
	MOVWF	INDF0		; then write this to RAM[FSR0]
	INCF	FSR0L		; point to the next RAM address then loop

	DECFSZ	A_VAR, F    ; decrement COUNT and skip the next instruction
	GOTO	write		; if COUNT==0 after decrement

;------------------------------------------------------------------------
;   Input/Output
;------------------------------------------------------------------------ 
	MOVLB	0x0F

CkRC4	
    BTFSC	PORTC, 4, BANKED
	GOTO	SetRC1
	GOTO	ClrRC1

SetRC1	
    BSF     PORTC, 1, BANKED
	GOTO	CkRC4

ClrRC1	
    BCF     PORTC, 1, BANKED
	GOTO	CkRC4

;**********************************************************************
; Its a good idea to give the processor something to do when you program
; terminates.  Most of the time we will stick our programs in a while(1)
; loop and make them do some task forever.
;**********************************************************************
done	GOTO	done
		
;**********************************************************************
; INIT
; RC1 = Digital output
; RC4 = Digital input
;**********************************************************************
INIT	
    MOVLB   0x0F                ; point to the top of memory where these SFR's live.  Note BSR = 0xFE0
	BCF     TRISC, 1, BANKED    ; clear TRIS so RC1 is an output.  Note TRISC = 0xF94            
	BCF     PORTC, 1, BANKED    ; set output to 0 - no special reason.  Note TRISC = 0xF82
    BSF     TRISC, 4, BANKED    ; set TRIS so RC4 is an input.  Note TRISC = 0xF94                        
	CLRF	ANSELC, BANKED      ; make all PORTC pins digital inputs.  Note ANSELC = 0xF3A
	MOVLB   0x00                ; return the BSR back to the bottom of data memory

	RETURN              ; every subroutine ends with a return instruction

    END                 ; This must be the last line of your program - move at your peril.							
