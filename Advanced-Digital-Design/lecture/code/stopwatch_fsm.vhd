--------------------------------------------------------------------
-- Name:	Chris Coulston
-- Date:	Feb 3, 2015
-- File:	stopWatch_fsm.vhd
--	Crs:	EENG 498
--
-- Purp: The control unit for the stopwatch
--
-- Documentation:	No help, though I used an example from my Digital
--						Design text book.
--
-- Academic Integrity Statement: I certify that, while others may have 
-- assisted me in brain storming, debugging and validating this program, 
-- the program itself is my own work. I understand that submitting code 
-- which is the work of other individuals is a violation of the honor   
-- code.  I also understand that if I knowingly give my original work to 
-- another individual is also a violation of the honor code. 
------------------------------------------------------------------------- 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.stopWatch_package.all;					-- include your library here with added components ac97, ac97cmd


entity stopWatch_fsm is
    PORT (  clk : in  STD_LOGIC;
            resetn : in  STD_LOGIC;
            sw: in STD_LOGIC_VECTOR(SW_VECTOR_LENGTH - 1 downto 0);
            cw: out STD_LOGIC_VECTOR (CW_VECTOR_LENGTH - 1 downto 0));
end stopWatch_fsm;

architecture Behavioral of stopWatch_fsm is

	signal tenth, S1, S2: STD_LOGIC;
    signal state: state_type;
    
begin
    
	tenth <= sw(0);
	S1 <= sw(1);
	S2 <= sw(2);
	
	-------------------------------------------------------------------------------
	-------------------------------------------------------------------------------
	state_proces: process(clk)  
	begin
		if (rising_edge(clk)) then
			if (resetn = '0') then 
				state <= RESET_STATE;
			else 
				case state is
				
				    -- Welcome 
					when RESET_STATE =>
						  state <= STOP_STATE;
						  					
				    when STOP_STATE => 
					   if ( S1 = '0') then 
					       state <= S2RESET_STATE;
					   elsif ( S2 = '0') then
					       state <= S2R_STATE;							
					   end if;
					
					
				    when S2RESET_STATE => 
					   if (S1 = '1') then
					       state <= RESET_STATE;
					   end if;
					
				    when S2R_STATE =>
				        if (S2 = '1') then
				            state <= RUN_STATE;
				        end if;		 
					
				    when RUN_STATE =>
					   if (tenth = '1') then
					       state <= INC_STATE;
					   elsif (S1 = '0') then
							state <= R2LR_STATE;
                       elsif (S2 = '0') then							
							state <= R2S_STATE;							
					   end if;

				    when R2LR_STATE =>
    					if (S1 = '1') then
					       state <=  LAPRUN_STATE;
					    end if;
					
				    when R2S_STATE =>
					   if (S2 = '1') then
					       state <= STOP_STATE;							
					   end if;
					
				    when INC_STATE => 
					   state <= RUN_STATE;
													
				    when LAPRUN_STATE => 
					   if (tenth = '1') then
					       state <= LAPINC_STATE;		
                       elsif (S1 = '0') then					       			  
							 state <= LR2R_STATE;
                       elsif (S2 = '0') then							 
					         state <= LR2LS_STATE;							
					   end if;

				    when LR2R_STATE =>
					   if (S1 = '1') then 
					       state <= RUN_STATE;							
					   end if;
					
				    when LR2LS_STATE =>
				        if (S2 = '1') then
				            state <= LAPSTOP_STATE;
						end if;
					  
				    when LAPINC_STATE => 
					   state <= LAPRUN_STATE;
					
				    WHEN LAPSTOP_STATE => 
					   if (S1 = '0') then 
					       state <= LS2R_STATE;
					   elsif (S2 = '0') then
							state <= LS2LR_STATE;
					   end if;

				    when LS2R_STATE =>
					   if (S1 = '1') then
					       state <= RESET_STATE;
					   end if;

				    when LS2LR_STATE => 
				        if (S2 = '1') then
				            state <= LAPRUN_STATE;
				        end if;						  					 					 
                           
				end case;
			end if;
		end if;
	end process;

	-------------------------------------------------------------------------------    
    -- Define the control words for each state
    -- cw[5] 2x1 mux		cw[4]	register		cw[3]   mod10Counter		cw[2]   mod10Counter			cw[1:0] tenthSecondCounter        
    -- 0 mod10counter		0 hold lap reg		0 hold						0 hold							00    hold
    -- 1 lap register		1 load lap reg		1 synch reset				1 enable							01    count down
    -------------------------------------------------------------------------------
	output_process: process (state)
	begin
		case state is		      
            
            when RESET_STATE    => cw <= "001011";	 
            when S2RESET_STATE  => cw <= "000000";            
            when STOP_STATE     => cw <= "000000";
            when S2R_STATE      => cw <= "000000";            
            when RUN_STATE      => cw <= "000010";
            when R2LR_STATE     => cw <= "010000";
            when R2S_STATE      => cw <= "000000";
            
            when INC_STATE      => cw <= "000111";
            when LAPINC_STATE   => cw <= "100111";
            
            when LAPRUN_STATE   => cw <= "100010";
            when LR2R_STATE     => cw <= "100000";
            when LR2LS_STATE    => cw <= "100000";					 
            
            when LAPSTOP_STATE  => cw <= "100000";					 
            when LS2R_STATE     => cw <= "100000";
            when LS2LR_STATE    => cw <= "100000";

                      
		end case;
	end process;	                       

end Behavioral;



