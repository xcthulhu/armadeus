# Makefile for Armadeus
#
# Copyright (C) 2005-2011 by the Armadeus Team
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

include ./Makefile.in
ARMADEUS_TOPDIR:=$(shell pwd)
export ARMADEUS_TOPDIR

#--- User configurable stuff:
BUILDROOT_VERSION:=2010.11
BUILDROOT_SITE:=http://buildroot.uclibc.org/downloads
BUILDROOT_PATCH_DIR:=$(ARMADEUS_TOPDIR)/patches/buildroot/$(BUILDROOT_VERSION)
ARMADEUS_CONFIG_DIR:=$(BUILDROOT_DIR)/configs
#--- End of user conf (don't touch anything below unless you know what you're doing !! ;-) )

BUILDROOT_SOURCE:=buildroot-$(BUILDROOT_VERSION).tar.bz2
#BUILDROOT_DIR is defined in ./Makefile.in
PATCH_DIR = patches
BUILDROOT_FILE_PATH:=downloads
TAR_OPTIONS=--exclude=.svn --exclude=.git --exclude=.gitignore --strip-component=1 -xf

ARMADEUS_ENV_FILE:=$(ARMADEUS_TOPDIR)/armadeus_env.sh


ECHO_CONFIGURATION_NOT_DEFINED:= echo -en "\033[1m"; \
                echo "                                                   "; \
                echo " System not configured. Use make <board>_defconfig " >&2; \
                echo " armadeus valid configurations are:                " >&2; \
                echo "     "$(shell find $(ARMADEUS_CONFIG_DIR) -name "[ap][fp][fs]*_defconfig" | sed 's/.*\///');\
                echo "                                                   "; \
		echo -en "\033[0m";

ARMADEUS_BSP_VERSION=$(shell cat $(BUILDROOT_DIR)/target/device/armadeus/rootfs/target_skeleton/etc/armadeus-version)
ECHO_BUILD_IS_OK:= echo -en "\033[1m"; \
		echo ""; \
		echo "Your Armadeus BSP (version "$(ARMADEUS_BSP_VERSION)") was successfully built !"; \
		echo ""; \
		echo -en "\033[0m";

default: all

help:
	@echo 'First, choose the platform:'
	@echo '  apf9328_defconfig  - get default config for an APF9328 board'
	@echo '  apf27_defconfig    - get default config for an APF27 board'
	@echo '  pps_defconfig      - get default config for an APF27_PPS board'
	@echo '  apf51_defconfig    - get default config for an APF51 board'
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
	@echo ' Global:'
	@echo '  menuconfig           - interactive curses-based configurator'
	@echo '  oldconfig            - resolve any unresolved symbols in .config'
	@echo ' Specific:'
	@echo '  linux-menuconfig     - configure linux parameters'
	@echo '  uclibc-menuconfig    - configure uclibc library parameters'
	@echo '  busybox-menuconfig   - configure busybox shell parameters'
	@echo
	@echo 'Miscellaneous:'
	@echo '  source                 - download all sources needed for offline-build'
	@echo '  source-check           - check all packages for valid download URLs'
	@echo
	@echo 'See http://www.armadeus.org for further Armadeus details'
	@echo 'See ./buildroot/docs/README and ./buildroot/docs/buildroot.html'
	@echo '  for further Buildroot details'
	@echo


# configuration
# ---------------------------------------------------------------------------

$(BUILDROOT_FILE_PATH)/$(BUILDROOT_SOURCE):
	mkdir -p $(BUILDROOT_FILE_PATH)
	wget --passive-ftp -P $(BUILDROOT_FILE_PATH)  $(BUILDROOT_SITE)/$(BUILDROOT_SOURCE)

buildroot-sources: $(BUILDROOT_FILE_PATH)/$(BUILDROOT_SOURCE)

$(BUILDROOT_DIR)/.unpacked: $(BUILDROOT_FILE_PATH)/$(BUILDROOT_SOURCE) $(BUILDROOT_PATCH_DIR)/*.patch
	$(BUILDROOT_PATCH_DIR)/cleanup_buildroot.sh
	bzcat $(BUILDROOT_FILE_PATH)/$(BUILDROOT_SOURCE) | \
	tar -C $(BUILDROOT_DIR) $(TAR_OPTIONS) -
	touch $@

buildroot-unpacked: $(BUILDROOT_DIR)/.unpacked

$(BUILDROOT_DIR)/.patched: $(BUILDROOT_DIR)/.unpacked
	$(BUILDROOT_DIR)/toolchain/patch-kernel.sh $(BUILDROOT_DIR) $(BUILDROOT_PATCH_DIR) \*.patch
	touch $@

buildroot-patched: $(BUILDROOT_DIR)/.patched

$(BUILDROOT_DIR)/.configured: $(BUILDROOT_DIR)/.patched
	@if [ ! -f $@ ]; then \
		$(ECHO_CONFIGURATION_NOT_DEFINED) \
		exit 1; \
	fi

# To be called only one time if one wants to make an automatic build
buildauto: $(BUILDROOT_DIR)/.patched
	# ! Be sure that /local/downloads exists if you want to use automated build !
	sed -i -e 's/BR2_DL_DIR/BR2_DL_DIR=\"\/local\/downloads\" #/g' $(BUILDROOT_DIR)/.defconfig ;
	echo "ey" | $(MAKE) -C $(BUILDROOT_DIR) menuconfig
	$(MAKE) -C $(BUILDROOT_DIR)

linux-menuconfig:
	@if [ -e "$(ARMADEUS_LINUX_DIR)/.stamp_patched" ] ; then \
		$(MAKE) -C $(BUILDROOT_DIR) linux26-menuconfig ; \
	fi;

linux26: $(BUILDROOT_DIR)/.configured
	@-touch $(ARMADEUS_LINUX_DIR)/.config 
	@$(MAKE) -C $(BUILDROOT_DIR) linux26

linux26-clean: $(BUILDROOT_DIR)/.configured
ifneq (,$(filter $(BUILDROOT_VERSION),20081103 2010.05))
	@$(MAKE) -C $(BUILDROOT_DIR) linux26clean
else
	@$(MAKE) -C $(BUILDROOT_DIR) linux26-clean
endif

%_defconfig: $(BUILDROOT_DIR)/.patched
	@if [ -e "$(ARMADEUS_CONFIG_DIR)/$@" ] || [ -e "$(ARMADEUS_CONFIG_DIR)/$(patsubst %_defconfig,%,$@)/$@" ]; then \
		rm -f $(BUILDROOT_DIR)/.config ; \
		$(MAKE) -C $(BUILDROOT_DIR) $@ ; \
		$(MAKE) -C $(BUILDROOT_DIR) menuconfig ; \
		touch $(BUILDROOT_DIR)/.configured ; \
	else \
		echo -e "\033[1m\nThis configuration does not exist !!\n\033[0m" ; \
	fi;

%_autoconf: $(BUILDROOT_DIR)/.patched
	@if [ -e "$(BUILDROOT_DIR)/target/device/armadeus/$(patsubst %_autoconf,%,$@)/$(patsubst %_autoconf,%,$@)_defconfig" ]; then \
		rm -f $(BUILDROOT_DIR)/.config ; \
		$(MAKE) -C $(BUILDROOT_DIR) $(patsubst %_autoconf,%,$@)_defconfig ; \
		touch $(BUILDROOT_DIR)/.configured ; \
	else \
		echo -e "\033[1m\nThis configuration does not exist !!\n\033[0m" ; \
	fi;

all: $(BUILDROOT_DIR)/.configured
	@echo "If your /bin/sh link doesn't point to /bin/bash, please correct that."
	@ls -al /bin/sh | grep bash 
	@$(MAKE) -C $(BUILDROOT_DIR) $@
	@$(ECHO_BUILD_IS_OK)

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

# Generate shell's most useful variables:
shell_env:
	@echo ARMADEUS_BUILDROOT_DIR=$(BUILDROOT_DIR) > $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_BUILDROOT_PATCH_DIR=$(BUILDROOT_PATCH_DIR) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_LINUX_DIR=$(ARMADEUS_LINUX_DIR) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_LINUX_PATCH_DIR=$(ARMADEUS_LINUX_PATCH_DIR) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_U_BOOT_DIR=$(ARMADEUS_U_BOOT_DIR) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_U_BOOT_VERSION=$(ARMADEUS_U_BOOT_VERSION) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_ROOTFS_DIR=$(ARMADEUS_ROOTFS_DIR) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_BINARIES=$(ARMADEUS_BINARIES) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_ROOTFS_TAR=$(ARMADEUS_ROOTFS_TAR) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_STAGING_DIR=$(ARMADEUS_STAGING_DIR) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_TOOLCHAIN_PATH=$(ARMADEUS_TOOLCHAIN_PATH) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_XENOMAI_DIR=$(ARMADEUS_XENOMAI_DIR) >> $(ARMADEUS_ENV_FILE)
	@echo ARMADEUS_QT_DIR=$(ARMADEUS_QT_DIR) >> $(ARMADEUS_ENV_FILE)


PHONY_TARGETS+=dummy all linux-menuconfig linux26 linux26-clean buildroot-clean buildroot-dirclean
PHONY_TARGETS+=menuconfig $(BUILDROOT_DIR)/.config shell_env buildroot-sources buildroot-unpacked
PHONY_TARGETS+=buildroot-patched 
.PHONY: $(PHONY_TARGETS)

