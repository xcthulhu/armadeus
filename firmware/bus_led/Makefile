# Makefile for VHDL synthesis and simulation
# version 1.0
# Fabien Marteau

# project name
PROJECT=bus_led_top
# vhdl files
FILES = src/bus_led.vhd src/bus_led_top.vhd
# pin configuration
UCF_FILE = src/bus_led.ucf
# Synthesis constraints file
XCF_FILE = 
# testbench
SIMTOP = led_top_tb
SIMFILES = ../apf_pkg/apf_test_pkg.vhd testbench/led_top_tb.vhd
# Simu break condition
GHDL_SIM_OPT    = --assert-level=error
#GHDL_SIM_OPT    = --stop-time=500ns

# FPGA options
PART = xc3s200-4-tq144

############################
# Personnals options
############################

CAT=../Xtools/catcolor
BRIEF=../Xtools/brief.py


############################
# Xilinx options
############################
BIN = bin
WORK	= work

SYNTH = xst
NGDBUILD = ngdbuild
MAP = map 
PAR = par
TRACE = trce
BITGEN = bitgen

ifdef $(XCF_FILE)
XCF = -uc $(XCF_FILE)\n
endif

##############################
# GHDL options
##############################

SIMDIR = simu
SYNTHFILES = bin/bus_led_ise/netgen/synthesis

GHDL_CMD        = ghdl
GHDL_FLAGS      = --ieee=synopsys --warn-no-vital-generic
#GHDL_FLAGS      = --ieee=synopsys -P~/Xilinx92i/coregen/ip/xilinx/primary/com/xilinx/ip/unisim --warn-no-vital-generic

VIEW_CMD        = /usr/bin/gtkwave

OBJS_FILES      = $(patsubst %.vhd, %.o, $(notdir $(FILES)) )
OBJS_SIMFILES   = $(patsubst %.vhd, %.o, $(notdir $(SIMFILES)) )

#################################
# Synthesis with xst
#################################

messages : bit
	$(BRIEF) $(BIN)/$(PROJECT)	


bit : $(BIN)/$(PROJECT).ncd trace
	-$(BITGEN) -intstyle xflow -w \
	-g DebugBitstream:No \
	-g Binary:no \
	-g CRC:Enable \
	-g ConfigRate:6 \
	-g CclkPin:PullUp \
	-g M0Pin:PullUp \
	-g M1Pin:PullUp \
	-g M2Pin:PullUp \
	-g ProgPin:PullUp \
	-g DonePin:PullUp \
	-g TckPin:PullUp \
	-g TdiPin:PullUp \
	-g TdoPin:PullUp \
	-g TmsPin:PullUp \
	-g UnusedPin:PullDown \
	-g UserID:0xFFFFFFFF \
	-g DCMShutdown:Disable \
	-g DCIUpdateMode:AsRequired \
	-g StartUpClk:CClk \
	-g DONE_cycle:4 \
	-g GTS_cycle:5 \
	-g GWE_cycle:6 \
	-g LCK_cycle:NoWait \
	-g Match_cycle:Auto \
	-g Security:None \
	-g DonePipe:No \
	-g DriveDone:No \
	$(BIN)/$(PROJECT).ncd > $(BIN)/$(PROJECT).bitlog
	$(CAT) $(BIN)/$(PROJECT).bitlog
	cp $(BIN)/$(PROJECT).bit /tftpboot/

trace: $(BIN)/$(PROJECT).ncd $(BIN)/$(PROJECT).pcf
	-$(TRACE) -intstyle xflow -e 3 -l 3 -s 5 -xml $(BIN)/$(PROJECT) $(BIN)/$(PROJECT).ncd -o $(BIN)/$(PROJECT).twr $(BIN)/$(PROJECT).pcf > $(BIN)/$(PROJECT).tracelog 
	$(CAT) $(BIN)/$(PROJECT).tracelog

$(BIN)/$(PROJECT).ncd : par 

par : $(BIN)/$(PROJECT)_map.ncd $(BIN)/$(PROJECT).pcf
	- sh -c 'pid=$$$$;	$(PAR) -w -intstyle xflow -ol std -t 1 $(BIN)/$(PROJECT)_map.ncd $(BIN)/$(PROJECT).ncd $(BIN)/$(PROJECT).pcf | (sed "/^PAR done/q";kill $$pid)' > $(BIN)/$(PROJECT).parlog
	$(CAT) $(BIN)/$(PROJECT).parlog
	- killall -9 par

$(BIN)/$(PROJECT).pcf $(BIN)/$(PROJECT)_map.ncd	: map

map : $(BIN)/$(PROJECT).ngd
	- sh -c 'pid=$$$$ ;	$(MAP) -intstyle xflow -p $(PART) -cm area -pr b -k 4 -c 100 -o $(BIN)/$(PROJECT)_map.ncd $(BIN)/$(PROJECT).ngd $(BIN)/$(PROJECT).pcf  | (sed "/^See MAP report file/q" ; kill $$pid ) ' > $(BIN)/$(PROJECT).maplog
	$(CAT) $(BIN)/$(PROJECT).maplog
	- killall -9 map

$(BIN)/$(PROJECT).ngd : ngdbuild

ngdbuild : $(BIN)/$(PROJECT).ngr $(BIN)/$(PROJECT).ngc $(BIN)/$(PROJECT).lso 
	-$(NGDBUILD) -intstyle xflow -dd _ngo -nt timestamp -p $(PART) $(BIN)/$(PROJECT).ngc $(BIN)/$(PROJECT).ngd -uc $(UCF_FILE) > $(BIN)/$(PROJECT).ngdlog
	$(CAT) $(BIN)/$(PROJECT).ngdlog

$(BIN)/$(PROJECT).ngc $(BIN)/$(PROJECT).ngr $(BIN)/$(PROJECT).lso : synthesis

synthesis : $(BIN)/$(PROJECT).prj $(BIN)/$(PROJECT).xst
	$(SYNTH) -ifn $(BIN)/$(PROJECT).xst -ofn $(BIN)/$(PROJECT).log \
		| sed "s/^ERROR/[01\;31mERROR[00m/"\
		| sed "s/^WARNING/[01\;33mWARNING[00m/"\
		| sed "s/^INFO/[01\;32mINFO[0m/"

$(BIN)/$(PROJECT).xst $(BIN)/$(PROJECT).prj : 
	@mkdir -p $(BIN)
	@touch $(BIN)/$(PROJECT); 
	@for f in $(FILES); do \
		echo vhdl $(WORK) \"$$f\" >> $(BIN)/$(PROJECT).prj;\
	done
	@echo work > $(BIN)/$(PROJECT).lso
	@mkdir -p ./bin/xst/projnav.tmp
	@echo " \
	set -tmpdir "./bin/xst/projnav.tmp" \n\
	set -xsthdpdir "./bin/xst" \n\
	run \n\
	-ifn $(BIN)/$(PROJECT).prj \n\
	-ifmt mixed \n\
	-ofn $(BIN)/$(PROJECT) \n\
	-ofmt NGC \n\
	-p $(PART) \n\
	-top $(PROJECT) \n\
	-opt_mode Speed \n\
	-opt_level 1 \n\
	$(XCF)\
	-iuc NO \n\
	-lso $(BIN)/$(PROJECT).lso \n\
	-keep_hierarchy NO \n\
	-rtlview Yes \n\
	-glob_opt AllClockNets \n\
	-read_cores YES \n\
	-write_timing_constraints NO \n\
	-cross_clock_analysis NO \n\
	-hierarchy_separator / \n\
	-bus_delimiter <> \n\
	-case maintain \n\
	-slice_utilization_ratio 100   \n\
	-bram_utilization_ratio 100 \n\
	-verilog2001 YES \n\
	-fsm_extract YES -fsm_encoding Auto \n\
	-safe_implementation No \n\
	-fsm_style lut \n\
	-ram_extract Yes \n\
	-ram_style Auto \n\
	-rom_extract Yes \n\
	-mux_style Auto \n\
	-decoder_extract YES \n\
	-priority_extract YES \n\
	-shreg_extract YES  \n\
	-shift_extract YES  \n\
	-xor_collapse YES \n\
	-rom_style Auto \n\
	-auto_bram_packing NO \n\
	-mux_extract YES \n\
	-resource_sharing YES \n\
	-async_to_sync NO \n\
	-mult_style auto \n\
	-iobuf YES \n\
	-max_fanout 500 \n\
	-bufg 8 \n\
	-register_duplication YES \n\
	-register_balancing No \n\
	-slice_packing YES \n\
	-optimize_primitives NO \n\
	-use_clock_enable Yes \n\
	-use_sync_set Yes \n\
	-use_sync_reset Yes \n\
	-iob auto \n\
	-equivalent_register_removal YES \n\
	-slice_utilization_ratio_maxmargin 5\n\
	" >> $(BIN)/$(PROJECT).xst

clean : ghdl-clean
	-rm -rf $(BIN)
	-rm -rf _ngo
	-rm xilinx_device_details.xml

########################
# Simulation with GHDL
########################

ghdl-compil :
	mkdir -p simu
	$(GHDL_CMD) -i $(GHDL_FLAGS) --workdir=simu --work=work $(SIMFILES) $(FILES)
	$(GHDL_CMD) -m $(GHDL_FLAGS) --workdir=simu --work=work $(SIMTOP)
	@mv $(SIMTOP) simu/$(SIMTOP)

#TODO: make it working !
ghdl-simsynth :
	$(GHDL_CMD) -i $(GHDL_FLAGS) --workdir=simu --work=work $(SYNTHFILES) $(SIMFILES)
	$(GHDL_CMD) -m $(GHDL_FLAGS) --workdir=simu --work=work $(SIMTOP)
	@mv $(SIMTOP) simu/$(SIMTOP)

ghdl-run :
	@$(SIMDIR)/$(SIMTOP) $(GHDL_SIM_OPT) --vcdgz=$(SIMDIR)/$(SIMTOP).vcdgz

ghdl-view:
	gunzip --stdout $(SIMDIR)/$(SIMTOP).vcdgz | $(VIEW_CMD) --vcd

ghdl-clean :
	$(GHDL_CMD) --clean --workdir=simu
	-rm -rf simu

