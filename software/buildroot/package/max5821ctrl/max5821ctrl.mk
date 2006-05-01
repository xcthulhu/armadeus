#############################################################
#
# imxregs debug tool
#
#############################################################

MAX5821CTRL_VER:=1.0
MAX5821CTRL_SOURCE:=$(BUILD_DIR)/../../target/packages/max5821ctrl
MAX5821CTRL_DIR:=$(BUILD_DIR)/max5821ctrl-$(MAX5821CTRL_VER)
MAX5821CTRL_CAT:=zcat
MAX5821CTRL_BINARY:=setDAC
MAX5821CTRL_TARGET_BINARY:=usr/bin/setDAC

$(MAX5821CTRL_DIR)/setDAC.c:
	set
	mkdir -p $(MAX5821CTRL_DIR)/
	cp $(MAX5821CTRL_SOURCE)/* $(MAX5821CTRL_DIR)/

$(MAX5821CTRL_DIR)/$(MAX5821CTRL_BINARY): $(MAX5821CTRL_DIR)/setDAC.c $(MAX5821CTRL_DIR)/max5821.h
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(MAX5821CTRL_DIR)

$(TARGET_DIR)/$(MAX5821CTRL_TARGET_BINARY): $(MAX5821CTRL_DIR)/$(MAX5821CTRL_BINARY)
	install -D $< $@
	$(STRIP) $(TARGET_DIR)/$(MAX5821CTRL_TARGET_BINARY)

max5821ctrl: uclibc $(TARGET_DIR)/$(MAX5821CTRL_TARGET_BINARY)

max5821ctrl-clean:
	rm -f $(TARGET_DIR)/$(MAX5821CTRL_TARGET_BINARY)
	-$(MAKE) -C $(MAX5821CTRL_DIR) clean

max5821ctrl-dirclean:
	rm -rf $(MAX5821CTRL_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_MAX5821CTRL)),y)
TARGETS+=max5821ctrl
endif
