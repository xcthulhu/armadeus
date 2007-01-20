#############################################################
#
# u-boot
#
#############################################################
U-BOOT_VER:=1.1.6
U-BOOT_SOURCE:=u-boot-$(U-BOOT_VER).tar.bz2
U-BOOT_SITE:=http://ovh.dl.sourceforge.net/sourceforge/u-boot/
U-BOOT_DIR:=$(BUILD_DIR)/u-boot-$(U-BOOT_VER)

U-BOOT_PACKAGE_DIR:=$(ARMADEUS_PATH)/u-boot
U-BOOT_TARGET_NAME:=apx9328

$(DL_DIR)/$(U-BOOT_SOURCE):
	 $(WGET) -P $(DL_DIR) $(U-BOOT_SITE)/$(U-BOOT_SOURCE)

u-boot-source: $(DL_DIR)/$(U-BOOT_SOURCE)

$(U-BOOT_DIR)/.unpacked: $(DL_DIR)/$(U-BOOT_SOURCE)
	bzcat $(DL_DIR)/$(U-BOOT_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(U-BOOT_DIR) $(U-BOOT_PACKAGE_DIR) *.patch
	touch $(U-BOOT_DIR)/.unpacked	

$(U-BOOT_DIR)/.configured: $(U-BOOT_DIR)/.unpacked
	make -C $(U-BOOT_DIR) CROSS_COMPILE=$(TARGET_CROSS) $(U-BOOT_TARGET_NAME)_config
ifneq ($(BR2_TARGET_ARMADEUS_SDRAM_SIZE),)
	$(SED) "s,^#define CFG_SDRAM_MBYTE_SYZE.*,#define CFG_SDRAM_MBYTE_SYZE $(BR2_TARGET_ARMADEUS_SDRAM_SIZE),g;"\
		$(U-BOOT_DIR)/include/configs/$(U-BOOT_TARGET_NAME).h
endif

ifneq ($(BR2_TARGET_ARMADEUS_APF9328),)
	$(SED) 's,^#define CONFIG_MACH_TYPE.*,#define CONFIG_MACH_TYPE MACH_TYPE_APF9328,g' \
		$(U-BOOT_DIR)/include/configs/$(U-BOOT_TARGET_NAME).h
	$(SED) 's,^#define CONFIG_DM9000_BASE.*,#define CONFIG_DM9000_BASE    0x15C00000,g' \
		$(U-BOOT_DIR)/include/configs/$(U-BOOT_TARGET_NAME).h
	$(SED) 's,^#define DM9000_DATA.*,#define DM9000_DATA		(CONFIG_DM9000_BASE+2),g' \
		$(U-BOOT_DIR)/include/configs/$(U-BOOT_TARGET_NAME).h
	$(SED) 's,^#define CONFIG_MTDMAP.*,#define CONFIG_MTDMAP  "apf9328_flash",g' \
		$(U-BOOT_DIR)/include/configs/$(U-BOOT_TARGET_NAME).h
else
	$(SED) 's,^#define CONFIG_MACH_TYPE.*,#define CONFIG_MACH_TYPE MACH_TYPE_APX9328,g' \
		$(U-BOOT_DIR)/include/configs/$(U-BOOT_TARGET_NAME).h
	$(SED) 's,^#define CONFIG_DM9000_BASE.*,#define CONFIG_DM9000_BASE    0x15C3FFFC,g' \
		$(U-BOOT_DIR)/include/configs/$(U-BOOT_TARGET_NAME).h
	$(SED) 's,^#define DM9000_DATA.*,#define DM9000_DATA		(CONFIG_DM9000_BASE+4),g' \
		$(U-BOOT_DIR)/include/configs/$(U-BOOT_TARGET_NAME).h
	$(SED) 's,^#define CONFIG_MTDMAP.*,#define CONFIG_MTDMAP  "scb9328_flash",g' \
		$(U-BOOT_DIR)/include/configs/$(U-BOOT_TARGET_NAME).h
endif
	touch $(U-BOOT_DIR)/.configured	

$(U-BOOT_DIR)/u-boot.bin: $(U-BOOT_DIR)/.configured
	make -C $(U-BOOT_DIR) CROSS_COMPILE=$(TARGET_CROSS)
	touch -c $(U-BOOT_DIR)/u-boot.bin

u-boot: $(U-BOOT_DIR)/u-boot.bin $(U-BOOT_DIR)/u-boot.brec
	cp $(U-BOOT_DIR)/u-boot.bin .
	cp $(U-BOOT_DIR)/u-boot.brec .
	ln -fs $(U-BOOT_DIR)/tools/mkimage $(STAGING_DIR)/bin/mkimage

u-boot-clean:
	make -C $(U-BOOT_DIR) clean
	rm -rf $(U-BOOT_DIR)/tools/mkbrecimage
	rm -rf $(U-BOOT_DIR)/.configured

u-boot-distclean: u-boot-clean
	make -C $(U-BOOT_DIR) distclean

u-boot-dirclean: 
	rm -rf $(U-BOOT_DIR)

u-boot-patch: u-boot-distclean
	-mkdir -p $(BUILD_DIR)/ref
	rm -rf $(BUILD_DIR)/ref/u-boot-$(U-BOOT_VER)
	bzcat $(DL_DIR)/$(U-BOOT_SOURCE) | tar -C $(BUILD_DIR)/ref $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(BUILD_DIR)/ref/u-boot-$(U-BOOT_VER) $(U-BOOT_PACKAGE_DIR) *.patch
	touch $(BUILD_DIR)/ref/u-boot-$(U-BOOT_VER)/.unpacked
	-(cd $(BUILD_DIR); \
	diff -purN -x '*~' ref/u-boot-$(U-BOOT_VER) u-boot-$(U-BOOT_VER) > $(BUILD_DIR)/../newu-boot.diff;\
	echo -e "\n\nu-boot patch generated!\n")


#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_TARGET_U-BOOT)),y)
TARGETS+=u-boot
endif

