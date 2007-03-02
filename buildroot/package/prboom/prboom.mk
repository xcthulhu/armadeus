#############################################################
#
# PrBoom
#
#############################################################
PRBOOM_VERSION:=2.2.6
PRBOOM_SOURCE:=prboom-$(PRBOOM_VERSION).tar.gz
PRBOOM_SITE:=http://downloads.sourceforge.net/prboom
PRBOOM_CAT:=zcat
PRBOOM_DIR:=$(BUILD_DIR)/prboom-$(PRBOOM_VERSION)

$(DL_DIR)/$(PRBOOM_SOURCE):
	$(WGET) -P $(DL_DIR) $(PRBOOM_SITE)/$(PRBOOM_SOURCE)

sdl_prboom-source: $(DL_DIR)/$(PRBOOM_SOURCE)

$(PRBOOM_DIR)/.unpacked: $(DL_DIR)/$(PRBOOM_SOURCE)
	$(PRBOOM_CAT) $(DL_DIR)/$(PRBOOM_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(PRBOOM_DIR)/.configured: $(PRBOOM_DIR)/.unpacked
	(cd $(PRBOOM_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS) " \
	LDFLAGS="-Wl,-rpath=$(STAGING_DIR)/lib/" \
	./configure \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix=$(STAGING_DIR)/usr \
	--oldincludedir=$(STAGING_DIR)/usr/include \
	--with-sdl-prefix=$(STAGING_DIR)/usr \
	--with-sdl-exec-prefix=$(STAGING_DIR)/usr \
	--without-x \
	--disable-sdltest \
	);
	touch $@
# !!!! ++ modif de config.status pour virer les /usr/include et /usr/lib en dur......!!!!!!!!!!

$(PRBOOM_DIR)/.compiled: $(PRBOOM_DIR)/.configured
	$(MAKE) -C $(PRBOOM_DIR) 
	touch $@

$(STAGING_DIR)/usr/lib/prdoom.so: $(PRBOOM_DIR)/.compiled
	$(MAKE) -C $(PRBOOM_DIR) install
	touch -c $@

$(TARGET_DIR)/usr/lib/prdoom.so: $(STAGING_DIR)/usr/lib/prdoom.so
#copier tous les .wad dans data (là où est prdoom.wad)
	cp -dpf $(STAGING_DIR)/usr/lib/libSDL_image*.so* $(TARGET_DIR)/usr/lib/
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libSDL_image.so

PRBOOM sdl_prboom: sdl $(PRBOOM_DIR)/.compiled

sdl_prboom-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(PRBOOM_DIR) uninstall
	-$(MAKE) -C $(PRBOOM_DIR) clean
 
sdl_prboom-dirclean:
	rm -rf $(PRBOOM_DIR)
 
#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_PRBOOM)),y)
TARGETS+=sdl_prboom
endif
