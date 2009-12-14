#!/bin/sh

# Script Written by Sebastien Van Cauwenberghe  <svancau@gmail.com>
# do what you want with this script ;)

echo "Please select the development Board : "
BOARDOPTIONS="apf9328 apf27"
select BOARD in $BOARDOPTIONS; do
	if [ "$BOARD" = "apf9328" ]; then
		echo "APF9328 selected"
		break
	elif [ "$BOARD" = "apf27" ]; then
		echo "APF27 selected"
		break
	else
		echo "Bad option"
	fi
done

VERSION=3.2
FILE="armadeus-$VERSION.tar.bz2"
URL=http://downloads.sourceforge.net/project/armadeus/armadeus/armadeus-$VERSION/$FILE

CURSHELL=`ls -al /bin/sh | cut -d " " -f 10`
BASH_LOCATION=`which bash`
if [ -L /bin/sh ]; then
	if [ "$CURSHELL" != "$BASH_LOCATION" ]; then
		echo "Warning: /bin/sh doesn't point to $BASH_LOCATION... fixing it"
		sudo rm /bin/sh
		sudo ln -s $BASH_LOCATION /bin/sh
	fi
fi

sudo aptitude -y install build-essential gcc  g++ autoconf  automake libtool bison flex gettext patch subversion texinfo wget git-core 
sudo aptitude -y install libncurses5 libncurses5-dev zlib1g-dev liblzo2-2 liblzo2-dev libacl1 libacl1-dev uuid-dev libglib2.0-dev libnetpbm10-dev

wget -c $URL
tar -xjf $FILE 
cd armadeus-$VERSION
make $BOARD"_defconfig"
make
