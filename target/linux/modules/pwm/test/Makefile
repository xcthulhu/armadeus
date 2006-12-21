

OUTPUT = ../..

$(OUTPUT)/testtonemode: testtonemode.c PWM_def.h MX1_def.h
	arm-linux-gcc -o $@ $<

clean:
	rm -f $(OUTPUT)/testtonemode
