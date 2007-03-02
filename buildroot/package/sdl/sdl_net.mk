#############################################################
#
# SDL_net: network addon for SDL
#
#############################################################
SDL_NET_VERSION:=1.2.6
SDL_NET_SOURCE:=SDL_net-$(SDL_NET_VERSION).tar.gz
SDL_NET_SITE:=http://www.libsdl.org/projects/SDL_net/release
SDL_NET_CAT:=zcat
SDL_NET_DIR:=$(BUILD_DIR)/SDL_net-$(SDL_NET_VERSION)

$(DL_DIR)/$(SDL_NET_SOURCE):
	$(WGET) -P $(DL_DIR) $(SDL_NET_SITE)/$(SDL_NET_SOURCE)

sdl_net-source: $(DL_DIR)/$(SDL_NET_SOURCE)

$(SDL_NET_DIR)/.unpacked: $(DL_DIR)/$(SDL_NET_SOURCE)
	$(SDL_NET_CAT) $(DL_DIR)/$(SDL_NET_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(SDL_NET_DIR)/.configured: $(SDL_NET_DIR)/.unpacked
	(cd $(SDL_NET_DIR); \
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

$(SDL_NET_DIR)/.compiled: $(SDL_NET_DIR)/.configured
	$(MAKE) -C $(SDL_NET_DIR) 
	touch $@

$(STAGING_DIR)/usr/lib/libSDL_net.so: $(SDL_NET_DIR)/.compiled
	$(MAKE) -C $(SDL_NET_DIR) install
	touch -c $@

$(TARGET_DIR)/usr/lib/libSDL_net.so: $(STAGING_DIR)/usr/lib/libSDL_net.so
	cp -dpf $(STAGING_DIR)/usr/lib/libSDL_net*.so* $(TARGET_DIR)/usr/lib/
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libSDL_net.so

SDL_NET sdl_net: sdl $(TARGET_DIR)/usr/lib/libSDL_net.so

sdl_net-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(SDL_NET_DIR) uninstall
	-$(MAKE) -C $(SDL_NET_DIR) clean
 
sdl_net-dirclean:
	rm -rf $(SDL_NET_DIR)
 
#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_SDL_NET)),y)
TARGETS+=sdl_net
endif
