#############################################################
#
# lcd4linux
#
#############################################################

LCD4LINUX_VER:=0.10.0
LCD4LINUX_SOURCE:=lcd4linux-$(LCD4LINUX_VER).tar.gz
LCD4LINUX_SITE:=http://puzzle.dl.sourceforge.net/lcd4linux/
LCD4LINUX_DIR:=$(BUILD_DIR)/lcd4linux-$(LCD4LINUX_VER)
LCD4LINUX_CAT:=zcat
LCD4LINUX_BINARY:=lcd4linux
LCD4LINUX_TARGET_BINARY:=usr/bin/lcd4linux
LCD4LINUX_PATCH:=patch-lcd4linux-armadeus
LCD4LINUX_CONF:=lcd4linux_armadeus.conf

# To be removed when integrated in buildroot:
WGET:=wget
DL_DIR:=./
BUILD_DIR:=./
TAR_OPTIONS:=x
TARGET_DIR:=/local/export/
GNU_TARGET_NAME:=arm-linux
LCD4LINUX_DIR:=$(BUILD_DIR)/lcd4linux-$(LCD4LINUX_VER)
HOSTCC:=arm-linux-gcc
TARGET_CC:=arm-linux-gcc
TARGET_STRIP:=arm-linux-strip
uclibc:
# end of to be removed

$(DL_DIR)/$(LCD4LINUX_SOURCE):
	$(WGET) -P $(DL_DIR) $(LCD4LINUX_SITE)/$(LCD4LINUX_SOURCE)

$(LCD4LINUX_DIR)/.unpacked: $(DL_DIR)/$(LCD4LINUX_SOURCE)
	$(LCD4LINUX_CAT) $(DL_DIR)/$(LCD4LINUX_SOURCE) | tar $(TAR_OPTIONS) -C $(BUILD_DIR)
	touch $(LCD4LINUX_DIR)/.unpacked

$(LCD4LINUX_DIR)/.patched: $(LCD4LINUX_DIR)/.unpacked
	cp $(LCD4LINUX_PATCH) $(LCD4LINUX_DIR)
	(cd $(LCD4LINUX_DIR); patch -p1 < $(LCD4LINUX_PATCH); cd ..);
	touch $(LCD4LINUX_DIR)/.patched

$(LCD4LINUX_DIR)/.configured: $(LCD4LINUX_DIR)/.patched
	(cd $(LCD4LINUX_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) CC_FOR_BUILD=$(HOSTCC) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--host=$(GNU_TARGET_NAME) \
	);
	touch $(LCD4LINUX_DIR)/.configured

$(LCD4LINUX_DIR)/$(LCD4LINUX_BINARY): $(LCD4LINUX_DIR)/.configured
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(LCD4LINUX_DIR)

$(TARGET_DIR)/$(LCD4LINUX_TARGET_BINARY): $(LCD4LINUX_DIR)/$(LCD4LINUX_BINARY)
	install -D $(LCD4LINUX_DIR)/$(LCD4LINUX_BINARY) $(TARGET_DIR)/$(LCD4LINUX_TARGET_BINARY)
	$(TARGET_STRIP) $(TARGET_DIR)/$(LCD4LINUX_TARGET_BINARY)
	cp $(LCD4LINUX_CONF) $(TARGET_DIR)/lcd4linux.conf

lcd4linux: uclibc $(TARGET_DIR)/$(LCD4LINUX_TARGET_BINARY)

lcd4linux-clean:
	rm -f $(TARGET_DIR)/$(LCD4LINUX_TARGET_BINARY)
	-$(MAKE) -C $(LCD4LINUX_DIR) clean

lcd4linux-dirclean:
	rm -rf $(LCD4LINUX_DIR)
#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_LCD4LINUX)),y)
TARGETS+=lcd4linux
endif
