#############################################################
#
# Mesa
#
#############################################################

MESALIB_VER:=7.0
MESALIB_SOURCE:=MesaLib-$(MESALIB_VER).tar.bz2
MESALIB_DEMOS:=MesaDemos-$(MESALIB_VER).tar.bz2
MESAGLUT_SOURCE:=MesaGLUT-$(MESALIB_VER).tar.bz2
MESALIB_SITE:=http://downloads.sourceforge.net/mesa3d
MESALIB_DIR:=$(BUILD_DIR)/Mesa-$(MESALIB_VER)
MESALIB_CAT:=bzcat
MESALIB_PATCH:=mesa.patch


$(DL_DIR)/$(MESALIB_SOURCE):
	$(WGET) -P $(DL_DIR) $(MESALIB_SITE)/$(MESALIB_SOURCE)
	
$(DL_DIR)/$(MESALIB_DEMOS):
	$(WGET) -P $(DL_DIR) $(MESALIB_SITE)/$(MESALIB_DEMOS)

$(DL_DIR)/$(MESAGLUT_SOURCE):
	$(WGET) -P $(DL_DIR) $(MESALIB_SITE)/$(MESAGLUT_SOURCE)

mesa-source: $(DL_DIR)/$(MESALIB_SOURCE) $(DL_DIR)/$(MESALIB_DEMOS)

$(MESALIB_DIR)/.unpacked: $(DL_DIR)/$(MESALIB_SOURCE) $(DL_DIR)/$(MESALIB_DEMOS) $(DL_DIR)/$(MESAGLUT_SOURCE)
	$(MESALIB_CAT) $(DL_DIR)/$(MESALIB_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(MESALIB_CAT) $(DL_DIR)/$(MESALIB_DEMOS) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(MESALIB_CAT) $(DL_DIR)/$(MESAGLUT_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(MESALIB_DIR)/.patched: $(MESALIB_DIR)/.unpacked
	toolchain/patch-kernel.sh $(MESALIB_DIR) package/mesa $(MESALIB_PATCH)
	touch $@

$(MESALIB_DIR)/.configured: $(MESALIB_DIR)/.patched
	touch $@

$(MESALIB_DIR)/.compiled: $(MESALIB_DIR)/.configured
	(cd $(MESALIB_DIR); \
		$(MAKE) realclean; \
        $(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CXX) linux-fbdev \
    );
#arm-linux-osmesa
	touch $@

MESA mesa: uclibc gpm $(MESALIB_DIR)/.compiled

mesa-clean:
	(cd $(MESALIB_DIR); \
        $(MAKE) CC=$(TARGET_CC) clean \
    );

mesa-dirclean:
	rm -rf $(MESALIB_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_MESALIB)),y)
TARGETS+=mesa
endif
