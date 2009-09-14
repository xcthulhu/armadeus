#############################################################
#
# Quake 1 (SDL_Port)
#
#############################################################

SDLQUAKE_VERSION = 1.0.9
SDLQUAKE_SOURCE = sdlquake-$(SDLQUAKE_VERSION).tar.gz
SDLQUAKE_SITE = http://www.libsdl.org/projects/quake/src/
#SDLQUAKE_AUTORECONF = NO
SDLQUAKE_INSTALL_STAGING = NO
SDLQUAKE_INSTALL_TARGET = YES

SDLQUAKE_CONF_OPT = --target=$(GNU_TARGET_NAME) --host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) --prefix=/usr --sysconfdir=/etc

SDLQUAKE_DEPENDENCIES = uclibc sdl sdl_mixer SDL_net

$(eval $(call AUTOTARGETS,package/games,sdlquake))

NAMEVER = $($(PKG)_NAME)-$($(PKG)_VERSION)
$(SDLQUAKE_TARGET_PATCH):
	$(call MESSAGE,"Patching $($(PKG)_DIR_PREFIX)/$($(PKG)_NAME)")
	toolchain/patch-kernel.sh $($(PKG)_DIR) $($(PKG)_DIR_PREFIX)/$($(PKG)_NAME) $(NAMEVER)\*.patch $(NAMEVER)\*.patch.$(ARCH) || exit 1;
	(cd $(SDLQUAKE_DIR); \
	touch INSTALL NEWS README AUTHORS ChangeLog; \
	echo "AC_PROG_CC" >> configure.in; \
	aclocal; \
	autoconf; \
	automake --add-missing;)
	touch $@
