#############################################################
#
# SDL_image addon for SDL
#
#############################################################
SDL_IMAGE_VERSION:=1.2.6
SDL_IMAGE_SOURCE:=SDL_image-$(SDL_IMAGE_VERSION).tar.gz
SDL_IMAGE_SITE:=http://www.libsdl.org/projects/SDL_image/release
SDL_IMAGE_CAT:=$(ZCAT)
SDL_IMAGE_DIR:=$(BUILD_DIR)/SDL_image-$(SDL_IMAGE_VERSION)

$(DL_DIR)/$(SDL_IMAGE_SOURCE):
	$(WGET) -P $(DL_DIR) $(SDL_IMAGE_SITE)/$(SDL_IMAGE_SOURCE)

sdl_image-source: $(DL_DIR)/$(SDL_IMAGE_SOURCE)

$(SDL_IMAGE_DIR)/.unpacked: $(DL_DIR)/$(SDL_IMAGE_SOURCE)
	$(SDL_IMAGE_CAT) $(DL_DIR)/$(SDL_IMAGE_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(CONFIG_UPDATE) $(SDL_IMAGE_DIR)
	touch $@

$(SDL_IMAGE_DIR)/.configured: $(SDL_IMAGE_DIR)/.unpacked
	(cd $(SDL_IMAGE_DIR); rm -rf config.cache; \
	$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_CONFIGURE_ARGS) \
	./configure \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix=$(STAGING_DIR)/usr \
	--with-sdl-prefix=$(STAGING_DIR)/usr \
	--with-sdl-exec-prefix=$(STAGING_DIR)/usr \
	--disable-sdltest \
	--disable-static \
	--enable-png \
	--enable-bmp \
	--disable-gif \
	--disable-jpg \
	--disable-lbm \
	--disable-pcx \
	--disable-pnm \
	--disable-tif \
	--disable-tga  );
	touch $@

$(SDL_IMAGE_DIR)/.compiled: $(SDL_IMAGE_DIR)/.configured
	$(MAKE) LDFLAGS=$(STAGING_DIR)/lib/libpng12.so -C $(SDL_IMAGE_DIR) 
	touch $@

$(STAGING_DIR)/usr/lib/libSDL_image.so: $(SDL_IMAGE_DIR)/.compiled
	$(MAKE) -C $(SDL_IMAGE_DIR) install
	touch -c $@

$(TARGET_DIR)/usr/lib/libSDL_image.so: $(STAGING_DIR)/usr/lib/libSDL_image.so
	cp -dpf $(STAGING_DIR)/usr/lib/libSDL_image*.so* $(TARGET_DIR)/usr/lib/
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libSDL_image.so

SDL_IMAGE sdl_image: uclibc sdl libpng $(TARGET_DIR)/usr/lib/libSDL_image.so

sdl_image-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(SDL_IMAGE_DIR) uninstall
	-$(MAKE) -C $(SDL_IMAGE_DIR) clean
 
sdl_image-dirclean:
	rm -rf $(SDL_IMAGE_DIR)
 
#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_SDL_IMAGE)),y)
TARGETS+=sdl_image
endif
