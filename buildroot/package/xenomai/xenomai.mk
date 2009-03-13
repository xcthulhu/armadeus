#############################################################
#
# xenomai : 
# URL  : http://xenomai.org
# NOTE : Real-Time Framework for Linux
#
# adeos :
# URL   : http://home.gna.org/adeos/
# NOTE  : The purpose of Adeos is to provide a flexible 
#         environment for sharing hardware resources among 
#         multiple operating systems, or among multiple 
#         instances of a single OS.
#
#############################################################

KERN_DIR:=$(PROJECT_BUILD_DIR)/linux-$(shell echo $(BR2_LINUX26_VERSION))
XENOMAI_VERSION:=2.4.7
XENOMAI_SOURCE:=xenomai-$(XENOMAI_VERSION).tar.bz2
XENOMAI_SITE:=http://download.gna.org/xenomai/stable
XENOMAI_DIR:=$(BUILD_DIR)/xenomai-$(XENOMAI_VERSION)
XENOMAI_CAT:=bzcat
XENOMAI_BINARY:=xeno-load
XENOMAI_TARGET_BINARY:=usr/xenomai/bin/xeno-load

ADEOS_VERSION:=1.12-00
ADEOS_SOURCE:=adeos-ipipe-$(shell echo $(BR2_KERNEL_THIS_VERSION))-arm-$(ADEOS_VERSION).patch
ADEOS_SITE:=http://download.gna.org/adeos/patches/v2.6/arm/older/

$(DL_DIR)/$(XENOMAI_SOURCE):
	$(WGET) -P $(DL_DIR) $(XENOMAI_SITE)/$(XENOMAI_SOURCE)

$(DL_DIR)/$(ADEOS_SOURCE):	
	$(WGET) -P $(DL_DIR) $(ADEOS_SITE)$(ADEOS_SOURCE)

xenomai-source: $(DL_DIR)/$(ADEOS_SOURCE)

$(XENOMAI_DIR)/.unpacked: $(DL_DIR)/$(ADEOS_SOURCE) $(DL_DIR)/$(XENOMAI_SOURCE)
	$(XENOMAI_CAT) $(DL_DIR)/$(XENOMAI_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@



$(XENOMAI_DIR)/.patchXeno: $(XENOMAI_DIR)/.unpacked
	toolchain/patch-kernel.sh $(XENOMAI_DIR) package/xenomai \
		02-xenomai-supprSudo.patch
	touch $(XENOMAI_DIR)/.patchXeno


$(KERN_DIR)/.patch_xenomai: $(XENOMAI_DIR)/.patchXeno
	@echo "$@"
	toolchain/patch-kernel.sh $(LINUX26_DIR) package/xenomai \
		04-suppr-patch.patch
	toolchain/patch-kernel.sh $(LINUX26_DIR) $(DL_DIR) \
		$(ADEOS_SOURCE)
	toolchain/patch-kernel.sh $(LINUX26_DIR) package/xenomai \
		01-adeos-2.6.27-1.12-00.patch
	$(XENOMAI_DIR)/scripts/prepare-kernel.sh \
		--linux=${LINUX_DIR} \
		--arch=arm
	cat package/xenomai/configXenoKernel >> $(LINUX26_DIR)/.config
	touch $@
	
xenomai-patch-kernel: $(KERN_DIR)/.patch_xenomai

$(XENOMAI_DIR)/.configured: $(KERN_DIR)/.patch_xenomai
	(cd $(XENOMAI_DIR); rm -rf config.cache; \
                $(TARGET_CONFIGURE_OPTS) \
		                $(TARGET_CONFIGURE_ARGS) \
                CCFLAGS_FOR_BUILD="$(HOST_CFLAGS)" \
			./configure \
			--enable-arm-mach=imx \
			--host=arm-linux \
			--datarootdir=/xenodoc \
	)
	touch $@

$(XENOMAI_DIR)/scripts/$(XENOMAI_BINARY): $(XENOMAI_DIR)/.configured
	$(MAKE) -C $(XENOMAI_DIR) \
		CC="$(TARGET_CC)" LD="$(TARGET_LD)"  

$(TARGET_DIR)/$(XENOMAI_TARGET_BINARY): $(XENOMAI_DIR)/scripts/$(XENOMAI_BINARY)
	echo $@
	$(MAKE) -C $(XENOMAI_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_LD) \
		DESTDIR=$(TARGET_DIR) install
	rm -rf $(TARGET_DIR)/xenodoc
	echo "/usr/xenomai/lib" > $(TARGET_DIR)/etc/ld.so.conf
	echo -e 'export PATH=/usr/xenomai/bin:$${PATH}' >> $(TARGET_DIR)/etc/profile 

xenomai: uclibc kernel-headers $(TARGET_DIR)/$(XENOMAI_TARGET_BINARY)

xenomai-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(XENOMAI_DIR) uninstall
	-$(MAKE) DESTDIR=$(TARGET_DIR) -C $(XENOMAI_DIR) clean

xenomai-dirclean: xenomai-clean
	rm -rf $(XENOMAI_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_XENOMAI)),y)
TARGETS+=xenomai
endif
