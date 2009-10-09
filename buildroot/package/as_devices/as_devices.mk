#############################################################
#
# as_devices library
#
#############################################################

PYTHON:=python2.4

AS_DEVICES_VER:=1.0
AS_DEVICES_SOURCE:=$(BUILD_DIR)/../../target/packages/as_devices
AS_DEVICES_DIR:=$(BUILD_DIR)/as_devices-$(AS_DEVICES_VER)
AS_DEVICES_CAT:=zcat
AS_DEVICES_BINARY:=libas_devices.so
AS_DEVICES_TARGET_DIR:=usr/lib
AS_DEVICES_PYTHON_TARGET_DIR:=usr/lib/${PYTHON}/
AS_DEVICES_TARGET_BINARY:=$(AS_DEVICES_TARGET_DIR)/$(AS_DEVICES_BINARY)

# Copy directory in build_arm…
$(AS_DEVICES_DIR)/Makefile:
	mkdir -p $(AS_DEVICES_DIR)/
	cp -rf $(AS_DEVICES_SOURCE)/* $(AS_DEVICES_DIR)

# Compile as_devices library using the makefile
$(AS_DEVICES_DIR)/c/$(AS_DEVICES_BINARY): $(AS_DEVICES_DIR)/Makefile
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) -C $(AS_DEVICES_DIR)/c
ifeq ($(strip $(BR2_PACKAGE_AS_DEVICES_CPP)),y)
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CXX) -C $(AS_DEVICES_DIR)/cpp
endif
ifeq ($(strip $(BR2_PACKAGE_AS_DEVICES_PYTHON)),y)
	$(TARGET_CONFIGURE_OPTS) $(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CXX) -C $(AS_DEVICES_DIR)/python
endif


$(TARGET_DIR)/$(AS_DEVICES_TARGET_BINARY): $(AS_DEVICES_DIR)/c/$(AS_DEVICES_BINARY)
	install -D $< $@
	$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$(AS_DEVICES_TARGET_BINARY)

as_devices: uclibc $(TARGET_DIR)/$(AS_DEVICES_TARGET_BINARY)

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

