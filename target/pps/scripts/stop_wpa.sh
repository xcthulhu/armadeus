#
#        This software has been developed by ARMADEUS SYSTEMS
#
# Customer Name: Armadeus
#
# Project Name: PPS
#
# Copyright (C) 2010 : 
# 
# This program is the property of the armadeus systems company or of the 
# final customer as soon as terms & conditions stipulated by the contract
# between the two parties are full-filled .
#
# 	Written by: Jérémie Scheer,,,
#

ifconfig eth1 down 2>/dev/null

#kill WPA Supplicant
killall wpa_supplicant

#Probe needed modules
modprobe -r libertas_sdio 2>/dev/null
modprobe -r mxcmmc
