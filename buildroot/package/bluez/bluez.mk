#############################################################
#
# bluez-utils - User Space Program For Controling Bluetooth
#
#############################################################
#
BLUEZ_SOURCE_URL=http://bluez.sourceforge.net/download
BLUEZ_LIBS_SOURCE=bluez-libs-3.20.tar.gz
BLUEZ_UTILS_SOURCE=bluez-utils-3.20.tar.gz
BLUEZ_HCIDUMP_SOURCE=bluez-hcidump-1.39.tar.gz
BLUEZ_LIBS_BUILD_DIR=$(BUILD_DIR)/bluez-libs-3.20
BLUEZ_UTILS_BUILD_DIR=$(BUILD_DIR)/bluez-utils-3.20
BLUEZ_HCIDUMP_BUILD_DIR=$(BUILD_DIR)/bluez-hcidump-1.39

$(DL_DIR)/$(BLUEZ_LIBS_SOURCE):
	$(WGET) -P $(DL_DIR) $(BLUEZ_SOURCE_URL)/$(BLUEZ_LIBS_SOURCE)

$(DL_DIR)/$(BLUEZ_UTILS_SOURCE):
	$(WGET) -P $(DL_DIR) $(BLUEZ_SOURCE_URL)/$(BLUEZ_UTILS_SOURCE)

$(DL_DIR)/$(BLUEZ_HCIDUMP_SOURCE):
	$(WGET) -P $(DL_DIR) $(BLUEZ_SOURCE_URL)/$(BLUEZ_HCIDUMP_SOURCE)

$(BLUEZ_LIBS_BUILD_DIR)/.unpacked: $(DL_DIR)/$(BLUEZ_LIBS_SOURCE)
	zcat $(DL_DIR)/$(BLUEZ_LIBS_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(BLUEZ_LIBS_BUILD_DIR) package/bluez bluez-libs*.patch
	touch $(BLUEZ_LIBS_BUILD_DIR)/.unpacked

$(BLUEZ_UTILS_BUILD_DIR)/.unpacked: $(DL_DIR)/$(BLUEZ_UTILS_SOURCE)
	zcat $(DL_DIR)/$(BLUEZ_UTILS_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(BLUEZ_UTILS_BUILD_DIR) package/bluez bluez-utils*.patch
	touch $(BLUEZ_UTILS_BUILD_DIR)/.unpacked

$(BLUEZ_HCIDUMP_BUILD_DIR)/.unpacked: $(DL_DIR)/$(BLUEZ_HCIDUMP_SOURCE)
	zcat $(DL_DIR)/$(BLUEZ_HCIDUMP_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(BLUEZ_HCIDUMP_BUILD_DIR)/.unpacked

$(BLUEZ_LIBS_BUILD_DIR)/.configured: $(BLUEZ_LIBS_BUILD_DIR)/.unpacked
	(cd $(BLUEZ_LIBS_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) CFLAGS="$(TARGET_CFLAGS)" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/lib/pkconfig:$(STAGING_DIR)/usr/lib/pkgconfig" PKG_CONFIG="$(STAGING_DIR)/usr/bin/pkg-config" PKG_CONFIG_SYSROOT=$(STAGING_DIR) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
	);
	touch  $(BLUEZ_LIBS_BUILD_DIR)/.configured

ifeq ($(strip $(BR2_PACKAGE_ALSA-LIB)),y)
WITH_ALSA=--with-alsa=$(STAGING_DIR)
else
WITH_ALSA=--without-alsa
endif

ifeq ($(strip $(BR2_PACKAGE_OPENOBEX)),y)
OBEX_DEP=$(STAGING_DIR)/lib/libopenobex.so
WITH_OBEX=--with-openobex=$(STAGING_DIR)
else
OBEX_DEP=
WITH_OBEX=--without-obex
endif

$(BLUEZ_UTILS_BUILD_DIR)/.configured: $(BLUEZ_UTILS_BUILD_DIR)/.unpacked $(TARGET_DIR)/usr/lib/libbluetooth.so $(OBEX_DEP)
	(cd $(BLUEZ_UTILS_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) CFLAGS="$(TARGET_CFLAGS)" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/lib/pkconfig:$(STAGING_DIR)/usr/lib/pkgconfig" PKG_CONFIG="$(STAGING_DIR)/usr/bin/pkg-config" PKG_CONFIG_SYSROOT=$(STAGING_DIR) \
		./configure \
		$(WITH_OBEX) \
		$(WITH_ALSA) \
		--disable-expat \
		--enable-network \
		--enable-serial \
		--disable-input \
		--enable-audio \
		--enable-sync \
		--enable-hcid \
		--enable-sdpd \
		--enable-hidd \
		--enable-pand \
		--disable-test \
		--enable-configfiles \
		--disable-manpages \
		--enable-initscripts \
		--disable-pcmciarules \
		--disable-bccmd \
		--enable-avctrl \
		--enable-hid2hci \
		--disable-dfutool \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
	);
	touch  $(BLUEZ_UTILS_BUILD_DIR)/.configured


$(BLUEZ_HCIDUMP_BUILD_DIR)/.configured: $(BLUEZ_HCIDUMP_BUILD_DIR)/.unpacked $(TARGET_DIR)/usr/lib/libbluetooth.so
	(cd $(BLUEZ_HCIDUMP_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) CFLAGS="$(TARGET_CFLAGS)" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/lib/pkconfig:$(STAGING_DIR)/usr/lib/pkgconfig" PKG_CONFIG="$(STAGING_DIR)/usr/bin/pkg-config" PKG_CONFIG_SYSROOT=$(STAGING_DIR) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
	);
	touch  $(BLUEZ_HCIDUMP_BUILD_DIR)/.configured

$(BLUEZ_LIBS_BUILD_DIR)/src/libbluetooth.la: $(BLUEZ_LIBS_BUILD_DIR)/.configured
	$(MAKE) -C $(BLUEZ_LIBS_BUILD_DIR)

$(BLUEZ_UTILS_BUILD_DIR)/tools/hciattach: $(BLUEZ_UTILS_BUILD_DIR)/.configured
	$(MAKE) -C $(BLUEZ_UTILS_BUILD_DIR)

$(BLUEZ_HCIDUMP_BUILD_DIR)/hcidump: $(BLUEZ_HCIDUMP_BUILD_DIR)/.configured
	$(MAKE) -C $(BLUEZ_HCIDUMP_BUILD_DIR)

$(STAGING_DIR)/usr/lib/libbluetooth.so $(STAGING_DIR)/usr/lib/libbluetooth.a: $(BLUEZ_LIBS_BUILD_DIR)/src/libbluetooth.la
	$(MAKE) DESTDIR="$(STAGING_DIR)" -C $(BLUEZ_LIBS_BUILD_DIR) install
	$(SED) "s,^libdir=.*,libdir=\'$(STAGING_DIR)/usr/lib\',g" $(STAGING_DIR)/usr/lib/libbluetooth.la

$(TARGET_DIR)/usr/lib/libbluetooth.so: $(STAGING_DIR)/usr/lib/libbluetooth.so
	mkdir -p $(TARGET_DIR)/usr/lib
	install -D -m 0755 $(STAGING_DIR)/usr/lib/libbluetooth.so* $(TARGET_DIR)/usr/lib
	$(STRIPCMD) $(TARGET_DIR)/usr/lib/libbluetooth.so

$(TARGET_DIR)/usr/sbin/hcidump: $(BLUEZ_HCIDUMP_BUILD_DIR)/hcidump
	$(MAKE) DESTDIR="$(TARGET_DIR)" -C $(BLUEZ_HCIDUMP_BUILD_DIR) install
	rm -rf $(TARGET_DIR)/usr/share/man

$(TARGET_DIR)/usr/sbin/hciattach: $(BLUEZ_UTILS_BUILD_DIR)/tools/hciattach
	(list='hcid dund hidd audio input network rfcomm sdpd tools'; for p in $$list; do \
		$(MAKE) DESTDIR="$(TARGET_DIR)" -C $(BLUEZ_UTILS_BUILD_DIR)/$$p install; \
	done)
	mkdir -p $(TARGET_DIR)/etc/bluetooth/pan $(TARGET_DIR)/etc/bluetooth/rfcomm
	install -D -m 0644 package/bluez/hcid.conf $(TARGET_DIR)/etc/bluetooth/hcid.conf
	install -D -m 0755 package/bluez/bluez-utils-bluepin.sh $(TARGET_DIR)/etc/bluetooth/bluepin
	install -D -m 0755 package/bluez/bluez-pand-devup.sh $(TARGET_DIR)/etc/bluetooth/pan/dev-up
	install -D -m 0755 package/bluez/rfcomm-getty $(TARGET_DIR)/etc/bluetooth/rfcomm/rfcomm-getty
	echo "1234" > $(TARGET_DIR)/etc/bluetooth/pin
	chmod 0600 $(TARGET_DIR)/etc/bluetooth/pin
	install -D -m 0644 package/bluez/rfcomm.conf $(TARGET_DIR)/etc/bluetooth/rfcomm.conf
	#install -D -m 0755 package/bluez/bluetooth.init $(TARGET_DIR)/etc/init.d/S30bluetooth   TBDJB
	mkdir -p $(TARGET_DIR)/etc/default
	install -D -m 0644 package/bluez/bluetooth.default $(TARGET_DIR)/etc/default/bluetooth
	install -D -m 0644 package/bluez/bluetooth.conf $(TARGET_DIR)/etc/dbus-1/system.d/bluetooth.conf

.PHONY: bluez-libs-staging bluez-libs bluez-libs-source bluez-utils

bluez-utils: bluez-libs dbus $(TARGET_DIR)/usr/sbin/hciattach

bluez-utils-dirclean: bluez-libs-dirclean
	rm -rf $(BLUEZ_UTILS_BUILD_DIR)

bluez-hcidump: bluez-libs $(TARGET_DIR)/usr/sbin/hcidump

bluez-libs-staging: $(STAGING_DIR)/usr/lib/libbluetooth.a

bluez-libs: $(TARGET_DIR)/usr/lib/libbluetooth.so

bluez-libs-source: $(DL_DIR)/$(BLUEZ_SOURCE)

bluez-libs-clean:
	-$(MAKE) -C $(BLUEZ_LIBS_BUILD_DIR) clean

bluez-libs-dirclean:
	rm -rf $(BLUEZ_LIBS_BUILD_DIR)

#############################################################
#
## Toplevel Makefile options
#
##############################################################
ifeq ($(strip $(BR2_PACKAGE_BLUEZ)),y)
TARGETS+=bluez-utils
endif
