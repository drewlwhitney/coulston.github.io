--------------------------------------------------------------------
-- Name:	Chris Coulston
-- Date:	Feb 3, 2015
-- Modified Sept 2022
-- File:	acquireToHDMI_Datapath.vhdl
-- HW:      Lab 3
-- Crs:     ECE 383 and EENG 498
--
-- Purp: The complete datapath for the audio O'scope
--
-- Documentation:	No help
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


use work.stopWatch_Package.all;					-- include your library here with added components ac97, ac97cmd
use work.basicBuildingBlocks_package.all;					-- include your library here with added components ac97, ac97cmd


entity stopWatch_datapath is
    PORT ( clk : in  STD_LOGIC;
           resetn : in  STD_LOGIC;
		   cw : in STD_LOGIC_VECTOR(CW_VECTOR_LENGTH -1 downto 0);
		   tenth: out STD_LOGIC;
		   tenHex, unitHex, tenthHex : out STD_LOGIC_VECTOR(6 downto 0));
end stopWatch_datapath;

architecture structure of stopWatch_datapath is

    signal clkCount: STD_LOGIC_VECTOR(N-1 downto 0);
    signal tenthSecondRollingOver, unitSecondRollingOver, tenSecondRollingOver: STD_LOGIC;
    signal tenthDigit, tenthDigitLap, tenthDigitToDisplay: STD_LOGIC_VECTOR(3 downto 0);	 
	signal tenDigit, tenDigitLap, tenDigitToDisplay: STD_LOGIC_VECTOR(3 downto 0); 
	signal unitDigit, unitDigitLap, unitDigitToDisplay: STD_LOGIC_VECTOR(3 downto 0);
	    
begin

    tenthSecondCounter: genericCounter 
        GENERIC MAP (N)
        PORT MAP (  clk => clk,
                    resetn => resetn,
                    c => cw(1 downto 0),
                    d => (others => '0'),
                    q => clkCount);
           
	tenthSecondCompare: genericCompare
        GENERIC MAP(N)
        PORT MAP(   x => clkCount,
                    y => tenthSecondConstant,
                    g => open,
                    l => open,
                    e => tenth);


	tenthSecondsCounter: mod10Counter
	   PORT MAP(   clk => clk, 
	               resetn => resetn, 
	               enb => cw(2),
	               synch => cw(3), 
	               roll => tenthSecondRollingOver, 
	               count => tenthDigit);
	               
    tenthSecondLap: genericRegister
        GENERIC MAP(4)
        PORT MAP(   clk => clk,
                    resetn => resetn,
                    d => tenthDigit, 
                    load => cw(4), 
                    q => tenthDigitLap);
                    
    tenthSecondMux: genericMux2x1
        GENERIC MAP (4)
        PORT MAP(   y1 => tenthDigitLap, 
                    y0 => tenthDigit, 
                    s => cw(5), 
                    f => tenthDigitToDisplay);
                    
                    	
   --sevenSegment         tenthSecondConverter (tenthDigitToDisplay, tenthHex);	

	unitSecondsCounter: mod10Counter
	   PORT MAP(   clk => clk, 
	               resetn => resetn, 
	               enb => tenthSecondRollingOver,
	               synch => cw(3), 
	               roll => unitSecondRollingOver, 
	               count => unitDigit);
	               
    unitSecondLap: genericRegister
        GENERIC MAP(4)
        PORT MAP(   clk => clk,
                    resetn => resetn,
                    d => unitDigit, 
                    load => cw(4), 
                    q => unitDigitLap);
                    
    unitSecondMux: genericMux2x1
        GENERIC MAP (4)
        PORT MAP(   y1 => unitDigitLap, 
                    y0 => unitDigit, 
                    s => cw(5), 
                    f => unitDigitToDisplay);
                    	
   -- sevenSegment         unitSecondConverter (unitDigitToDisplay, unitHex);	

	tenSecondsCounter: mod10Counter
	   PORT MAP(   clk => clk, 
	               resetn => resetn, 
	               enb => unitSecondRollingOver,
	               synch => cw(3), 
	               roll => tenSecondRollingOver, 
	               count => tenDigit);
	               
    tenSecondLap: genericRegister
        GENERIC MAP(4)
        PORT MAP(   clk => clk,
                    resetn => resetn,
                    d => tenDigit, 
                    load => cw(4), 
                    q => tenDigitLap);
                    
    tenSecondMux: genericMux2x1
        GENERIC MAP (4)
        PORT MAP(   y1 => tenDigitLap, 
                    y0 => tenDigit, 
                    s => cw(5), 
                    f => tenDigitToDisplay);

  -- sevenSegment         tenSecondConverter (tenDigitToDisplay, tenHex);	
	


end structure;
