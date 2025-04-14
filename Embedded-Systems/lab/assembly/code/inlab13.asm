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
; this is basically a #define
;**********************************************************************
SONG_LENGTH	equ	    3   	

;**********************************************************************
; reserve some locations in the data memory for variables
;**********************************************************************	
GPR_VAR		UDATA_ACS
half		RES	    2
note		RES	    1
halfPeriod	RES	    2*SONG_LENGTH
	
;**********************************************************************
; The bottom of the PIC's memory contains reset vector
;**********************************************************************
RES_VECT    CODE    0x0000		; Address 0 is the processor reset vector
	    GOTO    main		; go to beginning of program

MAIN_PROG CODE				; allows the assembler to put code where it wants

;**********************************************************************
; This is main
; Play Hot Cross Buns (B5, A5, G4) on upper button press
;**********************************************************************
main	CALL	INIT_PIC
 
	; Start by initializing the array of timer values	
init	MOVLW	0xE0			; 2^16 - 8,112 = 0x10000 - 0x1FB0 = 0xE057
	MOVWF	halfPeriod+1
	MOVLW	0x57
	MOVWF	halfPeriod+0
	
	MOVLW	0xDC			; 2^16 - 9,088 = 0x10000 - 0x2380 = 0xDC7E
	MOVWF	halfPeriod+3
	MOVLW	0x7E
	MOVWF	halfPeriod+2
	
	MOVLW	0xD8			; 2^16 - 10,208 = 0x10000 - 0x27E0 = 0xD817
	MOVWF	halfPeriod+5
	MOVLW	0x17
	MOVWF	halfPeriod+4
	
	; now the infinte loop, start by initalizinty 
begin	CLRF	FSR0H			; Array is in bank 0
	MOVLW	halfPeriod
	MOVWF	FSR0L
	
	MOVLW	SONG_LENGTH
	MOVWF	note, ACCESS
	
next	MOVF	INDF0, W
	MOVWF	half
	INCF	FSR0L	
	MOVF	INDF0, W
	MOVWF	half+1
	INCF	FSR0L

tickle	BTFSC	PORTA, 2, ACCESS
	GOTO	nextNote
	
	BTG	LATB, 5, ACCESS
	MOVF	half+1, W
	MOVWF	TMR1H, ACCESS
	MOVF	half, W
	MOVWF	TMR1L, ACCESS
	
	BCF	PIR1, TMR1IF, ACCESS
while1	BTFSS	PIR1, TMR1IF, ACCESS
	GOTO	while1
	
	GOTO	tickle
	
nextNote
	BTFSC	PORTA, 2, ACCESS
	GOTO	nextNote

	DECFSZ	note	
	GOTO	next
	
	GOTO	begin

;**********************************************************************
; Functions in assembly are straightforward
;**********************************************************************	
INIT_PIC    
	MOVLB	0x0F	
	
	BSF	OSCCON, IRCF2, BANKED	; Setup a 64Mhz internal oscillator
	BSF	OSCCON, IRCF1, BANKED
	BSF	OSCCON, IRCF0, BANKED
	BSF	OSCTUNE, PLLEN, BANKED
	
	CLRF	ANSELA, BANKED		; make port A digital	 	
	CLRF	ANSELB, BANKED		; make port B digital
	BSF	TRISA, 2, ACCESS	; Make RA2 an input
	BCF	TRISB, 5, ACCESS	; Make RB5 an output
	CLRF	T1CON, ACCESS
	BSF	T1CON, TMR1ON, ACCESS	
	
	MOVLB	0x00	
	RETURN
	
        END  	; This must be the last line of your program - move at your peril.	
