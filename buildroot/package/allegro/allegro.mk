#############################################################
#
# Allegro: a game programming library
#
#############################################################
ALLEGRO_VERSION:=4.2.1
ALLEGRO_SOURCE:=allegro-$(ALLEGRO_VERSION).tar.gz
ALLEGRO_SITE:=http://dfn.dl.sourceforge.net/sourceforge/alleg/
ALLEGRO_CAT:=zcat
ALLEGRO_DIR:=$(BUILD_DIR)/allegro-$(ALLEGRO_VERSION)

$(DL_DIR)/$(ALLEGRO_SOURCE):
	$(WGET) -P $(DL_DIR) $(ALLEGRO_SITE)/$(ALLEGRO_SOURCE)

allegro-source: $(DL_DIR)/$(ALLEGRO_SOURCE)

$(ALLEGRO_DIR)/.unpacked: $(DL_DIR)/$(ALLEGRO_SOURCE)
	$(ALLEGRO_CAT) $(DL_DIR)/$(ALLEGRO_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(ALLEGRO_DIR)/.configured: $(ALLEGRO_DIR)/.unpacked
	(cd $(ALLEGRO_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	CROSSCOMPILE=1 \
	./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--without-svgalib \
		--enable-modules=no \
		--enable-static=yes \
		--enable-shared=no \
		--disable-asm \
		--disable-mmx \
		--disable-sse \
		--disable-color24 \
		--disable-color32 \
		--without-x );
	touch $@
# allegro doesn't seem to build as shared library
# 		--disable-color24 \
# 		--disable-color32 \

$(ALLEGRO_DIR)/lib/unix/liballeg.a: $(ALLEGRO_DIR)/.configured
	$(MAKE) -C $(ALLEGRO_DIR) CROSSCOMPILE=1
	touch $@

#$(STAGING_DIR)/usr/lib/liballeg.so: $(ALLEGRO_DIR)/lib/unix/liballeg.a
#	$(MAKE) -C $(ALLEGRO_DIR) install
#	touch -c $@
# 
# $(TARGET_DIR)/usr/lib/liballeg.so.so: $(STAGING_DIR)/usr/lib/liballeg.so
# 	cp -dpf $(STAGING_DIR)/usr/lib/liballeg*.so* $(TARGET_DIR)/usr/lib/
# 	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/liballeg.so

ALLEGRO allegro: $(ALLEGRO_DIR)/lib/unix/liballeg.a

allegro-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(ALLEGRO_DIR) uninstall
	-$(MAKE) -C $(ALLEGRO_DIR) clean
 
allegro-dirclean:
	rm -rf $(ALLEGRO_DIR)
 
#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_ALLEGRO)),y)
TARGETS+=allegro
endif
