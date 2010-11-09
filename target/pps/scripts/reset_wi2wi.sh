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

source /usr/bin/gpio_helpers.sh

gpio_mode PE11 1
gpio_set_value PE11 0
gpio_set_value PE11 1
