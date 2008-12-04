/*******************************************************************************
 * NXP ISP176x Isochronous Transfer support code file
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
 * File Name: itdptd.c
 * 
 *
 * History:     
 *
 * Date                 Author/Modified by      Comments
 * -----------------------------------------------------------------------------
 * Jan 16, 2006         Grant H.               Initial version
 * Feb 6, 2006          Grant H.               First stable release
 * May 02 2007		Prabhakar	       ported to 2.6.20 with backward 
 *                                             compatibility with 2.6.9  
 *
 *******************************************************************************
 */

#ifdef CONFIG_ISO_SUPPORT

/*
 * phcd_iso_itd_to_ptd - convert an ITD into a PTD
 *
 * phci_hcd *hcd
 *      - Main host controller driver structure
 * struct ehci_itd *itd
 *  - Isochronous Transfer Descriptor, contains elements as defined by the
 *        EHCI standard plus a few more NXP specific elements.
 * struct urb *urb
 *  - USB Request Block, contains information regarding the type and how much data
 *    is requested to be transferred.
 * void  * ptd 
 *  - Points to the ISO ptd structure that needs to be initialized
 *
 * API Description
 * This is mainly responsible for:
 *  -Initializing the PTD that will be used for the ISO transfer
 */
void * phcd_iso_itd_to_ptd( phci_hcd *hcd,
        struct ehci_itd *itd,
        struct urb *urb,
        void  * ptd
        )
{
    struct _isp1761_isoptd *iso_ptd;
    struct isp1761_mem_addr *mem_addr;

    unsigned long max_packet, mult, length, td_info1, td_info3;
    unsigned long token, port_num, hub_num, data_addr;
    unsigned long frame_number;

    iso_dbg(ISO_DBG_ENTRY, "phcd_iso_itd_to_ptd entry\n");

    /* Variable initialization */
    iso_ptd = (struct _isp1761_isoptd *)ptd;
    mem_addr = &itd->mem_addr;

    /*
     * For both ISO and INT endpoints descriptors, new bit fields we added to
     * specify whether or not the endpoint supports high bandwidth, and if so
     * the number of additional packets that the endpoint can support during a 
     * single microframe.
     * Bits 12:11 specify whether the endpoint supports high-bandwidth transfers
     * Valid values:
     *             00 None (1 transaction/uFrame)
     *             01 1 additional transaction
     *             10 2 additional transactions
     *             11 reserved
     */
    max_packet = usb_maxpacket( urb->dev,
            urb->pipe,
            usb_pipeout(urb->pipe)
            );  

    /* 
     * We need to add 1 since our Multi starts with 1 instead of the USB specs defined
     * zero (0).
     */
    mult = 1 + ((max_packet >> 11) & 0x3);
    max_packet &= 0x7ff;

    /* This is the size of the request (bytes to write or bytes to read) */
    length = itd->length;

    /*
     * Set V bit to indicate that there is payload to be sent or received. And 
     * indicate that the current PTD is active.
     */
    td_info1 = QHA_VALID;

    /* 
     * Set the number of bytes that can be transferred by this PTD. This indicates
     * the depth of the data field.
     */
    td_info1 |= (length << 3);

    /*
     * Set the maximum packet length which indicates the maximum number of bytes that
     * can be sent to or received from the endpoint in a single data packet.
     */
    if (urb->dev->speed != USB_SPEED_HIGH)
    {
        /* 
         * According to the ISP1761 specs for sITDs, OUT token max packet should
         * not be more  than 188 bytes, while IN token max packet not more than
         * 192 bytes (ISP1761 Rev 3.01, Table 72, page 79
         */
        if(usb_pipein(urb->pipe) &&
                (max_packet > 192))
        {
            iso_dbg(ISO_DBG_INFO, "[phcd_iso_itd_to_ptd]: IN Max packet over maximum\n");
            max_packet = 192;                   
        }

        if((!usb_pipein(urb->pipe)) &&
                (max_packet > 188))
        {
            iso_dbg(ISO_DBG_INFO, "[phcd_iso_itd_to_ptd]: OUT Max packet over maximum\n");
            max_packet = 188;                   
        }
    }
    td_info1 |= (max_packet << 18);

    /*
     * Place the FIRST BIT of the endpoint number here.
     */
    td_info1 |= (usb_pipeendpoint(urb->pipe) << 31);

    /*
     * Set the number of successive packets the HC can submit to the endpoint.
     */
    if(urb->dev->speed == USB_SPEED_HIGH)
    {
        td_info1 |= MULTI(mult);
    }

    /* Set the first DWORD */
    iso_ptd->td_info1 = td_info1;
    iso_dbg(ISO_DBG_DATA, "[phcd_iso_itd_to_ptd]: DWORD0 = 0x%08x\n", iso_ptd->td_info1);

    /*
     * Since the first bit have already been added on the first DWORD of the PTD         
     * we only need to add the last 3-bits of the endpoint number.
     */
    token = (usb_pipeendpoint(urb->pipe) & 0xE) >> 1;

    /*
     * Get the device address and set it accordingly to its assigned bits of the 2nd
     * DWORD.
     */
    token |= usb_pipedevice(urb->pipe) << 3;

    /* See a split transaction is needed */
    if(urb->dev->speed != USB_SPEED_HIGH)
    {
        /* 
         * If we are performing a SPLIT transaction indicate that it is so by setting
         * the S bit of the second DWORD.
         */             
        token |= 1 << 14;

        port_num = urb->dev->ttport;
        hub_num = urb->dev->tt->hub->devnum;

        /* Set the the port number of the hub or embedded TT */
        token |= port_num << 18;

        /* 
         * Set the hub address, this should be zero for the internal or
         * embedded hub
         */
        token |= hub_num << 25;
    } /* if(urb->dev->speed != USB_SPEED_HIGH) */

    /* 
     * Determine if the direction of this pipe is IN, if so set the Token bit of 
     * the second DWORD to indicate it as IN. Since it is initialized to zero and
     * zero indicates an OUT token, then we do not need anything to the Token bit
     * if it is an OUT token.
     */
    if(usb_pipein(urb->pipe))
        token |= (IN_PID << 10);

    /* Set endpoint type to Isochronous */
    token |= EPTYPE_ISO;

    /* Set the second DWORD */  
    iso_ptd->td_info2 = token;  
    iso_dbg(ISO_DBG_DATA, "[phcd_iso_itd_to_ptd]: DWORD1 = 0x%08x\n", iso_ptd->td_info2);

    /*
     * Get the physical address of the memory location that was allocated for this PTD 
     * in the PAYLOAD region, using the formula indicated in sectin 7.2.2 of the ISP1761 specs
     * rev 3.01 page 17 to 18.
     */
    data_addr = ( (unsigned long) (mem_addr->phy_addr) & 0xffff ) - 0x400;
    data_addr >>= 3;

    /*  Set it to its location in the third DWORD */
    td_info3 = data_addr << 8;

    /*
     * Set the frame number when this PTD will be sent for ISO OUT or IN
     * Bits 0 to 2 are don't care, only bits 3 to 7.
     */
    frame_number = itd->framenumber;
    td_info3 |= (frame_number << 3);

    /* Set the third DWORD */
    iso_ptd->td_info3 = td_info3;
    iso_dbg(ISO_DBG_DATA, "[phcd_iso_itd_to_ptd]: DWORD2 = 0x%08x\n", iso_ptd->td_info3);

    /* 
     * Set the A bit of the fourth DWORD to 1 to indicate that this PTD is active.
     * This have the same functionality with the V bit of DWORD0 
     */
    iso_ptd->td_info4 = QHA_ACTIVE;
    iso_dbg(ISO_DBG_DATA, "[phcd_iso_itd_to_ptd]: DWORD3 = 0x%08x\n", iso_ptd->td_info4);

    /* Set the fourth DWORD to specify which uSOFs the start split needs to be placed */
    iso_ptd->td_info5 = itd->ssplit;
    iso_dbg(ISO_DBG_DATA, "[phcd_iso_itd_to_ptd]: DWORD4 = 0x%08x\n", iso_ptd->td_info5);

    /* 
     * Set the fifth DWORD to specify which uSOFs the complete split needs to be sent.
     * This is VALID only for IN (since ISO transfers don't have handshake stages)
     */
    iso_ptd->td_info6 = itd->csplit;
    iso_dbg(ISO_DBG_DATA, "[phcd_iso_itd_to_ptd]: DWORD5 = 0x%08x\n", iso_ptd->td_info6);

    iso_dbg(ISO_DBG_EXIT, "phcd_iso_itd_to_ptd exit\n");
    return iso_ptd;
}/* phcd_iso_itd_to_ptd */

/*
 * phcd_iso_scheduling_info - Initializing the start split and complete split.
 *
 * phci_hcd *hcd
 *      - Main host controller driver structure
 * struct ehci_qh *qhead
 *  - Contains information about the endpoint.
 * unsigned long max_pkt
 *  - Maximum packet size that the endpoint in capable of handling
 * unsigned long high_speed
 *  - Indicates if the bus is a high speed bus
 * unsigned long ep_in
 *  - Inidcates if the endpoint is an IN endpoint
 *
 * API Description
 * This is mainly responsible for:
 *  - Determining the number of start split needed during an OUT transaction or
 *    the number of complete splits needed during an IN transaction.
 */
unsigned long phcd_iso_scheduling_info( phci_hcd *hcd,
        struct ehci_qh *qhead,
        unsigned long max_pkt,
        unsigned long high_speed,
        unsigned long ep_in
        )
{
    unsigned long count, usof, temp;

    /* Local variable initialization */
    usof = 0x1;

    if(high_speed)
    {
        qhead->csplit = 0;

        /* Always send high speed transfers in first uframes */
        qhead->ssplit = 0x1;
        return 0;
    }

    /* Determine how many 188 byte-transfers are needed to send all data */
    count = max_pkt/188;

    /* 
     * Check is the data is not a factor of 188, if it is not then we need 
     * one more 188 transfer to move the last set of data less than 188.
     */
    if(max_pkt % 188)
        count += 1;

    /* 
     * Remember that usof was initialized to 0x1 so that means
     * that usof is always guranteed a value of 0x1 and then
     * depending on the maxp, other bits of usof will also be set.
     */
    for(temp = 0; temp < count; temp++)
        usof |= (0x1 << temp);

    if(ep_in)
    {
        /* 
         * Send start split into first frame.
         */     
        qhead->ssplit = 0x1;

        /* 
         * Inidicate that we can send a complete split starting from
         * the third uFrame to how much complete split is needed to
         * retrieve all data. 
         *
         * Of course, the first uFrame is reserved for the start split, the
         * second is reserved for the TT to send the request and get some
         * data.
         */      
        qhead->csplit = (usof << 2);
    } /* if(ep_in) */
    else
    {
        /* 
         * For ISO OUT we don't need to send out a complete split 
         * since we do not require and data coming in to us (since ISO
         * do not have integrity checking/handshake).
         *
         * For start split we indicate that we send a start split from the 
         * first uFrame up to the the last uFrame needed to retrieve all
         * data
         */
        qhead->ssplit = usof;
        qhead->csplit = 0;
    } /* else for if(ep_in) */          
    return 0;
}/* phcd_iso_scheduling_info */

/*
 * phcd_iso_itd_fill - Allocate memory from the PAYLOAD memory region
 *
 * phci_hcd *pHcd_st
 *  - Main host controller driver structure
 * struct ehci_itd *itd
 *  - Isochronous Transfer Descriptor, contains elements as defined by the
 *        EHCI standard plus a few more NXP specific elements.
 * struct urb *urb
 *  - USB Request Block, contains information regarding the type and how much data
 *    is requested to be transferred.
 * unsigned long packets
 *  - Total number of packets to completely transfer this ISO transfer request.
 *
 * API Description
 * This is mainly responsible for:
 * - Initialize the following elements of the ITS structure
 *       > itd->length = length;        -- the size of the request
 *       > itd->multi = multi;          -- the number of transactions for 
 *                                         this EP per micro frame
 *       > itd->hw_bufp[0] = buf_dma;   -- The base address of the buffer where 
 *                                         to put the data (this base address was
 *                                         the buffer provided plus the offset)
 * - Allocating memory from the PAYLOAD memory area, where the data coming from
 *   the requesting party will be placed or data requested by the requesting party will
 *   be retrieved when it is available.
 */
unsigned long phcd_iso_itd_fill ( phci_hcd *hcd,
        struct ehci_itd *itd,
        struct urb *urb,
        unsigned long packets
        )
{
    unsigned long length, offset, pipe;
    unsigned long max_pkt, mult;
    dma_addr_t buff_dma;
    struct isp1761_mem_addr *mem_addr;

    iso_dbg(ISO_DBG_ENTRY, "phcd_iso_itd_fill entry\n");
    /* 
     * The value for both these variables are supplied by the one
     * who submitted the URB.
     */
    length = urb->iso_frame_desc[packets].length;
    offset = urb->iso_frame_desc[packets].offset;

    /* Initialize the status and actual length of this packet*/
    urb->iso_frame_desc[packets].actual_length = 0;
    urb->iso_frame_desc[packets].status = -EXDEV;

    /* Buffer for this packet*/
    buff_dma = cpu_to_le32((unsigned char *)urb->transfer_buffer + offset);

    /* Memory for this packet*/
    mem_addr = &itd->mem_addr;

    pipe = urb->pipe;
    max_pkt = usb_maxpacket(urb->dev, pipe, usb_pipeout(pipe));

    mult = 1 + ((max_pkt >> 11) & 0x3); 
    max_pkt = max_pkt & 0x7FF;  
    max_pkt *= mult;

    if( (length < 0) || (max_pkt < length) )
    {
        iso_dbg(ISO_DBG_ERR,"[phcd_iso_itd_fill Error]: No available memory.\n");
        return -ENOSPC;
    }
    itd->buf_dma = buff_dma;

    /* 
     * Allocate memory in the PAYLOAD memory region for the 
     * data buffer for this ITD
     */
    phci_hcd_mem_alloc(length, mem_addr, 0);
    if( length && ( (mem_addr->phy_addr == 0 ) || 
                (mem_addr->virt_addr == 0) ) )
    {   
        mem_addr = 0;
        iso_dbg(ISO_DBG_ERR, "[phcd_iso_itd_fill Error]: No payload memory available\n");
        return -ENOMEM;
    }

    /* Length of this packet */
    itd->length = length;

    /* Number of transaction per uframe */
    itd->multi = mult;

    /* Buffer address, one ptd per packet */
    itd->hw_bufp[0] = buff_dma;

    iso_dbg(ISO_DBG_EXIT, "phcd_iso_itd_fill exit\n");  
    return 0;
} /* phcd_iso_itd_fill */

/*
 * phcd_iso_get_itd_ptd_index - Allocate an ISO PTD from the ISO PTD map list
 *
 * phci_hcd *hcd
 *      - Main host controller driver structure
 * struct ehci_itd *itd
 *  - Isochronous Transfer Descriptor, contains elements as defined by the
 *        EHCI standard plus a few more NXP specific elements.
 *
 * API Description
 * This is mainly responsible for:
 * - Allocating an ISO PTD from the ISO PTD map list
 * - Set the equivalent bit of the allocated PTD to active
 *   in the bitmap so that this PTD will be included into
 *   the periodic schedule
 */
void phcd_iso_get_itd_ptd_index( phci_hcd *hcd, struct ehci_itd *itd )
{
    td_ptd_map_buff_t *ptd_map_buff;    
    unsigned long buff_type, max_ptds;
    unsigned char itd_index, bitmap;

    /* Local variable initialization */
    bitmap = 0x1;
    buff_type = td_ptd_pipe_x_buff_type[TD_PTD_BUFF_TYPE_ISTL];
    ptd_map_buff = (td_ptd_map_buff_t *) &(td_ptd_map_buff[buff_type]); 
    max_ptds = ptd_map_buff->max_ptds;

    for(itd_index = 0; itd_index < max_ptds; itd_index++) 
    {   
        /* 
         * ISO have 32 PTDs, the first thing to do is look for a free PTD.
         */
        if(ptd_map_buff->map_list[itd_index].state == TD_PTD_NEW) 
        {
            /* 
             * Determine if this is a newly allocated ITD by checking the
             * itd_index, since it was set to TD_PTD_INV_PTD_INDEX during
             * initialization
             */
            if( itd->itd_index == TD_PTD_INV_PTD_INDEX ) 
            {
                itd->itd_index = itd_index;
            }

            /* Once there is a free slot, indicate that it is already taken */
            ptd_map_buff->map_list[itd_index].datatoggle = 0;
            ptd_map_buff->map_list[itd_index].state = TD_PTD_ACTIVE;
            ptd_map_buff->map_list[itd_index].qtd = NULL;

            /* Put a connection to the ITD with the PTD maplist */
            ptd_map_buff->map_list[itd_index].itd  = itd;
            ptd_map_buff->map_list[itd_index].qh = NULL;

            /* ptd_bitmap just holds the bit assigned to this PTD. */            
            ptd_map_buff->map_list[itd_index].ptd_bitmap = bitmap << itd_index;

            phci_hcd_fill_ptd_addresses(&ptd_map_buff->map_list[itd_index], 
                    itd->itd_index, buff_type); 

            /* 
             * Indicate that this ITD is the last in the list and update 
             * the number of active PTDs
             */
            ptd_map_buff->map_list[itd_index].lasttd = 0;
            ptd_map_buff->total_ptds ++;

            /* TO DO: This variable is not used from what I have seen. */                       
            ptd_map_buff->active_ptd_bitmap |= (bitmap << itd_index);
            break;
        }/* if(ptd_map_buff->map_list[itd_index].state == TD_PTD_NEW) */
    }/* for(itd_index = 0; itd_index < max_ptds; itd_index++) */
    return;
}/* phcd_iso_get_itd_ptd_index */

/*
 * phcd_iso_itd_free_list - Free memory used by ITDs in ITD list
 *
 * phci_hcd *hcd
 *      - Main host controller driver structure
 * struct urb *urb
 *  - USB Request Block, contains information regarding the type and how much data
 *    is requested to be transferred.
 * unsigned long status
 *  - Variable provided by the calling routine that contain the status of the 
 *        ITD list.
 *
 * API Description
 * This is mainly responsible for:
 *  - Cleaning up memory used by each ITD in the ITD list
 */
void phcd_iso_itd_free_list( phci_hcd *hcd,
        struct urb *urb,
        unsigned long status
        )
{
    td_ptd_map_buff_t *ptd_map_buff;
    struct ehci_itd *first_itd, *next_itd, *itd;
    td_ptd_map_t *td_ptd_map;

    /* Local variable initialization */
    ptd_map_buff = &(td_ptd_map_buff[TD_PTD_BUFF_TYPE_ISTL]);
    first_itd = (struct ehci_itd *) urb->hcpriv;        
    itd = first_itd;

    /* 
     * Check if there is only one ITD, if so immediately 
     * go and clean it up.
     */
    if(itd->hw_next == EHCI_LIST_END)
    {           
        if(itd->itd_index != TD_PTD_INV_PTD_INDEX)
        {
            td_ptd_map = &ptd_map_buff->map_list[itd->itd_index];
            td_ptd_map->state = TD_PTD_NEW;
        }

        if(status != -ENOMEM)
            phci_hcd_mem_free(&itd->mem_addr);

        list_del (&itd->itd_list);
        qha_free(qha_cache, itd);

        urb->hcpriv = 0;
        return;
    } /* if(itd->hw_next == EHCI_LIST_END) */

    while(1)
    {
        /* Get the ITD following the head ITD */
        next_itd = (struct ehci_itd *) le32_to_cpu(itd->hw_next);
        if(next_itd->hw_next == EHCI_LIST_END)
        {               
            /* 
             * If the next ITD is the end of the list, check if space have 
             * already been allocated in the PTD array.
             */
            if(next_itd->itd_index != TD_PTD_INV_PTD_INDEX)
            {
                /* Free up its allocation */
                td_ptd_map = &ptd_map_buff->map_list[next_itd->itd_index];
                td_ptd_map->state = TD_PTD_NEW;
            }

            /* 
             * If the error is not about memory allocation problems, then
             * free up the memory used.
             */
            if(status != -ENOMEM)       
            {
                iso_dbg(ISO_DBG_ERR,"[phcd_iso_itd_free_list Error]: Memory not available\n");
                phci_hcd_mem_free(&next_itd->mem_addr);
            }

            /* Remove from the ITD list and free up space allocated for ITD structure */
            list_del (&next_itd->itd_list);
            qha_free(qha_cache, next_itd);
            break;
        } /* if(next_itd->hw_next == EHCI_LIST_END)*/ 

        /* 
         * If ITD is not the end of the list, it only means that it already have everything allocated
         * and there is no need to check which procedure failed. So just free all resourcs immediately
         */
        itd->hw_next = next_itd->hw_next;

        td_ptd_map = &ptd_map_buff->map_list[next_itd->itd_index];
        td_ptd_map->state = TD_PTD_NEW;
        phci_hcd_mem_free(&next_itd->mem_addr);
        list_del (&next_itd->itd_list);
        qha_free(qha_cache, next_itd);
    } /*  while(1) */

    /* Now work on the head ITD, it is the last one processed. */
    if(first_itd->itd_index != TD_PTD_INV_PTD_INDEX)
    {
        td_ptd_map = &ptd_map_buff->map_list[first_itd->itd_index];
        td_ptd_map->state = TD_PTD_NEW;
    }

    if(status != -ENOMEM)
    {
        iso_dbg(ISO_DBG_ERR,"[phcd_iso_itd_free_list Error]: No memory\n");
        phci_hcd_mem_free(&first_itd->mem_addr);
    }

    list_del (&first_itd->itd_list);
    qha_free(qha_cache, first_itd);
    urb->hcpriv = 0;
    return;
}/* phcd_iso_itd_free_list */

/*
 * phcd_submit_iso - ISO transfer URB submit routine
 *
 * phci_hcd *hcd
 *      - Main host controller driver structure
 * struct urb *urb
 *  - USB Request Block, contains information regarding the type and how much data
 *    is requested to be transferred.
 * unsigned long *status
 *  - Variable provided by the calling routine that will contain the status of the 
 *        phcd_submit_iso actions
 *
 * API Description
 * This is mainly responsible for:
 *  - Allocating memory for the endpoint information structure (pQHead_st)
 *  - Requesting for bus bandwidth from the USB core
 *  - Allocating and initializing Payload and PTD memory
 */
unsigned long phcd_submit_iso( phci_hcd *hcd,
	struct usb_host_endpoint *ep,
        struct urb *urb,
        unsigned long *status
        )
{
    struct _periodic_list *periodic_list;
    struct hcd_dev *dev;
    struct ehci_qh *qhead;
    struct ehci_itd *itd, *prev_itd;
    struct list_head *itd_list;

    unsigned long ep_in, max_pkt, mult;
    unsigned long bus_time, high_speed, start_frame;
    unsigned long flags, packets;

    iso_dbg(ISO_DBG_ENTRY, "phcd_submit_iso Entry\n");

    /* Local variable initialization */
    high_speed = 0;
    periodic_list = &hcd->periodic_list[0];
    dev = (struct hcd_dev*) bus_to_hcd(urb->dev->bus);
    urb->hcpriv = (void *) 0;
    prev_itd = (struct ehci_itd *) 0;
    itd = (struct ehci_itd *) 0;
    start_frame = 0;

    ep_in = usb_pipein(urb->pipe);

    /* 
     * Take the endpoint, if there is still no memory allocated
     * for it allocate some and indicate this is for ISO.
     */
    qhead  = ep->hcpriv;
    if(!qhead)
    {
        /* 
         * TO DO: Check who free this up, I did not observed this
         * freed up 
         */
        qhead = phci_hcd_qh_alloc(hcd);
        if(qhead == 0)
        {
            iso_dbg(ISO_DBG_ERR,"[phcd_submit_iso Error]: Not enough memory\n");
            return -ENOMEM;
        }
        qhead->type = TD_PTD_BUFF_TYPE_ISTL;
        ep->hcpriv = qhead;
    } /* if(!qhead) */

    /* 
     * Get the number of additional packets that the endpoint can support during a 
     * single microframe.
     */
    max_pkt = usb_maxpacket(urb->dev, urb->pipe, usb_pipeout(urb->pipe)); 

    /* 
     * We need to add 1 since our Multi starts with 1 instead of the USB specs defined
     * zero (0).
     */
    mult  = 1 + ((max_pkt >> 11) & 0x3);

    /* This is the actual length per for the whole transaction */
    max_pkt *= mult;

    /* Check bandwidth */
    bus_time= 0;

    if(urb->dev->speed == USB_SPEED_FULL)
    {
        bus_time = usb_check_bandwidth(urb->dev, urb);
        if(bus_time < 0)
        {                           
            usb_dec_dev_use(urb->dev);
            *status = bus_time;
            return *status;
        }
    } /*if(urb->dev->speed == USB_SPEED_FULL)  */   
    else /*HIGH SPEED*/
    {
        high_speed = 1;

        /* 
         * Calculate bustime as dictated by the USB Specs Section 5.11.3 
         * for high speed ISO
         */
        bus_time = 633232L;
        bus_time +=   (2083L * ((3167L + BitTime(max_pkt) * 1000L)/1000L));
        bus_time = bus_time/1000L;
        bus_time += BW_HOST_DELAY;
        bus_time = NS_TO_US(bus_time);
    }

    usb_claim_bandwidth(urb->dev, urb, bus_time, 1);

    /* Initialize the start split (ssplit) and complete split (csplit) variables of qhead */
    if( phcd_iso_scheduling_info(hcd, qhead, max_pkt, high_speed, ep_in) < 0 )
    {
        iso_dbg(ISO_DBG_ERR, "[phcd_submit_iso Error]: No space available\n");
        return -ENOSPC;
    }

    if(urb->iso_frame_desc[0].offset != 0)
    {
        *status = -EINVAL;
        iso_dbg(ISO_DBG_ERR, "[phcd_submit_iso Error]: Invalid value\n");
        return *status;
    }

    if(qhead->next_uframe == -1 ||
            hcd->periodic_sched == 0)
    {   
        /* Calculate the current frame number */
        if(urb->transfer_flags & URB_ISO_ASAP)
            start_frame = isp1761_reg_read32( hcd->dev,
                    hcd->regs.frameindex, 
                    start_frame
                    );
        else
            start_frame = urb->start_frame;

        INIT_LIST_HEAD(&hcd->urb_list);

        /* The only valid bits of the frame index is the lower 14 bits. */
        start_frame = start_frame & 0xFF;

        /* 
         * Remove the count for the micro frame (uSOF) and just leave the 
         * count for the frame (SOF). Since 1 SOF is equal to 8 uSOF then
         * shift right by three is like dividing it by 8 (each shift is divide by two)
         */ 
        start_frame >>= 3;

        start_frame += 2;                           
        qhead->next_uframe = start_frame + urb->number_of_packets;

    } /* if(qhead->next_uframe == -1 || hcd->periodic_sched == 0) */
    else
    {
        /* 
         * The periodic frame list size is only 32 elements deep, so we need 
         * the frame index to be less than or equal to 32 (actually 31 if we 
         * start from 0) 
         */
        start_frame = (qhead->next_uframe) % PTD_PERIODIC_SIZE;
        qhead->next_uframe = start_frame + urb->number_of_packets;
        qhead->next_uframe %= PTD_PERIODIC_SIZE;
    }

    spin_lock_irqsave(&hcd->lock, flags);
    iso_dbg(ISO_DBG_DATA,"[phcd_submit_iso]: Number of packets: %d\n", urb->number_of_packets);
    iso_dbg(ISO_DBG_DATA,"[phcd_submit_iso]: Packet Length: %d\n",
            urb->iso_frame_desc[0].length);
    iso_dbg(ISO_DBG_DATA,"[phcd_submit_iso]: Max packet: %d\n", (int) max_pkt);

    /* Make as many tds as number of packets */
    for(packets = 0; packets < urb->number_of_packets; packets++)
    {
        /* 
         * Allocate memory for the ITD data structure and initialize it.
         *
         * This data structure follows the format of the ITD
         * structure defined by the EHCI standard on the top part
         * but also contains NXP specific elements in the bottom
         * part
         */
        itd = (struct ehci_itd *) kmalloc(sizeof(struct ehci_itd), GFP_ATOMIC);
        if(!itd)
        {
            *status = -ENOMEM;

            /* Handle ITD list cleanup */
            if(urb->hcpriv)
            {
                phcd_iso_itd_free_list(hcd, urb, *status);
            }
            iso_dbg(ISO_DBG_ERR,"[phcd_submit_iso Error]: No memory available\n");
            return *status;
        }

        memset(itd, 0, sizeof(struct ehci_itd));

        INIT_LIST_HEAD(&itd->itd_list);

        itd->itd_dma = cpu_to_le32(itd);
        itd->urb = urb;

        /* 
         * Indicate that this ITD is the last in the list.
         *
         * Also set the itd_index to TD_PTD_INV_PTD_INDEX 
         * (0xFFFFFFFF). This would indicate when we allocate
         * a PTD that this ITD did not have a PTD allocated
         * before.
         */

        itd->hw_next = EHCI_LIST_END;
        itd->itd_index = TD_PTD_INV_PTD_INDEX;

        /* This ITD will go into this frame*/
        itd->framenumber = start_frame + packets;

        /* Number of the packet*/
        itd->index = packets;

        itd->framenumber = itd->framenumber % PTD_PERIODIC_SIZE;
        itd->ssplit = qhead->ssplit;
        itd->csplit = qhead->csplit;

        /* Initialize the following elements of the ITS structure
         *      > itd->length = length;                 -- the size of the request
         *      > itd->multi = multi;                   -- the number of transactions for 
         *                                         this EP per micro frame
         *      > itd->hw_bufp[0] = buf_dma;    -- The base address of the buffer where 
         *                                         to put the data (this base address was
         *                                         the buffer provided plus the offset)
         * And then, allocating memory from the PAYLOAD memory area, where the data 
         * coming from the requesting party will be placed or data requested by the 
         * requesting party will be retrieved when it is available.
         */
        *status  = phcd_iso_itd_fill(hcd, itd, urb, packets);

        if(*status !=0)
        {
            /* Handle ITD list cleanup */
            if(urb->hcpriv)
            {
                phcd_iso_itd_free_list(hcd, urb, *status);
            }
            iso_dbg(ISO_DBG_ERR,"[phcd_submit_iso Error]: Error in filling up ITD\n");
            return *status;
        }

        /* 
         * If this ITD is not the head/root ITD, link this ITD to the ITD 
         * that came before it.
         */
        if(prev_itd)
            prev_itd->hw_next = cpu_to_le32(itd);

        prev_itd = itd;

        /*               
         * Allocate an ISO PTD from the ISO PTD map list and
         * set the equivalent bit of the allocated PTD to active
         * in the bitmap so that this PTD will be included into
         * the periodic schedule
         */
        phcd_iso_get_itd_ptd_index(hcd, itd);

        /*if we dont have any space left*/
        if(itd->itd_index == TD_PTD_INV_PTD_INDEX)
        {
            *status  =  -ENOSPC;

            /* Handle ITD list cleanup */
            if(urb->hcpriv)
            {
                phcd_iso_itd_free_list(hcd, urb, *status);
            }
            return *status;                     
        }

        /* Insert this td into the periodic list*/
        periodic_list[itd->framenumber].framenumber = itd->framenumber;
        itd_list = &periodic_list[itd->framenumber].itd_head;
        list_add_tail(&itd->itd_list, itd_list);

        /* Inidcate that a new ITD have been scheduled */
        hcd->periodic_sched++;

        /* Determine if there are any ITD scheduled before this one. */
        if(urb->hcpriv == 0)
            urb->hcpriv = itd;
    }/* for(packets = 0; packets... */

    spin_unlock_irqrestore(&hcd->lock, flags);   

    /* Last td of current transaction*/
    itd->hw_next = EHCI_LIST_END;
    urb->error_count = 0;
    return *status;
} /* phcd_submit_iso */
#endif /* CONFIG_ISO_SUPPORT */
