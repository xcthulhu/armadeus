#############################################################
#
# samba
#
#############################################################

# TARGETS
SAMBA_VER:=3.0.20
SAMBA_SITE:=http://us3.samba.org/samba/ftp/old-versions/
SAMBA_PUBLICKEY_SITE:=http://us4.samba.org/samba/ftp/
SAMBA_SOURCE:=samba-$(SAMBA_VER).tar.gz
SAMBA_SOURCE_KEY:=samba-$(SAMBA_VER).tar.asc
SAMBA_PUBLICKEY:=samba-pubkey.asc
SAMBA_SIGNATURE:=samba-$(SAMBA_VER).tar.gz.asc
SAMBA_DIR:=$(BUILD_DIR)/samba-$(SAMBA_VER)
SAMBA_SRC_DIR:=$(SAMBA_DIR)/source
SAMBA_BIN_DIR:=$(SAMBA_SRC_DIR)/bin
SAMBA_PREFIX:=/etc/samba

# remove the previous one
removeoldsamba:
	rm -f $(TARGET_DIR)/usr/bin/smbmount $(TARGET_DIR)/usr/bin/smbumount $(TARGET_DIR)/usr/bin/smbmnt
	rm -f $(TARGET_DIR)/usr/sbin/nmbd
	rm -f $(TARGET_DIR)/usr/sbin/smbd
	rm -f $(TARGET_DIR)/usr/bin/smbpasswd
	rm -Rf  $(TARGET_DIR)$(SAMBA_PREFIX) 
	rm -f  $(TARGET_DIR)/etc/init.d/S81smb
 

$(DL_DIR)/$(SAMBA_SOURCE):
	$(WGET) -P $(DL_DIR) $(SAMBA_SITE)/$(SAMBA_SOURCE)

$(DL_DIR)/$(SAMBA_SOURCE_KEY):
	$(WGET) -P $(DL_DIR) $(SAMBA_SITE)/$(SAMBA_SOURCE_KEY)

$(DL_DIR)/$(SAMBA_PUBLICKEY):
	$(WGET) -P $(DL_DIR) $(SAMBA_PUBLICKEY_SITE)/$(SAMBA_PUBLICKEY)


$(SAMBA_DIR)/.unpacked: $(DL_DIR)/$(SAMBA_SOURCE) 
	gunzip -c  $(DL_DIR)/$(SAMBA_SOURCE)  | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(SAMBA_DIR) package/samba/ samba\*.patch
	touch $(SAMBA_DIR)/.unpacked

# NOTES:  	--> disable cups (compiling problem) cups is not necessary
#		--> add option --enable-debug  if necessary!!
# 		--host="arm-armadeus-linux-uclibc" force cross compilation and means
#			host_cpu='arm'
#			host_os='linux-uclibc'
#			host_vendor='armadeus'
# 		use -DHAVE_GETTIMEOFDAY_TZ because of gettimeofday() use a second argument 
# 		(obsolete but defined TZ)
#		PIE (Periodic Interrupt Enable) must be disabled (not available on Armadeus)

$(SAMBA_SRC_DIR)/.configured: $(SAMBA_DIR)/.unpacked
	cd $(SAMBA_SRC_DIR) ;\
	./configure --host="arm-armadeus-linux-uclibc"  --prefix=$(SAMBA_PREFIX) \
	--with-logfilebase=/var/log/samba --with-piddir=/var/lock --with-configdir=$(SAMBA_PREFIX)\
 	--disable-cups  --disable-pie  --with-smbmount  --with-automount --disable-xmltest \
	CC=arm-linux-gcc \
	CFLAGS="-Wall -Wstrict-prototypes -Os -fomit-frame-pointer -DHAVE_GETTIMEOFDAY_TZ \
	-DHAVE_IFACE_IFCONF -DUSE_SETEUID -DHAVE_STRUCT_TIMESPEC "  
	touch $(SAMBA_SRC_DIR)/.configured


$(SAMBA_SRC_DIR)/Makefile: $(SAMBA_SRC_DIR)/.configured
	cd $(SAMBA_SRC_DIR) ;\
	$(MAKE) proto ; \
	$(MAKE)
	$(STRIP) $(SAMBA_BIN_DIR)/*

samba: $(SAMBA_SRC_DIR)/Makefile
	mkdir -p $(TARGET_DIR)$(SAMBA_PREFIX) 
	mkdir -p $(TARGET_DIR)$(SAMBA_PREFIX)/private
	mkdir -p $(TARGET_DIR)$(SAMBA_PREFIX)/var/locks
	$(INSTALL) -m 0644 -D package/samba/smb.conf  $(TARGET_DIR)$(SAMBA_PREFIX)/smb.conf
	$(INSTALL) -m 0600 -D package/samba/smbpasswd  $(TARGET_DIR)$(SAMBA_PREFIX)/private/smbpasswd

smbautostart:
	mkdir -p $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 0775 -D package/samba/S81smb $(TARGET_DIR)/etc/init.d/S81smb

smbmount: 
	mkdir -p $(TARGET_DIR)/usr/bin
	cp -dpf $(SAMBA_BIN_DIR)/smbmount $(TARGET_DIR)/usr/bin/
	cp -dpf $(SAMBA_BIN_DIR)/smbumount $(TARGET_DIR)/usr/bin/
	cp -dpf $(SAMBA_BIN_DIR)/smbmnt  $(TARGET_DIR)/usr/bin/

smbd: 
	mkdir -p $(TARGET_DIR)/usr/sbin
	cp -dpf $(SAMBA_BIN_DIR)/smbd $(TARGET_DIR)/usr/sbin/
	cp -dpf $(SAMBA_BIN_DIR)/nmbd $(TARGET_DIR)/usr/sbin/

smbpasswd: 
	mkdir -p $(TARGET_DIR)/usr/sbin
	cp -dpf $(SAMBA_BIN_DIR)/smbpasswd $(TARGET_DIR)/usr/bin/
	
samba-dirclean: 
	rm -rf $(SAMBA_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################

TARGETS+=removeoldsamba

ifeq ($(strip $(BR2_PACKAGE_SAMBA)),y)
TARGETS+=samba 
endif

ifeq ($(strip $(BR2_PACKAGE_SMBMOUNT)),y)
TARGETS+=smbmount
endif

ifeq ($(strip $(BR2_PACKAGE_SMBD)),y)
TARGETS+=smbd
endif

ifeq ($(strip $(BR2_PACKAGE_SMBPASSWD)),y)
TARGETS+=smbpasswd
endif

ifeq ($(strip $(BR2_PACKAGE_SMB_START)),y)
TARGETS+=smbautostart
endif


