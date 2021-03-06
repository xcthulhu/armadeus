#############################################################
#
# fpgaregs debug tool
#
#############################################################

FPGAREGS_VER:=1.1
FPGAREGS_SOURCE:=$(TOPDIR)/../target/linux/debug/fpgaregs/
FPGAREGS_DIR:=$(BUILD_DIR)/fpgaregs-$(FPGAREGS_VER)
FPGAREGS_CAT:=zcat
ifeq ($(BR2_CPU_NAME),"imx27")
FPGAREGS_BINARY:=fpga27regs
else
 ifeq ($(BR2_CPU_NAME),"imx51")
 FPGAREGS_BINARY:=fpga51regs
 else
 FPGAREGS_BINARY:=fpgaregs
 endif
endif
FPGAREGS_TARGET_BINARY:=usr/bin/fpgaregs

$(FPGAREGS_DIR)/fpgaregs.c:
	mkdir -p $(FPGAREGS_DIR)/
	cp $(FPGAREGS_SOURCE)/* $(FPGAREGS_DIR)/

$(FPGAREGS_DIR)/$(FPGAREGS_BINARY): $(FPGAREGS_DIR)/fpgaregs.c
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC="$(TARGET_CC)" -C $(FPGAREGS_DIR)

$(TARGET_DIR)/$(FPGAREGS_TARGET_BINARY): $(FPGAREGS_DIR)/$(FPGAREGS_BINARY)
	install -D $< $@
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(FPGAREGS_TARGET_BINARY)

fpgaregs: $(TARGET_DIR)/$(FPGAREGS_TARGET_BINARY)

fpgaregs-clean:
	rm -f $(TARGET_DIR)/$(FPGAREGS_TARGET_BINARY)
	-$(MAKE) -C $(FPGAREGS_DIR) clean

fpgaregs-dirclean:
	rm -rf $(FPGAREGS_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_FPGAREGS)),y)
TARGETS+=fpgaregs
endif
