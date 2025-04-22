----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2023 10:33:30 AM
-- Design Name: 
-- Module Name: stopWatch - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.stopwatch_package.all;

entity stopWatch is
  Port (    clk:    in STD_LOGIC;
            resetn: in STD_LOGIC;
            S1, S2: in STD_LOGIC;
            tenHex, unitHex, tenthHex:out STD_LOGIC_VECTOR(6 downto 0));     	
end stopWatch;

architecture Structure of stopWatch is

	signal cw: STD_LOGIC_VECTOR(CW_VECTOR_LENGTH-1 downto 0);
	signal sw: STD_LOGIC_VECTOR(SW_VECTOR_LENGTH-1 downto 0);
	signal tenth : STD_LOGIC;


begin

	sw(0) <= tenth;
	sw(1) <= S1;
	sw(2) <= S2;

	dpsw: stopWatch_datapath
	   port map (  clk => clk,
	               resetn => resetn, 
	               cw => cw, 
	               tenth => tenth, 
	               tenHex => tenHex,
	               unitHex => unitHex, 
	               tenthHex => tenthHex);
	               	
	cpsw: stopWatch_fsm
	   port map(  clk => clk,
	               resetn => resetn, 
	               cw => cw, 
	               sw => sw);


end Structure;
