

OUTPUT = .

all: testtonemode testplaymode

testtonemode: testtonemode.c PWM_def.h MX1_def.h
	arm-linux-gcc -o $@ $<

testplaymode: testplaymode.c PWM_def.h MX1_def.h
	arm-linux-gcc -o $@ $<

clean:
	rm -f testtonemode testplaymode
