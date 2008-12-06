# Makefile for armadeus
#
# Copyright (C) 2005-2007 by <jorasse@armadeus.com>
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
#BUILDROOT_SITE:=http://buildroot.uclibc.org/downloads/snapshots
BUILDROOT_SITE:=http://downloads.sourceforge.net/armadeus
BUILDROOT_VERSION:=20081103
BUILDROOT_SOURCE:=buildroot-$(BUILDROOT_VERSION).tar.bz2

-include $(BUILDROOT_DIR)/.config

ARMADEUS_TOPDIR:=$(shell pwd)
export ARMADEUS_TOPDIR

PATCH_DIR=patches
TAR_OPTIONS=--exclude=.svn -xf 

LINUX_DIR=$(BUILDROOT_DIR)/project_build_$(BR2_GCC_TARGET_ARCH)/$(BR2_BOARD_NAME)/linux-$(BR2_LINUX26_VERSION)

ECHO_CONFIGURATION_NOT_DEFINED:= echo -en "\033[1m"; \
                echo "                                                   "; \
                echo " System not configured. Use make <board>_defconfig " >&2; \
                echo " armadeus valid configurations are:                " >&2; \
                echo "     "$(shell find ./$(BUILDROOT_DIR)/target/device/armadeus -name *_defconfig|sed 's/.*\///');\
                echo "                                                   "; \
		echo -en "\033[0m";

default: all

help:
	@echo 'First, choose the platform:'
	@echo '  apf9328_defconfig  - must be launch before using apf9328 card'
	@echo '  apf27_defconfig    - must be launch before using apf27 card'
	@echo ''
	@echo 'Cleaning:'
	@echo '  buildroot-clean    - delete all non-source files in buildroot'
	@echo '  clean              - delete temporary files created by build'
	@echo '  distclean          - delete all non-source files (including .config)'
	@echo
	@echo 'Build:'
	@echo '  all            - make world '
	@echo '  <Package>      - a single package (ex: u-boot linux or buildroot'
	@echo
	@echo 'Configuration:'
	@echo '  menuconfig           - interactive curses-based configurator'
	@echo '  oldconfig            - resolve any unresolved symbols in .config'
	@echo
	@echo 'Miscellaneous:'
	@echo '  source                 - download all sources needed for offline-build'
	@echo '  source-check           - check all packages for valid download URLs'
	@echo
	@echo 'See www.armadeus.org for further armadeus details'
	@echo 'See /buildroot/docs/README and /buildroot/docs/buildroot.html'
	@echo '  for further buildroot details'
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
	touch $@

$(BUILDROOT_DIR)/.configured: $(BUILDROOT_DIR)/.unpacked
	@if [ ! -f $@ ]; then \
		$(ECHO_CONFIGURATION_NOT_DEFINED) \
		exit 1; \
	fi

# To be called only one time if one wants to make an automatic build
buildauto: $(BUILDROOT_DIR)/.unpacked
	# Be sure that /local/downloads exists if you want to use automated build
	sed -e 's/BR2_DL_DIR/BR2_DL_DIR=\"\/local\/downloads\" #/g' $(BUILDROOT_DIR)/.defconfig > $(BUILDROOT_DIR)/.defconfig_new ; \
		mv -f $(BUILDROOT_DIR)/.defconfig_new $(BUILDROOT_DIR)/.defconfig
	echo "ey" | $(MAKE) -C $(BUILDROOT_DIR) menuconfig
	$(MAKE) -C $(BUILDROOT_DIR)

linux-menuconfig:
	@if [ ! -e "target/linux/modules/fpga/POD/.pod" ] ; then \
		if [ -x target/linux/modules/fpga/podscript.sh ]; then \
			target/linux/modules/fpga/podscript.sh ; \
		fi \
	fi;
	@if [ -e "$(LINUX_DIR)/.unpacked" ] ; then \
		$(MAKE) -C $(BUILDROOT_DIR) linux26-menuconfig ; \
	fi;

linux26: $(BUILDROOT_DIR)/.configured
	@-touch $(LINUX_DIR)/.config 
	@$(MAKE) -C $(BUILDROOT_DIR) linux26

linux26-clean: $(BUILDROOT_DIR)/.configured
	@$(MAKE) -C $(BUILDROOT_DIR) linux26clean

%_defconfig: $(BUILDROOT_DIR)/.unpacked
	@if [ -e "./$(BUILDROOT_DIR)/target/device/armadeus/$(patsubst %_defconfig,%,$@)/$@" ]; then \
		rm -f $(BUILDROOT_DIR)/.config ; \
		$(MAKE) -C $(BUILDROOT_DIR) $@ ; \
		$(MAKE) -C $(BUILDROOT_DIR) menuconfig ; \
		touch $(BUILDROOT_DIR)/.configured ; \
	else \
		echo -e "\033[1m\nThis configuration does not exist !!\n\033[0m" ; \
	fi;

all: $(BUILDROOT_DIR)/.configured
	@echo "If your /bin/sh link doesn't point to /bin/bash, please correct that."
	@ls -al /bin/sh | grep bash 
	@$(MAKE) -C $(BUILDROOT_DIR) $@

menuconfig: $(BUILDROOT_DIR)/.configured
	@$(MAKE) -C $(BUILDROOT_DIR) $@

# !! .DEFAULT do NOT handle dependancies !!
.DEFAULT:
	@if [ ! -e "$(BUILDROOT_DIR)/.configured" ]; then \
		$(ECHO_CONFIGURATION_NOT_DEFINED) \
		exit 1; \
	fi
	@$(MAKE) -C $(BUILDROOT_DIR) $@

buildroot-clean:
	rm -rf $(BUILDROOT_DIR)/build*
	rm -rf $(BUILDROOT_DIR)/binaries
	rm -rf $(BUILDROOT_DIR)/project_build*
	rm -rf $(BUILDROOT_DIR)/toolchain_build*

buildroot-dirclean:
	rm -rf $(BUILDROOT_DIR)	
	
.PHONY: dummy all linux-menuconfig linux26 linux26-clean buildroot-clean buildroot-dirclean menuconfig $(BUILDROOT_DIR)/.config

