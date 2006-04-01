#############################################################
#
# qte_gpl: Build of an Open Source Qt/Embedded environment.
#          Includes Qt/E for host (x86), QVfb and Opie (target)
#
#############################################################

ifeq ($(BR2_QTE_GPL_VERSION),)
BR2_QTE_GPL_VERSION:=FOOBAR1
endif
#ifeq ($(BR2_QTE_QT3_VERSION),)
#BR2_QTE_QT3_VERSION:=FOOBAR2
#endif
ifeq ($(BR2_QTE_GPL_QVFB_VERSION),)
BR2_QTE_GPL_QVFB_VERSION:=FOOBAR3
endif
ifeq ($(BR2_QTE_GPL_OPIE_VERSION),)
BR2_QTE_GPL_OPIE_VERSION:=FOOBAR4
endif
ifeq ($(BR2_QTE_GPL_TMAKE_VERSION),)
BR2_QTE_GPL_TMAKE_VERSION:=FOOBAR5
endif

# Versions of the libraries/tools choosen by user ---
BR2_QTE_GPL_C_QTE_VERSION:=$(shell echo $(BR2_QTE_GPL_VERSION)| sed -e 's/"//g')
BR2_QTE_GPL_C_QVFB_VERSION:=$(shell echo $(BR2_QTE_GPL_QVFB_VERSION)| sed -e 's/"//g')
BR2_QTE_GPL_C_OPIE_VERSION:=$(shell echo $(BR2_QTE_GPL_OPIE_VERSION)| sed -e 's/"//g')
BR2_QTE_GPL_C_TMAKE_VERSION:=$(shell echo $(BR2_QTE_GPL_TMAKE_VERSION)| sed -e 's/"//g')
BR2_QTE_GPL_C_QVFB_VERSION:=2.3.2
QTE_GPL_OPIE_VERSION:=1.2.0

# Source tarball to download ---
QTE_GPL_QTE_SOURCE:=qt-embedded-$(BR2_QTE_GPL_C_QTE_VERSION)-free.tar.gz
QTE_GPL_TMAKE_SOURCE:=tmake-$(BR2_QTE_GPL_C_TMAKE_VERSION).tar.gz
QTE_GPL_QVFB_SOURCE:=qt-x11-$(BR2_QTE_GPL_C_QVFB_VERSION).tar.gz
QTE_GPL_OPIE_SOURCE:=opie-source-$(QTE_GPL_OPIE_VERSION).tar.bz2

# Web site to load tarballs ---
QTE_GPL_SITE:=ftp://ftp.trolltech.com/qt/source/
QTE_GPL_TMAKE_SITE:=ftp://ftp.trolltech.com/freebies/tmake/
QTE_GPL_UIC_BINARY_SITE:=http://www.vanille.de/tools/
QTE_GPL_UIC_VERSION:=uic-qt2
QTE_GPL_QVFB_BINARY_SITE:=http://www.vanille.de/tools/
QTE_GPL_QVFB_VERSION:=qvfb-qt2
QTE_GPL_OPIE_SITE:=http://opie.handhelds.org/feed/source/stable/

# Directories containing extracted tarballs ---
QTE_GPL_BUILD_DIR:=$(BUILD_DIR)/qt_gpl/
QTE_GPL_QTE_DIR:=$(QTE_GPL_BUILD_DIR)/qt-$(BR2_QTE_GPL_C_QTE_VERSION)
QTE_GPL_QTE_HOST_DIR:=$(QTE_GPL_BUILD_DIR)/qt-$(BR2_QTE_GPL_C_QTE_VERSION)-x86
QTE_GPL_TMAKE_DIR:=$(QTE_GPL_BUILD_DIR)/tmake-$(BR2_QTE_GPL_C_TMAKE_VERSION)
QTE_GPL_QVFB_DIR:=$(QTE_GPL_BUILD_DIR)/qt-$(BR2_QTE_GPL_C_QVFB_VERSION)
QTE_GPL_OPIE_DIR:=$(QTE_GPL_BUILD_DIR)/opie-$(QTE_GPL_OPIE_VERSION)

QTE_GPL_CAT:=zcat
QTE_GPL_OPIE_CAT:=bzcat
# Target file name ---
TMAKE:=$(QTE_GPL_TMAKE_DIR)/bin/tmake
QTE_GPL_UIC_BINARY:=bin/uic
QTE_GPL_QVFB_BINARY:=bin/qvfb
#QTE_QTE_LIB:=$(QTE_QTE_DIR)/lib/libqte-mt.so.$(BR2_QTE_C_QTE_VERSION)
QTE_GPL_QTE_LIB:=$(QTE_GPL_QTE_DIR)/lib/libqte-mt.so
QTE_GPL_QTE_HOST_LIB:=$(QTE_GPL_QTE_HOST_DIR)/lib/libqte-mt.so
#QTE_QTE_LIB:=$(TARGET_DIR)/lib/libqte-mt.so.$(BR2_QTE_C_QTE_VERSION)
#QTE_QTOPIA_FILE:=$(QTE_QTOPIA_DIR)/bin/qpe
#QTE_QTOPIA_IFILE:=$(QTE_QTOPIA_DIR)/opt/Qtopia/bin/qpe
QTE_GPL_OPIE_FILE:=$(QTE_GPL_OPIE_DIR)/bin/qpe

# Directory where to install libraries
QTE_GPL_QTE_LIB_INSTALL_DIR:=/not_defined
BR2_QTE_GPL_C_LIB_INSTALL_DIR:=$(shell echo $(BR2_QTE_GPL_LIB_INSTALL_DIR)| sed -e 's/"//g')
ifeq ($(BR2_QTE_GPL_NFS_DIR),y)
QTE_GPL_QTE_LIB_INSTALL_DIR:=$(BR2_QTE_GPL_C_LIB_INSTALL_DIR)/lib
endif
ifeq ($(BR2_QTE_GPL_TARGET_DIR),y)
QTE_GPL_QTE_LIB_INSTALL_DIR:=$(TARGET_DIR)/$(BR2_QTE_GPL_C_LIB_INSTALL_DIR)/lib
endif

#export QT2DIR=$(pwd)/qt-2.3.2
#export QT3DIR=$(pwd)/qt-%{qt_version}
#export QTEDIR=$(pwd)/qt-%{qte_version}
#export QPEDIR=$(pwd)

#############################################################
#
# Calculate configure options... scary eventually, trivial now
#
# currently only tested with threading
# FIXME: I should use the staging directory here, but I don't yet.
#
#############################################################
# I choose to make the link in libqte so that the linking later is trivial -- a user may choose to use -luuid, or not, and it'll just work.
# ...since libqte* needs -luuid anyhow... 
QTE_GPL_QTE_CONFIGURE:=-no-xft -L$(E2FSPROGS_DIR)/lib -luuid
QTE_GPL_QVFB_CONFIGURE:=-no-xft -qvfb -no-thread
#QTE_QTOPIA_CONFIGURE:=
#QTE_QT3_CONFIGURE:=

ifeq ($(BR2_PTHREADS_NATIVE),y)
QTE_GPL_QTE_CONFIGURE:=$(QTE_GPL_QTE_CONFIGURE) -thread 
#QTE_GPL_QVFB_CONFIGURE:=$(QTE_GPL_QVFB_CONFIGURE) -no-thread
#QTE_QTOPIA_CONFIGURE:=$(QTE_QTOPIA_CONFIGURE) -thread
#QTE_QT3_CONFIGURE:=$(QTE_QT3_CONFIGURE) -thread
endif

# If libjpeg is already configured in buildroot, use it
ifeq ($(BR2_PACKAGE_JPEG),y)
QTE_GPL_QTE_CONFIGURE:=$(QTE_GPL_QTE_CONFIGURE) -system-jpeg
#FIXME: Do I need an else on this?
endif

# If libpng is already configured in buildroot, use it
ifeq ($(BR2_PACKAGE_LIBPNG),y)
QTE_GPL_QTE_CONFIGURE:=$(QTE_GPL_QTE_CONFIGURE) -system-libpng
else
QTE_GPL_QTE_CONFIGURE:=$(QTE_GPL_QTE_CONFIGURE) -qt-libpng
endif

# as of 2005-08-17's snapshot, uClibc's pthread does NOT support pthread_yield, which is needed
# for ffmpeg's qtopia-phone-2.1.1/src/3rdparty/plugins/codecs/libffmpeg/mediapacketbuffer.h:230
# (also called at line 232).  ...so we have to disable ffmpeg
#QTE_QTOPIA_CONFIGURE:=$(QTE_QTOPIA_CONFIGURE) -without-libffmpeg

#QTE_QTOPIA_CONFIGURE:=$(QTE_QTOPIA_CONFIGURE) -L $(E2FSPROGS_DIR)/lib -I $(E2FSPROGS_DIR)/lib -luuid


#############################################################
#
# Build portion
#
#############################################################

############################
## Download source packages:
#

#ifneq ($(BR2_QTE_C_QTE_VERSION),$(BR2_QTE_C_QT3_VERSION))
#$(DL_DIR)/$(QTE_QT3_SOURCE):
#	$(WGET) -P $(DL_DIR) $(QTE_SITE)/$(@F)
#endif

# Qt/E for target and host ---
$(DL_DIR)/$(QTE_GPL_QTE_SOURCE) $(DL_DIR)/$(QTE_GPL_QVFB_SOURCE):
	$(WGET) -P $(DL_DIR) $(QTE_GPL_SITE)/$(@F)

# Opie for target and host ---
$(DL_DIR)/$(QTE_GPL_OPIE_SOURCE):
	$(WGET) -P $(DL_DIR) $(QTE_GPL_OPIE_SITE)/$(@F)

# Tmake for (host and target) Makefile generation from .pro ---
$(DL_DIR)/$(QTE_GPL_TMAKE_SOURCE):
	$(WGET) -P $(DL_DIR) $(QTE_GPL_TMAKE_SITE)/$(@F)

# Pre-compiled uic ---
$(DL_DIR)/$(QTE_GPL_UIC_VERSION):
	$(WGET) -P $(DL_DIR) $(QTE_GPL_UIC_BINARY_SITE)/$(QTE_GPL_UIC_VERSION)

# Pre-compiled qvfb ---
$(DL_DIR)/$(QTE_GPL_QVFB_VERSION):
	$(WGET) -P $(DL_DIR) $(QTE_GPL_QVFB_BINARY_SITE)/$(QTE_GPL_QVFB_VERSION)


##########################
## Unpack source packages:
#

$(QTE_GPL_TMAKE_DIR)/.unpacked: $(DL_DIR)/$(QTE_GPL_TMAKE_SOURCE)
	$(QTE_GPL_CAT) $(DL_DIR)/$(QTE_GPL_TMAKE_SOURCE) | tar -C $(QTE_GPL_BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

#ifneq ($(BR2_QTE_C_QTE_VERSION),$(BR2_QTE_C_QT3_VERSION))
#$(QTE_QT3_DIR)/.unpacked: $(DL_DIR)/$(QTE_QT3_SOURCE)
#	$(QTE_CAT) $(DL_DIR)/$(QTE_QT3_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
#	touch $@
#endif

$(QTE_GPL_QTE_DIR)/.unpacked: $(DL_DIR)/$(QTE_GPL_QTE_SOURCE)
	test -d $(QTE_GPL_BUILD_DIR) || install -dm 0755 $(QTE_GPL_BUILD_DIR)
	$(QTE_GPL_CAT) $(DL_DIR)/$(QTE_GPL_QTE_SOURCE) | tar -C $(QTE_GPL_BUILD_DIR) $(TAR_OPTIONS) -
#ifeq ($(strip $(BR2_PACKAGE_QTE_GPL_QVFB)),y)
	cp -r $(QTE_GPL_QTE_DIR)/ $(QTE_GPL_QTE_HOST_DIR)/
#endif
	touch $@
	touch $(QTE_GPL_QTE_HOST_DIR)/.unpacked

$(QTE_GPL_QTE_HOST_DIR)/.unpacked: $(DL_DIR)/$(QTE_GPL_QTE_SOURCE) $(QTE_GPL_QTE_DIR)/.unpacked
	cp -r $(QTE_GPL_QTE_DIR)/ $(QTE_GPL_QTE_HOST_DIR)/
	touch $@

$(QTE_GPL_QVFB_DIR)/.unpacked: $(DL_DIR)/$(QTE_GPL_QVFB_SOURCE)
	$(QTE_GPL_CAT) $(DL_DIR)/$(QTE_GPL_QVFB_SOURCE) | tar -C $(QTE_GPL_BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(QTE_GPL_OPIE_DIR)/.unpacked: $(DL_DIR)/$(QTE_GPL_OPIE_SOURCE)
	$(QTE_GPL_OPIE_CAT) $(DL_DIR)/$(QTE_GPL_OPIE_SOURCE) | tar -C $(QTE_GPL_BUILD_DIR) $(TAR_OPTIONS) -
	touch $@


############
## Patches:
#

# Qt/E target patch for Opie & some compilation issues
$(QTE_GPL_QTE_DIR)/.patched: $(QTE_GPL_OPIE_DIR)/.unpacked $(QTE_GPL_QTE_DIR)/.unpacked
	toolchain/patch-kernel.sh $(QTE_GPL_QTE_DIR) $(QTE_GPL_OPIE_DIR)/qt/qt-$(BR2_QTE_GPL_C_QTE_VERSION).patch/ qt-$(BR2_QTE_GPL_C_QTE_VERSION)-all.patch
	toolchain/patch-kernel.sh $(QTE_GPL_QTE_DIR) package/qte_gpl/ *.patch
	touch $@

# Qt/E host patch for Opie
$(QTE_GPL_QTE_HOST_DIR)/.patched: $(QTE_GPL_OPIE_DIR)/.unpacked $(QTE_GPL_QTE_HOST_DIR)/.unpacked
	toolchain/patch-kernel.sh $(QTE_GPL_QTE_HOST_DIR) $(QTE_GPL_OPIE_DIR)/qt/qt-$(BR2_QTE_GPL_C_QTE_VERSION).patch/ qt-$(BR2_QTE_GPL_C_QTE_VERSION)-all.patch
	touch $@

# Install uic in Qt/E target dir
$(QTE_GPL_QTE_DIR)/$(QTE_GPL_UIC_BINARY): $(QTE_GPL_QTE_DIR)/.unpacked $(DL_DIR)/$(QTE_GPL_UIC_VERSION)
	test -d $(@D) || install -dm 0755 $(@D)
	install -m 0755 $(DL_DIR)/$(QTE_GPL_UIC_VERSION) $@

# Install uic in Qt/E host dir
$(QTE_GPL_QTE_HOST_DIR)/$(QTE_GPL_UIC_BINARY): $(QTE_GPL_QTE_HOST_DIR)/.unpacked $(DL_DIR)/$(QTE_GPL_UIC_VERSION)
	test -d $(@D) || install -dm 0755 $(@D)
	install -m 0755 $(DL_DIR)/$(QTE_GPL_UIC_VERSION) $@

# Install QVfb in Qt/E host dir
$(QTE_GPL_QTE_HOST_DIR)/$(QTE_GPL_QVFB_BINARY): $(QTE_GPL_QTE_HOST_DIR)/.unpacked $(DL_DIR)/$(QTE_GPL_QVFB_VERSION)
	test -d $(@D) || install -dm 0755 $(@D)
	install -m 0755 $(DL_DIR)/$(QTE_GPL_QVFB_VERSION) $@


#################
## Configuration
#

# Qt/Embedded for the target ---
$(QTE_GPL_QTE_DIR)/.configured: $(QTE_GPL_QTE_DIR)/.unpacked $(QTE_GPL_TMAKE_DIR)/.unpacked $(QTE_GPL_QTE_DIR)/$(QTE_GPL_UIC_BINARY) $(QTE_GPL_OPIE_DIR)/.unpacked 
	ln -sf $(QTE_GPL_OPIE_DIR)/qt/qconfig-qpe.h $(QTE_GPL_QTE_DIR)/src/tools/qconfig-qpe.h
	(cd $(@D); export QTDIR=`pwd`; export TMAKEPATH=$(QTE_GPL_TMAKE_DIR)/lib/qws/linux-arm-g++; export PATH=$(STAGING_DIR)/bin:$$QTDIR/bin:$$PATH; export LD_LIBRARY_PATH=$$QTDIR/lib:$$LD_LIBRARY_PATH; echo 'yes' | \
		$(TARGET_CONFIGURE_OPTS) CC_FOR_BUILD=$(HOSTCC) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		$(QTE_GPL_QTE_CONFIGURE) -qconfig qpe -keypad-mode -no-qvfb -depths 4,8,16,32 -xplatform linux-arm-g++ \
	);
	touch $@

# Qt/Embedded for the host, to have a simulation environment ---
$(QTE_GPL_QTE_HOST_DIR)/.configured: $(QTE_GPL_QTE_HOST_DIR)/.unpacked $(QTE_GPL_TMAKE_DIR)/.unpacked $(QTE_GPL_QTE_DIR)/$(QTE_GPL_UIC_BINARY) $(QTE_GPL_OPIE_DIR)/.unpacked
	ln -sf $(QTE_GPL_OPIE_DIR)/qt/qconfig-qpe.h $(QTE_GPL_QTE_HOST_DIR)/src/tools/qconfig-qpe.h
	(cd $(@D); export QTDIR=`pwd`; export TMAKEPATH=$(QTE_GPL_TMAKE_DIR)/lib/qws/linux-x86-g++; export PATH=$(STAGING_DIR)/bin:$$QTDIR/bin:$$PATH; export LD_LIBRARY_PATH=$$QTDIR/lib:$$LD_LIBRARY_PATH; echo 'yes' | \
		./configure \
		$(QTE_GPL_QTE_CONFIGURE) -qconfig qpe -qvfb -depths 4,8,16,32 \
        );
	touch $@

#ifneq ($(BR2_QTE_GPL_C_QTE_VERSION),$(BR2_QTE_C_QT3_VERSION))
## this is a host-side build, so we don't use any staging dir stuff, nor any TARGET_CONFIGURE_OPTS
#$(QTE_QT3_DIR)/.configured: $(QTE_QT3_DIR)/.unpacked $(QTE_TMAKE_DIR)/.unpacked
#	(cd $(@D); export QTDIR=`pwd`; export TMAKEPATH=$(QTE_TMAKE_DIR)/lib/qws/linux-x86-g++; export PATH=$$QTDIR/bin:$$PATH; export LD_LIBRARY_PATH=$$QTDIR/lib:$$LD_LIBRARY_PATH; echo 'yes' | \
#		CC_FOR_BUILD=$(HOSTCC) \
#		./configure \
#		-fast $(QTE_QT3_CONFIGURE) \
#	);
#	touch $@
#endif

# QVfb for the host from Qt/X11 sources ---
$(QTE_GPL_QVFB_DIR)/.configured: $(QTE_GPL_QVFB_DIR)/.unpacked $(QTE_GPL_TMAKE_DIR)/.unpacked
	(cd $(@D); export QTDIR=`pwd`; export TMAKEPATH=$(QTE_GPL_TMAKE_DIR)/lib/linux-g++; export $$QTDIR/bin:$$PATH; export LD_LIBRARY_PATH=$$QTDIR/lib:$$LD_LIBRARY_PATH; echo 'yes' | \
		./configure \
		$(QTE_GPL_QVFB_CONFIGURE)\
	);
	touch $@

# There is no build for tmake, only unpack ---
$(TMAKE): $(QTE_GPL_TMAKE_DIR)/.unpacked


###############
## Compilation
#

#ifneq ($(BR2_QTE_C_QTE_VERSION),$(BR2_QTE_C_QT3_VERSION))
#$(QTE_QT3_DIR)/.make: $(QTE_QT3_DIR)/.unpacked
#	( export QTDIR=$(QTE_QT3_DIR); export PATH=$$QTDIR/bin:$$PATH; export LD_LIBRARY_PATH=$$QTDIR/lib:$$LD_LIBRARY_PATH; \
#	$(MAKE) -C $(QTE_QT3_DIR) sub-src && \
#	$(MAKE) -C $(QTE_QT3_DIR)/tools/linguist/lrelease \
#	$(MAKE) -C $(QTE_QT3_DIR)/tools/linguist/lupdate \
#	$(MAKE) -C $(QTE_QT3_DIR)/tools/designer/uilib \
#	$(MAKE) -C $(QTE_QT3_DIR)/tools/designer/uic
#	);
#	touch $@
#endif

## Build Qt/X11 for the host to have uic ---
#$(QTE_QVFB_DIR)/.make: $(QTE_QVFB_DIR)/.configured
#	export QTDIR=$(QTE_QVFB_DIR); export QPEDIR=$(QTE_OPIE_DIR); export PATH=$$QTDIR/bin:$$PATH; export LD_LIBRARY_PATH=$$QTDIR/lib:$$LD_LIBRARY_PATH; \
#	$(MAKE) -C $(QTE_QVFB_DIR)
#	# ... and make sure it actually built... grrr... make deep-deep-deep makefile recursion for this habit
#	touch $@
#	test -f $@

## Build QVfb ---
## In this step we build and link the qvfb sources from Qt/Embedded 2.3.10 against the static Qt library from Qt 2.3.2. 
#$(QTE_QTE_HOST_DIR)/$(QTE_QVFB_BINARY): $(QTE_QVFB_DIR)/.make $(QTE_QTE_DIR)/.unpacked $(TMAKE)
#	( export QTDIR=$(QTE_QVFB_DIR); export PATH=$$QTDIR/bin:$$PATH; export LD_LIBRARY_PATH=$$QTDIR/lib:$$LD_LIBRARY_PATH; export QT_THREAD_SUFFIX=-mt;\
#	cd $(QTE_QTE_HOST_DIR)/tools/qvfb && TMAKEPATH=$(QTE_TMAKE_DIR)/lib/linux-g++ $(TMAKE) -o Makefile qvfb.pro; $(MAKE) )
#	test -d $(@D) || install -dm 0755 $(@D)
#	install -m 0755 $(QTE_QTE_HOST_DIR)/tools/qvfb/$(@F) $@

# Build Qt/Embedded for host ---
$(QTE_GPL_QTE_HOST_LIB): $(QTE_GPL_QTE_HOST_DIR)/.configured $(QTE_GPL_QTE_HOST_DIR)/$(QTE_GPL_QVFB_BINARY)
	export QTDIR=$(QTE_GPL_QTE_HOST_DIR); export QPEDIR=$(QTE_GPL_OPIE_DIR); export PATH=$(STAGING_DIR)/bin:$$QTDIR/bin:$$PATH; \
	$(MAKE) -C $(QTE_GPL_QTE_HOST_DIR)
	# ... and make sure it actually built... grrr... make deep-deep-deep makefile recursion for this habit
	test -f $@

# Build Qt/Embedded for the target ---
$(QTE_GPL_QTE_DIR)/src-mt.mk: $(QTE_GPL_QTE_DIR)/.configured
	# I don't like the src-mk that gets built, so blow it away.  Too many includes to override yet
	echo "SHELL=/bin/sh" > $@
	echo "" >> $@
	echo "src-mt:" >> $@
	echo "	cd src; "'$$(MAKE)'" 'QT_THREAD_SUFFIX=-mt' 'QT_LFLAGS_MT="'$$$$(SYSCONF_LFLAGS_THREAD)'" "'$$$$(SYSCONF_LIBS_THREAD)'"' 'QT_CXX_MT="'$$$$(SYSCONF_CXXFLAGS_THREAD)'" -DQT_THREAD_SUPPORT' 'QT_C_MT="'$$$$(SYSCONF_CFLAGS_THREAD)'" -DQT_THREAD_SUPPORT'" >> $@

$(QTE_GPL_QTE_LIB): $(QTE_GPL_QTE_DIR)/src-mt.mk
	export QTDIR=$(QTE_GPL_QTE_DIR); export QPEDIR=$(QTE_GPL_OPIE_DIR); export PATH=$(STAGING_DIR)/bin:$$QTDIR/bin:$$PATH; \
	$(TARGET_CONFIGURE_OPTS) $(MAKE) $(TARGET_CC) -C $(QTE_GPL_QTE_DIR) src-mt
	echo "Installing Qt to [" $(QTE_GPL_QTE_LIB_INSTALL_DIR) "]"
	$(TARGET_CONFIGURE_OPTS) $(MAKE) $(TARGET_CC) DESTDIR=$(QTE_GPL_QTE_LIB_INSTALL_DIR) -C $(QTE_GPL_QTE_DIR) src-mt
#	$(TARGET_CONFIGURE_OPTS) $(MAKE) $(TARGET_CC) -C $(QTE_GPL_QTE_DIR) sub-examples
	# ... and make sure it actually built... grrr... make deep-deep-deep makefile recursion for this habit
	test -f $@

#$(QTE_QTOPIA_FILE): $(QTE_QTOPIA_DIR)/.configured
#	export QTDIR=$(QTE_QT3_DIR); export QPEDIR=$(QTE_QTOPIA_DIR); export PATH=$(STAGING_DIR)/bin:$$QTDIR/bin:$$PATH; \
#	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(QTE_QTOPIA_DIR)

#$(QTE_QTOPIA_IFILE): $(QTE_QTOPIA_FILE)
#	export QTDIR=$(QTE_QT3_DIR); export QPEDIR=$(QTE_QTOPIA_DIR); export PATH=$(STAGING_DIR)/bin:$$QTDIR/bin:$$PATH; \
#	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(QTE_QTOPIA_DIR) install PREFIX=$(TARGET_DIR)


######################
## Main build targets
#

qte_gpl:: $(QTE_GPL_QTE_LIB)

ifeq ($(strip $(BR2_PACKAGE_QTE_GPL_QVFB)),y)
qte_gpl:: $(QTE_GPL_QTE_HOST_LIB)
endif

ifeq ($(strip $(BR2_PACKAGE_QTE_GPL_OPIE)),y)
qte_gpl:: $(QTE_OPIE_IFILE)
endif

# kinda no-op right now, these are built anyhow
#ifeq ($(strip $(BR2_PACKAGE_QTE_QVFB)),y)
#qte:: $(QTE_QTE_DIR)/$(QTE_UIC_BINARY) $(QTE_QTE_DIR)/$(QTE_QVFB_BINARY)
#endif

qte_gpl-clean:
	rm -f $(QTE_GPL_QTE_DIR)/$(QTE_GPL_UIC_BINARY) $(QTE_GPL_QTE_DIR)/$(QTE_GPL_QVFB_BINARY) $(QTE_GPL_QTE_LIB)
	-$(MAKE) -C $(QTE_GPL_QTE_DIR) clean
	-$(MAKE) -C $(QTE_GPL_QVFB_DIR) clean
	-$(MAKE) -C $(QTE_GPL_OPIE_DIR) clean

qte_gpl-dirclean:
	rm -rf $(QTE_GPL_QTE_DIR) $(QTE_GPL_QVFB_DIR) $(QTE_GPL_OPIE_DIR)


#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_QTE_GPL)),y)
TARGETS+=qte_gpl
endif

