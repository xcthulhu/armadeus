#!/bin/sh
#
# Script to test your PWM: plug either an oscilloscope or a buzzer on the signal
#   and launch this script
#

SYS_DIR="/sys/class/pwm/pwm0/"
PERIOD="1000 2000 3000 4000 5000 6000 7000 8000 9000 10000"
FREQUENCY="100 200 300 400 500 600 700 800"
DUTY="001 250 333 500 666 750 999"

modprobe imx-pwm

if [ ! -d "$SYS_DIR" ]; then
	echo "can't find /sys/ interface"
	exit 1
fi

echo "Starting PWM test"
echo "Press ENTER when ready to check PWM output (frequencies)"
read

echo 1 > $SYS_DIR/active

# Test frequency setting
for freq in $FREQUENCY; do
	echo $freq > $SYS_DIR/frequency
	echo "Setting PWM frequency to $freq Hz"
	sleep 5
done

echo "Press ENTER when ready to check PWM output (periods)"
read
# Test period setting
for period in $PERIOD; do
	echo $period > $SYS_DIR/period
	freq=$((1000000/period))
	echo "Setting PWM period to $period us (-> $freq Hz)"
	sleep 5
done

echo "Press ENTER when ready to check PWM output (duty cycle @1kHz)"
read
# Test duty cycle setting @1kHz -> duty value = positive width signal value
echo 1000 > $SYS_DIR/frequency
for duty in $DUTY; do
	echo $duty > $SYS_DIR/duty
	echo "Setting duty cycle to $duty/1000 "
	sleep 5
done

echo 500 > $SYS_DIR/duty
echo 0 > $SYS_DIR/active
echo "End of PWM test"

