# Makefile for armadeus
#
# Copyright (C) 2005-2006 by Eric Jarrige <jorasse@sourceforge.net>
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
BUILDROOT_FILE_PATH=downloads/buildroot.tar.bz2

PATCH_DIR=patches
PATCH_CYGWIN_DIR=$(PATCH_DIR)/cygwin
TAR_OPTIONS=--exclude=.svn -xvf 

all: buildroot

help:
	@echo 'Cleaning:'
	@echo '  buildroot-clean	- delete all non-source files in buildroot (including .config)'
	@echo
	@echo 'Build:'
	@echo '  all			- everything needed (default)'
	@echo '  <Package>		- a single package (ex: u-boot linux or buildroot'
	@echo
	@echo 'Configuration:'
	@echo '  menuconfig		- interactive curses-based configurator'
	@echo
	@echo 'Development:'
	@echo '  buildroot-patch	- generate patch file for buildroot'
	@echo '  sourceball		- create a distribution tarball'
	@echo


# configuration
# ---------------------------------------------------------------------------

$(BUILDROOT_DIR)/.unpacked: $(BUILDROOT_FILE_PATH) $(PATCH_DIR)/*.diff
	bzcat $(BUILDROOT_FILE_PATH) | tar -C $(BUILDROOT_DIR)/.. $(TAR_OPTIONS) -
	$(BUILDROOT_DIR)/toolchain/patch-kernel.sh $(BUILDROOT_DIR) $(PATCH_DIR) *.diff 
	if uname | grep -i cygwin >/dev/null 2>&1 ; then  \
		$(BUILDROOT_DIR)/toolchain/patch-kernel.sh $(BUILDROOT_DIR) $(PATCH_CYGWIN_DIR) *.diff ; \
	fi;
	touch $(BUILDROOT_DIR)/.unpacked

buildroot: $(BUILDROOT_DIR)/.unpacked
	@$(MAKE) -C $(BUILDROOT_DIR)

menuconfig: $(BUILDROOT_DIR)/.unpacked
	@$(MAKE) -C $(BUILDROOT_DIR) menuconfig

.DEFAULT: $(BUILDROOT_DIR)/.unpacked
	@$(MAKE) -C $(BUILDROOT_DIR) $@

buildroot-clean:
	@$(MAKE) -C $(BUILDROOT_DIR)  clean
	rm -rf $(BUILDROOT_DIR)/build*
	rm -rf $(BUILDROOT_DIR)/toolchain_build*
	rm -rf $(BUILDROOT_DIR)/dl
	rm -rf $(BUILDROOT_DIR)/u-boot* $(BUILDROOT_DIR)/rootfs* $(BUILDROOT_DIR)/linux*

buildroot-patch: buildroot-clean
	rm -rf buildroot
	svn co svn://ericjarrige.homelinux.org/armadeus/trunk/software/buildroot buildroot
	bzcat $(BUILDROOT_FILE_PATH) | tar -C . $(TAR_OPTIONS) -
	buildroot/toolchain/patch-kernel.sh buildroot $(PATCH_DIR)
	touch buildroot/.unpacked
	diff -purN -x .svn -x '*~' buildroot $(BUILDROOT_DIR) > newBuildroot.diff

sourceball: buildroot-clean
	cd ..;\
	cp -a armadeus armadeus-0.1; \
	tar --exclude=.svn --exclude=*~ -cvjf armadeus-0.1.tar.bz2 armadeus-0.1;


.PHONY: dummy buildroot


