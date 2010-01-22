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
