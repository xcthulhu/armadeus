#############################################################
#
# LCD4Linux daemon
#
#############################################################

LCD4LINUX_VER:=0.10.0
LCD4LINUX_SOURCE:=lcd4linux-$(LCD4LINUX_VER).tar.gz
LCD4LINUX_SITE:=http://puzzle.dl.sourceforge.net/lcd4linux/
LCD4LINUX_DIR:=$(BUILD_DIR)/lcd4linux-$(LCD4LINUX_VER)
LCD4LINUX_CAT:=zcat
LCD4LINUX_BINARY:=lcd4linux
LCD4LINUX_TARGET_BINARY:=usr/bin/lcd4linux
LCD4LINUX_PATCH:=lcd4linux_armadeus.patch
LCD4LINUX_CONF:=lcd4linux_armadeus.conf
LCD4LINUX_TARGET_CONF:=etc/lcd4linux.conf


$(DL_DIR)/$(LCD4LINUX_SOURCE):
	$(WGET) -P $(DL_DIR) $(LCD4LINUX_SITE)/$(LCD4LINUX_SOURCE)

$(LCD4LINUX_DIR)/.unpacked: $(DL_DIR)/$(LCD4LINUX_SOURCE)
	$(LCD4LINUX_CAT) $(DL_DIR)/$(LCD4LINUX_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(LCD4LINUX_DIR)/.patched: $(LCD4LINUX_DIR)/.unpacked
	toolchain/patch-kernel.sh $(LCD4LINUX_DIR) package/lcd4linux lcd4linux\*.patch
	(if [ -d $(BUILD_DIR)/linux-2.6.12/include/asm-generic ]; then ln -sf $(BUILD_DIR)/linux-2.6.12/include/asm-generic/ $(BUILD_DIR)/staging_dir/arm-linux-uclibc/sys-include/asm-generic; fi)
	touch $@

$(LCD4LINUX_DIR)/.configured: $(LCD4LINUX_DIR)/.patched
	(cd $(LCD4LINUX_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) CC_FOR_BUILD=$(HOSTCC) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--host=$(GNU_TARGET_NAME) \
	);
	touch $@

$(LCD4LINUX_DIR)/$(LCD4LINUX_BINARY): $(LCD4LINUX_DIR)/.configured
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(LCD4LINUX_DIR)

$(TARGET_DIR)/$(LCD4LINUX_TARGET_BINARY): $(LCD4LINUX_DIR)/$(LCD4LINUX_BINARY)
	#cp -f $< $@	
	install -D $< $@
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $@
	cp package/lcd4linux/$(LCD4LINUX_CONF) $(TARGET_DIR)/$(LCD4LINUX_TARGET_CONF)

lcd4linux: uclibc ncurses $(TARGET_DIR)/$(LCD4LINUX_TARGET_BINARY)

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
