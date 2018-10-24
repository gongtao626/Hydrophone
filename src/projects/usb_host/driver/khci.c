#include "khci.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usbmsgq.h"
#include "usbevent.h"
#include "usb_host_hub.h"
#include "rtc.h"
//#include "derivative.h"
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
#include "usb_lock.h"
#endif
/* Global variables */


#if defined (CW)
	__attribute__((__aligned__(512)))
	uint_8 bdt[512] = { 1 };
#else
  #if defined (IAR)
    #pragma data_alignment=512
    static uint_8 bdt[512] = { 1 };
  #else
    #warning "No compiler defined"
  #endif
#endif


/** b17643
*/


#define MCF522X_USBOTG_BD_BC(n)                 ((n & 0x3ff) << 16)
#define MCF522X_USBOTG_ADDR_ADDR(x)             (((x)&0x7F)<<0)
#define MCF522X_USBOTG_BD_OWN                   0x80
#define MCF522X_USBOTG_BD_DATA01(n)             ((n & 1) << 6)
#define MCF522X_USBOTG_TOKEN_TOKEN_ENDPT(x)     (((x)&0xF)<<0)
#define _int_enable() USB_unlock()
#define HWTICKS_PER_MSEC     12500
#define HWTICKS_PER_TICK     62500

#define BDT_BASE              ((uint_32*)(bdt))

#define BD_PTR(ep, rxtx, odd)      (((uint_32)BDT_BASE) & 0xfffffe00 | ((ep & 0x0f) << 5) | ((rxtx & 1) << 4) | ((odd & 1) << 3))

/* FIXME: actually, only endpoint_data[0] is used in atom transaction */
static EP_STRUCT endpoint_data[16];


static USB_HOST_STATE_STRUCT_PTR _usb_host_ptr;
static USB_EVENT_STRUCT khci_event;
/* Transaction que */
USBMSGQ_STRUCT tr_que;
static PIPE_DESCRIPTOR_STRUCT_PTR last_to_pipe = NULL;
extern USB_HOST_STATE_STRUCT_PTR usb_host_state_struct_ptr;

static TR_INT_QUE_ITM_STRUCT TR_INT_QUE[USBCFG_KHCI_MAX_INT_TR];

static uint_32 HWTICKS_CONST_FS_ADD;
static uint_32 HWTICKS_CONST_FS_MUL;
static uint_32 HWTICKS_CONST_LS_ADD;
static uint_32 HWTICKS_CONST_LS_MUL;

/* Function prototypes */
static void   _usb_khci_process_tr_complete(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr, uint_32 remain, int_32 err);
static int    _usb_khci_get_hot_int_tr(TR_MSG_STRUCT *msg);
static int_32 _usb_khci_atom_tr(uint_32 type, uint_32 host_speed, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, uint_8 *buf, uint_16 len);
USB_STATUS    _usb_khci_init(_usb_host_handle handle);
USB_STATUS    _usb_khci_shutdown(_usb_host_handle handle);
USB_STATUS    _usb_khci_send(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr);
USB_STATUS    _usb_khci_send_setup(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr);
USB_STATUS    _usb_khci_recv(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr);
USB_STATUS    _usb_khci_cancel(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr);
USB_STATUS    _usb_khci_bus_control(_usb_host_handle handle, uint_8 bus_control);
uint_32       _usb_khci_get_frame_number(_usb_host_handle handle);
USB_STATUS    _usb_khci_host_open_pipe(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr);
USB_STATUS    _usb_khci_host_close_pipe(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr);



const USB_HOST_CALLBACK_FUNCTIONS_STRUCT _bsp_usb_host_callback_table = {
   /* The Host/Device init function */
   _usb_khci_init,

   /* The function to shutdown the host/device */
   _usb_khci_shutdown,

   /* The function to send data */
   _usb_khci_send,

   /* The function to send setup data */
   _usb_khci_send_setup,

   /* The function to receive data */
   _usb_khci_recv,
   
   /* The function to cancel the transfer */
   _usb_khci_cancel,
   
   /* The function for USB bus control */
   _usb_khci_bus_control,

   NULL,

   /* The function to close pipe */
   _usb_khci_host_close_pipe,
    
   /* Get frame number function */

   //_usb_khci_get_frame_number,
   _usb_khci_get_frame_number,
   /* Get microframe number function - not applicable on FS */
   NULL,
   
   /* Open pipe function */
   _usb_khci_host_open_pipe,
   
   NULL,
   
   NULL
};

/*
#if 1
//asm uint_32 mcf5xxx_byterev(uint_32 val);

__declspec(register_abi) asm uint_32 mcf5xxx_byterev(uint_32 val)
{
#if __STDABI__ == 1
    move.l 4(sp), d0
#endif
    
    byterev.l d0
    rts    
}

#else

#define mcf5xxx_byterev(val)    (val)

#endif
*/

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_init_int_tr
*  Returned Value : None
*  Comments       :
*        Initialize interrupt transaction que
*END*-----------------------------------------------------------------*/
static void _usb_khci_init_int_tr(void) {
    uint_32 i;
    TR_INT_QUE_ITM_STRUCT_PTR tr = TR_INT_QUE;
    
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++) {
        tr->msg.type = TR_MSG_UNKNOWN;
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_add_int_tr
*  Returned Value : -1 que is full
*  Comments       :
*        Add new interrupt transaction to que
*END*-----------------------------------_------------------------------*/
static int _usb_khci_add_int_tr(TR_MSG_STRUCT *msg, uint_32 period) {
    uint_32 i = 0;
    TR_INT_QUE_ITM_STRUCT_PTR tr = TR_INT_QUE;
    
    /* Find free position */
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++)
    {  
        if (tr->msg.type == TR_MSG_UNKNOWN)
        {
            memcpy(&tr->msg, msg,  sizeof(TR_MSG_STRUCT));
           
            break;

        }
    }
    
    return (i < USBCFG_KHCI_MAX_INT_TR) ? i : -1;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_atom_tr
*  Returned Value :  
*  Comments       :
*        
*END*------------------------------------------------------------------*/
static int_32 _usb_khci_atom_tr(uint_32 type, uint_32 host_speed, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, uint_8 *buf, uint_16 len) {
    EP_STRUCT_PTR ep_ptr;
    uint_32 bd;
    int_32 res;
    int_32 retry, old_len,result;
    int_32 delay_const = 10;
    uint_32 event_wait_ticks_count; 

#ifdef KHCICFG_TIMEBASED_SCHEDULING
    static TICK_STRUCT SOF_time;
    TICK_STRUCT get_time, elapsed_hwticks;
    uint_32 tr_hwticks, elapsed_time;
#endif
       
    last_to_pipe = NULL; /* At the beginning, consider that there was not timeout */
    
    old_len = len;
    len = (len > pipe_desc_ptr->MAX_PACKET_SIZE) ? pipe_desc_ptr->MAX_PACKET_SIZE : len;
    
    ep_ptr = &endpoint_data[0];
	
  	/* ADDR must be written before ENDPT0 (undocumented behavior) for to generate PRE packet */
    USB0_ADDR = (uint_8)((pipe_desc_ptr->SPEED == USB_SPEED_FULL) ? MCF522X_USBOTG_ADDR_ADDR(pipe_desc_ptr->DEVICE_ADDRESS) : 0x80 | MCF522X_USBOTG_ADDR_ADDR(pipe_desc_ptr->DEVICE_ADDRESS));
    
    USB0_ENDPT0 =
        (host_speed == USB_SPEED_LOW ? (USB_ENDPT_HOSTWOHUB_MASK) : 0) | (USB_ENDPT_RETRYDIS_MASK) |
        USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK | USB_ENDPT_EPHSHK_MASK;

    retry = (pipe_desc_ptr->PIPETYPE == USB_INTERRUPT_PIPE) ? 0 : pipe_desc_ptr->NAK_COUNT; /* set retry count - do not retry interrupt transaction */
	  
    do {
        res = 0;
        
        /*  Wait for USB ready, but with timeout */
        event_wait_ticks_count = 0;
        while (USB0_CTL & USB_CTL_TXSUSPENDTOKENBUSY_MASK) {
            time_delay(1);
            event_wait_ticks_count++;
            if (USB_EVENT_SET == _usb_event_wait_ticks(&khci_event, KHCI_EVENT_MASK, FALSE, 1) == USB_OK) {
                break;
            } else
              if(5 == event_wait_ticks_count){
                res = KHCI_ATOM_TR_RESET;
                break;
              }
        }
	    
        if (!res) {
          /* All is ok, do transaction */
    
#ifdef KHCICFG_TIMEBASED_SCHEDULING
          /* Compute the length of packet in hardware ticks */
          if (pipe_desc_ptr->SPEED)
              tr_hwticks = (HWTICKS_CONST_FS_ADD + HWTICKS_CONST_FS_MUL * len) >> 16;
          else
              tr_hwticks = (HWTICKS_CONST_LS_ADD + HWTICKS_CONST_LS_MUL * len) >> 16;
                  
          /* we are going to compute time if the packet can be placed to bus
          ** if we calculate that yes, then since here to the token must be performed as soon as possible
          ** therefore we disabled interrupts
          */
          USB_lock(); /* it should be before time_get_elapsed_ticks, but that function enables interrupts */
          get_time.HW_TICKS = get_hw_ticks();
          
          elapsed_hwticks.HW_TICKS = (get_time.HW_TICKS > SOF_time.HW_TICKS) ? (get_time.HW_TICKS - SOF_time.HW_TICKS) : (SOF_time.HW_TICKS - get_time.HW_TICKS)  ;
          
          elapsed_time = elapsed_hwticks.HW_TICKS % 12500;
          if ((elapsed_time + tr_hwticks) > 0)
          {
              USB0_ISTAT |= USB_ISTAT_SOFTOK_MASK; /* Clear SOF */
              while (!(USB0_ISTAT & USB_ISTAT_SOFTOK_MASK));
               /* wait for next SOF */ 
               
              SOF_time.HW_TICKS = get_hw_ticks(); /* Synchronize time & enable interrupts */
          }
#endif

          USB0_SOFTHLD = 0; /* this is needed as without this you can get errors */
          USB0_ISTAT |= USB_ISTAT_SOFTOK_MASK; /* clear SOF */
          USB0_ERRSTAT = 0xFF;	/* clear error status */
          
          
          switch (type) {
            case TR_CTRL:    
            	pipe_desc_ptr->bd_ptr = (uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd);

#warning"check for endianess"
         	//*((uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd) + 1) = mcf5xxx_byterev((uint_32)buf);
            	//*((uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd)) = mcf5xxx_byterev(MCF522X_USBOTG_BD_BC(len) |                  

         	*((uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd) + 1) = ((uint_32)buf);
            	*((uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd)) = (MCF522X_USBOTG_BD_BC(len) |
    	                                                                    (0x80));
        	    USB0_TOKEN = (uint_8)(MCF522X_USBOTG_TOKEN_TOKEN_ENDPT((uint_8)pipe_desc_ptr->ENDPOINT_NUMBER) | (0xD0));
        	    ep_ptr->tx_bd ^= 1;
        	    break;
            case TR_IN:
                pipe_desc_ptr->bd_ptr = (uint_32*)BD_PTR(0, 0, ep_ptr->rx_bd);
        	//*((uint_32*)BD_PTR(0, 0, ep_ptr->rx_bd) + 1) = mcf5xxx_byterev((uint_32)buf);
            	//*((uint_32*)BD_PTR(0, 0, ep_ptr->rx_bd)) = mcf5xxx_byterev( MCF522X_USBOTG_BD_BC(len) | 
        	    *((uint_32*)BD_PTR(0, 0, ep_ptr->rx_bd) + 1) =((uint_32)buf);

                *((uint_32*)BD_PTR(0, 0, ep_ptr->rx_bd)) = (MCF522X_USBOTG_BD_BC (len) | 
    	                                                                    0x80 |
    	                                                                    MCF522X_USBOTG_BD_DATA01(pipe_desc_ptr->NEXTDATA01));
   	            USB0_TOKEN = (uint_8)(MCF522X_USBOTG_TOKEN_TOKEN_ENDPT(pipe_desc_ptr->ENDPOINT_NUMBER) | (0x90));
        	    ep_ptr->rx_bd ^= 1;   	    
        	    break;
            case TR_OUT:
                pipe_desc_ptr->bd_ptr = (uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd);
              
//    	        *((uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd) + 1) = mcf5xxx_byterev((uint_32)buf);
//    	        *((uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd)) = mcf5xxx_byterev( MCF522X_USBOTG_BD_BC(len) |
    	        *((uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd) + 1) = ((uint_32)buf);
    	        *((uint_32*)BD_PTR(0, 1, ep_ptr->tx_bd)) = ( MCF522X_USBOTG_BD_BC(len) |
    	                                                                    0x80 |
    	                                                                    MCF522X_USBOTG_BD_DATA01(pipe_desc_ptr->NEXTDATA01));
   	            USB0_TOKEN = (uint_8)(MCF522X_USBOTG_TOKEN_TOKEN_ENDPT(pipe_desc_ptr->ENDPOINT_NUMBER) | (0x10));
    	        ep_ptr->tx_bd ^= 1;
    	        break;
	      }

#if defined(KHCICFG_TIMEBASED_SCHEDULING)
          _int_enable(); /* Reenable interrupts */
#endif	      
	      
	      event_wait_ticks_count = 0;
	        
	        while(USB_EVENT_NOT_SET == _usb_event_wait_ticks(&khci_event, KHCI_EVENT_MASK, FALSE, 100)){
            time_delay(1);
            event_wait_ticks_count++;
            if(100 == event_wait_ticks_count) {
              res = KHCI_ATOM_TR_TO;
              last_to_pipe = pipe_desc_ptr;
              break;
            }
	        };
        }
        
try_again:
        /* check for reset event (detach)*/
        if (khci_event.VALUE & KHCI_EVENT_RESET) {
            res = KHCI_ATOM_TR_RESET;
            break;
        }
    
        if (khci_event.VALUE & KHCI_EVENT_TOK_DONE) {
            /* transaction finished */
            _usb_event_clear(&khci_event,KHCI_EVENT_TOK_DONE); 
            /* check result*/

            bd = (*pipe_desc_ptr->bd_ptr);
            //bd = mcf5xxx_byterev(*pipe_desc_ptr->bd_ptr);
            
            
            if (USB0_ERRSTAT & (
            	    USB_ERRSTAT_PIDERR_MASK  |                   
                    USB_ERRSTAT_CRC16_MASK    |
                    USB_ERRSTAT_DFN8_MASK     |
                    USB_ERRSTAT_DMAERR_MASK  |
                    USB_ERRSTAT_BTSERR_MASK))
            {
                 
                res = -USB0_ERRSTAT;
                break;
            }
            else 
            {
                switch (bd >> 2 & 0xf) {
                    case 0x03:  /* DATA0 */
                    case 0x0b:  /* DATA1 */
                    case 0x02:  /* ACK */
                        retry = 0;
                        res = (bd >> 16) & 0x3ff;
                        pipe_desc_ptr->NEXTDATA01 ^= 1;     /* switch data toggle */
                        break;

                    case 0x0e:  /* STALL */
                        res = KHCI_ATOM_TR_STALL;
                        retry = 0;
                        break;

                    case 0x0a:  /* NAK */
                        res = KHCI_ATOM_TR_NAK;
                        if (retry)
                            time_delay(delay_const * (pipe_desc_ptr->NAK_COUNT - retry));
                            time_delay(5);
                        break;

                    case 0x00:  /* bus timeout */
                        /* wait a bit, but not too much, perhaps some error occurs on the bus */
                        event_wait_ticks_count = 0;
                        while(USB_EVENT_NOT_SET == _usb_event_wait_ticks(&khci_event, KHCI_EVENT_MASK, FALSE, delay_const)) 
                        {                         
                          time_delay(1);
                          event_wait_ticks_count++;
                          if(50 == event_wait_ticks_count) {
                            res = KHCI_ATOM_TR_TO;
                            last_to_pipe = pipe_desc_ptr; /* remember this pipe as it had timeout last time */
                            if (retry)
                              time_delay(delay_const * (pipe_desc_ptr->NAK_COUNT - retry));
                            break;
                          }
                        }
                        if( 50 != event_wait_ticks_count)
                          goto try_again;
                        break;

                    case 0x0f:  /* data error */
                    	/* if this event happens during enumeration, then return means not finished enumeration */
                        res = KHCI_ATOM_TR_DATA_ERROR;
                        time_delay(delay_const * (pipe_desc_ptr->NAK_COUNT - retry));
                        break;
                        
                    default:
                        break;
                }
            }
        }
    } while (retry--);
    
    return res;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_process_tr_complete
*  Returned Value : none
*  Comments       :
*        Transaction complete
*END*-----------------------------------------------------------------*/
USB_HUB_CLASS_INTF_STRUCT_PTR if_ptr_debug;
static void _usb_khci_process_tr_complete(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr, uint_32 remaining_bytes, int_32 err) {
    USB_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
    
    uchar_ptr buffer_ptr = NULL;
    uint_32 total_req_bytes = 0;
    uint_32 status = 0;
    
    if (err == KHCI_ATOM_TR_STALL)
    	status = USBERR_ENDPOINT_STALLED;
    else if ((err == KHCI_ATOM_TR_NAK) || (err >= 0)) {
        status = USB_OK;
        
        if ((remaining_bytes > 0) || (err == KHCI_ATOM_TR_NAK)) /* remaining bytes */
            status = USBERR_TR_FAILED;

    	if (pipe_desc_ptr->PIPETYPE == USB_CONTROL_PIPE) {
        	if (pipe_tr_ptr->SEND_PHASE) {
            	total_req_bytes = pipe_tr_ptr->TX_LENGTH;
            	buffer_ptr = pipe_tr_ptr->TX_BUFFER;
            	pipe_tr_ptr->SEND_PHASE = FALSE;
        	}
        	else {
            	total_req_bytes = pipe_tr_ptr->RX_LENGTH;
            	buffer_ptr = pipe_tr_ptr->RX_BUFFER;
        	}
    	}
    	else {
        	if (pipe_desc_ptr->DIRECTION) {
            	total_req_bytes = pipe_tr_ptr->TX_LENGTH;
            	buffer_ptr = pipe_tr_ptr->TX_BUFFER;
        	}
        	else {
            	total_req_bytes = pipe_tr_ptr->RX_LENGTH;
            	buffer_ptr = pipe_tr_ptr->RX_BUFFER;
        	}
    	}
    }
    else if (err < 0)
    	status = USBERR_TR_FAILED;
    
    pipe_tr_ptr->status = USB_STATUS_IDLE;
    pipe_tr_ptr->TR_INDEX = 0;
    if_ptr_debug = (USB_HUB_CLASS_INTF_STRUCT_PTR)pipe_tr_ptr->CALLBACK_PARAM;

    if (pipe_tr_ptr->status == USB_STATUS_IDLE) {
        /* Transfer done. Call the callback function for this 
        ** transaction if there is one (usually true). 
        */
        if (pipe_tr_ptr->CALLBACK != NULL) {
            pipe_tr_ptr->CALLBACK((ptr)pipe_desc_ptr, pipe_tr_ptr->CALLBACK_PARAM,
                buffer_ptr, (total_req_bytes - remaining_bytes), status);
                
            /* If the application enqueued another request on this pipe 
            ** in this callback context then it will be at the end of the list
            */
        }
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_rm_int_tr
*  Returned Value : 0 successful
*  Comments       :
*        Remove interrupt transaction from que
*END*-----------------------------------------------------------------*/
static int _usb_khci_rm_int_tr(TR_MSG_STRUCT *msg) {
    int_32 i = 0, res = -1;
    static uint_32 countrm_int_tr = 0;
    TR_INT_QUE_ITM_STRUCT_PTR tr = TR_INT_QUE;
    
    /* find record */
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++)
    {
        if ((tr->msg.pipe_desc == msg->pipe_desc) && (tr->msg.pipe_tr == msg->pipe_tr))
        {  
          tr->msg.type = TR_MSG_UNKNOWN;           
          return 0;  
        }
        
    }
    
    return -1;
}



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_init
*  Returned Value : error or USB_OK
*  Comments       :
*        Initialize the kirin HCI controller
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_init(_usb_host_handle handle) {
	
	USB_STATUS                 status = USB_OK;

  static const uint_32  msg_size_in_max_type = 1 + (sizeof(TR_MSG_STRUCT) - 1) / sizeof(uint_32);
  if (_usb_msgq_init(USBCFG_KHCI_TR_QUE_MSG_CNT, sizeof(TR_MSG_STRUCT)/sizeof(uint_32)) != USB_MSGQ_OK) {
        /* usb_msgq_init failed */
        return USBERR_INIT_FAILED;
  }
  
#if defined(KHCICFG_TIMEBASED_SCHEDULING)    
  HWTICKS_CONST_FS_ADD = (59684 + 1734) * HWTICKS_PER_MSEC / 100;
	HWTICKS_CONST_FS_MUL = 51099 * HWTICKS_PER_MSEC / 1000;
	HWTICKS_CONST_LS_ADD = (41943 + 1404 + 2 * 6000) * HWTICKS_PER_MSEC / 10; /* 3000 is the hub delay const */
	HWTICKS_CONST_LS_MUL = 41390 * HWTICKS_PER_MSEC / 100;
#endif	

  _usb_event_init(&khci_event);
	USB0_ISTAT = 0xff;

        //USBOTG_USBCTRL = USBOTG_USBCTRLCLKSRC0_MASK;

	USB0_CTL |= USB_CTL_ODDRST_MASK;
	
	memset(endpoint_data, 0, sizeof(endpoint_data));
	
	USB0_BDTPAGE1 = (uint_8)((uint_32)BDT_BASE >> 8);
        USB0_BDTPAGE2 = (uint_8)((uint_32)BDT_BASE >> 16);
        USB0_BDTPAGE3 = (uint_8)((uint_32)BDT_BASE >> 24);
    
    /* Set SOF threshold */
  USB0_SOFTHLD = USB_SOFTHLD_CNT_MASK;     
  USB0_INTEN= USB_INTEN_ATTACHEN_MASK;  

//  USBOTG_USBCTRL = USBOTG_USBCTRL_DMLOW_MASK | USBOTG_USBCTRL_DPLOW_MASK | USBOTG_USBCTRL_OTGEN_MASK;     
	USB0_CTL = USB_CTL_HOSTMODEEN_MASK;
	
  _usb_khci_init_int_tr();
		
	return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_shutdown - NOT IMPLEMENT YET
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to shutdown the host
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_shutdown(_usb_host_handle handle) {
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_send
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to send data
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_send(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr) {
	USB_STATUS status = USB_OK;
	TR_MSG_STRUCT msg;

	msg.type = TR_MSG_SEND;
	msg.pipe_desc = pipe_desc_ptr;
  msg.pipe_tr = pipe_tr_ptr;
	

  //	_usb_msgq_send(&msg);    
	_usb_msgq_send((int*)&msg);    
    
	return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_cancel
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to cancel the transfer
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_cancel(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr) {
    TR_MSG_STRUCT msg;
    
    /* remove messages from KHCI msg que */

//    while (_usb_msgq_receive(&msg) == USB_MSGQ_OK)
        while (_usb_msgq_receive((int*)&msg) == USB_MSGQ_OK)
        /* void */;
    
	return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_recv
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to receive data
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_recv(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr) {
	USB_STATUS status = USB_OK;
	TR_MSG_STRUCT msg;

	msg.type = TR_MSG_RECV;
	msg.pipe_desc = pipe_desc_ptr;
  msg.pipe_tr = pipe_tr_ptr; 	

        
//        _usb_msgq_send(&msg);    
        _usb_msgq_send((int*)&msg);    
    
	return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_bus_control
*  Returned Value : error or USB_OK
*  Comments       :
*        The function for USB bus control
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_bus_control(_usb_host_handle handle, uint_8 bus_control) {
	return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_get_frame_number
*  Returned Value : frame number
*  Comments       :
*        The function to get frame number
*END*-----------------------------------------------------------------*/
uint_32 _usb_khci_get_frame_number(_usb_host_handle handle) {
  return (USB0_FRMNUMH << 8 | USB0_FRMNUML);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_task
*  Returned Value : none
*  Comments       :
*        KHCI task
*END*-----------------------------------------------------------------*/
void _usb_khci_task(void) {
    static uint_32 seq_ints = 10;    
    static uint_32 wait_on = 0;
    static TR_MSG_STRUCT msg;
    static uint_32 remain = 0;
    static int_32 res;
    static uint_8 *buf;
    int i;
        
        if (usb_host_state_struct_ptr->DEVICE_LIST_PTR) {

            msg.type = TR_MSG_UNKNOWN;
        
            /* look for interrupt transaction */
            if ((_usb_khci_get_hot_int_tr(&msg) != 0) || (!seq_ints--)) {                
                seq_ints = 10;
                if (_usb_msgq_receive((int *)&msg) == USB_MSGQ_OK)
                    if (msg.pipe_desc->PIPETYPE == USB_INTERRUPT_PIPE)                       
                      _usb_khci_add_int_tr(&msg, msg.pipe_desc->INTERVAL);
                    
                     
            }
          
          if (msg.type != TR_MSG_UNKNOWN) {
                /* evaluate message */
                
                switch (msg.type) {
                    case TR_MSG_SETUP:
    //                        msg.pipe_desc->NEXTDATA01 = 0; done in upper layer

//                      _usb_khci_atom_tr(TR_CTRL, usb_host_state_struct_ptr->speed, msg.pipe_desc,&msg.pipe_tr->setup_packet, 8);
                      _usb_khci_atom_tr(TR_CTRL, usb_host_state_struct_ptr->speed, msg.pipe_desc, (uint_8*)&msg.pipe_tr->setup_packet, 8);
                                                       
                        /* send next token */
                        if (msg.pipe_tr->RX_LENGTH) {
                            buf = msg.pipe_tr->RX_BUFFER;
                            remain = msg.pipe_tr->RX_LENGTH;
                        
                            while (remain > 0) {
                                res = _usb_khci_atom_tr(TR_IN, usb_host_state_struct_ptr->speed, msg.pipe_desc, buf, remain);
                                
                                if (res < 0)
                                    break;
                            
                                buf += res;
                                remain -=res; 
                            }
                        
                            msg.pipe_desc->NEXTDATA01 = 1;
                            res = _usb_khci_atom_tr(TR_OUT, usb_host_state_struct_ptr->speed, msg.pipe_desc, 0, 0);
                        }
                        else if (msg.pipe_tr->TX_LENGTH) {
                            buf = msg.pipe_tr->TX_BUFFER;
                            remain = msg.pipe_tr->TX_LENGTH;
                        
                            while (remain > 0) {
                                res = _usb_khci_atom_tr(TR_OUT, usb_host_state_struct_ptr->speed, msg.pipe_desc, buf, remain);
                        
                                if (res < 0)
                                    break;
                            
                                buf += res;
                                remain -=res;
                            }

                            msg.pipe_desc->NEXTDATA01 = 1;
                            res = _usb_khci_atom_tr(TR_IN, usb_host_state_struct_ptr->speed, msg.pipe_desc, 0, 0);
                        }
                        else {
                            msg.pipe_desc->NEXTDATA01 = 1;
                            res = _usb_khci_atom_tr(TR_IN, usb_host_state_struct_ptr->speed, msg.pipe_desc, 0, 0);
                            remain = 0;
                        }
                        break;

                    case TR_MSG_RECV:
                        buf = msg.pipe_tr->RX_BUFFER;
                        remain = msg.pipe_tr->RX_LENGTH;

                        do {
                            res = _usb_khci_atom_tr(TR_IN, usb_host_state_struct_ptr->speed, msg.pipe_desc, buf, remain);
                            time_delay(10);   
                            if (msg.pipe_desc->PIPETYPE == USB_INTERRUPT_PIPE) {
                                i = 10;
                            }
                            if (res < 0) 
                                break;

                            buf += res;
                            remain -=res;
                            
                            if ((remain == 0) || (res < msg.pipe_desc->MAX_PACKET_SIZE)) 
                            {
                            	res = remain;
                                break;
                            }
                        
                        } while (1);
                        break;

                    case TR_MSG_SEND:
                        buf = msg.pipe_tr->TX_BUFFER;
                        remain = msg.pipe_tr->TX_LENGTH;
                    
                        do {
                            res = _usb_khci_atom_tr(TR_OUT, usb_host_state_struct_ptr->speed, msg.pipe_desc, buf, remain);
                            time_delay(10);
                            
                            if (res < 0)
                                break;
                        
                            buf += res;
                            remain -=res;
                            
                            if ((remain == 0) || (res < msg.pipe_desc->MAX_PACKET_SIZE)) 
                            {
                            	res = remain;
                                break;
                            }
                            
                        } while (1);
                        break;

                    default:
                       break;
                }
        
                if (res < 0) {
                    if (msg.pipe_desc->PIPETYPE != USB_INTERRUPT_PIPE) {    /* TODO enhance condition for error (not NAK) in interrupt transaction */
                        _usb_khci_process_tr_complete((ptr)usb_host_state_struct_ptr, msg.pipe_desc, msg.pipe_tr, remain, res);
                    }
                    if (msg.pipe_desc->PIPETYPE == USB_INTERRUPT_PIPE) {
                                i = 10;
                            }
                    }
                else {
                    
                    if (msg.pipe_desc->PIPETYPE == USB_INTERRUPT_PIPE) {
                        i = 10;
                    }
                    
                    _usb_khci_process_tr_complete((ptr)usb_host_state_struct_ptr, msg.pipe_desc, msg.pipe_tr, remain, res);
        
                    if (msg.pipe_desc->PIPETYPE == USB_INTERRUPT_PIPE) {
                        _usb_khci_rm_int_tr(&msg);
                    }
                }
            }
        }
    
    if ((khci_event.VALUE & KHCI_EVENT_MASK)) {
        /* some event occurred */
        
        if (khci_event.VALUE & KHCI_EVENT_ATTACH) {     /* device attached */
        	USB0_ADDR = 0;

        	time_delay(100);

        	/* speed check, set */
        	usb_host_state_struct_ptr->speed = (uint_8)((USB0_CTL & (0x80)) ? USB_SPEED_FULL : USB_SPEED_LOW);

        	if (usb_host_state_struct_ptr->speed == USB_SPEED_FULL) {
        		USB0_ADDR |= USB_ENDPT_HOSTWOHUB_MASK;  
        	}

        	USB0_ISTAT = 0xff;   /* clean each int flags */
        	USB0_INTEN = USB_INTEN_TOKDNEEN_MASK | USB_INTEN_USBRSTEN_MASK;
        	USB0_ENDPT0 |= USB_ENDPT_HOSTWOHUB_MASK;

        	/* bus reset */
        	USB0_CTL |= USB_CTL_RESET_MASK;
        	time_delay(10);
        	USB0_CTL &= ~USB_CTL_RESET_MASK;

        	/* enable SOF sending */
        	USB0_CTL |= USB_CTL_USBENSOFEN_MASK;

        	time_delay(100);

        	usb_dev_list_attach_device((ptr)usb_host_state_struct_ptr, (uint_8)(usb_host_state_struct_ptr->speed), 0, 0);
        	_usb_event_clear(&khci_event, KHCI_EVENT_ATTACH);
        }

        if (khci_event.VALUE & KHCI_EVENT_RESET) {   /* usb reset */
        	/* test for device attach flag */
        	USB0_ISTAT |= USB_ISTAT_ATTACH_MASK;

        	if (USB0_ISTAT & USB_ISTAT_ATTACH_MASK) {
        	    /* device attached */
        	}
        	else {
        	  /* device detached */
            usb_dev_list_detach_device((ptr)usb_host_state_struct_ptr, 0, 0);
            /* TODO: some deinitialization and clearing in KHCI
            **
        	  ** cleaning interrupt transaction queue from device is done
            ** by calling _usb_khci_host_close_pipe from upper layer
        		*/
        		/* enable interrupt for usb attach */
        	    USB0_ISTAT |= USB_ISTAT_ATTACH_MASK;
                    USB0_INTEN = USB_INTEN_ATTACHEN_MASK;
        	}
        	
            _usb_event_clear(&khci_event, KHCI_EVENT_RESET | KHCI_EVENT_TOK_DONE);
        }
    }
return;
}

USB_STATUS _usb_khci_send_setup(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr, PIPE_TR_STRUCT_PTR pipe_tr_ptr) {
	USB_STATUS status = USB_OK;
	TR_MSG_STRUCT msg;
	
	msg.type = TR_MSG_SETUP;
	msg.pipe_desc = pipe_desc_ptr;
	msg.pipe_tr = pipe_tr_ptr;
	

	//status = _usb_msgq_send(&msg);
	status = _usb_msgq_send((int*)&msg);

	return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : USB_ISR
*  Returned Value :  
*  Comments       : Service interrupt in usb hardware. 
*        
*END*------------------------------------------------------------------*/

void  USB_ISR(void){
    uchar status;
    
    #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    
     if(usb_int_disable)
     {
       usb_int_en_copy = INT_ENB;
       INT_ENB = 0;
       return;
     }
     
    #endif
    
    while (1)
    {
        status = (uint_8)(USB0_ISTAT & USB0_INTEN);
        
        if (!status)
            break;
        
        USB0_ISTAT = status;
    
        if (status & USB_ISTAT_ATTACH_MASK)
        {
            /* usb device attached */
            USB0_INTEN &= ~USB_INTEN_ATTACHEN_MASK;
            _usb_event_set(&khci_event, KHCI_EVENT_ATTACH);
        }
        
        if (status & USB_ISTAT_TOKDNE_MASK)
        {
            /* atom transaction done - token done */
            _usb_event_set(&khci_event, KHCI_EVENT_TOK_DONE);
        }
        
        if (status & USB_ISTAT_USBRST_MASK)
        {
            /* usb reset */
            _usb_event_set(&khci_event, KHCI_EVENT_RESET);
        }
    };
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_host_open_pipe
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to open pipe
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_host_open_pipe(_usb_host_handle handle, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr) {
    uint_32 *ep = ((uint_32 *)&USB0_ENDPT0) + pipe_desc_ptr->ENDPOINT_NUMBER;

    switch (pipe_desc_ptr->PIPETYPE) {
    case USB_CONTROL_PIPE:
        *(uint_8*)ep |= USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK;
        pipe_desc_ptr->ep_ptr = ep;
        break;
    case USB_BULK_PIPE:
        break;
    case USB_INTERRUPT_PIPE:
        *(uint_8*)ep = USB_ENDPT_EPRXEN_MASK;
        pipe_desc_ptr->ep_ptr = ep;
        break;
    case USB_ISOCHRONOUS_PIPE:
        break;
    default:
        return USBERR_OPEN_PIPE_FAILED;
    }
    
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_host_close_pipe
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to close pipe
*END*-----------------------------------------------------------------*/

//USB_STATUS _usb_khci_host_close_pipe(_usb_host_handle , PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr) {
USB_STATUS _usb_khci_host_close_pipe(_usb_host_handle a, PIPE_DESCRIPTOR_STRUCT_PTR pipe_desc_ptr) {

    if (pipe_desc_ptr->PIPETYPE == USB_INTERRUPT_PIPE) {
        uint_16 i;
        TR_INT_QUE_ITM_STRUCT_PTR tr = TR_INT_QUE;

        /* find record */
        for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++) {
            if (tr->msg.pipe_desc == pipe_desc_ptr)
                tr->msg.type = TR_MSG_UNKNOWN;    
        }
    }
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_get_hot_int_tr
*  Returned Value : 0 successful
*  Comments       :
*        Make a message copy for transaction which need evaluation
*END*-----------------------------------------------------------------*/
static int _usb_khci_get_hot_int_tr(TR_MSG_STRUCT *msg) {
    uint_32 i, res = -1;     
    TR_INT_QUE_ITM_STRUCT_PTR tr = TR_INT_QUE;
    TR_INT_QUE_ITM_STRUCT_PTR hot_tr = NULL;     

    /* find hottest transaction */
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++) {
      if (tr->msg.type != TR_MSG_UNKNOWN){
        hot_tr = tr;
      }
    }
    
   
    if (hot_tr) {
        /* test if hottest transaction was not the last one with timeout - if yes, don't allow to block USB transfers with timeouts */
      if (last_to_pipe == hot_tr->msg.pipe_desc) {
    	  last_to_pipe = NULL; /* it is allowed to perform this interupt next time */
            return res;
        }
    
      memcpy(msg, &hot_tr->msg,  sizeof(TR_MSG_STRUCT));
      res = 0;
    }
    
    return res;
}

