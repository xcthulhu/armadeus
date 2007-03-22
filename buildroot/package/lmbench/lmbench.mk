#############################################################
#
# LMBENCH, tools for performance analysis
#
#############################################################
LMBENCH_VERSION:=3
LMBENCH_SOURCE:=lmbench$(LMBENCH_VERSION).tar.gz
LMBENCH_SITE:=http://www.bitmover.com/lmbench/
LMBENCH_CAT:=$(ZCAT)
LMBENCH_DIR:=$(BUILD_DIR)/lmbench$(LMBENCH_VERSION)
LMBENCH_OS_NAME:=armv5tejl-linux-gnu

$(DL_DIR)/$(LMBENCH_SOURCE):
	$(WGET) -P $(DL_DIR) $(LMBENCH_SITE)/$(LMBENCH_SOURCE)

lmbench-source: $(DL_DIR)/$(LMBENCH_SOURCE)

$(LMBENCH_DIR)/.unpacked: $(DL_DIR)/$(LMBENCH_SOURCE)
	$(LMBENCH_CAT) $(DL_DIR)/$(LMBENCH_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LMBENCH_DIR)/.unpacked

$(LMBENCH_DIR)/.configured: $(LMBENCH_DIR)/.unpacked
	# Nothing to do yet
	mkdir -p $(LMBENCH_DIR)/SCCS
	touch $(LMBENCH_DIR)/SCCS/s.ChangeSet
	touch $(LMBENCH_DIR)/.configured

$(LMBENCH_DIR)/.compiled: $(LMBENCH_DIR)/.configured
	$(MAKE) -C $(LMBENCH_DIR) OS=$(LMBENCH_OS_NAME) CC=$(TARGET_CROSS)gcc AR=$(TARGET_CROSS)ar
	touch $(LMBENCH_DIR)/.compiled

$(STAGING_DIR)/usr/bin/lmbench/bin/$(LMBENCH_OS_NAME)/lmbench: $(LMBENCH_DIR)/.compiled
	mkdir -p $(STAGING_DIR)/usr/bin/lmbench/
	cp -r $(LMBENCH_DIR)/bin $(STAGING_DIR)/usr/bin/lmbench/
	cp -r $(LMBENCH_DIR)/scripts $(STAGING_DIR)/usr/bin/lmbench/
	cp -r $(LMBENCH_DIR)/results $(STAGING_DIR)/usr/bin/lmbench/

$(TARGET_DIR)/usr/bin/lmbench/bin/$(LMBENCH_OS_NAME)/lmbench: $(STAGING_DIR)/usr/bin/lmbench/bin/$(LMBENCH_OS_NAME)/lmbench
	mkdir -p $(TARGET_DIR)/usr/bin/lmbench/
	cp -r $(STAGING_DIR)/usr/bin/lmbench/* $(TARGET_DIR)/usr/bin/lmbench/

LMBENCH lmbench: uclibc $(TARGET_DIR)/usr/bin/lmbench/bin/$(LMBENCH_OS_NAME)/lmbench

lmbench-clean:
	#$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(LMBENCH_DIR) uninstall
	-$(MAKE) -C $(LMBENCH_DIR) clean
	rm -rf $(TARGET_DIR)/usr/bin/lmbench/

lmbench-dirclean:
	rm -rf $(STAGING_DIR)/usr/bin/lmbench/
	rm -rf $(TARGET_DIR)/usr/bin/lmbench/
	rm -rf $(LMBENCH_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_LMBENCH)),y)
TARGETS+=lmbench
endif
