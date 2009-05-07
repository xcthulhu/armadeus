#############################################################
#
# Armadeus Helpers script for loading Linux drivers
#
#############################################################

ARMADEUS-DRIVERS_SOURCE_DIR:=$(BUILD_DIR)/../../target/linux/modules
ARMADEUS-DRIVERS-HELPERS_TARGET_DIR:=$(TARGET_DIR)/usr/bin

$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadgpio.sh: 
	cp $(ARMADEUS-DRIVERS_SOURCE_DIR)/gpio/loadgpio.sh $(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/

$(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadmax.sh:
	cp $(ARMADEUS-DRIVERS_SOURCE_DIR)/max1027/loadmax.sh $(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/
#	find $(ARMADEUS-TESTSUITE_DIR) -name "*.sh" -exec install -D {} $(TARGET_DIR)/$(ARMADEUS-TESTSUITE_TARGET_DIR) \;

armadeus-drivers-helpers: $(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadgpio.sh $(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadmax.sh

armadeus-drivers-helpers-clean:
	rm -f $(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadgpio.sh
	rm -f $(ARMADEUS-DRIVERS-HELPERS_TARGET_DIR)/loadmax.sh


#############################################################
#
# Toplevel Makefile options
#
#############################################################
# Always installed by default
TARGETS+=armadeus-drivers-helpers

