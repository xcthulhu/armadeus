#############################################################
#
# SNES9X  SNES emulator
#
#############################################################
SNES9X_VERSION:=1.39
SNES9X_SOURCE:=snes9x-sdl-$(SNES9X_VERSION).tar.bz2
SNES9X_SITE:=http://www.vanille.de/mirror/
# roms: http://rtgamer.free.fr/index.php?pg=2&rub=sn
SNES9X_CAT:=bzcat
SNES9X_DIR:=$(BUILD_DIR)/snes9x-sdl-$(SNES9X_VERSION)

#SNES9X_VERSION:=1.43
#SNES9X_SOURCE:=snes9x-$(SNES9X_VERSION)-src.tar.gz
#SNES9X_SITE:=http://vincent.grigorieff.free.fr/snes9x/
#SNES9X_CAT:=$(ZCAT)
#SNES9X_DIR:=$(BUILD_DIR)/snes9x-$(SNES9X_VERSION)-src

$(DL_DIR)/$(SNES9X_SOURCE):
	$(WGET) -P $(DL_DIR) $(SNES9X_SITE)/$(SNES9X_SOURCE)

snes9x-source: $(DL_DIR)/$(SNES9X_SOURCE)

$(SNES9X_DIR)/.unpacked: $(DL_DIR)/$(SNES9X_SOURCE)
	$(SNES9X_CAT) $(DL_DIR)/$(SNES9X_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(SNES9X_DIR)/.patched: $(SNES9X_DIR)/.unpacked
	toolchain/patch-kernel.sh $(SNES9X_DIR) package/snes9x/ snes9x-$(SNES9X_VERSION)\*.patch
	touch $@

$(SNES9X_DIR)/.configured: $(SNES9X_DIR)/.patched
# For v1.49 only !!
#	(cd $(SNES9X_DIR)/snes9x; \
#	$(TARGET_CONFIGURE_OPTS) \
#	CFLAGS="$(TARGET_CFLAGS) " \
#	./configure \
#	--target=$(GNU_TARGET_NAME) \
#	--host=$(GNU_TARGET_NAME) \
#	--build=$(GNU_HOST_NAME) \
#	--prefix=$(STAGING_DIR)/usr \
#	--with-sdl-prefix=$(STAGING_DIR)/usr \
#	--with-sdl-exec-prefix=$(STAGING_DIR)/usr \
#	--disable-sdltest \
#	--without-x \
#	--without-opengl \
#	--without-nasm \
#	--without-mmx );
	touch $@

$(SNES9X_DIR)/snes9x: $(SNES9X_DIR)/.configured
#	$(MAKE) -C $(SNES9X_DIR)/snes9x
	$(MAKE) -C $(SNES9X_DIR) \
	CC="$(STAGING_DIR)/bin/arm-linux-gcc" CCC="$(STAGING_DIR)/bin/arm-linux-c++ -fno-rtti -fno-exceptions" \
	INCLUDES="-I$(STAGING_DIR)/usr/lib/include `$(STAGING_DIR)/usr/bin/sdl-config --cflags`" \
	LDLIBS="`$(STAGING_DIR)/usr/bin/sdl-config --libs`"
	touch $@

# $(STAGING_DIR)/usr/lib/libSDL.so: $(SNES9X_DIR)/.compiled
# 	$(MAKE) -C $(SNES9X_DIR) install
# 	touch -c $(STAGING_DIR)/usr/lib/libSDL.so
# 
# $(TARGET_DIR)/usr/lib/libSDL.so: $(STAGING_DIR)/usr/lib/libSDL.so
# 	cp -dpf $(STAGING_DIR)/usr/lib/libSDL*.so* $(TARGET_DIR)/usr/lib/
# 	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libSDL.so

SNES9X snes9x: sdl $(SNES9X_DIR)/snes9x

snes9x-clean:
#	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(SNES9X_DIR) uninstall
	-$(MAKE) -C $(SNES9X_DIR) clean

snes9x-dirclean:
	rm -rf $(SNES9X_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_SNES9X)),y)
TARGETS+=snes9x
endif
