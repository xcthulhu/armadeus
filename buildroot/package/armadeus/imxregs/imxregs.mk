#############################################################
#
# imxregs debug tool
#
#############################################################

IMXREGS_VER:=1.0
IMXREGS_SOURCE:=$(TOPDIR)/../target/linux/debug/imxregs/
IMXREGS_DIR:=$(BUILD_DIR)/imxregs-$(IMXREGS_VER)
IMXREGS_CAT:=zcat
ifeq ($(BR2_TARGET_ARMADEUS_APF27),y)
IMXREGS_BINARY:=imx27regs
else
IMXREGS_BINARY:=imxregs
endif
IMXREGS_TARGET_BINARY:=usr/bin/imxregs

$(IMXREGS_DIR)/imxregs.c:
	mkdir -p $(IMXREGS_DIR)/
	cp $(IMXREGS_SOURCE)/* $(IMXREGS_DIR)/

$(IMXREGS_DIR)/$(IMXREGS_BINARY): $(IMXREGS_DIR)/imxregs.c
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(IMXREGS_DIR)

$(TARGET_DIR)/$(IMXREGS_TARGET_BINARY): $(IMXREGS_DIR)/$(IMXREGS_BINARY)
	install -D $< $@
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(IMXREGS_TARGET_BINARY)

imxregs: $(TARGET_DIR)/$(IMXREGS_TARGET_BINARY)

imxregs-clean:
	rm -f $(TARGET_DIR)/$(IMXREGS_TARGET_BINARY)
	-$(MAKE) -C $(IMXREGS_DIR) clean

imxregs-dirclean:
	rm -rf $(IMXREGS_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_IMXREGS)),y)
TARGETS+=imxregs
endif
