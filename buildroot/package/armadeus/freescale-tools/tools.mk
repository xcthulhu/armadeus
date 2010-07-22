#############################################################
#
# Miscellaneous tools from Freescale BSP
#
#############################################################

FREESCALE_TOOLS_VERSION:=20071002
FREESCALE_TOOLS_SOURCE:=misc-$(FREESCALE_TOOLS_VERSION).tar.gz
FREESCALE_TOOLS_PATCH_SOURCE:=misc-vpu_lib-2.2.2-mx27-2.patch
FREESCALE_TOOLS_SITE:=http://www.bitshrine.org/gpp/
FREESCALE_TOOLS_PATCH_SITE:=ftp://ftp2.armadeus.com/armadeusw/download/
FREESCALE_TOOLS_CAT:=$(ZCAT)
FREESCALE_TOOLS_DIR:=$(BUILD_DIR)/misc-$(FREESCALE_TOOLS_VERSION)
FREESCALE_TOOLS_TARGET_DIR:=$(TARGET_DIR)/usr/bin/vpu
FREESCALE_TOOLS_BIN:=mxc_vpu_test.out


$(DL_DIR)/$(FREESCALE_TOOLS_PATCH_SOURCE):
	$(call DOWNLOAD,$(FREESCALE_TOOLS_PATCH_SITE),$(FREESCALE_TOOLS_PATCH_SOURCE))
	$(call DOWNLOAD,$(FREESCALE_TOOLS_PATCH_SITE),$(FREESCALE_TOOLS_PATCH_SOURCE).md5)

$(DL_DIR)/$(FREESCALE_TOOLS_SOURCE):
	$(call DOWNLOAD,$(FREESCALE_TOOLS_SITE),$(FREESCALE_TOOLS_SOURCE))
	$(call DOWNLOAD,$(FREESCALE_TOOLS_SITE),$(FREESCALE_TOOLS_SOURCE).md5)

freescale-tools-source: $(DL_DIR)/$(FREESCALE_TOOLS_SOURCE) $(DL_DIR)/$(FREESCALE_TOOLS_PATCH_SOURCE)

$(FREESCALE_TOOLS_DIR)/.unpacked: $(DL_DIR)/$(FREESCALE_TOOLS_SOURCE) $(DL_DIR)/$(FREESCALE_TOOLS_PATCH_SOURCE)
	$(FREESCALE_TOOLS_CAT) $(DL_DIR)/$(FREESCALE_TOOLS_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	mv $(BUILD_DIR)/misc $(FREESCALE_TOOLS_DIR)
	toolchain/patch-kernel.sh $(FREESCALE_TOOLS_DIR) $(DL_DIR) misc-\*.patch
	toolchain/patch-kernel.sh $(FREESCALE_TOOLS_DIR) package/armadeus/freescale-tools \*misc-$(FREESCALE_TOOLS_VERSION)\*.patch
	touch $@

$(FREESCALE_TOOLS_TARGET_DIR)/$(FREESCALE_TOOLS_BIN): $(FREESCALE_TOOLS_DIR)/.unpacked
	$(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" PLATFORM=IMX27ADS -C $(FREESCALE_TOOLS_DIR)/lib/vpu/
	$(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" PLATFORM=IMX27ADS \
		TOPDIR=$(FREESCALE_TOOLS_DIR) \
		OBJDIR="$(FREESCALE_TOOLS_TARGET_DIR)" \
		-C $(FREESCALE_TOOLS_DIR)/test/mxc_vpu_test/
	cp $(FREESCALE_TOOLS_DIR)/lib/vpu/*.h $(STAGING_DIR)/usr/include/
	cp $(FREESCALE_TOOLS_DIR)/lib/vpu/libvpu.a $(STAGING_DIR)/usr/lib/
	touch $@

FREESCALE-TOOLS freescale-tools: $(FREESCALE_TOOLS_TARGET_DIR)/$(FREESCALE_TOOLS_BIN)

freescale-tools-clean:
	-$(MAKE) -C $(FREESCALE_TOOLS_DIR)/lib/vpu/ clean
	-$(MAKE) OBJDIR="$(FREESCALE_TOOLS_TARGET_DIR)" \
		 -C $(FREESCALE_TOOLS_DIR)/test/mxc_vpu_test/ clean

freescale-tools-dirclean:
	rm -rf $(FREESCALE_TOOLS_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_FREESCALE_TOOLS)),y)
TARGETS+=freescale-tools
endif
