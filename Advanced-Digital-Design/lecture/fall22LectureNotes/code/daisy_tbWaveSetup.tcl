#################################################################################
#Simulator TCL starts in: C:/Users/Chris/AppData/Roaming/Xilinx/Vivado
# cd C:/Users/Chris/Dropbox/Mycourses/EENG498/VHDL/classExamples/Daisy
# source daisy_tbWaveSetup.tcl
#################################################################################
restart


add_wave  -color green /daisy_tb/uut/clk
add_wave  -color green /daisy_tb/uut/resetn

add_wave   -color aqua /daisy_tb/uut/state

add_wave   -color red -radix hex	/daisy_tb/uut/sw
add_wave   -color red -radix hex -name RFID /daisy_tb/uut/sw[2]
add_wave   -color red -radix hex -name COW /daisy_tb/uut/sw[1]
add_wave   -color red -radix hex -name TIMER /daisy_tb/uut/sw[0]


add_wave   -color orange -radix hex	/daisy_tb/uut/cw
add_wave   -color orange -radix hex -name GATE1 /daisy_tb/uut/cw[4]
add_wave   -color orange -radix hex -name GATE2 /daisy_tb/uut/cw[3]
add_wave   -color orange -radix hex -name TIMER_MSB /daisy_tb/uut/cw[2]
add_wave   -color orange -radix hex -name TIMER_LSB /daisy_tb/uut/cw[1]
add_wave   -color orange -radix hex -name AIR /daisy_tb/uut/cw[0]











