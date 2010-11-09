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

if [ -f /etc/wpa_supplicant/networkConfig ]; then

source /etc/wpa_supplicant/networkConfig

if [ "$SSID" != "" -a "$PSK" != "" ]; then

#create WPA configuration
mkdir -p /etc/wpa_supplicant/
echo "ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=wheel
network={
ssid=\"$SSID\"
scan_ssid=1
proto=WPA
key_mgmt=WPA-PSK
pairwise=TKIP
psk=\"$PSK\"
}" > /etc/wpa_supplicant/wpa_supplicant.conf

#Hardware reset of wi2wi chip
sh reset_wi2wi.sh

#Load mmc and libertas modules
modprobe mxcmmc
sleep 1
modprobe libertas_sdio

#   shutdown other ports
ifconfig usb0   down 2>/dev/null
ifconfig eth0   down 2>/dev/null
ifconfig usb1   down 2>/dev/null

#Execute Wpa Supplicant
ifconfig eth1 up
wpa_supplicant -ieth1 -c /etc/wpa_supplicant/wpa_supplicant.conf -Dwext -B dhclient

ifconfig eth1 192.168.0.251
sleep 1
echo -e "\nPour verifier le port wifi, veuiller tester la commande \"ping 192.168.0.251\" sur le PC hote\n"
echo -e "\nPour redemarrer le WPA Supplicant, avant de lancer la commande \"sh init_wpa.sh\", ou pour larreter, executer \"sh stop_wpa.sh\"\n"

else #[ "$SSID" != "" -a "$PSK" != "" ]; print usage

echo -e '\nLes variables $SSID et $PSK ne sont pas definies dans /etc/wpa_supplicant/networkConfig.
Pour executer correctement le WPA Supplicant, vous devez renseigner ces variables de la facon suivante :
export SSID=mon_ssid
export PSK=ma_passphrase\n'

fi

else #[ -f /etc/wpa_supplicant/networkConfig ]; print usage

echo -e 'Le fichier /etc/wpa_supplicant/networkConfig nexiste pas.
Pour executer correctement le WPA Supplicant, vous devez creer ce fichier et y definir les variables $SSID et $PSK de la facon suivante :
export SSID=mon_ssid
export PSK=ma_passphrase\n'

fi

