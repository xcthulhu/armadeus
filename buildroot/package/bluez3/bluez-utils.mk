#############################################################
#
# bluez-utils - User space programs for controling Bluetooth
#
#############################################################
BLUEZ_UTILS_VERSION:=3.36
BLUEZ_UTILS_SITE=http://bluez.sourceforge.net/download
BLUEZ_UTILS_SOURCE=bluez-utils-$(BLUEZ_UTILS_VERSION).tar.gz

#BLUEZ_HCIDUMP_SOURCE=bluez-hcidump-1.39.tar.gz
#BLUEZ_HCIDUMP_BUILD_DIR=$(BUILD_DIR)/bluez-hcidump-1.39


ifeq ($(strip $(BR2_PACKAGE_ALSA-LIB)),y)
ALSA_DEP=alsa
WITH_ALSA=--with-alsa=$(STAGING_DIR)
else
WITH_ALSA=--without-alsa
endif

ifeq ($(strip $(BR2_PACKAGE_OPENOBEX)),y)
OBEX_DEP=openobex
WITH_OBEX=--with-openobex=$(STAGING_DIR)
else
OBEX_DEP=
WITH_OBEX=--without-obex
endif

BLUEZ_UTILS_CONF_OPT = \
		$(WITH_OBEX) \
		$(WITH_ALSA) \
		--disable-pie \
		--disable-expat \
		--enable-network \
		--enable-serial \
		--enable-audio \
		--enable-input \
		--enable-sync \
		--enable-hcid \
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
		--disable-dfutool

# --disable-pie: uClibc doesn't handle "position independent executables" (segfault at launching)
# sdpd is obsolete

#$(BLUEZ_UTILS_BUILD_DIR)/.configured: $(BLUEZ_UTILS_BUILD_DIR)/.unpacked $(TARGET_DIR)/usr/lib/libbluetooth.so $(OBEX_DEP)

BLUEZ_UTILS_DEPENDENCIES = bluez-libs dbus $(OBEX_DEP) $(ALSA_DEP)

$(eval $(call AUTOTARGETS,package,bluez-utils))

# Hook to prevent link with Host asound and libusb
# (seems not needed with recent BR)
$(BLUEZ_UTILS_HOOK_POST_CONFIGURE):
	@$(call MESSAGE,"Post configuring")
	$(SED) 's,^sys_lib_search_path_spec=.*,sys_lib_search_path_spec="$(STAGING_DIR)/usr/lib $(STAGING_DIR)/lib",' $(@D)/libtool
	touch $@



$(BLUEZ_HCIDUMP_BUILD_DIR)/hcidump: $(BLUEZ_HCIDUMP_BUILD_DIR)/.configured
	$(MAKE) -C $(BLUEZ_HCIDUMP_BUILD_DIR)

$(STAGING_DIR)/usr/lib/libbluetooth.so $(STAGING_DIR)/usr/lib/libbluetooth.a: $(BLUEZ_LIBS_BUILD_DIR)/src/libbluetooth.la
	$(MAKE) DESTDIR="$(STAGING_DIR)" -C $(BLUEZ_LIBS_BUILD_DIR) install
	$(SED) "s,^libdir=.*,libdir=\'$(STAGING_DIR)/usr/lib\',g" $(STAGING_DIR)/usr/lib/libbluetooth.la

$(TARGET_DIR)/usr/sbin/hcidump: $(BLUEZ_HCIDUMP_BUILD_DIR)/hcidump
	$(MAKE) DESTDIR="$(TARGET_DIR)" -C $(BLUEZ_HCIDUMP_BUILD_DIR) install
	rm -rf $(TARGET_DIR)/usr/share/man

$(TARGET_DIR)/usr/sbin/hciattach: $(BLUEZ_UTILS_BUILD_DIR)/tools/hciattach
	(list='hcid dund hidd audio input network pand rfcomm sdpd tools'; for p in $$list; do \
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

#bluez-hcidump: bluez-libs $(TARGET_DIR)/usr/sbin/hcidump

