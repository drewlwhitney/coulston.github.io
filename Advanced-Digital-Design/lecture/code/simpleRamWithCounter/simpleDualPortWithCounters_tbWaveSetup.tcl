#################################################################################
# Include as a simulation source in Vivado
# cd C:/Users/coulston/Dropbox/Mycourses/EENG498/VHDL_fall2023/classExamples/simpleDualPortRamWithCounters
# source simpleDualPortWithCounters_tbWaveSetup.tcl
#################################################################################
restart
remove_wave [get_waves *]

add_wave  -color green /simpleDualPortRamWithCounter_tb/uut/clk
add_wave  -color green /simpleDualPortRamWithCounter_tb/uut/resetn

add_wave  -color yellow /simpleDualPortRamWithCounter_tb/uut/ena
add_wave  -color yellow /simpleDualPortRamWithCounter_tb/uut/wea
add_wave  -color yellow /simpleDualPortRamWithCounter_tb/uut/ramWriteAddr
add_wave  -color yellow /simpleDualPortRamWithCounter_tb/uut/ramWriteIn


add_wave  -color orange /simpleDualPortRamWithCounter_tb/uut/enb
add_wave  -color orange /simpleDualPortRamWithCounter_tb/uut/ramReadAddr
add_wave  -color orange /simpleDualPortRamWithCounter_tb/uut/ramReadOut

add_wave  -color red /simpleDualPortRamWithCounter_tb/uut/bram_inst/inst/native_mem_module.blk_mem_gen_v8_4_6_inst/memory