#############################################################
#
# imxregs debug tool
#
#############################################################

IMXREGS_VER:=1.0
IMXREGS_SOURCE:=$(TOPDIR)/../target/linux/debug/imxregs/
IMXREGS_DIR:=$(BUILD_DIR)/imxregs-$(IMXREGS_VER)
IMXREGS_CAT:=zcat
CPU_NAME=$(call qstrip,$(BR2_CPU_NAME))
ifeq ($(CPU_NAME),imx27)
IMXREGS_BINARY:=imx27regs
else
 ifeq ($(CPU_NAME),imxl)
 IMXREGS_BINARY:=imxregs
 else
 IMXREGS_BINARY:=unknown_arch
 endif
endif
IMXREGS_TARGET_BINARY:=usr/bin/imxregs

$(IMXREGS_DIR)/imxregs.c:
	mkdir -p $(IMXREGS_DIR)/
	cp $(IMXREGS_SOURCE)/* $(IMXREGS_DIR)/

$(IMXREGS_DIR)/$(IMXREGS_BINARY): $(IMXREGS_DIR)/imxregs.c
	$(MAKE) CC="$(TARGET_CC)" -C $(IMXREGS_DIR)

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
