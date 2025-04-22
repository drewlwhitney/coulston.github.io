----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.basicBuildingBlocks_package.all;
--use work.simpleDualPortRamWithCounters_Package.all;


entity simpleDualPortRamWithCounters is
    PORT (  clk : in  STD_LOGIC;
            resetn : in  STD_LOGIC;
            cw: in STD_LOGIC_VECTOR(2 downto 0));
end simpleDualPortRamWithCounters;

architecture structure of simpleDualPortRamWithCounters is

CONSTANT DATA_WORD_WIDTH : NATURAL := 5; 

component blk_mem_gen_0 IS
  PORT (
    clka : IN STD_LOGIC;
    ena : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(DATA_WORD_WIDTH - 1 DOWNTO 0);
    clkb : IN STD_LOGIC;
    enb : IN STD_LOGIC;
    addrb : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    doutb : OUT STD_LOGIC_VECTOR(DATA_WORD_WIDTH - 1 DOWNTO 0)
  );
END component;

signal ena, enb: STD_LOGIC;
signal wea: STD_LOGIC_VECTOR(0 downto 0);
signal  writeCounterQ: STD_LOGIC_VECTOR(5 downto 0);
signal  readCounterQ, ramReadAddr, ramWriteAddr: STD_LOGIC_VECTOR(2 downto 0);
signal  adderOut, ramReadOut, ramWriteIn: STD_LOGIC_VECTOR(DATA_WORD_WIDTH - 1 downto 0);


begin

    ena <= cw(2);
    wea <= cw(1 downto 1);
    enb <= cw(0);

    ramWriteAddr <= writeCounterQ(2 downto 0);
    
    bram_inst: blk_mem_gen_0
      PORT MAP (
        clka => clk,
        ena => ena,
        wea => wea,
        addra => ramWriteAddr,
        dina => ramWriteIn, 
        clkb => clk,
        enb => enb,
        addrb => ramReadAddr,
        doutb => ramReadOut);


    adderWrite_inst: genericAdder 
        GENERIC MAP (DATA_WORD_WIDTH)
        PORT MAP(   a => ramReadOut,
                    b => writeCounterQ(5 downto 1),
                    sum => ramWriteIn);


    writeCounter_inst: genericCounter 
            GENERIC MAP (6)
            PORT MAP (  clk => clk,
                        resetn => resetn,
                        c => "10",
                        d => (others => '0'),
                        q => writeCounterQ);

    adderRead_inst: genericAdder 
        GENERIC MAP (3)
        PORT MAP(   a => readCounterQ,
                    b => "010",
                    sum => ramReadAddr);
                
    readCounter_inst: genericCounter 
            GENERIC MAP (3)
            PORT MAP (  clk => clk,
                        resetn => resetn,
                        c => "10",
                        d => (others => '0'),
                        q => readCounterQ);


end structure;
