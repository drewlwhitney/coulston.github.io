-------------------------------------------------
-- Author:      Chris Coulston
-- Date:        Fall 2002
-- Purp:        A sweet register with asynchronous
--		active low reset and a normal
--		load/hold control input.
-- Call:	r: reg generic map(8) port map(clk,reset,c,d,q);
-- 		r: reg port map(clk,reset,c,d,q);
-------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;

entity genericRegister is
        generic(N: integer := 4);
        port (  clk, reset,load: in std_logic;
                d: in  std_logic_vector(N-1 downto 0);
                q: out std_logic_vector(N-1 downto 0) );
end genericRegister;

architecture behavior of genericRegister is
begin
    process (clk,reset)
    begin
	if (reset = '0') then q <= (others => '0');
	elsif (clk'event and clk='1') then
	    if (load='1') then q <= d;
	    end if;
	end if;
    end process;
end architecture;

