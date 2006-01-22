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

BUILDROOT_DIR = software/buildroot

BUILDROOT_NAME=buildroot
BUILDROOT_FILE_PATH=software/downloads/buildroot.tar.bz2

PATCH_DIR=software/patches

TAR_OPTIONS=-xvf

all: buildroot

# configuration
# ---------------------------------------------------------------------------

$(BUILDROOT_DIR)/.unpacked:
	bzcat $(BUILDROOT_FILE_PATH) | tar -C $(BUILDROOT_DIR)/.. $(TAR_OPTIONS) -
	$(BUILDROOT_DIR)/toolchain/patch-kernel.sh $(BUILDROOT_DIR) $(PATCH_DIR) 
	touch $(BUILDROOT_DIR)/.unpacked

buildroot: $(BUILDROOT_DIR)/.unpacked
	@$(MAKE) -C $(BUILDROOT_DIR)

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

.PHONY: dummy buildroot


