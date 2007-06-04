#############################################################
#
# Lua scripting language
#
#############################################################
LUA_VER=-5.1.2
LUA_SOURCE=lua$(LUA_VER).tar.gz
LUA_SITE=http://www.lua.org/ftp/
LUA_DIR:=$(BUILD_DIR)/lua$(LUA_VER)
LUA_BINARY:=lua
LUA_TARGET_BINARY:=usr/bin/lua

$(DL_DIR)/$(LUA_SOURCE):
	 $(WGET) -P $(DL_DIR) $(LUA_SITE)/$(LUA_SOURCE)

lua-source: $(DL_DIR)/$(LUA_SOURCE)

$(LUA_DIR)/.unpacked: $(DL_DIR)/$(LUA_SOURCE)
	rm -rf $(LUA_DIR)
	zcat $(DL_DIR)/$(LUA_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(LUA_DIR)/.configured: $(LUA_DIR)/.unpacked
	touch $@

$(LUA_DIR)/$(LUA_BINARY): $(LUA_DIR)/.configured
	$(MAKE) linux -C $(LUA_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(TARGET_CROSS) CFLAGS=-I$(STAGING_DIR)/usr/include MYLDFLAGS=-Wl,-E,-L$(TARGET_DIR)/lib/ CC=$(TARGET_CROSS)gcc

$(TARGET_DIR)/$(LUA_TARGET_BINARY): $(LUA_DIR)/$(LUA_BINARY)
	cp -f $(BUILD_DIR)/lua$(LUA_VER)/src/lua $@

lua: uclibc readline-target ncurses $(TARGET_DIR)/$(LUA_TARGET_BINARY)

lua-clean:
	-$(MAKE) -C $(LUA_DIR) clean

lua-dirclean:
	rm -rf $(LUA_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_LUA)),y)
TARGETS+=lua
endif

