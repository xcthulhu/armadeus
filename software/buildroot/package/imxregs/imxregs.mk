#############################################################
#
# imxregs debug tool
#
#############################################################

IMXREGS_VER:=1.0
IMXREGS_SOURCE:=$(BUILD_DIR)/../../target/linux/debug/imxregs/
IMXREGS_DIR:=$(BUILD_DIR)/imxregs-$(IMXREGS_VER)
IMXREGS_CAT:=zcat
IMXREGS_BINARY:=imxregs
IMXREGS_TARGET_BINARY:=usr/bin/imxregs

$(IMXREGS_DIR)/imxregs.c:
	mkdir -p $(IMXREGS_DIR)/
	cp $(IMXREGS_SOURCE)/* $(IMXREGS_DIR)/

$(IMXREGS_DIR)/$(IMXREGS_BINARY): $(IMXREGS_DIR)/imxregs.c
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(IMXREGS_DIR)

$(TARGET_DIR)/$(IMXREGS_TARGET_BINARY): $(IMXREGS_DIR)/$(IMXREGS_BINARY)
	install -D $< $@
	$(STRIP) $(TARGET_DIR)/$(IMXREGS_TARGET_BINARY)

imxregs: uclibc $(TARGET_DIR)/$(IMXREGS_TARGET_BINARY)

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
