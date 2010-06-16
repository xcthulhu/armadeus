#!/bin/bash
#
# Things to do before patching Buildroot
#

# Removes packages/files added through patches:
rm -rf buildroot/package/games/quake-pak/
rm -rf buildroot/package/games/sdlquake/
rm -rf buildroot/package/gpm/
rm -rf buildroot/package/pygame/
rm -f  buildroot/package/proftpd/proftpd-1.3.2-fix-kernel-header-capability-version.patch
rm -rf buildroot/package/uucp/
rm -f  buildroot/package/audio/libsndfile/libsndfile-1.0.*-srconly.patch
rm -rf buildroot/package/audio/flite/
rm -rf buildroot/package/libcurl/
rm -f  buildroot/package/audio/madplay/madplay-audio-alsa.patch
rm -f  buildroot/package/mplayer/mplayer-1.0rc1-add_pld_asm_inst_for_non_armv5_arch.patch
rm -f  buildroot/package/busybox/busybox-1.12.1-speed_table-B921600.patch
rm -rf buildroot/package/firmware/
rm -rf buildroot/package/wpa_supplicant/
rm -f  buildroot/package/strace/strace-arm-eabi-fix.patch
rm -f  buildroot/package/strace/strace-arm-no-cachectl.patch
rm -f  buildroot/toolchain/gcc/4.2.1/999-4.2.1-armeabi-aapcs-linux.patch
rm -f  buildroot/toolchain/gcc/4.2.4/999-4.2.4-armeabi-aapcs-linux.patch
rm -rf buildroot/package/games/wolf4sdl/
rm -rf buildroot/package/qtopia4/
