#############################################################
#
# ch7024 tool
#
#############################################################

CH7024CTRL_VER:=1.0
CH7024CTRL_SOURCE:=$(TOPDIR)/../target/packages/ch7024ctrl
CH7024CTRL_DIR:=$(BUILD_DIR)/ch7024ctrl-$(CH7024CTRL_VER)
CH7024CTRL_CAT:=zcat
CH7024CTRL_BINARY:=ch7024
CH7024CTRL_TARGET_DIR:=usr/bin
CH7024CTRL_TARGET_BINARY:=$(CH7024CTRL_TARGET_DIR)/$(CH7024CTRL_BINARY)
CH7024CTRL_CONF_FILES:=ch*x*.conf

$(CH7024CTRL_DIR)/ch7024.c:
	set
	mkdir -p $(CH7024CTRL_DIR)/
	cp $(CH7024CTRL_SOURCE)/* $(CH7024CTRL_DIR)/

$(CH7024CTRL_DIR)/$(CH7024CTRL_BINARY): $(CH7024CTRL_DIR)/ch7024.c $(CH7024CTRL_DIR)/ch7024.h
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(CH7024CTRL_DIR)

$(TARGET_DIR)/$(CH7024CTRL_TARGET_BINARY): $(CH7024CTRL_DIR)/$(CH7024CTRL_BINARY)
	install -D $< $@
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(CH7024CTRL_TARGET_BINARY)
	cp $(CH7024CTRL_DIR)/$(CH7024CTRL_CONF_FILES) $(TARGET_DIR)/$(CH7024CTRL_TARGET_DIR)

ch7024ctrl: $(TARGET_DIR)/$(CH7024CTRL_TARGET_BINARY)

ch7024ctrl-clean:
	rm -f $(TARGET_DIR)/$(CH7024CTRL_TARGET_DIR)/$(CH7024CTRL_CONF_FILES)
	rm -f $(TARGET_DIR)/$(CH7024CTRL_TARGET_BINARY)
	-$(MAKE) -C $(CH7024CTRL_DIR) clean

ch7024ctrl-dirclean:
	rm -rf $(CH7024CTRL_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_CH7024CTRL)),y)
TARGETS+=ch7024ctrl
endif
