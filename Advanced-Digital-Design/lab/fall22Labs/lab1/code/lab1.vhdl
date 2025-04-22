----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use ieee.std_logic_unsigned.all;


entity lab1 is
    PORT ( sysClk : in  STD_LOGIC;
         resetn : in  STD_LOGIC;
         btn: in	STD_LOGIC_VECTOR(2 downto 0);
         tmdsDataP : out  STD_LOGIC_VECTOR (2 downto 0);
         tmdsDataN : out  STD_LOGIC_VECTOR (2 downto 0);
         tmdsClkP : out STD_LOGIC;
         tmdsClkN : out STD_LOGIC;
         hdmiOen:    out STD_LOGIC);
end lab1;

architecture structure of lab1 is


    component videoSignalGenerator is
        PORT(	<stuff> );

    component scopeFace is
        Port ( 	<stuff>		);
    end component;


    component rgb2dvi_0 is
        PORT (
            TMDS_Clk_p : OUT STD_LOGIC;
            TMDS_Clk_n : OUT STD_LOGIC;
            TMDS_Data_p : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
            TMDS_Data_n : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
            oen : OUT STD_LOGIC;
            aRst_n : IN STD_LOGIC;
            vid_pData : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
            vid_pVDE : IN STD_LOGIC;
            vid_pHSync : IN STD_LOGIC;
            vid_pVSync : IN STD_LOGIC;
            PixelClk : IN STD_LOGIC;
            SerialClk : IN STD_LOGIC
        );
    end component;


   component video_clk is
        PORT (
            clk_out1 : OUT STD_LOGIC;
            clk_out2 : OUT STD_LOGIC;
            resetn : IN STD_LOGIC;
            clk_in1 : IN STD_LOGIC);
    end component;

    signal rgbVector: STD_LOGIC_VECTOR(23 downto 0);

    signal triggerTime, triggerVolt: STD_LOGIC_VECTOR(11 downto 0);
    signal pixelHorz, pixelVert: STD_LOGIC_VECTOR(11 downto 0);
	    
    signal ch1Wave, ch2Wave: STD_LOGIC;

    signal videoClk, videoClk5x : STD_LOGIC;

begin


    vsg: videoSignalGenerator
        PORT MAP (clk => videoClk, <other stuff>	);
                 

    sf: scopeFace
        PORT MAP (clk => videoClk,	<other stuff>	);
                 

    rgbVector <= red & green & blue;

    r2d: rgb2dvi_0
        PORT MAP (
            TMDS_Clk_p => tmdsClkP,	<other stuff>	);
            

    vc: video_clk
        PORT MAP (
            clk_in1 => sysClk,
            clk_out1 => videoClk,
            resetn => resetn,
            clk_out2 => videoClk5x);

    ------------------------------------------------------------------------------
    -- Create a process which generates a 3-bit vector which shows if button
    -- has change state.  Use this change vector to determine if you should 
    -- increment/decrement the triggerTime or triggerVolt values
    ------------------------------------------------------------------------------
 

    ch1Wave <= '1' when  (pixelHorz = pixelVert) else '0';
    ch2Wave <= '1' when  (pixelVert = triggerVolt) else '0';

end structure;