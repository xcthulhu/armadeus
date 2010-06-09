#############################################################
#
# bluez-libs
#
#############################################################
BLUEZ_LIBS_VERSION:=3.36
BLUEZ_LIBS_SITE = http://bluez.sourceforge.net/download
BLUEZ_LIBS_SOURCE = bluez-libs-$(BLUEZ_LIBS_VERSION).tar.gz
BLUEZ_LIBS_INSTALL_STAGING = YES
BLUEZ_LIBS_INSTALL_TARGET = YES

$(eval $(call AUTOTARGETS,package,bluez-libs))

