#############################################################
#
# Slideshow
#
#############################################################
SLIDESHOW_VERSION:=1.1
SLIDESHOW_SOURCE:=slideshow-source-$(SLIDESHOW_VERSION).zip
SLIDESHOW_SITE:=http://www.xmission.com/~jstanley/
SLIDESHOW_CAT:=zcat
SLIDESHOW_DIR:=$(BUILD_DIR)/slideshow

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

$(SLIDESHOW_DIR)/slideshow: $(SLIDESHOW_DIR)/.configured
	export PATH=$(STAGING_DIR)/bin::$$PATH; export STAGING_DIR=$(STAGING_DIR); $(MAKE) -C $(SLIDESHOW_DIR) ARCH=arm

$(TARGET_DIR)/usr/bin/slideshow: $(SLIDESHOW_DIR)/slideshow
	cp -dpf $< $(TARGET_DIR)/usr/bin
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED)  $@

SLIDESHOW slideshow: sdl sdl_image $(TARGET_DIR)/usr/bin/slideshow

slideshow-clean:
	rm -f $(TARGET_DIR)/usr/bin/slideshow
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
