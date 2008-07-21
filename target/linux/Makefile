# To get from Buildroot:
LINUX_VERSION=2.6.23.1
TARBALL_DIR=/local/downloads
BUILD_DIR=../../buildroot/build_arm

LINUX_NAME=linux-$(LINUX_VERSION)
LINUX_ARCHIVE=$(LINUX_NAME).tar.bz2
# Working Linux:
LINUX_DIR=$(BUILD_DIR)/$(LINUX_NAME)
# Original Linux:
LINUX_DIR_TO_DIFF=$(LINUX_DIR).original

PATCH_DIR=../../buildroot/target/device/armadeus/linux/kernel-patches/$(LINUX_VERSION)
TMP_DIR=./tmp
PATCH_CMD=./generate_patch.sh
CLEAN_PATCH_CMD=sed --in-place "s,$(LINUX_DIR),$(LINUX_NAME)," $@ && sed --in-place=.bak "s,\.\./linux/,$(LINUX_NAME).mod/," $@
START_CMD=echo "*** Generating patch: $@"

### ALSA Patches:

$(TMP_DIR)/090-alsa-create-imx-alsa_h.diff: include/asm-arm/arch-imx/imx-alsa.h
	@$(START_CMD)
	$(PATCH_CMD) $(LINUX_DIR_TO_DIFF)/$< ../linux/$< $@
	$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TMP_DIR)/090-alsa-create-imx-alsa_h.diff

$(TMP_DIR)/091-alsa-add-imx-ssi-tsc2102-driver.diff: sound/arm/imx-alsa.c sound/arm/imx-alsa-dma.h sound/arm/imx-alsa-tsc2102.c sound/arm/imx-alsa-tsc2102.h sound/arm/imx-alsa-tsc2102-mixer.c
	@$(START_CMD)
	for patch in $^; do $(PATCH_CMD) --append $(LINUX_DIR_TO_DIFF)/$$patch ../linux/$$patch $@; done
	$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TMP_DIR)/091-alsa-add-imx-ssi-tsc2102-driver.diff


### APF9328 Patches:
$(TMP_DIR)/021-linux-2.6.23.1-apf9328.diff: arch/arm/mach-imx/apf9328.c arch/arm/mach-imx/apf9328_lcd_config.h include/asm-arm/arch-imx/apf9328.h
	@$(START_CMD)
	for patch in $^; do $(PATCH_CMD) --append $(LINUX_DIR_TO_DIFF)/$$patch ../linux/$$patch $@; done
	$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TMP_DIR)/021-linux-2.6.23.1-apf9328.diff


### Other Patches:


### Patches generation:

.PHONY: clean all help

$(LINUX_DIR_TO_DIFF)/.unpacked:
	@echo "***Installing original kernel in $(BUILD_DIR)"
	mv $(LINUX_DIR) $(LINUX_DIR).modified
	tar jxf $(TARBALL_DIR)/$(LINUX_ARCHIVE) -C $(BUILD_DIR)/
	mv $(LINUX_DIR) $(LINUX_DIR_TO_DIFF)
	mv $(LINUX_DIR).modified $(LINUX_DIR)
	touch $@
 
patch: $(LINUX_DIR_TO_DIFF)/.unpacked $(PATCHES)
	mkdir -p ./tmp

patch_install: patch
	echo $(PATCHES)
	cp -a $(PATCHES) $(PATCH_DIR)/

help:
	@echo "make [patch|patch_install|clean|help]"
	@echo "    help: this help"
	@echo "    patch: generate all patches in $(TMP_DIR) directory"
	@echo "    patch_install: install patch in $(PATCH_DIR)"

all: patch

clean:
	rm -f $(TMP_DIR)/*
 
