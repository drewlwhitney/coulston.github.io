----------------------------------------------------------------------------------
-- Include proper comment header block
-- ***Do not use mod operator in this code***
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity scopeFace is
    PORT ( 	clk: in  STD_LOGIC;
         resetn : in  STD_LOGIC;
         pixelHorz : in  STD_LOGIC_VECTOR(11 downto 0);
         pixelVert : in  STD_LOGIC_VECTOR(11 downto 0);
         triggerVolt: in STD_LOGIC_VECTOR (11 downto 0);
         triggerTime: in STD_LOGIC_VECTOR (11 downto 0);
         red : out  STD_LOGIC_VECTOR(7 downto 0);
         green : out  STD_LOGIC_VECTOR(7 downto 0);
         blue : out  STD_LOGIC_VECTOR(7 downto 0);
         ch1: in STD_LOGIC;
         ch1Enb: in STD_LOGIC;
         ch2: in STD_LOGIC;
         ch2Enb: in STD_LOGIC);
end scopeFace;


architecture Behavioral of scopeFace is

    -- You should create constants for your display so that you can easily move the
    -- oscilloscope face around the display.  This makes it easier to make things look
    -- just right.
    constant H_ACTIVE : STD_LOGIC_VECTOR(11 downto 0) := std_logic_vector(to_unsigned(1280, 12));
    constant V_ACTIVE : STD_LOGIC_VECTOR(11 downto 0) := std_logic_vector(to_unsigned(720, 12));

    -- Finish and add more 
    constant L_EDGE : 
    constant R_EDGE : 
    constant WIDTH : 

    -- You should create constants for your display colors so that modify the colors 
    -- to make things look just right.
    constant BORDER_R : STD_LOGIC_VECTOR(7 downto 0) := X"FF";
    constant BORDER_G : STD_LOGIC_VECTOR(7 downto 0) := X"FF";
    constant BORDER_B : STD_LOGIC_VECTOR(7 downto 0) := X"FF";

    -- Finish and add more 
    constant GRID_R : 
    constant GRID_G : 
    constant GRID_B : 


    -- Set these signals to '1' when the features should be drawn at the current pixelHorz, pixelVert 
    -- cordinate.  These act like Feature Booleans which you will use in the process(clk) to set the 
    -- correct RGB for this pixel location. Finish and add more.
    signal borderH, borderV : STD_LOGIC;



begin


    ---------------------------------------------------------------------
    -- Use the Feature Booleans to set the RGB at this pixel location.
    -- The waveforms should sit "on top" of the grid.
    ---------------------------------------------------------------------
    process(clk)
    begin
        if rising_edge (clk) then
            if resetn = '0' then
                red <= (others => '0');
                green <= (others => '0');
                blue <= (others => '0');
            else
                if ((borderH = '1') or (borderV = '1')) then
                    red <= BORDER_R;
                    green <= BORDER_G;
                    blue <= BORDER_B;
                elsif 

                    <add elsif for each Feature Boolean>
              
                else
                    red <= X"00";
                    green <= X"00";
                    blue <= X"00";
                end if;
            end if;
        end if;
    end process;


    borderH <=	'1' when <lots of stuff> else '0';
    borderV <=	'1' when <lots of stuff> else '0';
  

end Behavioral;


