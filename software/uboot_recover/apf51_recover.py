#
#  APF51 bootloader recovering tool
#
#  Copyright (C) 2010-2011  ARMadeus Systems
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software 
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 
# authors: nicolas.colombain@armadeus.com, eric.jarrige@armadeus.com
#  
#

import sys
import time
from struct import *
import os
import serial
import thread
import re

import usb

class Error(Exception):
    """ Manage specific error

    attributes:
        message -- the message
        level   -- the exception level
    """

    def __init__(self,message,level=0):
        self._message = message
        self.level   = level
        Exception.__init__(self,message)
                
    def __repr__(self):
        return self.message

    def _get_message(self): 
        return self._message
    def _set_message(self, message): 
        self._message = message
    message = property(_get_message, _set_message)


    def __str__(self):
        print self.message

    def setLevel(self,level):
        self.level = int(str(level))
    def getLevel(self):
        return self.level


class apf51Bootloader:

    statuscode = {  0x8D: 'data specified is out of bounds',
                    0x55: 'error during assert verification',
                    0x36: 'hash verification failed', 
                    0x33: 'certificate parsing failed or the certificate contained an unsupported key', 
                    0x35: 'signature verification failed', 
                    0x47: 'Super-Root key installation failed', 
                    0x39: 'Failure not matching any other description', 
                    0x4B: 'CSF command sequence contains unsupported command identifier', 
                    0x4E: 'absence of expected CSF header',  
                    0x4D: 'CSF length is unsupported', 
                    0x2E: 'CSF TYPE does not match processor TYPE', 
                    0x2D: 'CSF UID does not match either processor UID or generic UID', 
                    0x3A: 'CSF customer/product code does not match processor customer/product code',                    
                    0x87: 'key indexis either unsupported, or an attempt is made to overwrite the SRK from a CSF command', 
                    0xF0: 'successful operation completion', 
                    0x17: 'SCC unexpectedly not in secure state', 
                    0x1E: 'secureRAM secret key invalid', 
                    0x1D: 'secureRAM initialization failure', 
                    0x1B: 'secureRAM self test failure', 
                    0x2B: 'secureRAM internal failure', 
                    0x27: 'secureRAM secrect key unexpectedly in use', 
                    0x3C: 'SAHARA failure', 
                    0x59: 'SAHARA/SCC connectivity failure', 
                    0xA3: 'All RTIC regions are allocated', 
                    0x93: 'RTIC/SCC connectivity failure', 
                    0x0F: 'SHW is not enabled', 
                    0x8B: 'an attempt is made to read a key from the list of subordinate public keys at a location where no key is installed', 
                    0x8E: 'algorithm type is either invalid or ortherwise unsupported', 
                    0x66: 'write operation to register failed', 
                    0x88: 'Successful download completion' }

    def __init__(self, serialDevice):
        self.serdev = serialDevice

    def getStatus(self):
        self.serdev.flushInput()
        self.serdev.write(self.buildcmd("0505", "00000000", "l", "00000000", "00000000", "0"))
        result = self.serdev.read(4)
        if len(result) > 0:
            try:
                if self.serdev.port == 'USB':
                    status = result[0]
                else:
                    status = ord(result[0])

                print "Status: %s" % self.statuscode[status]
                #for i in range(len(result)):
                #    print '%02x' % result[i],
            except KeyError,e:
                print "Unknown status " + str(status)
        else:
            print '\nTimeout while asking for status'

    def __displayProgress(self, text):
        sys.stdout.write(chr(13))
        print text,
        sys.stdout.flush()

    def get(self, addr, count, mode):
        """
        addr: address to read
        count: number of bytes to read
        mode: b, w, l for byte, word, long
        """
        assert len(addr) <= 8, 'address too long'
        assert int(count,16) > 0, 'zero length get'
        assert int(count,16) < 0xffffffff, 'Too long get, should be < 31 bytes'   
        self.serdev.write(self.buildcmd("0101" ,addr, mode, count.zfill(9-len(count)), "00000000", "0"))

        #nbbyte = 1
        #if mode == 'w':
        #    nbbyte = 2
        #if mode == 'l':
        #    nbbyte = 4

        #nbbyte = nbbyte * int(count)
        temp = self.serdev.read(int(count)+4) # read n+4 char
        if len(temp) == 0:
            print 'read timeout'
            return
        #print '\nread ACK:',
        #for i in range (len(temp)):
        #    print '%02x' % ord(temp[i]),

        #print 'read data:',
        result = []
        for i in range (4,len(temp)):
            
            result += '%02x' % ord(temp[len(temp)-i+3]),
        print ''.join(result)
        return ''.join(result)

    def put(self, addr, data, mode):
        """
        addr: address where the data have to be written
        data: data to write. must match the given mode
        mode: b, w, l for byte, word, long
        """
        assert len(data) > 0, 'zero length put'
        assert len(addr) == 8, 'address too long'
        self.serdev.write(self.buildcmd("0202", addr,mode, "00000000", data, "0"))
        result = self.serdev.read(4) #ack
        if len(result) == 0: 
            print 'read timeout'
            return
        #print ' \nwrite ACK:',
    #    for i in range (len(result)):
    #        print '%02x' % ord(result[i]),

        result = self.serdev.read(4)
        if len(result) == 0:
            print 'read timeout'
            return

        #print '\nwrite success:',
        #for i in range (len(result)):
            #print '%02x' % ord(result[i]),

    def download(self, addr, data, size, type):
        assert size > 0, 'file empty'
        f = open(data, 'rb')
        self.serdev.write(self.buildcmd("0404", addr, "x", size, "00000000", type))
        result = self.serdev.read(4) #ack
        if len(result) == 0: 
            print '\nread timeout'
            return

        data = f.read(16)
        count = int(size,16)
        oldcount = count
        total = 0
        b = 0
        print '\nTransfering:'
        while count >= 0: #len(data):
            n = len(data)
            b = 16-n  # ensure always 4 bytes
            #tr = unpack('B'*n,data)
            self.serdev.write(data)
            count = count - 16
            if count < 16:
                total = total + count
            else:
                total = total + 16
            if (oldcount - count) >= 4096:
                self.__displayProgress("%06d bytes" % total)
                oldcount = count
            data = f.read(16)
        self.__displayProgress("%d bytes transfered" % total)
        print ""
        f.close()
        return

    def buildcmd(self, cmd, addr, ds, cnt, data, ft):
        if ds == 'b':
            datatmp = ('%c' % int('00',16)) + ('%c' % int('00',16)) + ('%c' % int('00',16)) + ('%c' % int(data[0:2],16))
            datasize = "08"
        elif ds == 'w':
            datasize = "10"
            datatmp = ('%c' % int('00',16)) + ('%c' % int('00',16)) + ('%c' % int(data[0:2],16))  + ('%c' % int(data[2:4],16))
        elif ds == 'l':
            datasize = "20"
            datatmp  = ('%c' % int(data[0:2],16)) + ('%c' % int(data[2:4],16)) + ('%c' % int(data[4:6],16))  + ('%c' % int(data[6:8],16))
        else:
            datasize = "00"
            datatmp  = ('%c' % int(data[0:2],16)) + ('%c' % int(data[2:4],16)) + ('%c' % int(data[4:6],16))  + ('%c' % int(data[6:8],16))

        result = ('%c' % int(cmd[0:2],16)) + ('%c' % int(cmd[2:4],16)) + \
                 ('%c' % int(addr[0:2],16)) + ('%c' % int(addr[2:4],16)) + ('%c' % int(addr[4:6],16)) + ('%c' % int(addr[6:8],16)) + \
                 ('%c' % int(datasize,16)) + \
                 ('%c' % int(cnt[0:2],16)) + ('%c' % int(cnt[2:4],16)) + ('%c' % int(cnt[4:6],16)) + ('%c' % int(cnt[6:8],16)) + \
                 datatmp + ('%c' % int(ft,16))

    #    for i in range(len(result)):
    #        print '%02x' % ord(result[i]),
        #print cnt
        return result   

    def initIOMUX(self):
        print "SDRAM INIT IOMUX APF",
        # RAS
        self.put("73FA84A4","00000006","l")
        # CAS
        self.put("73FA84A8","00000006","l")
        # SDWE
        self.put("73FA84AC","000000E6","l")
        # SDCKE0
        self.put("73FA84B0","000000E6","l")
        # SDCKE1
        self.put("73FA84B4","000000E6","l")
        # SDCLK
        self.put("73FA84B8","000000E6","l")
        # SDQS0
        self.put("73FA84BC","000000C6","l")
        # SDQS1
        self.put("73FA84C0","000000C6","l")
        # SDQS2
        self.put("73FA84C4","000000C6","l")
        # SDQS3
        self.put("73FA84C8","000000C6","l")
        # CS0
        self.put("73FA84CC","000000E6","l")
        # CS1
        self.put("73FA84D0","000000E6","l")
        # DQM0
        self.put("73FA84D4","000000E6","l")
        # DQM1
        self.put("73FA84D8","000000E6","l")
        # DQM2
        self.put("73FA84DC","000000E6","l")
        # DQM3
        self.put("73FA84E0","000000E6","l")
        # DDRPKS
        self.put("73FA8820","00000040","l")
        # DRAM_B4
        self.put("73FA882C","00000006","l")
        # PKEDDR
        self.put("73FA8838","00000080","l")
        # DDR_A0
        self.put("73FA883C","00000006","l")
        # DDR_A1
        self.put("73FA8848","00000006","l")
        # DRAM_SR_B0
        self.put("73FA8878","00000000","l")
        # DDRAPKS
        self.put("73FA887C","00000040","l")
        # DRAM_SR_B1
        self.put("73FA8880","00000000","l")
        # DDRPUS
        self.put("73FA8884","00000020","l")
        # DRAM_SR_B2
        self.put("73FA888C","00000000","l")
        # PKEADDR
        self.put("73FA8890","00000080","l")
        # DRAM_SR_B4
        self.put("73FA889C","00000000","l")
        # DRAM_B0
        self.put("73FA88A4","00000006","l")
        # DRAM_B1
        self.put("73FA88AC","00000006","l")
        # DDR_SR_A0
        self.put("73FA88B0","00000000","l")
        # DRAM_B2
        self.put("73FA88B8","00000006","l")
        # DDR_SR_A1
        self.put("73FA88BC","00000000","l")

    def initSDRAM2(self):
        print "SDRAM INIT2 APF",
        #ESDCTL0: 13 ROW, 10 COL, 32Bit
        self.put("83FD9008","82220000","l")
        #ESDSCR: Precharge command 
        self.put("83FD9014","0400800C","l")
        #ESDSCR: Refresh command
        self.put("83FD9014","00008014","l")
        #ESDSCR: Refresh command
        self.put("83FD9014","00008014","l")
        #ESDSCR: LMR with CAS=3 and BL=3 (Burst Length = 8)
        self.put("83FD9014","0033801C","l")
        #ESDSCR: EMR with full Drive strength
        self.put("83FD9014","0000801E","l")
        #ESDCTL0: 13 ROW, 10 COL, 32Bit, SREF=4
        self.put("83FD9008","B2420000","l") 
        #ESDCFG0: tRFC:22clks, tXSR:28clks, tXP:3clks, tWTR:2clk, tRP:3clks, tMRD:2clks
        #tRAS:8clks, tRRD:2clks, tWR:3clks, tRCD:3clks, tRC:11clks 
        self.put("83FD900C","C33574AA","l") 
        #ESDMISC: AP=10, Bank interleaving on, MIF3 en, RALAT=2
        self.put("83FD9010","000a1700","l")

    def initSDRAM1(self, sdramType):
        print "SDRAM INIT1 APF",
        #ESDCTL0: 13 ROW, 10 COL, 32Bit
        self.put("83FD9000","82220000","l")
#        self.put("83FD9000","82120000","l") # 64MB
        #ESDSCR: Precharge command 
        self.put("83FD9014","04008008","l") 
        #ESDSCR: Refresh command
        self.put("83FD9014","00008010","l") 
        #ESDSCR: Refresh command
        self.put("83FD9014","00008010","l")
        #ESDSCR: LMR with CAS=3 and BL=3 (Burst Length = 8)
        self.put("83FD9014","00338018","l")
        #ESDSCR: EMR with full Drive strength
        self.put("83FD9014","0000801a","l")
        #ESDCTL0: 13 ROW, 10 COL, 32Bit, SREF=4
        self.put("83FD9000","B2220000","l")
#        self.put("83FD9000","D2120000","l") # 64MB
        #ESDCFG0: tRFC:22clks, tXSR:28clks, tXP:3clks, tWTR:2clk, tRP:3clks, tMRD:2clks
        #tRAS:8clks, tRRD:2clks, tWR:3clks, tRCD:3clks, tRC:11clks
        self.put("83FD9004","c33574aa","l")
#        self.put("83FD9004","FFFFFFFF","l") # 64MB
        #ESDMISC: AP=10, Bank interleaving on, MIF3 en, RALAT=2
        self.put("83FD9010","c00a5680","l")

    def consoleDump(tid):
        while 1:
            sys.stdout.write(tid.read(1))
    #        print '%s' % tid.read(1),


class USBDevice:
    """
    """
    def __init__(self, idVendor=0x15a2, idProduct=0x0041):
        # find our device
        dev = self.findDevice(idVendor, idProduct)
        # was it found?
        if dev is None:
            raise ValueError('MX51 not found')
        self.__dev = dev

        handle = dev.open()
        handle.setConfiguration(1) # choose the first configuration
        handle.claimInterface(0)   # choose the first interface
        print('Found i.MX51')

        self.__devhandle = handle
        self.__epBulkIn = 0x82
        self.__epBulkOut = 1
        self.port = 'USB'

    def findDevice(self, idVendor, idProduct):
        """Find a USB device by product and vendor id."""
        for bus in usb.busses():
            for device in bus.devices:
                if device.idVendor == idVendor and device.idProduct == idProduct:
                    return device
        return None

    def read(self, length):
        data = self.__devhandle.bulkRead(self.__epBulkIn, length, 100)
        return data

    def write(self, data):
        self.__devhandle.bulkWrite(self.__epBulkOut, data, 100)

    def flushInput(self):
        pass


class UBoot:
    """
    """
    def __init__(self, serial):
        self.serial = serial

    def __getOutput(self, error_msg=None):
        """ This function waits for U-Boot answer and returns it"""
        #debugser = serial.Serial("/dev/tty0",115200,timeout=1)
        response = ""
        while not re.search(r'BIOS>', response):
            char = self.serial.read(1)
            if char == '':
                raise Error(response)
            response = response + char
            if error_msg!=None:
                if re.search(error_msg,response):
                    raise Error("getOutput error")

        sys.stdout.write(response)
            #debugser.write(char)
        #debugser.close()
        return response
 
    def resetEnv(self):
        self.serial.write("run flash_reset_env\n")
        return self.__getOutput()

    def waitForPrompt(self):
        time.sleep(1)
        self.serial.write("\n")
        return self.__getOutput()

    def flash(self):
        """ Ask U-Boot to flash itself """
        self.serial.write("run flash_uboot\n")
        try:
            ret = self.__getOutput()
        except Error,e:
            raise Error("Flashing U-Boot")
        if not re.search(r'uboot succeed',ret):
            print ret
            raise Error("U-Boot flash failed",0)
        return ret
    

if __name__ == "__main__":    

    def testRAM():
        apfBootloader.put("90000000", "00000001", "l")
        apfBootloader.put("90000004", "00000002", "l")
        apfBootloader.put("90000008", "00000004", "l")
        apfBootloader.put("90000010", "00000008", "l")
        apfBootloader.put("90000020", "00000010", "l")
        apfBootloader.put("9000002C", "00000020", "l")
        apfBootloader.put("90000040", "00000040", "l")
        apfBootloader.put("90000080", "00000080", "l")
        apfBootloader.put("90000100", "00000100", "l")
        apfBootloader.put("90000200", "00000200", "l")
        apfBootloader.put("90000400", "00000400", "l")
        apfBootloader.put("90000800", "00000800", "l")
        apfBootloader.put("90001000", "00001000", "l")
        apfBootloader.put("90001004", "00002000", "l")
        apfBootloader.put("90002000", "00004000", "l")
        apfBootloader.put("90004000", "00008000", "l")
        apfBootloader.put("90010000", "00010000", "l")
        apfBootloader.put("90020000", "00020000", "l")
        apfBootloader.put("90040000", "00040000", "l")
        apfBootloader.put("90080000", "00080000", "l")
        apfBootloader.put("90100000", "00100000", "l")
        apfBootloader.put("90200000", "00200000", "l")
        apfBootloader.put("90400000", "00400000", "l")
        apfBootloader.put("90800000", "00800000", "l")
        apfBootloader.put("91000000", "01000000", "l")
        apfBootloader.put("92000000", "02000000", "l")
        apfBootloader.put("93000000", "04000000", "l")
        apfBootloader.put("90000024", "08000000", "l")
        apfBootloader.put("90000014", "10000000", "l")
        apfBootloader.put("90000018", "20000000", "l")
        apfBootloader.put("9000001C", "40000000", "l")
        apfBootloader.put("90000024", "80000000", "l")

        time.sleep(1)

        apfBootloader.get("90000000", "4", "l")
        apfBootloader.get("90000004", "4", "l")
        apfBootloader.get("90000008", "4", "l")
        apfBootloader.get("90000010", "4", "l")
        apfBootloader.get("90000020", "4", "l")
        apfBootloader.get("9000002C", "4", "l")
        apfBootloader.get("90000040", "4", "l")
        apfBootloader.get("90000080", "4", "l")
        apfBootloader.get("90000100", "4", "l")
        apfBootloader.get("90000200", "4", "l")
        apfBootloader.get("90000400", "4", "l")
        apfBootloader.get("90000800", "4", "l")
        apfBootloader.get("90001000", "4", "l")
        apfBootloader.get("90001004", "4", "l")
        apfBootloader.get("90002000", "4", "l")
        apfBootloader.get("90004000", "4", "l")
        apfBootloader.get("90010000", "4", "l")
        apfBootloader.get("90020000", "4", "l")
        apfBootloader.get("90040000", "4", "l")
        apfBootloader.get("90080000", "4", "l")
        apfBootloader.get("90100000", "4", "l")
        apfBootloader.get("90200000", "4", "l")
        apfBootloader.get("90400000", "4", "l")
        apfBootloader.get("90800000", "4", "l")
        apfBootloader.get("91000000", "4", "l")
        apfBootloader.get("92000000", "4", "l")
        apfBootloader.get("93000000", "4", "l")
        apfBootloader.get("90000024", "4", "l")
        apfBootloader.get("90000014", "4", "l")
        apfBootloader.get("90000018", "4", "l")
        apfBootloader.get("9000001C", "4", "l")
        apfBootloader.get("90000024", "4", "l")


    SPEED = 115200
    BINARY = "apf51-u-boot.bin"

    print "\n--- APF51 Bootstrap Tool ---\n"
    print "Procedure to follow:"
    print "1] Power off your board, close all serial terminal sessions, remove all USB cables"
    print "2] Put bootstrap jumper"
    print "3] Power on your board"
    print "4] Connect a USB cable from your PC to the APF51Dev OTG miniUSB connector"
    print "5] Connect a USB cable from your PC to the APF51Dev console miniUSB connector"
    print "(be sure to have a %s image in current dir too)\n" % BINARY
    port = raw_input('--- Enter serial port number or name to use for console (/dev/ttyACM0 under Linux (default) or COMx under Window$): ')
    if port is '':
        port = '/dev/ttyACM0'

    usb = USBDevice()
    apfBootloader = apf51Bootloader(usb)

    apfBootloader.getStatus()
    apfBootloader.initIOMUX()
    apfBootloader.initSDRAM1("APF51")
    #clear configuration request
    apfBootloader.put("83FD9014","00000000","l")
    #testRAM()

    print "--- Successfully initialized APF51, now loading U-Boot"
    if not os.path.exists(BINARY):
        raise Error("file "+BINARY+" doesn't exit",0)
    fsize = "%08x" % os.path.getsize(BINARY) #os.stat(filesize)[6]
    print "Loading %s, size = %d bytes" % (BINARY, os.path.getsize(BINARY))
    apfBootloader.download("90000000", BINARY, fsize, "AA")

    try:
        ser = serial.Serial(port, SPEED, timeout=2)
    except Exception, msg:
        print "unable to open serial port %s !" % port
	print msg
        sys.exit()
    ser.flush()

    uboot = UBoot(ser)

    # execute loaded code:
    apfBootloader.put("90000000", "90000004", "l")
    #self.bootstrap.getStatus() ?

    uboot.waitForPrompt()

    eraseAll = raw_input('\n--- Would you like to erase the environment variables ? y/N: ')
    if eraseAll == 'y':
        uboot.resetEnv()
    uboot.flash()
    print "\n--- U-Boot successfully recovered !"
    print "--- Now you can remove miniUSB OTG cable and bootstrap jumper. Then, restart your board"

    del ser

