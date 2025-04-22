--------------------------------------------------------------------------------
-- Name:	Chris Coulston
-- Date:	Jan 10, 2015
-- Modified: Aug 29, 2022
-- File:	modCounter_tb.vhdl
-- HW:	Lecture 4
-- Crs:	ECE 383 and EENG 498
--
-- Purp:	testbench for modCounter.vhdl
-- Note:    Removed unsigned and referenced to NUMERIC library
--
-- Academic Integrity Statement: I certify that, while others may have 
-- assisted me in brain storming, debugging and validating this program, 
-- the program itself is my own work. I understand that submitting code 
-- which is the work of other individuals is a violation of the honor   
-- code.  I also understand that if I knowingly give my original work to 
-- another individual is also a violation of the honor code. 
--------------------------------------------------------------------------------
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;


ENTITY modCounter_tb IS
END modCounter_tb;
 
ARCHITECTURE behavior OF modCounter_tb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT modCounter
	Port(	clk: in  STD_LOGIC;
			reset : in  STD_LOGIC;
			ctrl: in STD_LOGIC_VECTOR(1 downto 0);
			D: in STD_LOGIC_VECTOR (3 downto 0);
			Q: out STD_LOGIC_VECTOR (3 downto 0));
    END COMPONENT;
    

   --Inputs
   signal clk : STD_LOGIC := '0';
   signal reset : STD_LOGIC := '0';
	signal ctrl : STD_LOGIC_VECTOR(1 downto 0) := (others => '0');
   signal D : STD_LOGIC_VECTOR(3 downto 0) := (others => '0');

 	--Outputs
   signal Q : STD_LOGIC_VECTOR(3 downto 0);

   -- Clock period definitions
   constant clk_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: modCounter PORT MAP (
          clk => clk,
          reset => reset,
			 ctrl => ctrl,
          D => D,
          Q => Q
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '1';
		wait for clk_period/2;
		clk <= '0';
		wait for clk_period/2;
   end process;
 
 	-----------------------------------------------------------------------------
	--		ctrl
	--		00			hold
	--		01			count up mod 10
	--		10			load D
	--		11			synch reset
	-----------------------------------------------------------------------------
	ctrl <= "10", "01" after 15ns, "11" after 60ns, "01" after 70ns;
	D <= "0011";
	reset <= '0', '1' after 5ns;

END;
