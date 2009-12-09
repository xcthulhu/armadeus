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

ifeq ($(BR2_TARGET_ARMADEUS_APF27),y)
IMXSSI_DEFINES += -DIMX27
endif
ifeq ($(BR2_TARGET_ARMADEUS_APF9328),y)
IMXSSI_DEFINES += -DIMXL
endif

$(IMXSSI_DIR)/imxssi.c:
	mkdir -p $(IMXSSI_DIR)/
	cp -Lr $(IMXSSI_SOURCE)/* $(IMXSSI_DIR)/

$(IMXSSI_DIR)/.configured: $(IMXSSI_DIR)/imxssi.c
	(cd $(IMXSSI_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_ARGS) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(IMXSSI_DEFINES)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--sysconfdir=/etc \
        )
	touch $@

$(IMXSSI_DIR)/$(IMXSSI_BINARY): $(IMXSSI_DIR)/.configured
	$(MAKE) -C $(IMXSSI_DIR)

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
