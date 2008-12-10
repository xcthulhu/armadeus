#############################################################
#
# python
#
#############################################################
PYMYSQL_VERSION=1.2.2
PYMYSQL_SOURCE:=MySQL-python-$(PYMYSQL_VERSION).tar.gz
PYMYSQL_SITE:=http://python.org/ftp/python/$(PYMYSQL_VERSION)
PYMYSQL_DIR:=$(BUILD_DIR)/MySQL-python-$(PYMYSQL_VERSION)
PYMYSQL_CAT:=$(ZCAT)
PYMYSQL_BINARY:=_mysql.so
PYMYSQL_TARGET_BINARY:=usr/bin/python
PYMYSQL_DEPS:=mysql_client
TARGET_PYTHON_PACKAGE_DIR:=usr/lib/python2.4/site-packages
PYMYSQL_SITE_PACKAGE_DIR=$(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)
#HOST_PYTHON:=$(BUILD_DIR)/Python-2.4.5/hostpython
HOST_PYTHON=python2.4


$(DL_DIR)/$(PYMYSQL_SOURCE):
	$(WGET) -P $(DL_DIR) $(PYMYSQL_SITE)/$(PYMYSQL_SOURCE)

pymysql-source: $(DL_DIR)/$(PYMYSQL_SOURCE)

$(PYMYSQL_DIR)/.unpacked: $(DL_DIR)/$(PYMYSQL_SOURCE)
	$(PYMYSQL_CAT) $(DL_DIR)/$(PYMYSQL_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(PYMYSQL_DIR)/.patched: $(PYMYSQL_DIR)/.unpacked
	toolchain/patch-kernel.sh $(PYMYSQL_DIR) package/pymysql/patches/ \*.patch
	touch $@

$(PYMYSQL_DIR)/.configured: $(PYMYSQL_DIR)/.patched
	(cd $(PYMYSQL_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDSHARED="$(TARGET_CROSS)gcc -shared" \
	$(HOST_PYTHON) setup.py build );
	touch $@

$(PYMYSQL_DIR)/.installed: $(PYMYSQL_DIR)/.configured
	mkdir -p $(STAGING_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)
	(cd $(PYMYSQL_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDSHARED="$(TARGET_CROSS)gcc -shared" \
	$(HOST_PYTHON) setup.py install --prefix=$(STAGING_DIR)/usr );
	touch $@

$(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)/$(PYMYSQL_BINARY): $(PYMYSQL_DIR)/.installed
	mkdir -p $(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)
	(cd $(STAGING_DIR)/$(TARGET_PYTHON_PACKAGE_DIR); \
	cp -f _mysql.so $(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)/; \
	cp -f _mysql_exceptions.py* $(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)/; \
	cp -Rdpf MySQLdb $(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)/ );
	touch $@

pymysql: uclibc $(PYMYSQL_DEPS) $(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)/$(PYMYSQL_BINARY)

pymysql-clean:
	-$(MAKE) -C $(PYMYSQL_DIR) distclean
	rm -rf $(PYMYSQL_DIR)/.configured $(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)/MySQLdb
	rm -f $(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)/_mysql.so
	rm -f $(TARGET_DIR)/$(TARGET_PYTHON_PACKAGE_DIR)/_mysql_exceptions.py

pymysql-dirclean:
	rm -rf $(PYMYSQL_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_PYMYSQL)),y)
TARGETS+=pymysql
endif
