##################################################################
#
# Helpers scripts for loading/using Armadeus custom Linux drivers
#
##################################################################

ARMADEUS-DRIVERS_SOURCE_DIR:=$(TOPDIR)/../target/linux/modules
ARMADEUS-DRIVERS-HELPERS_TARGET_DIR:=$(TARGET_DIR)/usr/bin


$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadgpio.sh: $(ARMADEUS-DRIVERS_SOURCE_DIR)/gpio/loadgpio.sh
	cp -f $^ $@
ARMADEUS-DRIVERS_TARGETS+=$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadgpio.sh

$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/gpio_helpers.sh: $(ARMADEUS-DRIVERS_SOURCE_DIR)/gpio/gpio_helpers.sh
	cp -f $^ $@
ARMADEUS-DRIVERS_TARGETS+=$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/gpio_helpers.sh

$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadmax.sh: $(ARMADEUS-DRIVERS_SOURCE_DIR)/max1027/loadmax.sh
	cp -f $^ $@
#	find $(ARMADEUS-TESTSUITE_DIR) -name "*.sh" -exec install -D {} $(TARGET_DIR)/$(ARMADEUS-TESTSUITE_TARGET_DIR) \;
ARMADEUS-DRIVERS_TARGETS+=$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadmax.sh

$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/load_fpga: $(ARMADEUS-DRIVERS_SOURCE_DIR)/fpga/dev_tools/loader/load_fpga
	cp -f $^ $@
ARMADEUS-DRIVERS_TARGETS+=$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/load_fpga


armadeus-drivers-helpers: $(ARMADEUS-DRIVERS_TARGETS)

armadeus-drivers-helpers-clean:
	rm -f $(ARMADEUS-DRIVERS_TARGETS)


#############################################################
#
# Toplevel Makefile options
#
#############################################################
# Always installed by default
TARGETS+=armadeus-drivers-helpers

