#############################################################
#
# fbtest
#
#############################################################
FBTEST_VERSION:=20041102-1
FBTEST_SOURCE:=fbtest-$(FBTEST_VERSION).tar.gz
FBTEST_SITE:=http://www.pengutronix.de/software/ptxdist/temporary-src/
FBTEST_DIR:=$(BUILD_DIR)/fbtest-$(FBTEST_VERSION)
FBTEST_CAT:=$(ZCAT)
FBTEST_BINARY:=fbtest
FBTEST_TARGET_BINARY:=usr/bin/$(FBTEST_BINARY)

$(DL_DIR)/$(FBTEST_SOURCE):
	$(WGET) -P $(DL_DIR) $(FBTEST_SITE)/$(FBTEST_SOURCE)

$(FBTEST_DIR)/.unpacked: $(DL_DIR)/$(FBTEST_SOURCE)
	$(FBTEST_CAT) $(DL_DIR)/$(FBTEST_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(FBTEST_DIR) package/fbtest/patches/ \
		fbtest-$(FBTEST_VERSION)\*.patch \
		fbtest-$(FBTEST_VERSION)\*.patch.$(ARCH)
	touch $@

$(FBTEST_DIR)/$(FBTEST_BINARY): $(FBTEST_DIR)/.unpacked
	$(MAKE1) ARCH=$(ARCH) CROSS_COMPILE=$(TARGET_CROSS) -C $(FBTEST_DIR)
	touch -c $@

$(TARGET_DIR)/$(FBTEST_TARGET_BINARY): $(FBTEST_DIR)/$(FBTEST_BINARY)
	$(INSTALL) -m 755 $(FBTEST_DIR)/$(FBTEST_BINARY) $(TARGET_DIR)/$(FBTEST_TARGET_BINARY)
	-$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(FBTEST_TARGET_BINARY)
	touch -c $@

fbtest: uclibc $(TARGET_DIR)/$(FBTEST_TARGET_BINARY)

fbtest-source: $(DL_DIR)/$(FBTEST_SOURCE)

fbtest-clean:
	rm -f $(TARGET_DIR)/$(FBTEST_TARGET_BINARY)
	-$(MAKE) -C $(FBTEST_DIR) clean

fbtest-dirclean:
	rm -rf $(FBTEST_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_FBTEST)),y)
TARGETS+=fbtest
endif
