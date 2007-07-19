#############################################################
#
# SDL Water Demo
#
#############################################################
SDL_WATER_VERSION:=1.0
SDL_WATER_SOURCE:=water-$(SDL_WATER_VERSION).tar.gz
SDL_WATER_SITE:=http://www.libsdl.org/projects/water/src/
SDL_WATER_CAT:=$(ZCAT)
SDL_WATER_DIR:=$(BUILD_DIR)/water-$(SDL_WATER_VERSION)

$(DL_DIR)/$(SDL_WATER_SOURCE):
	$(WGET) -P $(DL_DIR) $(SDL_WATER_SITE)/$(SDL_WATER_SOURCE)

sdl-water-source: $(DL_DIR)/$(SDL_WATER_SOURCE)

$(SDL_WATER_DIR)/.unpacked: $(DL_DIR)/$(SDL_WATER_SOURCE)
	$(SDL_WATER_CAT) $(DL_DIR)/$(SDL_WATER_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(SDL_WATER_DIR)/.configured: $(SDL_WATER_DIR)/.unpacked
	(cd $(SDL_WATER_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS) " \
	./configure \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix=$(STAGING_DIR)/usr \
	--with-sdl-exec-prefix=$(STAGING_DIR)/usr/ \
	--disable-video-x11 );
	touch $@

$(SDL_WATER_DIR)/water: $(SDL_WATER_DIR)/.configured
	$(MAKE) -C $(SDL_WATER_DIR)

$(STAGING_DIR)/usr/bin/water: $(SDL_WATER_DIR)/water
	$(MAKE) -C $(SDL_WATER_DIR) install
	touch -c $@

$(TARGET_DIR)/usr/bin/water: $(STAGING_DIR)/usr/bin/water
	cp -dpf  $< $(SDL_WATER_DIR)/water320.bmp $(TARGET_DIR)/usr/bin/
	-$(STRIP) --strip-unneeded $@

SDL-WATER sdl-water: uclibc sdl $(TARGET_DIR)/usr/bin/water

sdl-water-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(SDL_WATER_DIR) uninstall
	rm $(TARGET_DIR)/usr/bin/water320.bmp
	-$(MAKE) -C $(SDL_WATER_DIR) clean

sdl-water-dirclean:
	rm -rf $(SDL_WATER_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_SDL_WATER)),y)
TARGETS+=sdl-water
endif
