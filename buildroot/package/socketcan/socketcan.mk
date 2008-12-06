#############################################################
#
# Socket CAN
#
#############################################################
SOCKETCAN_TRUNK:=http://svn.berlios.de/svnroot/repos/socketcan/trunk
SOCKETCAN_REVISION:=874
SOCKETCAN_NAME:=socketcan-svn-rev$(SOCKETCAN_REVISION)
SOCKETCAN_DIR:=$(BUILD_DIR)/$(SOCKETCAN_NAME)
SOCKETCAN_SOURCE:=$(SOCKETCAN_NAME).tar.bz2
SOCKETCAN_CAT=$(BZCAT)
SOCKETCAN_EXE=candump

$(DL_DIR)/$(SOCKETCAN_SOURCE):
	(cd $(BUILD_DIR); \
		$(SVN_CO) -r $(SOCKETCAN_REVISION) $(SOCKETCAN_TRUNK) $(SOCKETCAN_NAME); \
		tar -cvf $(SOCKETCAN_NAME).tar $(SOCKETCAN_NAME); \
		bzip2 $(SOCKETCAN_NAME).tar; \
		rm -rf $(SOCKETCAN_DIR); \
		mv $(SOCKETCAN_SOURCE) $(DL_DIR)/$(SOCKETCAN_SOURCE); \
	)

$(SOCKETCAN_DIR)/.unpacked: $(DL_DIR)/$(SOCKETCAN_SOURCE)
	$(SOCKETCAN_CAT) $(DL_DIR)/$(SOCKETCAN_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(SOCKETCAN_DIR) package/socketcan $(SOCKETCAN_NAME)\*.patch	
	touch $@

$(SOCKETCAN_DIR)/.configured: $(SOCKETCAN_DIR)/.unpacked
	touch $@

$(SOCKETCAN_DIR)/.compiled: $(SOCKETCAN_DIR)/.configured
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(SOCKETCAN_DIR)/can-utils
ifeq ($(strip $(BR2_PACKAGE_SOCKETCAN_TEST)),y)
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(SOCKETCAN_DIR)/test
endif
	touch $@

$(TARGET_DIR)/usr/bin/$(SOCKETCAN_EXE): $(SOCKETCAN_DIR)/.compiled
	$(MAKE) STRIP_CMD="$(STRIPCMD) $(STRIP_STRIP_UNNEEDED)" -C $(SOCKETCAN_DIR)/can-utils strip
	$(MAKE) INSTALL_DIR=$(TARGET_DIR)/usr/bin -C $(SOCKETCAN_DIR)/can-utils install
ifeq ($(strip $(BR2_PACKAGE_SOCKETCAN_TEST)),y)
	$(MAKE) STRIP_CMD="$(STRIPCMD) $(STRIP_STRIP_UNNEEDED)" -C $(SOCKETCAN_DIR)/test strip
	$(MAKE) INSTALL_DIR=$(TARGET_DIR)/usr/bin -C $(SOCKETCAN_DIR)/test install
endif
	touch $@

SOCKETCAN socketcan: uclibc $(TARGET_DIR)/usr/bin/$(SOCKETCAN_EXE)

socketcan-source: $(DL_DIR)/$(SOCKETCAN_SOURCE)

socketcan-clean:
	$(MAKE) INSTALL_DIR=$(STAGING_DIR)/usr/bin -C $(SOCKETCAN_DIR) uninstall
	-$(MAKE) -C $(SOCKETCAN_DIR) clean

socketcan-dirclean:
	rm -rf $(SOCKETCAN_DIR)


############################################################
#
# Toplevel Makefile options
#
############################################################
ifeq ($(strip $(BR2_PACKAGE_SOCKETCAN)),y)
TARGETS+=socketcan
endif

