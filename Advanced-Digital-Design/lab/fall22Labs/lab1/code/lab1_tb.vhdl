--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY lab1_tb IS
END lab1_tb;

ARCHITECTURE behavior OF lab1_tb IS

    -- Component Declaration for the Unit Under Test (UUT)
    component videoSignalGenerator is
        PORT(	clk: in  STD_LOGIC;
             resetn : in  STD_LOGIC;
             hs: out STD_LOGIC;
             vs: out STD_LOGIC;
             de: out STD_LOGIC;
             pixelHorz: out STD_LOGIC_VECTOR(11 downto 0);
             pixelVert: out STD_LOGIC_VECTOR(11 downto 0));
    end component;

    component scopeFace is
        Port (	clk: in  STD_LOGIC;
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
    end component;

    --Inputs
    signal clk : STD_LOGIC := '0';
    signal resetn : STD_LOGIC := '0';
    signal triggerVolt : STD_LOGIC_VECTOR(11 downto 0) := (others => '0');
    signal triggerTime : STD_LOGIC_VECTOR(11 downto 0) := (others => '0');
    signal pixelVert : STD_LOGIC_VECTOR(11 downto 0) := (others => '0');
    signal pixelHorz : STD_LOGIC_VECTOR(11 downto 0) := (others => '0');
    signal ch1Wave, ch1_enb, ch2Wave, ch2_enb: STD_LOGIC := '0';

    --Outputs
    signal hs : STD_LOGIC;
    signal vs : STD_LOGIC;
    signal de : STD_LOGIC;

    signal red : STD_LOGIC_VECTOR(7 downto 0);
    signal green : STD_LOGIC_VECTOR(7 downto 0);
    signal blue : STD_LOGIC_VECTOR(7 downto 0);

    -- Clock period definitions
    constant clk_period : time := 10 ns;

BEGIN

    vsg: videoSignalGenerator
        PORT MAP (clk => clk,
                 resetn => resetn,
                 hs => hs,
                 vs => vs,
                 de => de,
                 pixelHorz => pixelHorz,
                 pixelVert => pixelVert);

    sf: scopeFace
        PORT MAP (clk => clk,
                 resetn => resetn,
                 pixelHorz => pixelHorz,
                 pixelVert => pixelVert,
                 triggerVolt => triggerVolt,
                 triggerTime => triggerTime,
                 red => red,
                 green => green,
                 blue => blue,
                 ch1 => ch1Wave,
                 ch1Enb => '1',
                 ch2 => ch2Wave,
                 ch2Enb => '1');



    -- Clock process definitions
    clk_process :process
    begin
        clk <= '0';
        wait for clk_period/2;
        clk <= '1';
        wait for clk_period/2;
    end process;

    resetn <= '0', '1' after 30nS;


END;
