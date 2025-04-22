----------------------------------------------------------------------------------
-- Comments start with two dashes
-- You should always have the following
--		lines in all of your code
----------------------------------------------------------------------------------
-- Name:	Prof Chris Coulston
-- Date:	Fall 2022
-- Course: 	EENG 498 
-- File: 	mod10Counter
-- Purp:	Classic mod 10 counter from EENg 294 Lab #8
--
-- Doc:	<list the names of the people who you helped>
-- 		<list the names of the people who assisted you>
-- 	
-- Academic Integrity Statement: I certify that, while others may have 
-- assisted me in brain storming, debugging and validating this program, 
-- the program itself is my own work. I understand that submitting code 
-- which is the work of other individuals is a violation of the honor   
-- code.  I also understand that if I knowingly give my original work to 
-- another individual is also a violation of the honor code. 
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.basicBuildingBlocks.ALL;

entity mod10Counter is
    Port ( clk : in STD_LOGIC;
        reset : in STD_LOGIC;
         enb : ???;
         synch : ???;
         roll : ???;
         count : ???;
end mod10Counter;

architecture Behavioral of mod10Counter is

	component genericAdder is
	    generic(N: integer := 4);
	    port(a,b: in std_logic_vector(N-1 downto 0);
		 sum: out std_logic_vector(N-1 downto 0));
	end component;

	component genericCompare is
	    generic(N: integer := 4);
	    port(x,y : in std_logic_vector(N-1 downto 0);
		 g,l,e: out std_logic);
	end component;

	component genericMux2x1 is
	    	generic(N: integer := 4);
    		port(y1,y0: in std_logic_vector(N-1 downto 0);
			 s: in std_logic;
			 f: out std_logic_vector(N-1 downto 0) );
	end component;


	component genericRegister is
        	generic(N: integer := 4);
        	port (  clk, reset,load: in std_logic;
         	       d: in  std_logic_vector(N-1 downto 0);
          	      q: out std_logic_vector(N-1 downto 0) );
	end component;
	

    signal countPlusOne : STD_LOGIC_VECTOR(3 downto 0);
    signal nextCount : STD_LOGIC_VECTOR(3 downto 0);
    signal currentCount : STD_LOGIC_VECTOR(3 downto 0);
    signal inputCtrlVec: STD_LOGIC_VECTOR(2 downto 0);
    signal outputCtrlVec: STD_LOGIC_VECTOR(2 downto 0);
    signal equalNine, muxSel, regEnb: STD_LOGIC; 

begin

    add_inst : genericAdder
        GENERIC MAP(???)
        PORT MAP(???);

    comp_inst : genericCompare
        GENERIC MAP(???);
        PORT MAP(???);

    mux_inst: genericMux2x1
        GENERIC MAP(???)
        PORT MAP(???);

    reg_inst: genericRegister
        GENERIC MAP(???)
        PORT MAP(???);

    count <= currentCount;

    inputCtrlVec <= enb & synch & equalNine;
    
    muxSel <= outputCtrlVec(2);
    regEnb <= outputCtrlVec(1);
    roll <= outputCtrlVec(0);
    
    outputCtrlVec <= "???" when inputCtrlVec = "000" else
                     "???" when inputCtrlVec = "001" else
                     "???" when inputCtrlVec = "010" else
                     "???" when inputCtrlVec = "011" else
                     "???" when inputCtrlVec = "100" else
                     "???" when inputCtrlVec = "101" else
                     "???" when inputCtrlVec = "110" else
                     "???";

end Behavioral;

