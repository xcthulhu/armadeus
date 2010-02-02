#############################################################
#
# Armadeus Demos
#
#############################################################

ARMADEUS-DEMOS_VER:=1.0
ARMADEUS-DEMOS_SOURCE:=$(TOPDIR)/../target/demos
ARMADEUS-DEMOS_DIR:=$(BUILD_DIR)/armadeus-demos-$(ARMADEUS-DEMOS_VER)
ARMADEUS-DEMOS_TARGET_DIR:=$(TARGET_DIR)/usr/local/bin
ARMADEUS-DEMOS_TARGET_DATA_DIR:=$(TARGET_DIR)/usr/share


ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_ARMANOID),y)
ARMADEUS-DEMOS_SUBDIRS+=armanoid
ARMADEUS-DEMOS_DEPS=sdl
endif

ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_BACKLIGHT),y)
ARMADEUS-DEMOS_SUBDIRS+=backlight_control
ARMADEUS-DEMOS_DEPS=sdl
endif

ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_GPIO),y)
ARMADEUS-DEMOS_SUBDIRS+=gpio
endif

ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_GPS),y)
ARMADEUS-DEMOS_SUBDIRS+=gps
endif

ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_INPUT),y)
ARMADEUS-DEMOS_SUBDIRS+=input_test
ARMADEUS-DEMOS_DEPS=sdl
endif

ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_MUSIC_PLAYER),y)
ARMADEUS-DEMOS_SUBDIRS+=music_player
ARMADEUS-DEMOS_DEPS=sdl
endif

ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_OSCILLO),y)
ARMADEUS-DEMOS_SUBDIRS+=oscillo
endif

ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_SHOW_IMAGE),y)
ARMADEUS-DEMOS_SUBDIRS+=show_image
ARMADEUS-DEMOS_DEPS=sdl
endif

ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_TEST_LCD),y)
ARMADEUS-DEMOS_SUBDIRS+=test_lcd
ARMADEUS-DEMOS_DEPS=sdl
endif


ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_MUSIC_PLAYER),y)
ARMADEUS-DEMOS_DEPS+=sdl_ttf sdl_mixer
endif
ifeq ($(BR2_PACKAGE_ARMADEUS_DEMOS_SHOW_IMAGE),y)
ARMADEUS-DEMOS_DEPS+=sdl_image
endif


$(ARMADEUS-DEMOS_DIR)/.unpacked:
	mkdir -p $(ARMADEUS-DEMOS_DIR)/
	cd $(ARMADEUS-DEMOS_SOURCE); tar ch --exclude=.svn --exclude=.git . | tar x -C $(ARMADEUS-DEMOS_DIR)/
	touch $@

$(ARMADEUS-DEMOS_DIR)/.compiled: $(ARMADEUS-DEMOS_DIR)/.unpacked
	@for dir in $(ARMADEUS-DEMOS_SUBDIRS) ; do \
		echo -e "\n   >>> Compiling $$dir demo <<<" ; \
		dir=$(ARMADEUS-DEMOS_DIR)/$$dir ; \
                if [ -d "$$dir" ]; then \
                        (cd $$dir && $(MAKE) CC=$(TARGET_CC) SDL_DIR=$(SDL_DIR) STAGING_DIR=$(STAGING_DIR) ARMADEUS_BOARD_NAME=$(BR2_BOARD_NAME)) || exit 1 ; \
                fi \
        done
	touch $@

$(ARMADEUS-DEMOS_DIR)/.installed: $(ARMADEUS-DEMOS_DIR)/.compiled
	@for dir in $(ARMADEUS-DEMOS_SUBDIRS) ; do \
		echo -e "\n   >>> Installing $$dir demo <<<" ; \
		dir=$(ARMADEUS-DEMOS_DIR)/$$dir ; \
                if [ -d $$dir ] ; then \
                        (cd $$dir && $(MAKE) install INSTALL_DIR=$(ARMADEUS-DEMOS_TARGET_DIR) \
                               INSTALL_DATA_DIR=$(ARMADEUS-DEMOS_TARGET_DATA_DIR) STRIP=$(TARGET_STRIP)) || exit 1 ; \
                fi \
        done
	touch $@

armadeus-demos: $(ARMADEUS-DEMOS_DEPS) $(ARMADEUS-DEMOS_DIR)/.installed

armadeus-demos-clean:
	rm -rf $(ARMADEUS-DEMOS_TARGET_DIR)
	rm -f $(ARMADEUS-DEMOS_DIR)/.installed
	rm -f $(ARMADEUS-DEMOS_DIR)/.compiled
	@for dir in $(ARMADEUS-DEMOS_SUBDIRS) ; do \
		dir=$(ARMADEUS-DEMOS_DIR)/$$dir ; \
                if [ -d $$dir ] ; then \
                        (cd $$dir && $(MAKE) clean) || exit 1 ; \
                fi \
	done

armadeus-demos-dirclean:
	rm -rf $(ARMADEUS-DEMOS_DIR)

#############################################################
#
# Toplevel Makefile options
#
#############################################################
ifeq ($(strip $(BR2_PACKAGE_ARMADEUS_DEMOS)),y)
TARGETS+=armadeus-demos
endif
