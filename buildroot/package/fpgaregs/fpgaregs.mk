#############################################################
#
# fpgaregs debug tool
#
#############################################################

FPGAREGS_VER:=1.0
FPGAREGS_SOURCE:=$(BUILD_DIR)/../../target/linux/debug/fpgaregs/
FPGAREGS_DIR:=$(BUILD_DIR)/fpgaregs-$(FPGAREGS_VER)
FPGAREGS_CAT:=zcat
FPGAREGS_BINARY:=fpgaregs
FPGAREGS_TARGET_BINARY:=usr/bin/fpgaregs

$(FPGAREGS_DIR)/fpgaregs.c:
	mkdir -p $(FPGAREGS_DIR)/
	cp $(FPGAREGS_SOURCE)/* $(FPGAREGS_DIR)/

$(FPGAREGS_DIR)/$(FPGAREGS_BINARY): $(FPGAREGS_DIR)/fpgaregs.c
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(FPGAREGS_DIR)

$(TARGET_DIR)/$(FPGAREGS_TARGET_BINARY): $(FPGAREGS_DIR)/$(FPGAREGS_BINARY)
	install -D $< $@
#	$(STRIP) $(TARGET_DIR)/$(FPGAREGS_TARGET_BINARY)

fpgaregs: uclibc $(TARGET_DIR)/$(FPGAREGS_TARGET_BINARY)

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
