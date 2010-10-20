#############################################################
#
# u-boot
#
#############################################################

BR2_TARGET_U_BOOT_CONFIG_BOARD=$(BR2_TARGET_UBOOT_BOARDNAME)_config
ifndef BR2_TARGET_U_BOOT_CONFIG_HEADER_FILE
BR2_TARGET_U_BOOT_CONFIG_HEADER_FILE=$(BOARD_PATH)/$(BOARD_NAME).h
endif
ifeq ($(BR2_VERSION),"0.10.0-svn")
U_BOOT_BIN=$(BOARD_NAME)-u-boot.bin
MKIMAGE_BINLOC:=$(STAGING_DIR)/usr/bin/mkimage
MKIMAGE:=$(KERNEL_CROSS)mkimage
else
# New BR:
U_BOOT_TARGET_BIN=$(BOARD_NAME)-u-boot.bin
endif

BR2_TARGET_UBOOT_ETH1ADDR=""
BR2_TARGET_UBOOT_BOOTARGS=""
BR2_TARGET_UBOOT_BOOTCMD=""

$(BINARIES_DIR)/$(BOARD_NAME)-u-boot.brec: u-boot
	mkdir -p $(BINARIES_DIR)
	cp $(U_BOOT_DIR)/u-boot.brec $@

u-boot-brec: $(BINARIES_DIR)/$(BOARD_NAME)-u-boot.brec

u-boot-distclean: u-boot-clean
	$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" ARCH=$(U_BOOT_ARCH) \
		 -C $(U_BOOT_DIR) distclean
	rm -f $(U_BOOT_DIR)/.configured

ifeq ($(BR2_VERSION),"0.10.0-svn")
$(MKIMAGE): $(BINARIES_DIR)/$(U_BOOT_BIN)
	ln -fs $(MKIMAGE_BINLOC) $(MKIMAGE)
endif
u-boot-patch: u-boot-distclean
	-mkdir -p $(PROJECT_BUILD_DIR)/ref
	rm -rf $(PROJECT_BUILD_DIR)/ref/u-boot-$(U_BOOT_VERSION)
	bzcat $(DL_DIR)/$(U_BOOT_SOURCE) | tar -C $(PROJECT_BUILD_DIR)/ref \
		$(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(PROJECT_BUILD_DIR)/ref/u-boot-$(U_BOOT_VERSION) \
	target/u-boot/ u-boot-$(U_BOOT_VERSION)-\*.patch \
		u-boot-$(U_BOOT_VERSION)-\*.patch.$(ARCH)
	touch $(PROJECT_BUILD_DIR)/ref/u-boot-$(U_BOOT_VERSION)/.unpacked
	-@(cd $(PROJECT_BUILD_DIR); \
	diff -purN -x '*~' -x .depend ref/u-boot-$(U_BOOT_VERSION) \
		u-boot-$(U_BOOT_VERSION) > u-boot.patch;\
	echo ; echo -e u-boot patch generated!;echo ;)

ifeq ($(BR2_PACKAGE_LINUX),y)
ifeq ($(BR2_VERSION),"0.10.0-svn")
TARGETS+=$(MKIMAGE)
endif
endif
