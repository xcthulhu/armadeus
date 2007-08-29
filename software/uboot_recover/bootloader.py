#
#  APF9328 bootloader
#
#  Copyright (2006)  ARMADEUS
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
# author: nicolas.colombain@armadeus.com
#

import sys
import time
from struct import *
import os
import serial

class imxlBootloader:

    def __init__(self, serialport, sizeofRam=16 ):
        self.port = serialport
        self.ramSize = sizeofRam #default APF

    def sync(self):
        sync_done = False
        print "Re-synchronizing...\n"
        self.port.write('a')
        while sync_done <> True:
            if self.port.inWaiting():
                if self.port.read(1) == ':':
                    sync_done = True
                    print "Connection with target bootstrap done !"
            else:
                self.port.write('a')
                print "Please reset your board in bootstrap mode"
            self.port.flush()        
            time.sleep(1)
        if sync_done:
            self.__initRam()
        return sync_done
        
    def __displayProgress( self, text ):
        sys.stdout.write(chr(13))
        print text,
    
    def __initRam( self ):
        if self.ramSize == 16:
            self.put("221000","910A9267","l")
            self.get("8000000", "4","b")
            self.put("221000","A10A9267","l")
            self.get("8000000", "4","b")
            self.get("8000000", "4","b")
            self.get("8000000", "4","b")
            self.get("8000000", "4","b")
            self.get("8000000", "4","b")
            self.get("8000000", "4","b")
            self.get("8000000", "4","b")
            self.get("8000000", "4","b")
            self.put("221000","B10A9267","l")
            self.get("8023000", "4","b")
            self.put("221000","810A9267","l")
        else:
            print "Warning ramSize %d not supported" % self.ramSize
    #--------------------------------------------------------------
    # WaitFlashReady Test. Wait until the flash is ready to process next command
    def waitFlashReady(self) :
        flashStatus = 0
        while flashStatus < 0x0080 :
            flashStatus = (int( self.get('10000000', '1', 'w'),16) & 0x0080) 


    #----------------------------------------------------------------
    # Erase the Flash block containing the given (hexa ASCII) address
    def eraseFlashBlock( self, address ):
        self.put(address, "70", 'w')
        self.waitFlashReady()
        self.put(address, "20", 'w')
        self.put(address, "D0", 'w')
        self.waitFlashReady()


    #--------------------------------------------------------------
    # Write a word in Flash at the given (hexa ASCII) address
    # The block (64kWords) containing the address must have been previously cleared
    def writeFlashWord( self, address, word ):
        self.put(address, "70", 'w')
        self.waitFlashReady()
        self.put(address, "40", 'w')
        self.put(address, word, 'w')
        self.waitFlashReady()

    def fastloadRam( self, fileName, address ) :
        # load RamLoader program at end of 4Mbytes ram (min RAM on APD9328)
        self.loadbin( "ramprogrammer.bin","08400000")  
        f=open(fileName, 'rb')
        # store file size at address 0x08000000 for RamLoader program
        fsize = "%08x" % os.path.getsize(fileName) #os.stat(filesize)[6]
        self.put("08000000", fsize, "l")
        # store file destination address at address 0x08000004 for RamLoader program
        self.put("08000004", address, 'l')
        self.execute("08400000")
        print "Loading file %s. Please wait..." % fileName
        cin = ' '
        while cin <> '%' :        # wait start of RamLoader
            time.sleep(1)
            cin = self.port.read(1)

        data = f.read(16)
        count = 16
        b = 0
        while len(data):
            n = len(data)
            b = 16-n  # ensure always 4 bytes 
            str = unpack('B'*n,data)
            self.port.write(data)
            count = count + 16
            if (count % 1024) == 0:
                self.__displayProgress( "%d octets" % count ) 
            data = f.read(16)
        self.__displayProgress( "%d octets" % count ) 

        cin = ' '
        while cin <> '%' :                     # wait end of RamLoader
            time.sleep(1)
            cin = self.port.read(1)
        print ""
        f.close()

    def execute( self, address ) :
        assert len(address) <= 8, 'address too long'
        try:
            self.write(address[0:(len(address)-1)].zfill(7)+"000")
        except:
            pass

    def loadFlashbin( self, fileName, address ) :
        #a=glob.glob(fileName)[0]
        self.fastloadRam( fileName,"08001000");   
        self.Flashbin(address, '%x' % os.path.getsize(fileName),"08001000")
        #self.Flashbin(address, '%x' % os.stat(a)[6],"08001000")

    def Flashbin( self, address, size, fileRamPosition ):
        # erase the whole flash
        self.put("00220004", "00000D01", "l")  # Configure CS0 as 16bits wide bus
        self.put("10000000", "0060", "w")  # Clear block lock bit
        self.put("10000000", "00D0", "w")  # Clear block lock bit
        self.eraseFlashBlock("10000000")
        self.eraseFlashBlock("10020000")
        self.eraseFlashBlock("10040000")
        self.eraseFlashBlock("10080000")
        self.eraseFlashBlock("10100000")
        self.eraseFlashBlock("10200000")
        self.eraseFlashBlock("10400000")	

        # load FlashBurner program
        self.loadbin( "flashprogrammer.bin","08000400")   
        # store file size at address 0x08000000 for FlashBurner program
        self.put("08000000", size, 'l')
        # store file destination address at address 0x08000004 for FlashBurner program
        self.put("08000004", address, 'l')
        # store file position in RAM address 0x08000008 for FlashBurner program
        self.put("08000008", fileRamPosition, 'l')
        self.execute( "08000400")
        print "Flashing please wait..."
        data = self.port.read(1)
        while data <> '%':
            if self.port.inWaiting():
                data = self.port.read(1)
                if data == '.':
                    print ".",
        print ""

    def loadbin( self, fileName, addr ):
        f=open(fileName, 'rb')
        bytecount = 0
        currAddr = int(addr,16)
        data = f.read(16)
        print "loading file %s. Please wait..." % fileName
        while len(data):
            n = len(data)
            bytecount = bytecount + n
            str = unpack('B'*n,data)
            self.put( '%08x' % currAddr, '%02X'*n % str, 'b')
            currAddr = currAddr + 16
            data = f.read(16)
            if (bytecount % 1024) == 0: 
                self.__displayProgress( "%d octets" % bytecount )      
        self.__displayProgress( "%d octets" % bytecount )      
        print ""
        f.close()

    def read( self, count ):
        result = self.port.read(count+1) # trailer /
        return result[0:len(result)-1] # remove trailer

    def write( self, text2write ):
        self.port.write(text2write.upper())
        self.port.read(len(text2write))   
 
    def get( self, addr, count, mode ):
        countmode = 0x20
        addrmask = int(addr[len(addr)-1],16) & 0x0f
    
        assert len(addr) <= 8, 'address too long'
        assert len(count) > 0, 'zero length get'
        assert len(count) < 32, 'Too long get, should be < 31 bytes'   
    
        nbbyte = 1
        if mode == 'w':
            countmode = 0x60
            addrmask = int(addr[len(addr)-1],16) & 0x0e
            nbbyte = 2
        if mode == 'l':
            countmode = 0xe0
            addrmask = int(addr[len(addr)-1],16) & 0x0c
            nbbyte = 4

        nbbyte = nbbyte * int(count)
        addr2write = addr[0:(len(addr)-1)].zfill(7)  + (hex(addrmask)[2])
        cmbyte2write = '%02x' % (countmode + nbbyte)
        self.write( addr2write + cmbyte2write )
        return self.read(nbbyte*2) # read n char

    def put( self, addr, data, mode ):
        countmode = 0
        addrmask = int(addr[len(addr)-1],16) & 0x0f
        nbchar = 2
        data2write = data

        assert len(data) > 0, 'zero length put'
        assert len(addr) <= 8, 'address too long'
    
        if mode == 'w':
            countmode = 0x40
            addrmask = int(addr[len(addr)-1],16) & 0x0e
            nbchar = 4
        if mode == 'l':
            countmode = 0xC0
            addrmask = int(addr[len(addr)-1],16) & 0x0c
            nbchar = 8

        data2write = data.zfill(((nbchar-(len(data)%nbchar))%nbchar) + len(data) )
        addr2write = addr[0:(len(addr)-1)].zfill(7) + (hex(addrmask)[2])
        cmbyte2write = '%02x' % (countmode + len(data2write)/2)
        self.write( addr2write + cmbyte2write + data2write )
        self.port.read(1)
    
