#############################################################
#
# ipkg: a package management tool
#
#############################################################

IPKG_NAME:=ipkg
IPKG_VERSION:=0.99.149
IPKG_RELEASE:=2
IPKG_MD5SUM:=975cc419d6db5fb279dc58177c68373b

# IPKG_SOURCE_URL:=http://www.handhelds.org/packages/ipkg \
# 	http://www.gtlib.gatech.edu/pub/handhelds.org/packages/ipkg \
# 	http://ftp.gwdg.de/pub/linux/handhelds/packages/ipkg
IPKG_SITE:=http://ftp.gwdg.de/pub/linux/handhelds/packages/ipkg
IPKG_SOURCE:=$(IPKG_NAME)-$(IPKG_VERSION).tar.gz
IPKG_CAT:=zcat

IPKG_BUILD_DIR:=$(BUILD_DIR)/$(IPKG_NAME)-$(IPKG_VERSION)
IPKG_INSTALL_DIR:=$(IPKG_BUILD_DIR)/ipkg-install
CP:="cp -f"


$(DL_DIR)/$(IPKG_SOURCE):
	$(WGET) -P $(DL_DIR) $(IPKG_SITE)/$(IPKG_SOURCE)

ipkg-source: $(DL_DIR)/$(IPKG_SOURCE)

$(IPKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(IPKG_SOURCE)
	$(IPKG_CAT) $(DL_DIR)/$(IPKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(IPKG_BUILD_DIR)/.patched: $(IPKG_BUILD_DIR)/.unpacked
	toolchain/patch-kernel.sh $(IPKG_BUILD_DIR) package/ipkg/patches \*.patch
	touch $@

$(IPKG_BUILD_DIR)/.configured: $(IPKG_BUILD_DIR)/.patched
	(cd $(IPKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/bin \
		  --datadir=/usr/share \
		  --includedir=/usr/include \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
	)
	touch $@

$(IPKG_BUILD_DIR)/.compiled: $(IPKG_BUILD_DIR)/.configured
	rm -rf $(IPKG_INSTALL_DIR)
	mkdir -p $(IPKG_INSTALL_DIR)
	$(MAKE) -C $(IPKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		DESTDIR="$(IPKG_INSTALL_DIR)" \
		all install
	touch $@

ipkg-c: uclibc $(IPKG_BUILD_DIR)/.compiled
	cp $(IPKG_INSTALL_DIR)/bin/ipkg-cl $(TARGET_DIR)/bin/ipkg
	-$(STRIP) --strip-unneeded $(IPKG_INSTALL_DIR)/usr/lib/libipkg.so*
	cp -P $(IPKG_INSTALL_DIR)/usr/lib/libipkg.so* $(TARGET_DIR)/usr/lib/
#	$(RSTRIP) $(IDIR_IPKG_C)
#	$(IPKG_BUILD) $(IDIR_IPKG_C) $(PACKAGE_DIR)
	
ipkg-sh: uclibc
	install -m0755 ./package/ipkg/files/ipkg $(TARGET_DIR)/bin/ipkg
#	$(IPKG_BUILD) $(IDIR_IPKG_SH) $(PACKAGE_DIR)

ipkg-clean:
	rm -f $(TARGET_DIR)/bin/ipkg
	rm -f $(TARGET_DIR)/usr/lib/libipkg.so*
	$(MAKE) -C $(IPKG_BUILD_DIR) clean
	rm $(IPKG_BUILD_DIR)/.compiled

ipkg-dirclean:
	rm -f $(TARGET_DIR)/bin/ipkg
	rm -f $(TARGET_DIR)/usr/lib/libipkg.so*
	rm -rf $(IPKG_BUILD_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_IPKG_C)),y)
TARGETS+=ipkg-c
endif
ifeq ($(strip $(BR2_PACKAGE_IPKG_SH)),y)
TARGETS+=ipkg-sh
endif
