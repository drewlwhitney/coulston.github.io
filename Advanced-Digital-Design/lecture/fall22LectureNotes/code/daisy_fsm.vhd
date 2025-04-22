--------------------------------------------------------------------
-- Name:	Chris Coulston
-- Date:	Jan 15, 2015
-- File:	lec09.vhdl
-- HW:	Lecture 9
--	Crs:	ECE 383
--
-- Purp:	Demonstrate finite state machine construction with DAISY
--
-- Documentation:	No help, though I used an example from my Digital
--						Design text book.
--
-- Academic Integrity Statement: I certify that, while others may have 
-- assisted me in brain storming, debugging and validating this program, 
-- the program itself is my own work. I understand that submitting code 
-- which is the work of other individuals is a violation of the honor   
-- code.  I also understand that if I knowingly give my original work to 
-- another individual is also a violation of the honor code. 
------------------------------------------------------------------------- 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity daisy_fsm is
	Port(	clk: in  STD_LOGIC;
			resetn : in  STD_LOGIC;
			sw: in STD_LOGIC_VECTOR(2 downto 0);
			cw: out STD_LOGIC_VECTOR(4 downto 0));
end daisy_fsm;



architecture behavior of daisy_fsm is

	type state_type is (WAIT_ENTER_STATE, WAIT_READ_STATE, SET30_STATE, WAIT_LEAVE_STATE, SET3_STATE, GOOSE_STATE);
	signal state: state_type;
	
	constant rfid: integer := 2;		-- helps keep status bits straight
	constant cow: integer := 1;
	constant timer: integer := 0;
	
begin
	
	------------------------------------------------------------------------------
	-- 		MEMORY INPUT EQUATIONS
	-- 
	--		bit 2				bit 1				bit 0
	--		RFID scan		Cow Present		Timer Status
	--		1-cow checked	1-cow				0 - running
	--		0-waiting		0-no cow			1 - times up
	------------------------------------------------------------------------------	
    state_process: process(clk)
	 begin
		if (rising_edge(clk)) then
			if (resetn = '0') then 
				state <= WAIT_ENTER_STATE;
			else
				case state is
					when WAIT_ENTER_STATE =>
						if (sw(cow) = '1') then state <= WAIT_READ_STATE; end if;
					when WAIT_READ_STATE =>
						if (sw(rfid) = '1') then state <= SET30_STATE; end if;
					when SET30_STATE =>
						state <= WAIT_LEAVE_STATE;					
					when WAIT_LEAVE_STATE =>
						if 	(sw(cow) = '0') then state <= WAIT_ENTER_STATE;
						elsif (sw(timer) = '1' and sw(cow) = '1') then state <= SET3_STATE; end if;
					when SET3_STATE =>
						state <= GOOSE_STATE;
					when GOOSE_STATE =>
						if (sw(timer) = '1') then state <= SET30_STATE; end if;					
				end case;
			end if;
		end if;
	end process;


	------------------------------------------------------------------------------
	--			OUTPUT EQUATIONS
	--	
	--		bit 4			bit 3			bit 2,1					bit 0
	--		Gate1			Gate2			Timer						Control Air
	--		1-gate up	1-gate up	00 Stop timer			0 - closed
	--		0-gate down	0-gate down	01 Set to 30 secs		1 - open
	--										10 Set to 3 secs	
	--										11 Run timer	
	------------------------------------------------------------------------------	
	
	output_process: process (state)
	begin
		case state is
            when WAIT_ENTER_STATE   =>    cw <= '1' & '0' & "00" & '0';
            when WAIT_READ_STATE    =>    cw <= '0' & '0' & "00" & '0';
            when SET30_STATE        =>    cw <= '0' & '1' & "01" & '0';
            when WAIT_LEAVE_STATE   =>    cw <= '0' & '1' & "00" & '0';
            when SET3_STATE         =>    cw <= '0' & '1' & "10" & '0';
            when GOOSE_STATE        =>    cw <= '0' & '1' & "11" & '1';
            
		end case;
	end process;
	

end behavior;	