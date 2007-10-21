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
DOOM_SITE:=ftp://ftp.idsoftware.com/idstuff/doom
PRBOOM_DATA:=doom-1.8.wad.gz

$(DL_DIR)/$(PRBOOM_SOURCE):
	$(WGET) -P $(DL_DIR) $(PRBOOM_SITE)/$(PRBOOM_SOURCE)

prboom-source: $(DL_DIR)/$(PRBOOM_SOURCE)

$(DL_DIR)/$(PRBOOM_DATA):
	$(WGET) -P $(DL_DIR) $(DOOM_SITE)/$(PRBOOM_DATA)

$(PRBOOM_DIR)/.unpacked: $(DL_DIR)/$(PRBOOM_SOURCE) $(DL_DIR)/$(PRBOOM_DATA)
	$(PRBOOM_CAT) $(DL_DIR)/$(PRBOOM_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	# all *.wad should be in the same place
	$(PRBOOM_CAT) $(DL_DIR)/$(PRBOOM_DATA) > $(PRBOOM_DIR)/data/doom1.wad
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

$(PRBOOM_DIR)/src/prboom: $(PRBOOM_DIR)/.configured
	$(MAKE) -C $(PRBOOM_DIR)

$(STAGING_DIR)/usr/games/$(GNU_TARGET_NAME)-prboom: $(PRBOOM_DIR)/src/prboom
	$(MAKE) -C $(PRBOOM_DIR) install

$(TARGET_DIR)/usr/games/prboom: $(STAGING_DIR)/usr/games/$(GNU_TARGET_NAME)-prboom
	mkdir -p $(TARGET_DIR)/usr/games/
	mkdir -p $(TARGET_DIR)/usr/share/games/doom/
	cp -dpf $(STAGING_DIR)/usr/games/$(GNU_TARGET_NAME)-prboom $@
	cp -dpf $(PRBOOM_DIR)/data/*.wad $(TARGET_DIR)/usr/share/games/doom/ 
#	-$(STRIP) --strip-unneeded $@

PRBOOM prboom: sdl sdl_net sdl_mixer $(TARGET_DIR)/usr/games/prboom

prboom-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(PRBOOM_DIR) uninstall
	rm -rf $(TARGET_DIR)/usr/share/games/doom/
	rm -rf $(TARGET_DIR)/usr/games/
	-$(MAKE) -C $(PRBOOM_DIR) clean
 
prboom-dirclean:
	rm -rf $(PRBOOM_DIR)
 
#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_PRBOOM)),y)
TARGETS+=prboom
endif
