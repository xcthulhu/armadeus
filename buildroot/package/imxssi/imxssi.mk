#############################################################
#
# imxssi debug tool
#
#############################################################

IMXSSI_VER:=1.0
IMXSSI_SOURCE:=$(BUILD_DIR)/../../target/linux/debug/imxssi/
IMXSSI_DIR:=$(BUILD_DIR)/imxssi-$(IMXSSI_VER)
IMXSSI_CAT:=zcat
IMXSSI_BINARY:=imxssi
IMXSSI_TARGET_BINARY:=usr/bin/imxssi

$(IMXSSI_DIR)/imxssi.c:
	mkdir -p $(IMXSSI_DIR)/
	cp $(IMXSSI_SOURCE)/* $(IMXSSI_DIR)/

$(IMXSSI_DIR)/$(IMXSSI_BINARY): $(IMXSSI_DIR)/imxssi.c
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(IMXSSI_DIR)

$(TARGET_DIR)/$(IMXSSI_TARGET_BINARY): $(IMXSSI_DIR)/$(IMXSSI_BINARY)
	install -D $< $@
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(IMXSSI_TARGET_BINARY)

imxssi: uclibc $(TARGET_DIR)/$(IMXSSI_TARGET_BINARY)

imxssi-clean:
	rm -f $(TARGET_DIR)/$(IMXSSI_TARGET_BINARY)
	-$(MAKE) -C $(IMXSSI_DIR) clean

imxssi-dirclean:
	rm -rf $(IMXSSI_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_IMXSSI)),y)
TARGETS+=imxssi
endif
