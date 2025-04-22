library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
--use work.simpleDualPortRamWithCounters_Package.all;

entity simpleDualPortRamWithCounter_tb is
end simpleDualPortRamWithCounter_tb;

architecture Behavioral of simpleDualPortRamWithCounter_tb is

CONSTANT DATA_WORD_WIDTH : NATURAL := 5; 

component simpleDualPortRamWithCounters is
    PORT (  clk : in  STD_LOGIC;
            resetn : in  STD_LOGIC;
            cw: in STD_LOGIC_VECTOR(2 downto 0));
end component;

    signal clk_t, resetn_t: STD_LOGIC;
    signal cw_t : STD_LOGIC_VECTOR(2 downto 0);
    
begin

    UUT: simpleDualPortRamWithCounters
        PORT MAP   (    clk => clk_t,
                        resetn => resetn_t,
                        cw => cw_t);
                        
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
	-- Holds reset for 10nS, changes on clk negative edge
	-----------------------------------------
	resetn_t <= '0', '1' after 10nS;                        
	
	-- cw <= ena & wea & enb;
    
	cw_t <= "000", "001" after 30nS, "101" after 50ns, "111" after 70ns;
	
end Behavioral;
