#!/bin/sh
#
# Script to test your PWM: plug either an oscilloscope or a buzzer on the signal
#   and launch this script
#

SYS_DIR="/sys/class/pwm/pwm0/"
PERIOD="1000 2000 3000 4000 5000 6000 7000 8000 9000 10000"
FREQUENCY="100 200 300 400 500 600"

echo "Starting PWM test"
echo 1 > $SYS_DIR/active

for freq in $FREQUENCY
do
    echo $freq > $SYS_DIR/frequency
    echo "Setting PWM to $freq Hz"
    sleep 5
done
            
for period in $PERIOD
do
    echo $period > $SYS_DIR/period
    echo "Setting PWM to $period us"
    sleep 5

done

echo 0 > $SYS_DIR/active
echo "End of PWM test"
