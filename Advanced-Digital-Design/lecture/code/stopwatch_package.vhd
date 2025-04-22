----------------------------------------------------------------------------------

----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.NUMERIC_STD.ALL;


package stopwatch_package is


-- Clock period definitions
CONSTANT clk_period : time := 20 ns;			-- 50Mhz crystal input (XTL_IN).

type state_type is (RESET_STATE, S2RESET_STATE, STOP_STATE, S2R_STATE, RUN_STATE, R2LR_STATE,
                        R2S_STATE, INC_STATE, LAPINC_STATE, LAPRUN_STATE, LR2R_STATE,
					       LR2LS_STATE, LAPSTOP_STATE, LS2R_STATE, LS2LR_STATE);


---------------------------- ACQUIRE CONTROL WORD -----------------------------
CONSTANT SW_VECTOR_LENGTH : NATURAL := 3;		
CONSTANT CW_VECTOR_LENGTH : NATURAL := 6;
CONSTANT N : NATURAL := 24;
--CONSTANT tenthSecondConstant: STD_LOGIC_VECTOR(N-1 downto 0) := x"4c4b40";
CONSTANT tenthSecondConstant: STD_LOGIC_VECTOR(N-1 downto 0)   := x"000004";

component stopWatch_fsm is
    PORT (  clk : in  STD_LOGIC;
            resetn : in  STD_LOGIC;
            sw: in STD_LOGIC_VECTOR(SW_VECTOR_LENGTH - 1 downto 0);
            cw: out STD_LOGIC_VECTOR (CW_VECTOR_LENGTH - 1 downto 0));
end component;

component stopWatch_datapath is
    PORT ( clk : in  STD_LOGIC;
           resetn : in  STD_LOGIC;
		   cw : in STD_LOGIC_VECTOR(CW_VECTOR_LENGTH -1 downto 0);
		   tenth: out STD_LOGIC;
		   tenHex, unitHex, tenthHex : out STD_LOGIC_VECTOR(6 downto 0));
end component;

component stopWatch is
  Port (    clk:    in STD_LOGIC;
            resetn: in STD_LOGIC;
            S1, S2: in STD_LOGIC;
            tenHex, unitHex, tenthHex: out STD_LOGIC_VECTOR(6 downto 0));     	
end component;	

component mod10Counter is
    Port ( clk : in STD_LOGIC;
        resetn : in STD_LOGIC;
         enb : in STD_LOGIC;
         synch : in STD_LOGIC;         
         roll : out STD_LOGIC;
         count : out STD_LOGIC_VECTOR (3 downto 0));
end component;


end package;


