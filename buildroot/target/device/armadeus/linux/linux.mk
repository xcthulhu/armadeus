#############################################################
#
# Linux kernel targets
#
# Note:  If you have any patches to apply, create the directory
# sources/kernel-patches and put your patches in there and number
# them in the order you wish to apply them...  i.e.
#
#   sources/kernel-patches/001-my-special-stuff.bz2
#   sources/kernel-patches/003-gcc-Os.bz2
#   sources/kernel-patches/004_no-warnings.bz2
#   sources/kernel-patches/030-lowlatency-mini.bz2
#   sources/kernel-patches/031-lowlatency-fixes-5.bz2
#   sources/kernel-patches/099-shutup.bz2
#   etc...
#
# these patches will all be applied by the patch-kernel.sh
# script (which will also abort the build if it finds rejects)
#  -Erik
#
#############################################################
# Get linux version choosen by user (2.6.12 by default)
ifeq ($(BR2_PACKAGE_LINUX_VERSION),)
BR2_PACKAGE_LINUX_VERSION:=2.6.12
endif
# Remove " from string
BR2_PACKAGE_C_LINUX_VERSION:=$(shell echo $(BR2_PACKAGE_LINUX_VERSION)| sed -e 's/"//g')
# Base version of Linux kernel that we need to download
DOWNLOAD_LINUX_VERSION:=$(BR2_PACKAGE_C_LINUX_VERSION)
# Version of Linux kernel AFTER applying all patches
LINUX_VERSION:=$(BR2_PACKAGE_C_LINUX_VERSION)


# File name for the Linux kernel binary
LINUX_KERNEL=linux-kernel-$(LINUX_VERSION)-$(ARCH).bin

LINUX_PACKAGE_DIR:=$(ARMADEUS_PATH)/linux

# Linux kernel configuration file ?? overwritten by apX9328/Makefile.in ??
LINUX_KCONFIG:=$($(LINUX_PACKAGE_DIR)/linux-$(LINUX_VERSION).config)

# kernel patches
LINUX_PATCH_DIR:=$(LINUX_PACKAGE_DIR)/kernel-patches/$(BR2_PACKAGE_C_LINUX_VERSION)
CYGWIN_LINUX_PATCH_DIR:=$(LINUX_PATCH_DIR)/cygwin
LINUX_PATCH_FILTER:=$(BR2_PACKAGE_C_LINUX_VERSION)




LINUX_FORMAT=vmlinux
LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	)
LINUX_BINLOC=/arch/$(LINUX_KARCH)/boot/$(LINUX_FORMAT)
LINUX_DIR=$(BUILD_DIR)/linux-$(LINUX_VERSION)
LINUX_SOURCE=linux-$(DOWNLOAD_LINUX_VERSION).tar.bz2
LINUX_SITE=http://www.kernel.org/pub/linux/kernel/v2.6
# Used by pcmcia-cs and others
LINUX_SOURCE_DIR=$(LINUX_DIR)


$(DL_DIR)/$(LINUX_SOURCE):
	-mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(LINUX_SITE)/$(LINUX_SOURCE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE)
	-mkdir -p $(TOOL_BUILD_DIR)
	-(cd $(TOOL_BUILD_DIR); ln -snf $(LINUX_DIR) linux)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
ifneq ($(DOWNLOAD_LINUX_VERSION),$(LINUX_VERSION))
	# Rename the dir from the downloaded version to the AFTER patch version
	mv -f $(BUILD_DIR)/linux-$(DOWNLOAD_LINUX_VERSION) $(BUILD_DIR)/linux-$(LINUX_VERSION)
endif
	toolchain/patch-kernel.sh $(LINUX_DIR) $(LINUX_PATCH_DIR) \*.diff
	if uname | grep -i cygwin >/dev/null 2>&1 ; then  \
		toolchain/patch-kernel.sh $(LINUX_DIR) $(CYGWIN_LINUX_PATCH_DIR) ; \
	fi;
	touch $@

$(LINUX_KCONFIG):
	@if [ ! -f "$(LINUX_KCONFIG)" ] ; then \
		echo ""; \
		echo "You should create a .config for your kernel"; \
		echo "and install it as $(LINUX_KCONFIG)"; \
		echo ""; \
		sleep 5; \
	fi;

$(LINUX_DIR)/.configured $(BUILD_DIR)/linux/.configured:  $(LINUX_DIR)/.unpacked  $(LINUX_KCONFIG)
	$(SED) "s,^ARCH.*,ARCH=$(LINUX_KARCH),g;" $(LINUX_DIR)/Makefile
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" $(LINUX_DIR)/Makefile
#	$(SED) "s,^EXTRAVERSION =.*,EXTRAVERSION =,g;" $(LINUX_DIR)/Makefile
	-cp $(LINUX_KCONFIG) $(LINUX_DIR)/.config
ifeq ($(strip $(BR2_mips)),y)
	$(SED) "s,CONFIG_CPU_LITTLE_ENDIAN=y,# CONFIG_CPU_LITTLE_ENDIAN is not set\n# CONFIG_BINFMT_IRIX is not set," $(LINUX_DIR)/.config
endif
ifneq ($(BR2_TARGET_ARMADEUS_APF9328),)
	$(SED) 's,db->io_data = (void *).*,db->io_data = (void *)(base + 2 );,g' \
		$(LINUX_DIR)/drivers/net/dm9000.c
else
	$(SED) 's,db->io_data = (void *).*,db->io_data = (void *)(base + 4 );,g' \
		$(LINUX_DIR)/drivers/net/dm9000.c
endif
	(cd $(LINUX_DIR)/drivers; ln -sf ../../../../target/linux/modules/ armadeus)
	$(MAKE) PATH=$(TARGET_PATH) -C $(LINUX_DIR) oldconfig include/linux/version.h
	touch $@

$(LINUX_DIR)/.depend_done:  $(LINUX_DIR)/.configured
# 	make dep no more required with kernel 2.6.x 
#	$(MAKE) PATH=$(TARGET_PATH) -C $(LINUX_DIR) dep
	touch $@

$(LINUX_DIR)/$(LINUX_BINLOC): $(LINUX_DIR)/.depend_done u-boot
	$(MAKE) PATH=$(TARGET_PATH) -C $(LINUX_DIR) $(LINUX_FORMAT)
	$(MAKE) PATH=$(TARGET_PATH) -C $(LINUX_DIR) modules
	# Armadeus custom modules compilation:
	$(MAKE) PATH=$(TARGET_PATH) LINUX_DIR=$(LINUX_DIR) -C $(LINUX_DIR)/drivers/armadeus

$(LINUX_KERNEL): $(LINUX_DIR)/$(LINUX_BINLOC)
	cp -fa $(LINUX_DIR)/$(LINUX_BINLOC) $(LINUX_KERNEL)
	touch -c $(LINUX_KERNEL)

$(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/modules.dep: $(LINUX_KERNEL)
	rm -rf $(TARGET_DIR)/lib/modules
	rm -f $(TARGET_DIR)/sbin/cardmgr
	$(MAKE) PATH=$(TARGET_PATH) -C $(LINUX_DIR) DEPMOD=`which true` \
		INSTALL_MOD_PATH=$(TARGET_DIR) modules_install
	# Armadeus custom modules install:
	$(MAKE) -C $(LINUX_DIR)/drivers/armadeus DEPMOD=`which true` TARGET_DIR=$(TARGET_DIR) install	
	(cd $(TARGET_DIR)/lib/modules; ln -s $(LINUX_VERSION)/kernel/drivers .)
	$(LINUX_PACKAGE_DIR)/depmod.pl \
		-b $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/ \
		-k $(LINUX_DIR)/vmlinux \
		-F $(LINUX_DIR)/System.map \
		> $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/modules.dep

$(STAGING_DIR)/include/linux/version.h: $(LINUX_DIR)/.configured
	mkdir -p $(STAGING_DIR)/include
	tar -ch -C $(LINUX_DIR)/include -f - linux | tar -xf - -C $(STAGING_DIR)/include/
	tar -ch -C $(LINUX_DIR)/include -f - asm | tar -xf - -C $(STAGING_DIR)/include/

linux: $(STAGING_DIR)/include/linux/version.h $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/modules.dep

linux-source: $(DL_DIR)/$(LINUX_SOURCE)

# This has been renamed so we do _NOT_ by default run this on 'make clean'
linux-clean:
	rm -f $(LINUX_KERNEL)
	-$(MAKE) PATH=$(TARGET_PATH) -C $(LINUX_DIR) clean

linux-mrproper:
	-$(MAKE) PATH=$(TARGET_PATH) -C $(LINUX_DIR) mrproper
	rm -rf $(LINUX_DIR)/.configured
	rm -rf $(LINUX_DIR)/.depend_done

linux-dirclean:
	rm -rf $(LINUX_DIR)

linux-patch: linux-clean linux-mrproper
	-mkdir -p $(BUILD_DIR)/ref
	rm -rf $(BUILD_DIR)/ref/linux-$(LINUX_VERSION)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(BUILD_DIR)/ref $(TAR_OPTIONS) -
ifneq ($(DOWNLOAD_LINUX_VERSION),$(LINUX_VERSION))
	# Rename the dir from the downloaded version to the AFTER patch version
	mv -f $(BUILD_DIR)/ref/linux-$(DOWNLOAD_LINUX_VERSION) $(BUILD_DIR)/ref/linux-$(LINUX_VERSION)
endif
	toolchain/patch-kernel.sh $(BUILD_DIR)/ref/linux-$(LINUX_VERSION) $(LINUX_PATCH_DIR)
	$(SED) "s,^ARCH.*,ARCH=$(LINUX_KARCH),g;" $(BUILD_DIR)/ref/linux-$(LINUX_VERSION)/Makefile
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" $(BUILD_DIR)/ref/linux-$(LINUX_VERSION)/Makefile
	touch $(BUILD_DIR)/ref/linux-$(LINUX_VERSION)/.unpacked
	-(cd $(BUILD_DIR); \
	diff -purN -x '*~' ref/linux-$(LINUX_VERSION) linux-$(LINUX_VERSION) > $(BUILD_DIR)/../newlinux.diff)

ifeq ($(strip $(BR2_PACKAGE_LINUX)),y)
TARGETS+=linux
endif
