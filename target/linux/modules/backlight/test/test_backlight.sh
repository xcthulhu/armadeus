#!/bin/sh

failed()
{
    echo "Test FAILED !"
    exit 1
}

BL_SYS_DIR="/sys/class/backlight/imx-bl/"
if [ ! -d "$BL_SYS_DIR" ]; then
	echo "/sys interface not found:"
	echo "  driver not loaded or wrong version !"
	failed
fi

# Save actual brightness
ACTUAL=`cat "$BL_SYS_DIR/actual_brightness"`
for INTENSITY in 300 255 200 150 100 50 25 0; do
    echo "Setting brightness to: $INTENSITY"
    echo "$INTENSITY" > "$BL_SYS_DIR/brightness"
    SET=`cat "$BL_SYS_DIR/actual_brightness"`
    if [ "$SET" != "$INTENSITY" ]; then
        echo "Values differ, written: $INTENSITY read: $SET"
        if [ "$INTENSITY" != "300" ]; then
            failed
        fi
    fi
    sleep 1
done
# Restore brightness
echo "$ACTUAL" > "$BL_SYS_DIR/brightness"

exit 0
