
INCLUDE_PATH="../../../buildroot/build_arm_nofpu/SDL-1.2.9/include/"
LIBRARY_PATH="../../../buildroot/build_arm_nofpu/root/usr/lib/"
INSTALL_DIR="../../../buildroot/build_arm_nofpu/root/usr/bin/"

default: armanoid

all: armanoid armanoidpc

armanoid: armanoid.c
	arm-linux-g++ -I$(INCLUDE_PATH) -L$(LIBRARY_PATH) -o $@ $^ -lSDL

armanoidpc: armanoid.c
	g++ -I/usr/include/SDL -o $@ $^ -lSDL

install: armanoid
	cp armanoid $INSTALL_DIR
	cp ball.bmp $INSTALL_DIR
	cp block.bmp $INSTALL_DIR
	cp bar.bmp $INSTALL_DIR

clean: 
	rm -rf armanoid armanoidpc
	rm -rf *.o