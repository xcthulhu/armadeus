#!/bin/bash
#
# Things to do before patching Buildroot
#

# Removes packages added through patches:
rm -rf buildroot/package/games/quake-pak/
rm -rf buildroot/package/games/sdlquake/
