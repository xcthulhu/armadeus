#############################################################
#
# rxtx lib for java
#
#############################################################
RXTX_LIB_VERSION:=2.1-7r2
RXTX_LIB_SOURCE:=rxtx-$(RXTX_LIB_VERSION).zip
RXTX_LIB_SITE:=http://rxtx.qbang.org/pub/rxtx/
RXTX_LIB_DIR:=$(BUILD_DIR)/rxtx-$(RXTX_LIB_VERSION)
RXTX_LIB_BINARY:=RXTXcomm.jar
RXTX_LIB_TARGET_BINARY:=usr/lib/classpath

$(DL_DIR)/$(RXTX_LIB_SOURCE):
	$(WGET) -P $(DL_DIR) $(RXTX_LIB_SITE)/$(RXTX_LIB_SOURCE)

$(RXTX_LIB_DIR)/.unpacked: $(DL_DIR)/$(RXTX_LIB_SOURCE)
	unzip $(DL_DIR)/$(RXTX_LIB_SOURCE) -d $(BUILD_DIR)
	toolchain/patch-kernel.sh $(RXTX_LIB_DIR) package/armadeus/rxtx *.patch
	touch $@

$(RXTX_LIB_DIR)/.configured: $(RXTX_LIB_DIR)/.unpacked
	(cd $(RXTX_LIB_DIR); rm -rf config.cache; \
	$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_CONFIGURE_ARGS) \
	./configure \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix=/usr \
	--sysconfdir=/etc \
	)
	touch $@

$(RXTX_LIB_DIR)/$(RXTX_LIB_BINARY): $(RXTX_LIB_DIR)/.configured
	$(MAKE)  -C $(RXTX_LIB_DIR)

$(TARGET_DIR)/$(RXTX_LIB_TARGET_BINARY): $(RXTX_LIB_DIR)/$(RXTX_LIB_BINARY)
	cp -rdpf $(RXTX_LIB_DIR)/arm-unknown-linux-gnu/.libs/librxtx*.so* $(TARGET_DIR)/$(RXTX_LIB_TARGET_BINARY)
	cp -rdpf $(RXTX_LIB_DIR)/RXTXcomm.jar $(TARGET_DIR)/$(RXTX_LIB_TARGET_BINARY)
	-$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(RXTX_LIB_TARGET_BINARY)/librxtx*.so*

rxtx: uclibc $(TARGET_DIR)/$(RXTX_LIB_TARGET_BINARY)

rxtx-source: $(DL_DIR)/$(RXTX_LIB_SOURCE)

rxtx-clean:
	rm -f $(addprefix $(TARGET_DIR)/$(RXTX_LIB_TARGET_BINARY),librxtx*.so* RXTXcomm.jar)
	-$(MAKE) -C $(RXTX_LIB_DIR) clean

rxtx-dirclean:
	rm -rf $(RXTX_LIB_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_RXTX)),y)
TARGETS+=rxtx
endif
