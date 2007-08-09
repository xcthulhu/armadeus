# ipkg-utils
#
# http://handhelds.org/packages/ipkg-utils
#

IPKG-UTILS_NAME := ipkg-utils
IPKG-UTILS_VERSION := 1.7

IPKG-UTILS_SOURCE_SITE := http://handhelds.org/packages/ipkg-utils/
IPKG-UTILS_SOURCE_FILE := $(IPKG-UTILS_NAME)-$(IPKG-UTILS_VERSION).tar.gz
IPKG-UTILS_SOURCE_CAT := $(ZCAT)
IPKG-UTILS_SOURCE_DIR := $(IPKG-UTILS_NAME)-$(IPKG-UTILS_VERSION)

IPKG-UTILS_BUILD_DIR := $(BUILD_DIR)/$(IPKG-UTILS_SOURCE_DIR)


$(DL_DIR)/$(IPKG-UTILS_SOURCE_FILE):
	$(WGET) -P $(DL_DIR) $(IPKG-UTILS_SOURCE_SITE)/$(IPKG-UTILS_SOURCE_FILE)

$(IPKG-UTILS_BUILD_DIR)/.unpacked: $(DL_DIR)/$(IPKG-UTILS_SOURCE_FILE)
	$(IPKG-UTILS_SOURCE_CAT) $(DL_DIR)/$(IPKG-UTILS_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(IPKG-UTILS_BUILD_DIR)/.patched: $(IPKG-UTILS_BUILD_DIR)/.unpacked
	toolchain/patch-kernel.sh $(IPKG-UTILS_BUILD_DIR) toolchain/$(IPKG-UTILS_NAME)/$(IPKG-UTILS_VERSION) \*.patch
	touch $@

$(STAGING_DIR)/usr/bin/ipkg-build: $(IPKG-UTILS_BUILD_DIR)/.patched
	mkdir -p $(STAGING_DIR)/etc
	echo "dest root /" > $(STAGING_DIR)/etc/ipkg.conf
	echo "option offline_root $(TARGET_DIR)" >> $(STAGING_DIR)/etc/ipkg.conf
	mkdir -p $(STAGING_DIR)/usr/bin
	install -m0755 $(IPKG-UTILS_BUILD_DIR)/ipkg-build $(STAGING_DIR)/usr/bin
	install -m0755 $(IPKG-UTILS_BUILD_DIR)/ipkg-buildpackage $(STAGING_DIR)/usr/bin
	install -m0755 $(IPKG-UTILS_BUILD_DIR)/ipkg-make-index $(STAGING_DIR)/usr/bin
	install -m0755 $(IPKG-UTILS_BUILD_DIR)/ipkg.py $(STAGING_DIR)/usr/bin

$(IPKG-UTILS_NAME)-source: $(DL_DIR)/$(IPKG-UTILS_SOURCE_FILE)

$(IPKG-UTILS_NAME): $(STAGING_DIR)/usr/bin/ipkg-build

$(IPKG-UTILS_NAME)-clean:
	rm -f $(STAGING_DIR)/etc/ipkg.conf
	rm -f $(STAGING_DIR)/usr/bin/ipkg*
	rm -rf $(IPKG-UTILS_BUILD_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
TARGETS+=$(IPKG-UTILS_NAME)

