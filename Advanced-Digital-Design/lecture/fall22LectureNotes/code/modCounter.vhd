----------------------------------------------
-- Name:	Chris Coulston
-- Date:	Jan 10, 2015
-- Modified: Aug 29, 2022
-- File:	lec04.vhdl
-- HW:	Lecture 4
-- Crs:	ECE 383 and EENG 498
--
-- Purp:	Demos the use of processes
-- Note:    Removed unsigned and referenced to NUMERIC library
--
-- Documentation:	I pulled some information from chapter .
--
-- Academic Integrity Statement: I certify that, while others may have 
-- assisted me in brain storming, debugging and validating this program, 
-- the program itself is my own work. I understand that submitting code 
-- which is the work of other individuals is a violation of the honor   
-- code.  I also understand that if I knowingly give my original work to 
-- another individual is also a violation of the honor code. 
------------------------------------------------------------------------- 
library IEEE;		
USE IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity modCounter is
	PORT(	clk: in  STD_LOGIC;
			reset : in  STD_LOGIC;
			ctrl: in STD_LOGIC_VECTOR(1 downto 0);
			D: in STD_LOGIC_VECTOR (3 downto 0);
			Q: out STD_LOGIC_VECTOR (3 downto 0));
end modCounter;

architecture behavior of modCounter is
	
	signal rollSynch, rollCombo: STD_LOGIC;
	signal processQ: STD_LOGIC_VECTOR (3 downto 0);

begin
	
	
	-----------------------------------------------------------------------------
	--		ctrl
	--		00			hold
	--		01			count up mod 10
	--		10			load D
	--		11			synch reset
	-----------------------------------------------------------------------------
	process(clk)
	begin
		if (rising_edge(clk)) then
			if (reset = '0') then
				processQ <= (others => '0');
				rollSynch <= '0';
			elsif ((processQ < "1001") and (ctrl = "01")) then
				processQ <= processQ + 1;
				rollSynch <= '0';
			elsif ((processQ = "1001") and (ctrl = "01")) then
				processQ <= (others => '0');
				rollSynch <= '1';
			elsif (ctrl = "10") then
				processQ <= D;
			elsif (ctrl = "11") then
				processQ <= (others => '0');
			end if;
		end if;
	end process;
 
	-- CSA
	rollCombo  <= '1' when (processQ = 9) else '0';
	Q <= processQ;
	
end behavior;
