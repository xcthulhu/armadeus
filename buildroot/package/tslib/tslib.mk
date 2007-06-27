#############################################################
#
# tslib
#
#############################################################
TSLIB_SOURCE:=tslib-1.0.tar.bz2
TSLIB_SITE:=http://download.berlios.de/tslib
TSLIB_DIR:=$(BUILD_DIR)/tslib-1.0
TSLIB_CAT:=bzcat

$(DL_DIR)/$(TSLIB_SOURCE):
	 $(WGET) -P $(DL_DIR) $(TSLIB_SITE)/$(TSLIB_SOURCE)

tslib-source: $(DL_DIR)/$(TSLIB_SOURCE)

$(TSLIB_DIR)/.unpacked: $(DL_DIR)/$(TSLIB_SOURCE)
	$(TSLIB_CAT) $(DL_DIR)/$(TSLIB_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(TSLIB_DIR)/.patched: $(TSLIB_DIR)/.unpacked
	toolchain/patch-kernel.sh $(TSLIB_DIR) package/tslib/patches \*.patch
	touch $@

$(TSLIB_DIR)/.configured: $(TSLIB_DIR)/.patched
	(cd $(TSLIB_DIR); \
	./autogen.sh; \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS) " \
	./configure \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix=$(STAGING_DIR)/usr \
	--disable-linear-h2200 \
	--disable-collie \
	--disable-h3600 \
	--disable-mk712 \
	--disable-arctic2 \
	--disable-ucb1x00 \
	--enable-input \
	);
	touch $@

$(TSLIB_DIR)/.compiled: $(TSLIB_DIR)/.configured
	$(MAKE) -C $(TSLIB_DIR) 
	touch $@

$(STAGING_DIR)/usr/lib/libts-0.0.so.0.1.1: $(TSLIB_DIR)/.compiled
	$(MAKE) -C $(TSLIB_DIR) install
	touch -c $(STAGING_DIR)/usr/lib/libts-0.0.so.0.1.1

$(TARGET_DIR)/usr/lib/libts-0.0.so.0.1.1: $(STAGING_DIR)/usr/lib/libts-0.0.so.0.1.1
	cp -dpf $(STAGING_DIR)/usr/lib/libts*.so* $(TARGET_DIR)/usr/lib/
	-$(STRIP) $(TARGET_DIR)/usr/lib/libts.so
	cp -dpf $(TSLIB_DIR)/plugins/.libs/dejitter.so $(TARGET_DIR)/usr/lib
	-$(STRIP) $(TARGET_DIR)/usr/lib/dejitter.so
	cp -dpf $(TSLIB_DIR)/plugins/.libs/input.so $(TARGET_DIR)/usr/lib
	-$(STRIP) $(TARGET_DIR)/usr/lib/input.so
	cp -dpf $(TSLIB_DIR)/plugins/.libs/linear.so $(TARGET_DIR)/usr/lib
	-$(STRIP) $(TARGET_DIR)/usr/lib/linear.so
	cp -dpf $(TSLIB_DIR)/plugins/.libs/pthres.so $(TARGET_DIR)/usr/lib
	-$(STRIP) $(TARGET_DIR)/usr/lib/pthres.so
	cp -dpf $(TSLIB_DIR)/plugins/.libs/variance.so $(TARGET_DIR)/usr/lib
	-$(STRIP) $(TARGET_DIR)/usr/lib/variance.so
	cp -dpf $(TSLIB_DIR)/tests/.libs/ts_* $(TARGET_DIR)/root
	-$(STRIP) $(TARGET_DIR)/root/ts_*
	cp -dpf $(TSLIB_DIR)/etc/ts.conf $(TARGET_DIR)/etc
	toolchain/patch-kernel.sh $(TARGET_DIR)/etc package/tslib/patches/ ts.conf.patch

tslib: uclibc $(TARGET_DIR)/usr/lib/libts-0.0.so.0.1.1

tslib-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(TSLIB_DIR) uninstall
	-$(MAKE) -C $(TSLIB_DIR) clean

tslib-dirclean:
	rm -rf $(TSLIB_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_TSLIB)),y)
TARGETS+=tslib
endif
