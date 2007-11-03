# Makefile for armadeus
#
# Copyright (C) 2005-2006 by <jorasse@armadeus.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

#--------------------------------------------------------------
# Just run 'make menuconfig', configure stuff, then run 'make'.
# You shouldn't need to mess with anything beyond this point...
#--------------------------------------------------------------

BUILDROOT_DIR = buildroot

BUILDROOT_NAME=buildroot
BUILDROOT_FILE_PATH:=downloads
BUILDROOT_SITE:=http://buildroot.uclibc.org/downloads/snapshots
BUILDROOT_VERSION:=20071027
BUILDROOT_SOURCE:=buildroot-$(BUILDROOT_VERSION).tar.bz2

ARMADEUS_TOPDIR:=$(shell pwd)
export ARMADEUS_TOPDIR

PATCH_DIR=patches
PATCH_CYGWIN_DIR=$(PATCH_DIR)/cygwin
TAR_OPTIONS=--exclude=.svn -xf 

#used only by cygwin to restore the rights on some rootfs directories
BUILDROOT_ROOT_DIR=$(BUILDROOT_DIR)/project_build_arm/root

LINUX-REL=`grep "BR2_PACKAGE_LINUX_VERSION=" $(BUILDROOT_DIR)/.config | \
      sed "s/BR2_PACKAGE_LINUX_VERSION=\\"//" | sed "s/\\"//"`
LINUX_DIR=$(BUILDROOT_DIR)/build_arm/linux-$(LINUX-REL)


all: buildroot

help:
	@echo 'Cleaning:'
	@echo '  buildroot-clean	- delete all non-source files in buildroot'
	@echo '  clean                  - delete temporary files created by build'
	@echo '  distclean              - delete all non-source files (including .config)'
	@echo
	@echo 'Build:'
	@echo '  all                    - make world'
	@echo '  <Package>		- a single package (ex: u-boot linux or buildroot'
	@echo
	@echo 'Configuration:'
	@echo '  menuconfig             - interactive curses-based configurator'
	@echo '  oldconfig              - resolve any unresolved symbols in .config'
	@echo
	@echo 'Miscellaneous:'
	@echo '  source                 - download all sources needed for offline-build'
	@echo '  source-check           - check all packages for valid download URLs'
	@echo
	@echo 'Development:'
	@echo '  buildroot-patch	- generate patch file for buildroot'
	@echo '  sourceball		- create a distribution tarball'
	@echo
	@echo 'See docs/README and docs/buildroot.html for further details'
	@echo


# configuration
# ---------------------------------------------------------------------------

$(BUILDROOT_FILE_PATH)/$(BUILDROOT_SOURCE):
	mkdir -p $(BUILDROOT_FILE_PATH)
	wget --passive-ftp -P $(BUILDROOT_FILE_PATH)  $(BUILDROOT_SITE)/$(BUILDROOT_SOURCE)


$(BUILDROOT_DIR)/.unpacked: $(BUILDROOT_FILE_PATH)/$(BUILDROOT_SOURCE) $(PATCH_DIR)/*.diff
	bzcat $(BUILDROOT_FILE_PATH)/$(BUILDROOT_SOURCE) | \
	tar --exclude=.svn -C $(BUILDROOT_DIR)/.. $(TAR_OPTIONS) -
	$(BUILDROOT_DIR)/toolchain/patch-kernel.sh $(BUILDROOT_DIR) $(PATCH_DIR) \*.diff 
	@if uname | grep -i cygwin >/dev/null 2>&1 ; then  \
		./host/patches/cygwin/run ; \
		svn revert . ; \
    	$(BUILDROOT_DIR)/toolchain/patch-kernel.sh $(BUILDROOT_DIR) $(PATCH_CYGWIN_DIR) *.diff ; \
     	sed -e 's/pc-linux-gnu/pc-cygwin/g' $(BUILDROOT_DIR)/.defconfig > $(BUILDROOT_DIR)/.defconfig_cygwin ; \
		mv -f $(BUILDROOT_DIR)/.defconfig_cygwin $(BUILDROOT_DIR)/.defconfig ; \
	fi;
	touch $(BUILDROOT_DIR)/.unpacked

buildroot: $(BUILDROOT_DIR)/.unpacked
	@if uname | grep -i cygwin >/dev/null 2>&1 ; then  \
		if [ -e "$(BUILDROOT_ROOT_DIR)/etc" ] ; then \
			chmod 777 $(BUILDROOT_DIR)/build_arm_nofpu/root/etc ; \
		fi; \
		if [ -e "$(BUILDROOT_ROOT_DIR)/etc" ] ; then \
			chmod 777 $(BUILDROOT_DIR)/build_arm_nofpu/root/dev ; \
		fi; \
	fi;
	@$(MAKE) -C $(BUILDROOT_DIR)

# To be called only one time if one wants to make an automatic build
buildauto: $(BUILDROOT_DIR)/.unpacked
	# Be sure that /local/downloads exists if you want to use automated build
	sed -e 's/BR2_DL_DIR/BR2_DL_DIR=\"\/local\/downloads\" #/g' $(BUILDROOT_DIR)/.defconfig > $(BUILDROOT_DIR)/.defconfig_new ; \
		mv -f $(BUILDROOT_DIR)/.defconfig_new $(BUILDROOT_DIR)/.defconfig
	echo "ey" | $(MAKE) -C $(BUILDROOT_DIR) menuconfig
	$(MAKE) -C $(BUILDROOT_DIR)

menuconfig: $(BUILDROOT_DIR)/.unpacked
	@$(MAKE) -C $(BUILDROOT_DIR) menuconfig


linux-menuconfig:
	@if [ -e "$(LINUX_DIR)/.unpacked" ] ; then \
		$(MAKE) -C $(LINUX_DIR) menuconfig ; \
	fi;

.DEFAULT: $(BUILDROOT_DIR)/.unpacked
	@$(MAKE) -C $(BUILDROOT_DIR) $@

buildroot-clean:
	rm -rf $(BUILDROOT_DIR)/build*
	rm -rf $(BUILDROOT_DIR)/binaries
	rm -rf $(BUILDROOT_DIR)/project_build*
	rm -rf $(BUILDROOT_DIR)/toolchain_build*

buildroot-dirclean:
	rm -rf $(BUILDROOT_DIR)	
	
.PHONY: dummy buildroot buildroot-clean


