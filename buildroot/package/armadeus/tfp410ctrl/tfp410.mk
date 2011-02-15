#############################################################
#
# tfp410 control tool
#
#############################################################

TFP410CTRL_VER:=1.0
TFP410CTRL_SOURCE:=$(TOPDIR)/../target/packages/tfp410ctrl
TFP410CTRL_DIR:=$(BUILD_DIR)/tfp410ctrl-$(TFP410CTRL_VER)
TFP410CTRL_CAT:=zcat
TFP410CTRL_BINARY:=tfp410ctrl
TFP410CTRL_TARGET_BINARY:=usr/bin/tfp410ctrl

$(TFP410CTRL_DIR)/tfp410ctrl.c:
	mkdir -p $(TFP410CTRL_DIR)/
	cp $(TFP410CTRL_SOURCE)/* $(TFP410CTRL_DIR)/

$(TFP410CTRL_DIR)/$(TFP410CTRL_BINARY): $(TFP410CTRL_DIR)/tfp410ctrl.c $(TFP410CTRL_DIR)/tfp410ctrl.h
	$(MAKE) CC="$(TARGET_CC)" -C $(TFP410CTRL_DIR)

$(TARGET_DIR)/$(TFP410CTRL_TARGET_BINARY): $(TFP410CTRL_DIR)/$(TFP410CTRL_BINARY)
	install -D $< $@
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(TFP410CTRL_TARGET_BINARY)

tfp410ctrl: $(TARGET_DIR)/$(TFP410CTRL_TARGET_BINARY)

tfp410ctrl-clean:
	rm -f $(TARGET_DIR)/$(TFP410CTRL_TARGET_BINARY)
	-$(MAKE) -C $(TFP410CTRL_DIR) clean

tfp410ctrl-dirclean:
	rm -rf $(TFP410CTRL_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_TFP410CTRL)),y)
TARGETS+=tfp410ctrl
endif
