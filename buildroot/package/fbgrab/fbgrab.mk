#############################################################
#
# FBGRAB
#
#############################################################
FBGRAB_VERSION:=1.0
FBGRAB_SOURCE:=fbgrab-$(FBGRAB_VERSION).tar.gz
FBGRAB_SITE:=http://hem.bredband.net/gmogmo/fbgrab/
FBGRAB_DIR:=$(BUILD_DIR)/fbgrab-$(FBGRAB_VERSION)
FBGRAB_CAT:=$(ZCAT)
FBGRAB_BINARY:=fbgrab
FBGRAB_TARGET_BINARY:=usr/bin/$(FBGRAB_BINARY)

$(DL_DIR)/$(FBGRAB_SOURCE):
	$(WGET) -P $(DL_DIR) $(FBGRAB_SITE)/$(FBGRAB_SOURCE)

$(FBGRAB_DIR)/.unpacked: $(DL_DIR)/$(FBGRAB_SOURCE)
	$(FBGRAB_CAT) $(DL_DIR)/$(FBGRAB_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(FBGRAB_DIR) package/fbgrab/ fbgrab-$(FBGRAB_VERSION)\*.patch\*
	touch $@

$(FBGRAB_DIR)/$(FBGRAB_BINARY): $(FBGRAB_DIR)/.unpacked
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(FBGRAB_DIR)
	touch -c $@

$(TARGET_DIR)/$(FBGRAB_TARGET_BINARY): $(FBGRAB_DIR)/$(FBGRAB_BINARY)
	$(INSTALL) -m 755 $(FBGRAB_DIR)/$(FBGRAB_BINARY) $(TARGET_DIR)/$(FBGRAB_TARGET_BINARY)
	-$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(FBGRAB_TARGET_BINARY)
	touch -c $@

fbgrab: libpng zlib $(TARGET_DIR)/$(FBGRAB_TARGET_BINARY)

fbgrab-clean:
	rm -f $(TARGET_DIR)/$(FBGRAB_TARGET_BINARY)
	-$(MAKE) -C $(FBGRAB_DIR) clean

fbgrab-dirclean:
	rm -rf $(FBGRAB_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_FBGRAB)),y)
TARGETS+=fbgrab
endif

