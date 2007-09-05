#############################################################
#
# Slideshow
#
#############################################################
SLIDESHOW_VERSION:=1.1
SLIDESHOW_SOURCE:=slideshow-source-$(SLIDESHOW_VERSION).zip
SLIDESHOW_SITE:=http://www.xmission.com/~jstanley/
SLIDESHOW_CAT:=zcat
SLIDESHOW_DIR:=$(BUILD_DIR)/slideshow/

$(DL_DIR)/$(SLIDESHOW_SOURCE):
	$(WGET) -P $(DL_DIR) $(SLIDESHOW_SITE)/$(SLIDESHOW_SOURCE)

slideshow-source: $(DL_DIR)/$(SLIDESHOW_SOURCE)

$(SLIDESHOW_DIR)/.unpacked: $(DL_DIR)/$(SLIDESHOW_SOURCE)
	unzip $(DL_DIR)/$(SLIDESHOW_SOURCE) -d $(BUILD_DIR)
	touch $@

$(SLIDESHOW_DIR)/.configured: $(SLIDESHOW_DIR)/.unpacked
	echo "Patching...."
	toolchain/patch-kernel.sh $(SLIDESHOW_DIR) package/slideshow/ slideshow-\*.patch
	touch $@

$(SLIDESHOW_DIR)/src/slideshow: $(SLIDESHOW_DIR)/.configured
	export PATH=$(STAGING_DIR)/bin::$$PATH; $(MAKE) -C $(SLIDESHOW_DIR) ARCH=arm

$(STAGING_DIR)/usr/bin/slideshow: $(SLIDESHOW_DIR)/src/slideshow
	$(MAKE) -C $(SLIDESHOW_DIR) install

# $(TARGET_DIR)/usr/games/prboom: $(STAGING_DIR)/usr/games/$(GNU_TARGET_NAME)-prboom
# 	mkdir -p $(TARGET_DIR)/usr/games/
# 	mkdir -p $(TARGET_DIR)/usr/share/games/doom/
# 	cp -dpf $(STAGING_DIR)/usr/games/$(GNU_TARGET_NAME)-prboom $@
# 	cp -dpf $(SLIDESHOW_DIR)/data/*.wad $(TARGET_DIR)/usr/share/games/doom/ 
# 	-$(STRIP) --strip-unneeded $@

SLIDESHOW slideshow: sdl sdl_image $(STAGING_DIR)/usr/bin/slideshow

slideshow-clean:
#	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(SLIDESHOW_DIR) uninstall
#	rm -rf $(TARGET_DIR)/usr/share/games/doom/
#	rm -rf $(TARGET_DIR)/usr/games/
	-$(MAKE) -C $(SLIDESHOW_DIR) clean
 
slideshow-dirclean:
	rm -rf $(SLIDESHOW_DIR)
 
#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_SLIDESHOW)),y)
TARGETS+=slideshow
endif
