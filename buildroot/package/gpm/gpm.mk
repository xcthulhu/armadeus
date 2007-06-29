#############################################################
#
# GPM: mouse support for linux in the console
#
#############################################################

GPM_VER:=1.20.0
GPM_SOURCE:=gpm-$(GPM_VER).tar.bz2
GPM_SITE:=http://unix.schottelius.org/gpm/archives/old/
GPM_DIR:=$(BUILD_DIR)/gpm-$(GPM_VER)
GPM_CAT:=bzcat
GPM_PATCH:=gpm.patch


$(DL_DIR)/$(GPM_SOURCE):
	$(WGET) -P $(DL_DIR) $(GPM_SITE)/$(GPM_SOURCE)
	
gpm-source: $(DL_DIR)/$(GPM_SOURCE)

$(GPM_DIR)/.unpacked: $(DL_DIR)/$(GPM_SOURCE) $(DL_DIR)/$(GPM_DEMOS) $(DL_DIR)/$(MESAGLUT_SOURCE)
	$(GPM_CAT) $(DL_DIR)/$(GPM_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(GPM_DIR)/.patched: $(GPM_DIR)/.unpacked
	toolchain/patch-kernel.sh $(GPM_DIR) package/gpm $(GPM_PATCH)
	touch $@

$(GPM_DIR)/.configured: $(GPM_DIR)/.patched
	(cd $(GPM_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS) " \
	LDFLAGS="-Wl,-rpath=$(STAGING_DIR)/lib/" \
	./configure \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix=$(STAGING_DIR)/usr \
	);
	touch $@

$(GPM_DIR)/.compiled: $(GPM_DIR)/.configured
	(cd $(GPM_DIR); \
        $(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CC) \
    );
	touch $@

$(STAGING_DIR)/usr/sbin/gpm: $(GPM_DIR)/.compiled
	(cd $(GPM_DIR); \
        $(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CC) install \
    );
# Bug in GPM automake/conf stuff ? anyone knows a better way to patch that ? -->
	(cd $(STAGING_DIR)/usr/lib/; \
		mv libgpm.so.1.19.0 libgpm.so.$(GPM_VER); \
		ln -sf libgpm.so.$(GPM_VER) libgpm.so.1; \
		ln -sf libgpm.so.$(GPM_VER) libgpm.so; \
	);
	touch $@

$(TARGET_DIR)/usr/sbin/gpm: $(STAGING_DIR)/usr/sbin/gpm
	cp -dpf $(STAGING_DIR)/usr/sbin/gpm $(TARGET_DIR)/usr/sbin/
	$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/sbin/gpm
	cp -dpf $(STAGING_DIR)/usr/bin/gpm-root $(TARGET_DIR)/usr/bin/
	$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/bin/gpm-root
	cp -dpf $(STAGING_DIR)/usr/lib/libgpm.so* $(TARGET_DIR)/usr/lib/
	$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libgpm.so.1.*

GPM gpm: uclibc $(TARGET_DIR)/usr/sbin/gpm

gpm-clean:
	(cd $(GPM_DIR); \
	$(MAKE) clean \
	);
	rm $(GPM_DIR)/.compiled
	rm $(TARGET_DIR)/usr/lib/libgpm.so*

gpm-dirclean:
	rm -rf $(GPM_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_GPM)),y)
TARGETS+=gpm
endif
