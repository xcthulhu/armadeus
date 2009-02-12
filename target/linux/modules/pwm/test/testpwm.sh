#!/bin/sh
#
# Script to test your PWM: plug either an oscilloscope or a buzzer on the signal
#   and launch this script
#

SYS_DIR="/sys/class/pwm/pwm0/"
PERIOD="1000 2000 3000 4000 5000 6000 7000 8000 9000 10000"
FREQUENCY="100 200 300 400 500 600"
DUTY="001 250 333 500 666 750 999"

if [ ! -d "$SYS_DIR" ]; then
	echo "Can't find /sys/ interface"
	exit 1
fi

echo "Starting PWM test"
echo 1 > $SYS_DIR/active

#Test frequency setting
for freq in $FREQUENCY
do
    echo $freq > $SYS_DIR/frequency
    echo "Setting PWM to $freq Hz"
    sleep 5
done

#Test period setting
for period in $PERIOD
do
    echo $period > $SYS_DIR/period
    echo "Setting PWM to $period us"
    sleep 5

done

#Test duty cycle setting @1kHz -> duty value = positive width signal value
echo 1000 > $SYS_DIR/frequency
for duty in $DUTY
do
    echo $duty > $SYS_DIR/duty
    echo "Setting duty cycle to $duty/1000 "
    sleep 5
done

echo 0 > $SYS_DIR/active
echo "End of PWM test"
