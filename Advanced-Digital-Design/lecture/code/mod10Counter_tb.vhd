----------------------------------------------------------------------
-- Name:	Prof Jeff Falkinburg
-- Date:	Spring 2020
-- Course: 	CSCE 436
-- File: 	lec01_tb.vhd
-- HW:		Lecture 1
-- Pupr:	Test bench for Lecture 1 - majority circuit  
--
-- Documentation:	Based this off a some previous labs and consulted 
--					page 36 of our text for some useful syntax.	
-- 	
-- Academic Integrity Statement: I certify that, while others may have 
-- assisted me in brain storming, debugging and validating this program, 
-- the program itself is my own work. I understand that submitting code 
-- which is the work of other individuals is a violation of the honor   
-- code.  I also understand that if I knowingly give my original work to 
-- another individual is also a violation of the honor code. 
----------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;

entity mod10Counter_tb is
end entity mod10Counter_tb;

architecture behavior of mod10Counter_tb is

    ----------------------------------------------------------------------
    -- These signal names must match the names of the I/O markers
    ----------------------------------------------------------------------
    component mod10Counter is
        Port ( clk : in STD_LOGIC;
             resetn : in STD_LOGIC;
             enb : in STD_LOGIC;
             synch : in STD_LOGIC;
             roll : out STD_LOGIC;
             count : out STD_LOGIC_VECTOR (3 downto 0));
    end component;

    signal clk_t, resetn_t, enb_t, synch_t, roll_t : STD_LOGIC;
    signal count_t: STD_LOGIC_VECTOR(3 downto 0);


begin

    ----------------------------------------------------------------------
    -- Create an instance of your majority
    ----------------------------------------------------------------------	
    UUT : mod10Counter  port map (clk => clk_t, resetn => resetn_t, enb => enb_t, synch => synch_t, roll => roll_t, count => count_t);
    
	-----------------------------------------
	-- Generate a clock with period of 10nS
	-----------------------------------------
	clk_process: process
	begin
		clk_t <= '0';
		wait for 5nS;
		clk_t <= '1';
		wait for 5nS;
	end process;
	
	-----------------------------------------
	-- Generate an asynch reset for 5nS
	-----------------------------------------
	resetn_t <= '0', '1' after 10nS;	
	
	-----------------------------------------
	-- Assert inputs and check outputs
	-----------------------------------------
	test_process: process
	begin
	
		enb_t <= '1';
		synch_t <= '0';
		wait for 100nS;

	    enb_t <= '0';
		synch_t <= '0';
		wait for 10nS;		

        enb_t <= '1';
		synch_t <= '0';		
		wait for 20nS;
		
	    enb_t <= '0';
		synch_t <= '0';
		wait for 10nS;		
		
		enb_t <= '0';
		synch_t <= '1';
		wait for 10nS;
		
		enb_t <= '1';
		synch_t <= '0';
		
	end process;
	
end architecture behavior;
