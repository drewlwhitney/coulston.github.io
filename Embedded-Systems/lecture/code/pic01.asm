;--------------------------------------------------------------------
;- Name:	Chris Coulston
;- Date:	Fall 2017
;- Purp:	A first assembly example for the class
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
;
;   Setup   Windows -> PIC Memories view -> Program Memory
;	Memory: Progam Memory	Format: Code
;	Pull to right of screen	    
;	    
;	Windows -> Debugging -> Variables 
;	    <Enter new watch> => WREG	(case sensitive)	    
;	    <Enter new watch> => 0x60	(memory address)
	    ;
;	Debug -> Pause
;	Debug -> Reset
;	Debug -> Step Into F7	    
;	    
;------------------------------------------------------------------------
	    #include <P18F26K22.INC>

RES_VECT    CODE    0x0000		; Address 0 is the processor reset
	    GOTO    main		; vector go to beginning of program
	    
MAIN_PROG CODE				; allows assembler to place code

main	    MOVLW	0x1		; W <- 1
	    MOVWF	0x60		; M[0x60] <- W
	    MOVLW	0x1		; W <- 1
	    ADDWF	0x60, F		; M[0x60] <- M[0x60] + W

done	    GOTO	done
	    
	    END				; Last line of file

