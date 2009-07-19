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
XENOMAI_ARCH:=$(shell echo $(BR2_ARCH))

XENOMAI_VERSION:=2.4.8
XENOMAI_SOURCE:=xenomai-$(XENOMAI_VERSION).tar.bz2
XENOMAI_SITE:=http://download.gna.org/xenomai/stable
XENOMAI_DIR:=$(BUILD_DIR)/xenomai-$(XENOMAI_VERSION)
XENOMAI_CAT:=bzcat
XENOMAI_BINARY:=xeno-load
XENOMAI_TARGET_BINARY:=usr/xenomai/bin/$(XENOMAI_BINARY)

ifeq ($(BR2_TARGET_ARMADEUS_APF27),y)
	CPU_TYPE:=mx2
else
ifeq ($(BR2_TARGET_ARMADEUS_APF9328),y)
	CPU_TYPE:=imx
endif
endif 

$(DL_DIR)/$(XENOMAI_SOURCE):
	$(WGET) -P $(DL_DIR) $(XENOMAI_SITE)/$(XENOMAI_SOURCE)

xenomai-source: $(DL_DIR)/$(XENOMAI_SOURCE)

$(XENOMAI_DIR)/.unpacked: $(DL_DIR)/$(XENOMAI_SOURCE)
	$(XENOMAI_CAT) $(DL_DIR)/$(XENOMAI_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(KERN_DIR)/.patched.xenomai: $(XENOMAI_DIR)/.unpacked
	 toolchain/patch-kernel.sh $(LINUX26_DIR) package/xenomai \
		\*-adeos-\*.patch				
	$(XENOMAI_DIR)/scripts/prepare-kernel.sh \
		--linux=$(LINUX26_DIR) \
		--arch=$(BR2_ARCH) \
		--default
	cat package/xenomai/xeno-kernel.config >> $(LINUX26_DIR)/.config
	touch $@

xenomai-patch-kernel: $(KERN_DIR)/.patched.xenomai

$(XENOMAI_DIR)/.configured: $(KERN_DIR)/.patched.xenomai
	(cd $(XENOMAI_DIR); rm -rf config.cache; \
                $(TARGET_CONFIGURE_OPTS) \
		                $(TARGET_CONFIGURE_ARGS) \
                CCFLAGS_FOR_BUILD="$(HOST_CFLAGS)" \
			./configure                		\
			--enable-$(XENOMAI_ARCH)-mach=$(CPU_TYPE) \
			--enable-$(XENOMAI_ARCH)-eabi=$(if $(BR2_ARM_EABI),yes,no) \
			--host=$(XENOMAI_ARCH)-linux \
			--datarootdir=/xenodoc \
	)
	touch $@

$(XENOMAI_DIR)/scripts/$(XENOMAI_BINARY): $(XENOMAI_DIR)/.configured
	$(MAKE) -C $(XENOMAI_DIR) \
		CC="$(TARGET_CC)" LD="$(TARGET_LD)"  

$(TARGET_DIR)/$(XENOMAI_TARGET_BINARY): $(XENOMAI_DIR)/scripts/$(XENOMAI_BINARY)
	echo $@
	$(STAGING_DIR)/usr/bin/fakeroot $(MAKE) -C $(XENOMAI_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_LD) \
		DESTDIR=$(TARGET_DIR) install
	rm -rf $(TARGET_DIR)/xenodoc
	echo "/usr/xenomai/lib" > $(TARGET_DIR)/etc/ld.so.conf
	echo -e 'export PATH=/usr/xenomai/bin:$${PATH}' >> $(TARGET_DIR)/etc/profile 

xenomai: uclibc host-fakeroot kernel-headers $(TARGET_DIR)/$(XENOMAI_TARGET_BINARY)

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
