#############################################################
#
# alsa-driver
#
#############################################################
ALSA_DRIVER_VERSION:=1.0.14
ALSA_DRIVER_SOURCE:=alsa-driver-$(ALSA_DRIVER_VERSION).tar.bz2
ALSA_DRIVER_SITE:=ftp://ftp.alsa-project.org/pub/driver
ALSA_DRIVER_DIR:=$(BUILD_DIR)/alsa-driver-$(ALSA_DRIVER_VERSION)
ALSA_DRIVER_CAT:=bzcat
ALSA_DRIVER_BINARY:=libasound.so.2.0.0
ALSA_DRIVER_TARGET_BINARY:=usr/lib/$(ALSA_DRIVER_BINARY)

$(DL_DIR)/$(ALSA_DRIVER_SOURCE):
	$(WGET) -P $(DL_DIR) $(ALSA_DRIVER_SITE)/$(ALSA_DRIVER_SOURCE)

$(ALSA_DRIVER_DIR)/.unpacked: $(DL_DIR)/$(ALSA_DRIVER_SOURCE)
	$(ALSA_DRIVER_CAT) $(DL_DIR)/$(ALSA_DRIVER_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(ALSA_DRIVER_DIR) package/alsa-driver/ alsa-driver-$(ALSA_DRIVER_VERSION)\*.patch*
	toolchain/patch-kernel.sh $(ALSA_DRIVER_DIR) package/alsa-driver/ alsa-driver-armadeus\*.patch
	# Copy our custom drivers (until they are integrated in alsa mainline)
	cp -f package/alsa-driver/imx* $(ALSA_DRIVER_DIR)/alsa-kernel/arm/
#	$(CONFIG_UPDATE) $(ALSA_DRIVER_DIR)
	touch $@

$(ALSA_DRIVER_DIR)/.configured: $(ALSA_DRIVER_DIR)/.unpacked
	(cd $(ALSA_DRIVER_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_ARGS) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--sysconfdir=/etc \
		--disable-docs \
		--with-sequencer=yes \
		--with-isapnp=no \
		--with-oss=yes \
		--with-kernel=${BUILD_DIR}/linux-2.6.18.1 \
		--with-kernel-version=2.6.18.1 \
		--with-cards=all \
	);
	touch $@

$(ALSA_DRIVER_DIR)/modules/snd-imx-tsc.o: $(ALSA_DRIVER_DIR)/.configured
	(export PATH=${PATH}:${STAGING_DIR}/bin ; \
		$(MAKE) -C $(ALSA_DRIVER_DIR) \
	);

# $(STAGING_DIR)/$(ALSA_DRIVER_TARGET_BINARY): $(ALSA_DRIVER_DIR)/src/.libs/$(ALSA_DRIVER_BINARY)
# 	$(MAKE) DESTDIR=$(STAGING_DIR) -C $(ALSA_DRIVER_DIR) install
# 	$(SED) "s,^libdir=.*,libdir=\'$(STAGING_DIR)/usr/lib\',g" $(STAGING_DIR)/usr/lib/libasound.la
# 
# $(TARGET_DIR)/$(ALSA_DRIVER_TARGET_BINARY): $(STAGING_DIR)/$(ALSA_DRIVER_TARGET_BINARY)
# 	mkdir -p $(TARGET_DIR)/usr/share/alsa
# 	mkdir -p $(TARGET_DIR)/usr/lib/alsa-lib
# 	cp -dpf  $(STAGING_DIR)/lib/libasound.so*  $(TARGET_DIR)/usr/lib/
# 	cp -rdpf $(STAGING_DIR)/usr/share/alsa/*   $(TARGET_DIR)/usr/share/alsa/
# 	cp -rdpf $(STAGING_DIR)/usr/lib/alsa-lib/* $(TARGET_DIR)/usr/lib/alsa-lib/
# 	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libasound.so*
# 	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/alsa-lib/*.so
# 	touch -c $@

alsa-driver: uclibc linux $(ALSA_DRIVER_DIR)/modules/snd-imx-tsc.o

alsa-driver-clean:
	rm -f $(TARGET_DIR)/$(ALSA_DRIVER_TARGET_BINARY)
	-$(MAKE) -C $(ALSA_DRIVER_DIR) clean

alsa-driver-dirclean:
	rm -rf $(ALSA_DRIVER_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_ALSA_DRIVER)),y)
TARGETS+=alsa-driver
endif