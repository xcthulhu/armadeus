#############################################################
#
# PPS Test scripts
#
#############################################################

ARMADEUS-PPS_TESTSCRIPTS_SOURCE:=$(TOPDIR)/../target/pps/scripts
ARMADEUS-PPS_TESTSCRIPTS_TARGET_DIR:=$(TARGET_DIR)/usr/local/pps/scripts
ARMADEUS-PPS_WWW_DIR:=$(TOPDIR)/../target/pps/www
ARMADEUS-PPS_WWW_TARGET_DIR:=$(TARGET_DIR)/var
ARMADEUS-PPS_CONFIGS_DIR:=$(TOPDIR)/../target/pps/configs
ARMADEUS-PPS_CONFIGS_TARGET_DIR:=$(TARGET_DIR)/etc/wpa_supplicant

$(ARMADEUS-PPS_TESTSCRIPTS_TARGET_DIR)/init_boa.sh:
	$(ARMADEUS-PPS_TESTSCRIPTS_SOURCE)/install.sh $(ARMADEUS-PPS_TESTSCRIPTS_TARGET_DIR)

$(ARMADEUS-PPS_WWW_TARGET_DIR)/www/index.html:
	cp -Rf $(ARMADEUS-PPS_WWW_DIR) $(ARMADEUS-PPS_WWW_TARGET_DIR)

$(ARMADEUS-PPS_CONFIGS_TARGET_DIR)/networkConfig:
	mkdir -p $(ARMADEUS-PPS_CONFIGS_TARGET_DIR)
	cp -f $(ARMADEUS-PPS_CONFIGS_DIR)/* $(ARMADEUS-PPS_CONFIGS_TARGET_DIR)

pps-testscripts: $(ARMADEUS-PPS_TESTSCRIPTS_TARGET_DIR)/init_boa.sh $(ARMADEUS-PPS_WWW_TARGET_DIR)/www/index.html $(ARMADEUS-PPS_CONFIGS_TARGET_DIR)/networkConfig

armadeus-pps-testscripts-clean:
	rm -rf $(ARMADEUS-PPS_TESTSCRIPTS_TARGET_DIR)
	rm -rf $(ARMADEUS-PPS_WWW_TARGET_DIR)/www
	rm -rf $(ARMADEUS-PPS_CONFIGS_TARGET_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_ARMADEUS_PPS_TEST_SCRIPTS)),y)
TARGETS+=pps-testscripts
endif
