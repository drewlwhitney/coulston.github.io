----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.basicBuildingBlocks_package.all;

entity enhancedPwm is
    PORT ( clk : in  STD_LOGIC;
           resetn : in  STD_LOGIC;
           enb: in STD_LOGIC;
           dutyCycle: in STD_LOGIC_VECTOR(8 downto 0);
           pwmCount: out STD_LOGIC_VECTOR(7 downto 0);
           rollOver: out STD_LOGIC;
           pwmSignal: out STD_LOGIC);		   
end enhancedPwm;

architecture Behavioral of enhancedPwm is

    signal cw: STD_LOGIC_VECTOR(1 downto 0);
    signal pwmCount_int: STD_LOGIC_VECTOR(7 downto 0);
    signal pwmCount9bit_int: STD_LOGIC_VECTOR(8 downto 0);
    signal dutyCycle_int: STD_LOGIC_VECTOR(8 downto 0);
    signal counterEqualPeriod, dutyCycleGreaterCounter: STD_LOGIC;
    
    
begin


    counter_inst: genericCounter
    GENERIC MAP(N => 8)
    PORT MAP(   clk => clk,
                resetn => resetn,
                c => cw,
                d => x"00",
                q => pwmCount_int);

    rollCompare: genericCompare
    GENERIC MAP(N => 8)
    PORT MAP(   x => pwmCount_int, y => x"FF", g => open, l => open, e => counterEqualPeriod);
            
	process(clk)  
	begin
		if (rising_edge(clk)) then
			if (resetn = '0') then 
                dutyCycle_int <= (others => '0');        
            else 
                if (counterEqualPeriod = '1') then dutyCycle_int <= dutyCycle; end if;                  
            end if;
        end if;        
    end process;
                
    pwmCount9bit_int <=  '0' & pwmCount_int;           
    dutyCompare: genericCompare
    GENERIC MAP(N => 9)
    PORT MAP(   x => dutyCycle_int, y => pwmCount9bit_int, g => dutyCycleGreaterCounter, l => open, e => open);

    cw <=   "00" when (enb = '0') else 
            "10" when ( (enb='1') and (counterEqualPeriod = '0')) else
            "11";
    
    pwmSignal <= dutyCycleGreaterCounter;
    pwmCount <= pwmCount_int;
    rollOver <= counterEqualPeriod and enb;
    
    
end Behavioral;
