#
#
obj-$(CONFIG_ARMADEUS_ISP1761) :=	pehci.o

clean:
	find . -name '*.mod.[co]' -or -name '.*.o.cmd' | xargs rm -f
	find . -name '*.ko' -or -name '*.ver' -or -name '*.mod' | xargs rm -f
	find . -name '*.o' -or -name '*.ko.cmd'| xargs rm -f
	find . -name '.*.*.cmd' | xargs rm -f
	rm -rf .tmp_versions
