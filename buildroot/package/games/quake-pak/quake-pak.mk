#############################################################
#
# Quake shareware data files (PAK/WAD)
#
#############################################################

QUAKE_PAK_VERSION = 1.0.6
QUAKE_PAK_SOURCE = quakesw-$(QUAKE_PAK_VERSION).tar.gz
QUAKE_PAK_SITE = http://www.libsdl.org/projects/quake/data/
QUAKE_PAK_CAT = $(ZCAT)
QUAKE_PAK_TARGET_DIR = $(TARGET_DIR)/usr/share/games/quake/

$(DL_DIR)/$(QUAKE_PAK_SOURCE):
	 $(WGET) -P $(DL_DIR) $(QUAKE_PAK_SITE)/$(QUAKE_PAK_SOURCE)

quake-pak-source: $(DL_DIR)/$(QUAKE_PAK_SOURCE)

$(QUAKE_PAK_TARGET_DIR)/id1/pak0.pak: $(DL_DIR)/$(QUAKE_PAK_SOURCE)
	mkdir -p $(QUAKE_PAK_TARGET_DIR)
	$(QUAKE_PAK_CAT) $(DL_DIR)/$(QUAKE_PAK_SOURCE) | tar -C $(QUAKE_PAK_TARGET_DIR) $(TAR_OPTIONS) -

quake-pak: $(QUAKE_PAK_TARGET_DIR)/id1/pak0.pak

quake-pak-dirclean:
	rm -rf $(QUAKE_PAK_TARGET_DIR)/id1/

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_QUAKE_PAK)),y)
TARGETS+=quake-pak
endif
