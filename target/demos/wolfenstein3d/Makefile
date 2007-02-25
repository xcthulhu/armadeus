ifeq ($(TARGET),arm)
INCLUDE_PATH="../../../buildroot/build_arm_nofpu/SDL-1.2.11/include/"
LIBRARY_PATH="../../../buildroot/build_arm_nofpu/root/usr/lib/"
INSTALL_DIR="../../../buildroot/build_arm_nofpu/root/usr/bin/"
CC:=../../../buildroot/build_arm_nofpu/staging_dir/bin/arm-linux-gcc
DEFINES=TARGET
ROOT=../../../buildroot/build_arm_nofpu/staging_dir/
else
INCLUDE_PATH="/usr/include/SDL"
LIBRARY_PATH="/usr/lib/"
INSTALL_DIR="."
CC=gcc
DEFINES=HOST
endif

CFLAGS = -O2 -Wall
#CFLAGS = -g -Wall -pedantic
#CFLAGS = -g -O2 -Wall -pedantic 
#CFLAGS = -Wall -O6 -fomit-frame-pointer -ffast-math -funroll-loops -mpentiumpro -mcpu=pentiumpro -march=pentiumpro
#CFLAGS = -g -Wall -pedantic -std=gnu99
#CFLAGS = -Os -Wall -pedantic
#CFLAGS = -Os -Wall -fomit-frame-pointer -ffast-math -mpentiumpro -mcpu=pentiumpro -march=pentiumpro

OBJS = objs.o misc.o id_ca.o id_vh.o id_us.o \
	wl_act1.o wl_act2.o wl_act3.o wl_agent.o wl_game.o \
	wl_inter.o wl_menu.o wl_play.o wl_state.o wl_text.o wl_main.o \
	wl_debug.o vi_comm.o sd_comm.o
ROBJS = wl_draw.o
DOBJS = $(OBJS) $(ROBJS) vi_sdl.o

LFLAGS = -lm

OBJS += sd_null.o
#OBJS += sd_oss.o fmopl.o
#CFLAGS += -D_REENTRANT
#LFLAGS += -lpthread

CFLAGS += `$(ROOT)/usr/bin/sdl-config --cflags`

DLFLAGS = $(LFLAGS) `$(ROOT)/usr/bin/sdl-config --libs`

NASM = nasm

.SUFFIXES: .asm

all:	sdlwolf3d

$(DOBJS): version.h id_heads.h wl_def.h

.asm.o:
	$(NASM) -f elf -o $@ $<

swolf3d: $(SOBJS)
	$(CC) -o swolf3d $(SOBJS) $(SLFLAGS) 

xwolf3d: $(XOBJS)
	$(CC) -o xwolf3d $(XOBJS) $(XLFLAGS)

sdlwolf3d: $(DOBJS)
	$(CC) -o sdlwolf3d $(DOBJS) $(DLFLAGS)

clean:
	rm -rf swolf3d xwolf3d sdlwolf3d *.o

distclean: clean
	rm -rf *~ DEADJOE

forced: distclean all
	rm -rf *.o
