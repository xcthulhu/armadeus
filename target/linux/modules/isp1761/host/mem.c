/************************************************************
 * NXP ISP176x Host Controller Interface code file
 *
 * (c) 2006 NXP B.V., All rights reserved. <usb.linux@nxp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * File Name: mem.c
 *
 * Refering linux kernel version 2.6.9
 *
 * History:
 *
 * Date                  Author                  Comments
 * --------------------------------------------------------------------
 * June 21, 2006          krishan                 Initial version
 *  
 **********************************************************************
 */

/*memory utilization fuctions*/
    void
phci_hcd_mem_init (void)
{
    int i = 0;
    u32 start_addr = 0x1000;
    struct isp1761_mem_addr *memaddr;
    for(i = 0;i<BLK_TOTAL;i++){
        memaddr = &memalloc[i];
        memset(memaddr,0,sizeof *memaddr);
    }

    /*initialize block of 256bytes*/
    for(i=0;i<BLK_256_;i++){
        memaddr = &memalloc[i];
        memaddr->blk_num = i;
        memaddr->blk_size = BLK_SIZE_256;
        memaddr->phy_addr = start_addr;
        start_addr += BLK_SIZE_256;
    }
    /*initialize block of 1024bytes*/
    for(i = BLK_256_;i<(BLK_256_ + BLK_1024_);i++){
        memaddr = &memalloc[i];
        memaddr->blk_num = i;
        memaddr->blk_size = BLK_SIZE_1024;
        memaddr->phy_addr = start_addr;
        start_addr += BLK_SIZE_1024;
    }

    /*initialize block of  4kbytes*/
    for(i=(BLK_256_ + BLK_1024_);i<(BLK_256_ + BLK_1024_+BLK_4096_);i++){
        memaddr = &memalloc[i];
        memaddr->blk_num = i;
        memaddr->blk_size = BLK_SIZE_4096;
        memaddr->phy_addr = start_addr;
        start_addr += BLK_SIZE_4096;
    }

}


/*free memory*/
    static void
phci_hcd_mem_free(struct isp1761_mem_addr *memptr)
{
    /*block number to be freed*/
    int block = memptr->blk_num;

    if((memptr->blk_size) &&
            (memalloc[block].used != 0))
    {
        memalloc[block].used = 0;
    }
}


/*allocate memory*/
static void
phci_hcd_mem_alloc(
        u32 size,
        struct isp1761_mem_addr *memptr,
        u32 flag)
{
    u32 blk_size = size;
    u16 i;
    u32 nextblk1=0,nextblk4=0;
    u32 start = 0,end = 0;
    struct isp1761_mem_addr *memaddr = 0;

    memset(memptr,0,sizeof *memptr);

    pehci_print("phci_hcd_mem_alloc(size = %d)\n",size);

    if(blk_size==0) {
        memptr->phy_addr = 0;
        memptr->virt_addr = 0;
        memptr->blk_size = 0;
        memptr->num_alloc = 0;
        memptr->blk_num = 0;
        return;
    }

    /*end of the 1k blocks*/
    nextblk1 = BLK_256_ +  BLK_1024_;
    /*end of the 4k blocks*/
    nextblk4 = nextblk1 +  BLK_4096_;


    if(blk_size<=BLK_SIZE_256){
        blk_size = BLK_SIZE_256;
        start = 0;
        end = BLK_256_;
    }
    else if(blk_size<=BLK_SIZE_1024){
        blk_size = BLK_SIZE_1024;
        start = BLK_256_;
        end = start + BLK_1024_;
    }
    else if(blk_size > BLK_SIZE_1024){
        blk_size = BLK_SIZE_4096;
        start = BLK_256_ + BLK_1024_;
        end = start + BLK_4096_;
    }

    for(i = start;i<end;i++){
        memaddr = &memalloc[i];
        if(!memaddr->used){
            memaddr->used = 1;
            memptr->blk_num = i;
            memptr->blk_size = blk_size;
            memptr->phy_addr = memaddr->phy_addr;
            memptr->virt_addr = memptr->phy_addr;
            return;
        }
    }

    /*look for in the next block if memory is free*/
    /*start from the first place of the next block*/
    start = end;

    /*for 1k and 256 size request only 4k can be returned*/
    end = nextblk4;

    for(i = start;i<end;i++){
        memaddr = &memalloc[i];
        if(!memaddr->used){
            memaddr->used = 1;
            memptr->blk_num = i;
            memptr->blk_size = blk_size;
            memptr->phy_addr = memaddr->phy_addr;
            memptr->virt_addr = memptr->phy_addr;
            return;
        }
    }

}

