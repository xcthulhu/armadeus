# Default values for local compiling:
ARMADEUS_BASE_DIR=../../../..
-include $(ARMADEUS_BASE_DIR)/Makefile.in

STAGING_DIR:=$(ARMADEUS_BUILD_DIR)/staging_dir/
INSTALL_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/local/bin
INSTALL_DATA_DIR:=$(ARMADEUS_ROOTFS_DIR)/usr/share
CC:=$(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc
STRIP:=$(ARMADEUS_TOOLCHAIN_PATH)arm-linux-sstrip

DEFINES=TARGET
CFLAGS=-Wall -g -O0
BINARY=testcom

${BINARY}: testcom.c
	${CC} -Wall -O0 $< -o ${BINARY}

clean:
	rm ${BINARY}
