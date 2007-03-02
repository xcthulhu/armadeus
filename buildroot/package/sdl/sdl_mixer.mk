#############################################################
#
# SDL_mixer: sound addon for SDL
#
#############################################################
SDL_MIXER_VERSION:=1.2.7
SDL_MIXER_SOURCE:=SDL_mixer-$(SDL_MIXER_VERSION).tar.gz
SDL_MIXER_SITE:=http://www.libsdl.org/projects/SDL_mixer/release
SDL_MIXER_CAT:=zcat
SDL_MIXER_DIR:=$(BUILD_DIR)/SDL_mixer-$(SDL_MIXER_VERSION)

$(DL_DIR)/$(SDL_MIXER_SOURCE):
	$(WGET) -P $(DL_DIR) $(SDL_MIXER_SITE)/$(SDL_MIXER_SOURCE)

sdl_image-source: $(DL_DIR)/$(SDL_MIXER_SOURCE)

$(SDL_MIXER_DIR)/.unpacked: $(DL_DIR)/$(SDL_MIXER_SOURCE)
	$(SDL_MIXER_CAT) $(DL_DIR)/$(SDL_MIXER_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(SDL_MIXER_DIR)/.configured: $(SDL_MIXER_DIR)/.unpacked
	(cd $(SDL_MIXER_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS) " \
	LDFLAGS="-Wl,-rpath=$(STAGING_DIR)/lib/" \
	./configure \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix=$(STAGING_DIR)/usr \
	--with-sdl-prefix=$(STAGING_DIR)/usr \
	--with-sdl-exec-prefix=$(STAGING_DIR)/usr \
	--disable-sdltest \
	--disable-static \
	);
	touch $@

$(SDL_MIXER_DIR)/.compiled: $(SDL_MIXER_DIR)/.configured
	$(MAKE) -C $(SDL_MIXER_DIR) 
	touch $@

$(STAGING_DIR)/usr/lib/libSDL_mixer.so: $(SDL_MIXER_DIR)/.compiled
	$(MAKE) -C $(SDL_MIXER_DIR) install
	touch -c $@

$(TARGET_DIR)/usr/lib/libSDL_mixer.so: $(STAGING_DIR)/usr/lib/libSDL_mixer.so
	cp -dpf $(STAGING_DIR)/usr/lib/libSDL_mixer*.so* $(TARGET_DIR)/usr/lib/
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libSDL_mixer.so

SDL_MIXER sdl_mixer: sdl $(TARGET_DIR)/usr/lib/libSDL_mixer.so

sdl_mixer-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(SDL_MIXER_DIR) uninstall
	-$(MAKE) -C $(SDL_MIXER_DIR) clean
 
sdl_mixer-dirclean:
	rm -rf $(SDL_MIXER_DIR)
 
#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_SDL_MIXER)),y)
TARGETS+=sdl_mixer
endif
