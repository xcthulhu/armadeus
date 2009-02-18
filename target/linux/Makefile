#
# Get important variables from Buildroot:
#
BASE_DIR=../
BUILDROOT_DIR=../../buildroot
include $(BUILDROOT_DIR)/.config

LINUX_VERSION:=$(shell echo $(BR2_LINUX26_VERSION))
TARBALL_DIR:=$(shell echo $(BR2_DL_DIR))
BOARD_NAME:=$(shell echo $(BR2_BOARD_NAME))
ARCH:=$(shell echo $(BR2_GCC_TARGET_ARCH))
BUILD_DIR=$(BUILDROOT_DIR)/project_build_$(ARCH)/$(BOARD_NAME)


LINUX_NAME:=linux-$(LINUX_VERSION)
LINUX_ARCHIVE:=$(LINUX_NAME).tar.bz2
# Working Linux:
LINUX_DIR:=$(BUILD_DIR)/$(LINUX_NAME)
# Original Linux:
LINUX_DIR_TO_DIFF:=$(LINUX_DIR).original

PATCH_DIR:=../../buildroot/target/device/armadeus/linux/kernel-patches/$(LINUX_VERSION)
TMP_DIR=./tmp
PATCH_CMD=./generate_patch.sh
CLEAN_PATCH_CMD=sed --in-place "s,$(LINUX_DIR),$(LINUX_NAME)," $@ && sed --in-place=.bak "s,\.\./linux/,$(LINUX_NAME).mod/," $@
START_CMD=echo "*** Generating patch: $@"; mkdir -p ./tmp

GENERATE_PATCH_FROM_SINGLE_DEPEND=$(PATCH_CMD) $(LINUX_DIR_TO_DIFF)/$< ../linux/$< $@; echo "    adding $<"
GENERATE_PATCH_FROM_MULTIPLE_DEPENDS=rm -f $@; for file in $^; do $(PATCH_CMD) --append $(LINUX_DIR_TO_DIFF)/$$file ../linux/$$file $@; echo "    adding $$file"; done


### ALSA/TSC2102 Patches: ---

DEP1=arch/arm/mach-imx/include/mach/imx-alsa.h
TARGET1:=$(TMP_DIR)/090-armadeus-alsa-create_imx-alsa_h.patch
$(TARGET1): $(DEP1)
	@$(START_CMD)
	@$(GENERATE_PATCH_FROM_SINGLE_DEPEND)
	@$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TARGET1)
FILES+=$(DEP1)

DEP2=sound/arm/imx-alsa.c sound/arm/imx-alsa-dma.h sound/arm/imx-alsa-tsc2102.c sound/arm/imx-alsa-tsc2102.h sound/arm/imx-alsa-tsc2102-mixer.c
TARGET2:=$(TMP_DIR)/091-armadeus-alsa-add_imx_ssi_tsc2102_driver.patch
$(TARGET2): $(DEP2)
	@$(START_CMD)
	@$(GENERATE_PATCH_FROM_MULTIPLE_DEPENDS)
	@$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TARGET2)
FILES+=$(DEP2)

DEP3=drivers/input/touchscreen/tsc2102_ts.c
TARGET3:=$(TMP_DIR)/074-armadeus-tsc2102-touchscreen.patch
$(TARGET3): $(DEP3)
	@$(START_CMD)
	@$(GENERATE_PATCH_FROM_SINGLE_DEPEND)
	@$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TARGET3)
FILES+=$(DEP3)

DEP4=include/linux/spi/tsc2102.h drivers/spi/tsc2102.c
TARGET4:=$(TMP_DIR)/073-armadeus-tsc2102.patch
$(TARGET4): $(DEP4)
	@$(START_CMD)
	@$(GENERATE_PATCH_FROM_MULTIPLE_DEPENDS)
	@$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TARGET4)
FILES+=$(DEP4)

DEP_SSI=sound/arm/imx-ssi.c sound/arm/imx-ssi.h arch/arm/mach-imx/include/mach/imx_ssi.h
TARGET_SSI:=$(TMP_DIR)/095-armadeus-imx-ssi-add_driver.patch
$(TARGET_SSI): $(DEP_SSI)
	@$(START_CMD)
	@$(GENERATE_PATCH_FROM_MULTIPLE_DEPENDS)
	@$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TARGET_SSI)
FILES+=$(DEP_SSI)
                                 

### APF9328 Patches: ---

DEP5=arch/arm/mach-imx/apf9328.c arch/arm/mach-imx/apf9328_lcd_config.h arch/arm/mach-imx/include/mach/apf9328.h
TARGET5:=$(TMP_DIR)/021-armadeus-apf9328.patch
$(TARGET5): $(DEP5)
	@$(START_CMD)
	@$(GENERATE_PATCH_FROM_MULTIPLE_DEPENDS)
	@$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TARGET5)
FILES+=$(DEP5)

DEP_DMA=arch/arm/mach-imx/include/mach/imx-dma.h arch/arm/mach-imx/dma.c
TARGET_DMA:=$(TMP_DIR)/097-armadeus-imx-dma-improvements.patch
$(TARGET_DMA): $(DEP_DMA)
	@$(START_CMD)
	@$(GENERATE_PATCH_FROM_MULTIPLE_DEPENDS)
	@$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TARGET_DMA)
FILES+=$(DEP_DMA)


### APF27 Patches: ---

DEP21=arch/arm/mach-mx2/apf27.c
TARGET21:=$(TMP_DIR)/281-apf27-armadeus-add_apf27.c.patch
$(TARGET21): $(DEP21)
	@$(START_CMD)
	@$(GENERATE_PATCH_FROM_SINGLE_DEPEND)
	@$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TARGET21)
FILES+=$(DEP21)


### Other Patches: ---

# Special case: drivers/video/imxfb.h doesn't exist
FILES_O1=drivers/video/imxfb.c drivers/video/imxfb.h
DEP_O1=drivers/video/imxfb.c
TARGET_O1:=$(TMP_DIR)/013-armadeus-imxfb-improvements.patch
$(TARGET_O1): $(DEP_O1)
	@$(START_CMD)
	@$(GENERATE_PATCH_FROM_MULTIPLE_DEPENDS)
	@$(CLEAN_PATCH_CMD)
	@echo
PATCHES+=$(TARGET_O1)
FILES+=$(FILES_O1)



### Patches generation: ---

.PHONY: clean all help show import patch patch_install

$(LINUX_DIR_TO_DIFF)/.unpacked:
	@echo "***Installing original kernel in $(BUILD_DIR)"
	mv $(LINUX_DIR) $(LINUX_DIR).modified
	make linux26-unpacked -C $(BUILDROOT_DIR)
	#tar jxf $(TARBALL_DIR)/$(LINUX_ARCHIVE) -C $(BUILD_DIR)/
	mv $(LINUX_DIR) $(LINUX_DIR_TO_DIFF)
	mv $(LINUX_DIR).modified $(LINUX_DIR)
	@touch $@
 
patch: $(LINUX_DIR_TO_DIFF)/.unpacked $(PATCHES)

patch_install: patch
	@cp -a $(PATCHES) $(PATCH_DIR)/
	@echo "Patches installed, you can check the diff in $(PATCH_DIR)/"
	@echo "    please don't commit <<timestamp only>> changes !"

show:
	@echo "Linux kernel files ($(LINUX_VERSION)) from which I can generate patch:"
	@for file in $(FILES); do echo "    $$file"; done
	@echo -e "\nNames of the patches I will generate:"
	@for patch in $(PATCHES); do echo "    $$patch"; done

import:
	@for file in $(FILES); do if [ -f $(LINUX_DIR)/$$file ]; then cp $(LINUX_DIR)/$$file $$file; fi; done
	@echo "Import done, check your modifications with  svn status -q"

help:
	@echo "make [patch|patch_install|clean|help]"
	@echo "    help ............ this help"
	@echo "    show ............ list handled kernel files and generated patches name"
	@echo "    import .......... import files we handle here from current kernel"
	@echo "    patch ........... generate all patches in $(TMP_DIR) directory"
	@echo "    patch_install ... install generated patches in $(PATCH_DIR)"

all: patch

clean:
	$(MAKE) clean -C modules
#	rm -f $(TMP_DIR)/*   humm... not a good idea to clean patches otherwise will be regenerated
#	                     with new timestamp even if no changes where made to the files...

