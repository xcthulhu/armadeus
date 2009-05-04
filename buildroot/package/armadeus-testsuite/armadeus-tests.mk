#############################################################
#
# Armadeus Test Suite
#
#############################################################

ARMADEUS-TESTSUITE_VER:=1.0
ARMADEUS-TESTSUITE_SOURCE:=$(BUILD_DIR)/../../target/test/
ARMADEUS-TESTSUITE_DIR:=$(BUILD_DIR)/armadeus-testsuite-$(ARMADEUS-TESTSUITE_VER)
ARMADEUS-TESTSUITE_CAT:=zcat
ARMADEUS-TESTSUITE_BINARY:=test_env.sh
ARMADEUS-TESTSUITE_TARGET_DIR:=$(TARGET_DIR)/usr/bin/testsuite
ARMADEUS-TESTSUITE_TARGET_BINARY:=$(ARMADEUS-TESTSUITE_TARGET_DIR)/$(ARMADEUS-TESTSUITE_BINARY)

$(ARMADEUS-TESTSUITE_DIR)/.unpacked:
	mkdir -p $(ARMADEUS-TESTSUITE_DIR)/
	cp -prL $(ARMADEUS-TESTSUITE_SOURCE)/* $(ARMADEUS-TESTSUITE_DIR)/
	touch $@

$(ARMADEUS-TESTSUITE_TARGET_BINARY): $(ARMADEUS-TESTSUITE_DIR)/.unpacked
	mkdir -p $(ARMADEUS-TESTSUITE_TARGET_DIR)
	find $(ARMADEUS-TESTSUITE_DIR) -name "*.sh" -exec install -D {} $(ARMADEUS-TESTSUITE_TARGET_DIR) \;
	cp -r $(ARMADEUS-TESTSUITE_DIR)/data $(ARMADEUS-TESTSUITE_TARGET_DIR)/
	touch $@

armadeus-testsuite: $(ARMADEUS-TESTSUITE_TARGET_BINARY)

armadeus-testsuite-clean:
	rm -rf $(ARMADEUS-TESTSUITE_TARGET_DIR)

armadeus-testsuite-dirclean:
	rm -rf $(ARMADEUS-TESTSUITE_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_ARMADEUS_TESTSUITE)),y)
TARGETS+=armadeus-testsuite
endif
