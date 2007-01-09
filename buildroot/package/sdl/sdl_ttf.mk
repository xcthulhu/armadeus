#############################################################
#
# SDL-TTF
#
# TODO: add OpenGL dependancy ??
#############################################################
SDL-TTF_VERSION:=2.0.8
SDL-TTF_SOURCE:=SDL_ttf-$(SDL-TTF_VERSION).tar.gz
SDL-TTF_SITE:=http://www.libsdl.org/projects/SDL_ttf/release
SDL-TTF_CAT:=zcat
SDL-TTF_DIR:=$(BUILD_DIR)/SDL_ttf-$(SDL-TTF_VERSION)

$(DL_DIR)/$(SDL-TTF_SOURCE):
	$(WGET) -P $(DL_DIR) $(SDL-TTF_SITE)/$(SDL-TTF_SOURCE)

sdl_ttf-source: $(DL_DIR)/$(SDL-TTF_SOURCE)

$(SDL-TTF_DIR)/.unpacked: $(DL_DIR)/$(SDL-TTF_SOURCE)
	$(SDL-TTF_CAT) $(DL_DIR)/$(SDL-TTF_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(SDL-TTF_DIR)/.configured: $(SDL-TTF_DIR)/.unpacked
	(cd $(SDL-TTF_DIR); \
        $(TARGET_CONFIGURE_OPTS) \
        CFLAGS="$(TARGET_CFLAGS) " \
        ./configure \
        --target=$(GNU_TARGET_NAME) \
        --host=$(GNU_TARGET_NAME) \
        --build=$(GNU_HOST_NAME) \
        --prefix=$(STAGING_DIR)/usr \
        --with-sdl-prefix=$(STAGING_DIR)/usr \
        --with-sdl-exec-prefix=$(STAGING_DIR)/usr \
        --with-freetype-prefix=$(STAGING_DIR)/usr \
        --with-freetype-exec-prefix=$(STAGING_DIR)/usr \
        --disable-sdltest \
	--without-x \
	--disable-opengl \
	--disable-static );
	touch $@

$(SDL-TTF_DIR)/.compiled: $(SDL-TTF_DIR)/.configured
	$(MAKE) -C $(SDL-TTF_DIR) 
	touch $@

$(STAGING_DIR)/usr/lib/libSDL_ttf.so: $(SDL-TTF_DIR)/.compiled
	$(MAKE) -C $(SDL-TTF_DIR) install
	touch -c $(STAGING_DIR)/usr/lib/libSDL_ttf.so

$(TARGET_DIR)/usr/lib/libSDL_ttf.so: $(STAGING_DIR)/usr/lib/libSDL_ttf.so
	cp -dpf $(STAGING_DIR)/usr/lib/libSDL_ttf*.so* $(TARGET_DIR)/usr/lib/
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libSDL_ttf.so

SDL_TTF sdl_ttf: uclibc sdl freetype $(TARGET_DIR)/usr/lib/libSDL_ttf.so

sdl_ttf-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(SDL-TTF_DIR) uninstall
	-$(MAKE) -C $(SDL-TTF_DIR) clean

sdl_ttf-dirclean:
	rm -rf $(SDL-TTF_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_SDL_TTF)),y)
TARGETS+=sdl_ttf
endif

