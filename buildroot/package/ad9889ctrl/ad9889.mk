#############################################################
#
# ad9889 tool
#
#############################################################

AD9889CTRL_VER:=1.0
AD9889CTRL_SOURCE:=$(BUILD_DIR)/../../target/packages/ad9889ctrl
AD9889CTRL_DIR:=$(BUILD_DIR)/ad9889ctrl-$(AD9889CTRL_VER)
AD9889CTRL_CAT:=zcat
AD9889CTRL_BINARY:=ad9889ctrl
AD9889CTRL_TARGET_BINARY:=usr/bin/ad9889ctrl

$(AD9889CTRL_DIR)/ad9889ctrl.c:
	set
	mkdir -p $(AD9889CTRL_DIR)/
	cp $(AD9889CTRL_SOURCE)/* $(AD9889CTRL_DIR)/

$(AD9889CTRL_DIR)/$(AD9889CTRL_BINARY): $(AD9889CTRL_DIR)/ad9889ctrl.c $(AD9889CTRL_DIR)/ad9889ctrl.h
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(AD9889CTRL_DIR)

$(TARGET_DIR)/$(AD9889CTRL_TARGET_BINARY): $(AD9889CTRL_DIR)/$(AD9889CTRL_BINARY)
	install -D $< $@
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(AD9889CTRL_TARGET_BINARY)

ad9889ctrl: uclibc $(TARGET_DIR)/$(AD9889CTRL_TARGET_BINARY)

ad9889ctrl-clean:
	rm -f $(TARGET_DIR)/$(AD9889CTRL_TARGET_BINARY)
	-$(MAKE) -C $(AD9889CTRL_DIR) clean

ad9889ctrl-dirclean:
	rm -rf $(AD9889CTRL_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_AD9889CTRL)),y)
TARGETS+=ad9889ctrl
endif
