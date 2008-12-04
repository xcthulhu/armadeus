PYSERIAL_VERSION=2.2
PYSERIAL_BINARY=pyserial
PYSERIAL_SOURCE=$(PYSERIAL_BINARY)-$(PYSERIAL_VERSION).zip
PYSERIAL_SITE=http://$(BR2_SOURCEFORGE_MIRROR).dl.sourceforge.net/sourceforge/pyserial
PYSERIAL_DIR=$(BUILD_DIR)/$(PYSERIAL_BINARY)-$(PYSERIAL_VERSION)
PYSERIAL_SITE_PKG=$(TARGET_DIR)/usr/lib/python2.4/site-packages/serial

$(DL_DIR)/$(PYSERIAL_SOURCE):
	$(WGET) -P $(DL_DIR) $(PYSERIAL_SITE)/$(PYSERIAL_SOURCE)

$(PYSERIAL_DIR)/.source: $(DL_DIR)/$(PYSERIAL_SOURCE)
	unzip -o $(DL_DIR)/$(PYSERIAL_SOURCE) -d $(BUILD_DIR)
	touch $(PYSERIAL_DIR)/.source

$(PYSERIAL_DIR)/.configured: $(PYSERIAL_DIR)/.source
	touch $(PYSERIAL_DIR)/.configured

$(PYSERIAL_BINARY): python $(PYSERIAL_DIR)/.configured
	mkdir -p $(PYSERIAL_SITE_PKG)
	cp -R $(PYSERIAL_DIR)/serial/* $(PYSERIAL_SITE_PKG)

$(PYSERIAL_BINARY)-source: $(DL_DIR)/$(PYSERIAL_SOURCE)

$(PYSERIAL_BINARY)-clean:
	rm -rf $(PYSERIAL_SITE_PKG)

$(PYSERIAL_BINARY)-dirclean:
	rm -rf $(PYSERIAL_DIR)

ifeq ($(strip $(BR2_PACKAGE_PYSERIAL)),y)
TARGETS+=$(PYSERIAL_BINARY)
endif
