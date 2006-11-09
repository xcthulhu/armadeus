#############################################################
#
# Linux Frame Buffer Device Configuration
#
#############################################################
FBSET_VER=-2.1
FBSET_SOURCE=fbset$(FBSET_VER).tar.gz
FBSET_SITE=http://users.telenet.be/geertu/Linux/fbdev/
FBSET_DIR:=$(BUILD_DIR)/fbset$(FBSET_VER)
FBSET_BINARY:=fbset
FBSET_TARGET_BINARY:=usr/sbin/fbset

$(DL_DIR)/$(FBSET_SOURCE):
	 $(WGET) -P $(DL_DIR) $(FBSET_SITE)/$(FBSET_SOURCE)


$(FBSET_DIR)/.unpacked: $(DL_DIR)/$(FBSET_SOURCE)
	rm -rf $(FBSET_DIR)
	zcat $(DL_DIR)/$(FBSET_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(FBSET_DIR) \
		package/fbset \*.patch
	touch $(FBSET_DIR)/.unpacked

$(FBSET_DIR)/.configured: $(FBSET_DIR)/.unpacked
	touch $(FBSET_DIR)/.configured

$(FBSET_DIR)/$(FBSET_BINARY): $(FBSET_DIR)/.configured
	$(MAKE) -C $(FBSET_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(TARGET_CROSS)

$(TARGET_DIR)/$(FBSET_TARGET_BINARY): $(FBSET_DIR)/$(FBSET_BINARY)
	cp -f $< $@

fbset: uclibc $(TARGET_DIR)/$(FBSET_TARGET_BINARY)

fbset-clean:
	-$(MAKE) -C $(FBSET_DIR) clean

fbset-dirclean:
	rm -rf $(FBSET_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_FBSET)),y)
TARGETS+=fbset
endif

