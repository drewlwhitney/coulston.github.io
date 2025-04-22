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

entity majority_tb is 
end entity majority_tb;

architecture behavior of majority_tb is

	----------------------------------------------------------------------
	-- These signal names must match the names of the I/O markers
	----------------------------------------------------------------------
	component majority is
        port(	a, b, c:	in std_logic; 
					f:   		out std_logic);
	end component;
  
	signal a_t, b_t, c_t, f_t : std_logic;
	  
	CONSTANT TEST_ELEMENTS:integer:=8;
	SUBTYPE INPUT is std_logic_vector(2 downto 0);
	TYPE TEST_INPUT_VECTOR is array (1 to TEST_ELEMENTS) of INPUT;
	SIGNAL TEST_IN: TEST_INPUT_VECTOR := (	"000", "001", "010", "011", "100", "101", "110", "111");

	TYPE TEST_OUTPUT_VECTOR is array (1 to TEST_ELEMENTS) of std_logic;
	SIGNAL TEST_OUT: TEST_OUTPUT_VECTOR := ('0', '0', '0', '1', '0', '1', '1', '1');

	SIGNAL i : integer;		

begin

	----------------------------------------------------------------------
	-- Create an instance of your majority
	----------------------------------------------------------------------	
    UUT : entity work.majority(behavior)  port map (a => a_t, b => b_t, c => c_t, f => f_t);
    --UUT : entity work.majority(structure)  port map (a => a_t, b => b_t, c => c_t, f => f_t);
    
	tb : PROCESS
	BEGIN
	for i in 1 to TEST_ELEMENTS loop
		-----------------------------------------
		-- Parse out the bits of the test_vector
		-----------------------------------------
		a_t <= test_in(i)(2);
		b_t <= test_in(i)(1);
		c_t <= test_in(i)(0);
		
		wait for 10 ns; 
		assert f_t = test_out(i)
 				report "Error with input " & integer'image(i) & " in majority circuit "
				severity FAILURE;
				
	end loop;
	
	---------------------------
	-- Halt the simulator
	---------------------------
    std.env.finish;
			
	END PROCESS tb;

end architecture behavior;
