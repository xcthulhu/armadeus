#############################################################
#
# as_devices library
#
#############################################################

PYTHON:=python2.4

AS_DEVICES_VER:=0.14
AS_DEVICES_SOURCE:=$(TOPDIR)/../target/packages/as_devices
AS_DEVICES_DIR:=$(BUILD_DIR)/as_devices-$(AS_DEVICES_VER)
AS_DEVICES_CAT:=zcat

AS_DEVICES_BINARY:=libas_devices.so
AS_DEVICES_TARGET_DIR:=usr/lib
AS_DEVICES_TARGET_BINARY:=$(AS_DEVICES_TARGET_DIR)/$(AS_DEVICES_BINARY)

# C++ wrapper variables
AS_DEVICES_CPP_BINARY:=libas_devices_cpp.so
AS_DEVICES_CPP_TARGET_DIR:=usr/lib
AS_DEVICES_CPP_TARGET_BINARY:=$(AS_DEVICES_CPP_TARGET_DIR)/$(AS_DEVICES_CPP_BINARY)

# Python wrapper variables
AS_DEVICES_PYTHON_BINARY:=AsGpio_wrap.so
AS_DEVICES_PYTHON_MODULE:=AsDevices
AS_DEVICES_PYTHON_TARGET_DIR:=usr/lib/${PYTHON}/
AS_DEVICES_PYTHON_TARGET_BINARY:=$(AS_DEVICES_PYTHON_TARGET_DIR)/$(AS_DEVICES_PYTHON_MODULE)

AS_DEVICES_BINARIES=$(TARGET_DIR)/$(AS_DEVICES_TARGET_BINARY)

ifeq ($(strip $(BR2_PACKAGE_AS_DEVICES_CPP)),y)
AS_DEVICES_BINARIES+= $(TARGET_DIR)/$(AS_DEVICES_CPP_TARGET_BINARY)
endif
ifeq ($(strip $(BR2_PACKAGE_AS_DEVICES_PYTHON)),y)
AS_DEVICES_BINARIES+= $(TARGET_DIR)/$(AS_DEVICES_PYTHON_TARGET_BINARY)
endif

# Copy directory in build_arm…
$(AS_DEVICES_DIR)/Makefile:
	mkdir -p $(AS_DEVICES_DIR)/
	cp -rf $(AS_DEVICES_SOURCE)/* $(AS_DEVICES_DIR)

# Compile as_devices library using the makefile
$(AS_DEVICES_DIR)/c/$(AS_DEVICES_BINARY): $(AS_DEVICES_DIR)/Makefile
	#XXX: BR2_TARGET_UBOOT_BOARDNAME  is wrong, to be fixed
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(AS_DEVICES_DIR)/c ARMADEUS_BOARD_NAME=$(BR2_TARGET_UBOOT_BOARDNAME)

# Install library on staging dir
$(STAGING_DIR)/$(AS_DEVICES_TARGET_BINARY): $(AS_DEVICES_DIR)/c/$(AS_DEVICES_BINARY)
	$(MAKE) INSTALL_DIR=$(STAGING_DIR) -C $(AS_DEVICES_DIR) install

# install library on target
$(TARGET_DIR)/$(AS_DEVICES_TARGET_BINARY): $(STAGING_DIR)/$(AS_DEVICES_TARGET_BINARY)
	$(MAKE) INSTALL_DIR=$(TARGET_DIR) STRIP="$(STRIPCMD) $(STRIP_STRIP_UNNEEDED)" -C $(AS_DEVICES_DIR) install-exe

ifeq ($(strip $(BR2_PACKAGE_AS_DEVICES_CPP)),y)
$(TARGET_DIR)/$(AS_DEVICES_CPP_TARGET_BINARY): $(AS_DEVICES_DIR)/cpp/$(AS_DEVICES_CPP_BINARY)
	install -D $< $@
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(AS_DEVICES_CPP_TARGET_BINARY)

$(AS_DEVICES_DIR)/cpp/$(AS_DEVICES_CPP_BINARY): $(AS_DEVICES_DIR)/Makefile
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CXX) -C $(AS_DEVICES_DIR)/cpp
endif

ifeq ($(strip $(BR2_PACKAGE_AS_DEVICES_PYTHON)),y)

$(TARGET_DIR)/$(AS_DEVICES_PYTHON_TARGET_BINARY): $(AS_DEVICES_DIR)/python/AsDevices/wrappers/$(AS_DEVICES_PYTHON_BINARY)
	cp -rf $(AS_DEVICES_DIR)/python/AsDevices $(TARGET_DIR)/$(AS_DEVICES_PYTHON_TARGET_DIR)
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(AS_DEVICES_PYTHON_TARGET_BINARY)/wrappers/*.so

$(AS_DEVICES_DIR)/python/AsDevices/wrappers/$(AS_DEVICES_PYTHON_BINARY): $(AS_DEVICES_DIR)/python/Makefile
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CXX) -C $(AS_DEVICES_DIR)/python

endif

as_devices: $(AS_DEVICES_BINARIES)

as_devices-clean:
	rm -f $(TARGET_DIR)/$(AS_DEVICES_TARGET_BINARY)
	-$(MAKE) -C $(AS_DEVICES_DIR)/c clean

as_devices-dirclean:
	rm -rf $(AS_DEVICES_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_AS_DEVICES)),y)
TARGETS+=as_devices
endif

