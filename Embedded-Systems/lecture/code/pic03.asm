;--------------------------------------------------------------------
;- Name:	Chris Coulston
;- Date:	Fall 2017
;-
;- Purp:	Third PIC program for the class - duplicates lecture 03
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
	;  C if/then
	;****************************************************
	MOVLW	20
	MOVWF	X8		; M[X8] <- W
	MOVLW	20
	MOVWF	Y8		; M[Y8] <- W	
	
;    if (x8 > y8 + 5) {
;	z8 = 0;
;    } else if (x8 > y8) {
;	z8 = 1;
;    } else if (x8 > y8-5) {
;	z8 = 2;
;    } else {
;	z8 = 3;
;    } // end if

	MOVLW	5
	ADDWF	Y8, W
	CPFSGT	X8	; Skip if F > W	    (X8 > Y8 + 5)
	GOTO	next1
	GOTO	CASE0
	
next1	MOVF	Y8, W
	CPFSGT	X8	; Skip if F > W	    (X8 > Y8)
	GOTO	next2
	GOTO	CASE1
	
next2	MOVLW	5	
	SUBWF	Y8, W
	CPFSGT	X8	; Skip if F > W	    (X8 > Y8 - 5)
	GOTO	next3
	GOTO	CASE2
	
next3	GOTO	CASE3
	
CASE0	MOVLW	0
	MOVWF	Z8
	GOTO	done
	
CASE1	MOVLW	1
	MOVWF	Z8
	GOTO	done
	
CASE2	MOVLW	2
	MOVWF	Z8
	GOTO	done
	
CASE3	MOVLW	3
	MOVWF	Z8
	GOTO	done	
	
;**********************************************************************
; Its a good idea to give the processor something to do when you program
; terminates.  Most of the time we will stick our programs in a while(1)
; loop and make them do some task forever.
;**********************************************************************
done	GOTO	done
	
        END  			; This must be the last line of your program - move at your peril.	
