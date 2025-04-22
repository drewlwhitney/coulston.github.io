#################################################################################
#Simulator TCL starts in: C:/Users/Chris/AppData/Roaming/Xilinx/Vivado
# cd C:\Users\Chris\Dropbox\Mycourses\EENG498\VHDL\lab01HDMI
# Note, you need to switch the direction of the slash from windows "/" to "\"
# source <tclScriptName>
# Note your signal names will probablly be different, edit to make consistent
#################################################################################
restart

# can anyone find a single command to delete all the waves with one command?
# otherwise you need to manually delete all the waves every
# time you run a simulation.

add_wave  -color green /lab1_tb/clk
add_wave  -color green /lab1_tb/resetn

add_wave   -color yellow -radix unsigned /lab1_tb/vsg/h_cnt
add_wave   -color yellow -radix unsigned /lab1_tb/vsg/pixelHorz
add_wave   -color yellow 		/lab1_tb/vsg/h_activeArea
add_wave   -color yellow 		/lab1_tb/vsg/hs

add_wave   -color orange -radix unsigned /lab1_tb/vsg/v_cnt
add_wave   -color yellow -radix unsigned /lab1_tb/vsg/pixelVert
add_wave   -color orange 		/lab1_tb/vsg/v_activeArea
add_wave   -color orange		/lab1_tb/vsg/vs

add_wave   -color aqua	 		/lab1_tb/vsg/de

add_wave   -color red -radix hex	/lab1_tb/sf/red
add_wave   -color green -radix hex	/lab1_tb/sf/green
add_wave   -color blue -radix hex	/lab1_tb/sf/blue







