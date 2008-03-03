#!/usr/bin/python
# Just a little script to print
# message from Xilinx synthesis
# Fabien Marteau
# 23/02/2008

import re
import sys


def report(file):
	# Key words message definition
	error = re.compile('^error',re.I)
	warning = re.compile('^warning',re.I)
	info 	= re.compile('^info',re.I)
	for line in file:
		if error.match(line):
			print error.sub('\033[01;31mERROR\033[00m',line)
		if warning.match(line):
			print warning.sub('\033[01;33mWARNING\033[00m',line)
		if info.match(line):
			print	info.sub('\033[01;32mINFO\033[00m',line)

print '---------------------------------------------------'
print ' Xst messages :'
file = open(sys.argv[1] + ".log" , 'r')
report(file)
file.close()
print '---------------------------------------------------'
print ' Ngdbuild messages :'
file = open(sys.argv[1] + ".ngdlog",'r')
report(file)
file.close()
print '---------------------------------------------------'
print ' Map messages :'
file = open(sys.argv[1] + ".maplog",'r')
report(file)
file.close()
print '---------------------------------------------------'
print ' Par messages :'
file = open(sys.argv[1] + ".parlog",'r')
report(file)
file.close()
print '---------------------------------------------------'
print ' trace messages :'
file = open(sys.argv[1] + ".tracelog",'r')
report(file)
file.close()
print '---------------------------------------------------'
print ' Bit messages :'
file = open(sys.argv[1] + ".bitlog",'r')
report(file)
file.close()

