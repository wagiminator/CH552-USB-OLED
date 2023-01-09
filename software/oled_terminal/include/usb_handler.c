// ===================================================================================
// USB Handler
// ===================================================================================

#include "ch554.h"
#include "usb_handler.h"

//CDC functions:
void CDC_reset(void);
void CDC_setLineCoding(void);
uint8_t CDC_getLineCoding(void);
void CDC_setControlLineState(void);
void USB_EP2_IN(void);
void USB_EP2_OUT(void);

__xdata __at (EP0_ADDR) uint8_t Ep0Buffer[EP0_SIZE];     
__xdata __at (EP1_ADDR) uint8_t Ep1Buffer[EP1_SIZE];
__xdata __at (EP2_ADDR) uint8_t Ep2Buffer[2*EP2_SIZE];

__code uint8_t *pDescr;

uint16_t SetupLen;
uint8_t  SetupReq, UsbConfig;

inline void NOP_Process(void) {}

// Copy descriptor *pDescr to Ep0
// (Thanks to Ralph Doncaster)
#pragma callee_saves USB_EP0_copyDescr
void USB_EP0_copyDescr(uint8_t len) {
  len;                          // stop unreferenced argument warning
  __asm
    push ar7                    ; r7 -> stack
    mov  r7, dpl                ; r7 <- len
    inc  _XBUS_AUX              ; select dptr1
    mov  dptr, #_Ep0Buffer      ; dptr1 <- Ep0Buffer
    dec  _XBUS_AUX              ; select dptr0
    mov  dpl, _pDescr           ; dptr0 <- *pDescr
    mov  dph, (_pDescr + 1)
    01$:
    clr  a                      ; acc <- #0
    movc a, @a+dptr             ; acc <- *pDescr[dptr0]
    inc  dptr                   ; inc dptr0
    .DB  0xA5                   ; acc -> Ep0Buffer[dptr1] & inc dptr1
    djnz r7, 01$                ; dec r7 & if r7 not zero jump to 01$
    pop  ar7                    ; r7 <- stack
  __endasm;
}

void USB_EP0_SETUP(void) {
  uint8_t len = USB_RX_LEN;
  if(len == (sizeof(USB_SETUP_REQ))) {
    SetupLen = ((uint16_t)UsbSetupBuf->wLengthH<<8) | (UsbSetupBuf->wLengthL);
    len = 0;                                // default is success and upload 0 length
    SetupReq = UsbSetupBuf->bRequest;
    if( (UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD ) {
      switch(UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) {
        case USB_REQ_TYP_VENDOR:
          switch(SetupReq) {
            default:
              len = 0xFF;                   // command not supported
              break;
          }
          break;
        case USB_REQ_TYP_CLASS:
          switch(SetupReq) {
            case GET_LINE_CODING:           // 0x21  currently configured
              len = CDC_getLineCoding();
              break;
            case SET_CONTROL_LINE_STATE:    // 0x22  generates RS-232/V.24 style control signals
              CDC_setControlLineState();
              break;
            case SET_LINE_CODING:           // 0x20  Configure
              break;            
            default:
              len = 0xFF;                   // command not supported
              break;
          }
          break;
        default:
          len = 0xFF;                       // command not supported
          break;
      }
    }
    else {                                  // standard request
      switch(SetupReq) {                    // request ccfType
        case USB_GET_DESCRIPTOR:
          switch(UsbSetupBuf->wValueH) {
            case USB_DESCR_TYP_DEVICE:      // Device Descriptor
              pDescr = (uint8_t*)&DevDescr; // put descriptor into out buffer
              len = sizeof(DevDescr);       // descriptor length
              break;
            case USB_DESCR_TYP_CONFIG:      // Configuration Descriptor
              pDescr = (uint8_t*)&CfgDescr; // put descriptor into out buffer
              len = sizeof(CfgDescr);       // descriptor length
              break;
            case USB_DESCR_TYP_STRING:
              switch(UsbSetupBuf->wValueL) {  // String Descriptor Index
                case 0:   pDescr = USB_STR_DESCR_i0; break;
                case 1:   pDescr = USB_STR_DESCR_i1; break;
                case 2:   pDescr = USB_STR_DESCR_i2; break;
                case 3:   pDescr = USB_STR_DESCR_i3; break;
                case 4:   pDescr = USB_STR_DESCR_i4; break;
                default:  pDescr = USB_STR_DESCR_ix; break;
              }
              len = pDescr[0];              // descriptor length
              break;
            default:
              len = 0xff;                   // unsupported descriptors or error
              break;
          }
          if(len != 0xff) {
            if(SetupLen > len) SetupLen = len;  // limit length
            len = SetupLen >= EP0_SIZE ? EP0_SIZE : SetupLen;
            USB_EP0_copyDescr(len);              // copy descriptor to Ep0
            SetupLen -= len;
            pDescr += len;
          }
          break;
        case USB_SET_ADDRESS:
          SetupLen = UsbSetupBuf->wValueL;  // save the assigned address
          break;
        case USB_GET_CONFIGURATION:
          Ep0Buffer[0] = UsbConfig;
          if (SetupLen >= 1) len = 1;
          break;
        case USB_SET_CONFIGURATION:
          UsbConfig = UsbSetupBuf->wValueL;
          break;
        case USB_GET_INTERFACE:
          break;
        case USB_SET_INTERFACE:
          break;
        case USB_CLEAR_FEATURE:             // Clear Feature
          if( (UsbSetupBuf->bRequestType & 0x1F) == USB_REQ_RECIP_DEVICE ) { // Clear the device featuee.
            if( ( ( (uint16_t)UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 ) {
              if( ((uint8_t*)&CfgDescr)[7] & 0x20) {
                // wake up
              }
              else len = 0xFF;               // failed
            }
            else len = 0xFF;                 // failed
          }
          else if( (UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP ) { // endpoint
            switch(UsbSetupBuf->wIndexL) {
              case 0x84:
                UEP4_CTRL = UEP4_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                break;
              case 0x04:
                UEP4_CTRL = UEP4_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                break;
              case 0x83:
                UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                break;
              case 0x03:
                UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                break;
              case 0x82:
                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                break;
              case 0x02:
                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                break;
              case 0x81:
                UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                break;
              case 0x01:
                UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                break;
              default:
                len = 0xFF;                 // unsupported endpoint
                break;
            }
          }
          else len = 0xFF;                  // unsupported for non-endpoint
          break;
        case USB_SET_FEATURE:               // Set Feature
          if( (UsbSetupBuf->bRequestType & 0x1F) == USB_REQ_RECIP_DEVICE ) { // set the device feature.
            if( ( ( (uint16_t)UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 ) {
              if( !(((uint8_t*)&CfgDescr)[7] & 0x20) ) len = 0xFF;  // failed
            }
            else len = 0xFF;                // failed
          }
          else if( (UsbSetupBuf->bRequestType & 0x1F) == USB_REQ_RECIP_ENDP ) { //endpoint
            if( ( ( (uint16_t)UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 ) {
              switch( ( (uint16_t)UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL ) {
                case 0x84:
                  UEP4_CTRL = UEP4_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// Set endpoint4 IN STALL 
                  break;
                case 0x04:
                  UEP4_CTRL = UEP4_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;// Set endpoint4 OUT Stall 
                  break;
                case 0x83:
                  UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// Set endpoint3 IN STALL 
                  break;
                case 0x03:
                  UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;// Set endpoint3 OUT Stall 
                  break;
                case 0x82:
                  UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// Set endpoint2 IN STALL 
                  break;
                case 0x02:
                  UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;// Set endpoint2 OUT Stall 
                  break;
                case 0x81:
                  UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// Set endpoint1 IN STALL 
                  break;
                case 0x01:
                  UEP1_CTRL = UEP1_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;// Set endpoint1 OUT Stall 
                default:
                  len = 0xFF;               // failed
                  break;
              }
            }
            else len = 0xFF;                // failed
          }
          else len = 0xFF;                  // failed
          break;
        case USB_GET_STATUS:
          Ep0Buffer[0] = 0x00;
          Ep0Buffer[1] = 0x00;
          if(SetupLen >= 2) len = 2;
          else len = SetupLen;
          break;
        default:
          len = 0xff;                       // failed
          break;
      }
    }
  }
  else len = 0xff;                          // wrong packet length

  if(len == 0xff) {
    SetupReq = 0xFF;
    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
  }
  else if(len <= EP0_SIZE) {      // Tx data to host or send 0-length packet
    UEP0_T_LEN = len;
    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// Expect DATA1, Answer ACK
  }
  else {
    UEP0_T_LEN = 0;  // Tx data to host or send 0-length packet
    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// Expect DATA1, Answer ACK
  }
}

void USB_EP0_IN(void) {
  uint8_t len;
  switch(SetupReq) {
    case USB_GET_DESCRIPTOR:
      len = SetupLen >= EP0_SIZE ? EP0_SIZE : SetupLen;
      USB_EP0_copyDescr(len);                    // copy descriptor to Ep0                                
      SetupLen -= len;
      pDescr += len;
      UEP0_T_LEN = len;
      UEP0_CTRL ^= bUEP_T_TOG;              // switch between DATA0 and DATA1
      break;
    case USB_SET_ADDRESS:
      USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
      UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
      break;
    default:
      UEP0_T_LEN = 0;                       // end of transaction
      UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
      break;
  }
}

void USB_EP0_OUT(void) {
  if(SetupReq == SET_LINE_CODING) {         // set line coding
    if( U_TOG_OK ) {
      CDC_setLineCoding();
      UEP0_T_LEN = 0;
      UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_ACK;  // send 0-length packet
    }
  }
  else {
    UEP0_T_LEN = 0;
    UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_NAK;  // respond Nak
  }
}

void USB_EP1_IN(void) {
  UEP1_T_LEN = 0;
  UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // default NAK
}


#pragma save
#pragma nooverlay
void USB_interrupt(void) {   // inline not really working in multiple files in SDCC
  if(UIF_TRANSFER) {
    // Dispatch to service functions
    uint8_t callIndex = USB_INT_ST & MASK_UIS_ENDP;
    switch (USB_INT_ST & MASK_UIS_TOKEN) {
      case UIS_TOKEN_OUT:
        // SDCC will take IRAM if array of function pointer is used.
        switch (callIndex) {
          case 0: EP0_OUT_Callback(); break;
          case 1: EP1_OUT_Callback(); break;
          case 2: EP2_OUT_Callback(); break;
          case 3: EP3_OUT_Callback(); break;
          case 4: EP4_OUT_Callback(); break;
          default: break;
        }
        break;
      case UIS_TOKEN_SOF:
        // SDCC will take IRAM if array of function pointer is used.
        switch (callIndex) {
          case 0: EP0_SOF_Callback(); break;
          case 1: EP1_SOF_Callback(); break;
          case 2: EP2_SOF_Callback(); break;
          case 3: EP3_SOF_Callback(); break;
          case 4: EP4_SOF_Callback(); break;
          default: break;
        }
        break;
      case UIS_TOKEN_IN:
        // SDCC will take IRAM if array of function pointer is used.
        switch (callIndex) {
          case 0: EP0_IN_Callback(); break;
          case 1: EP1_IN_Callback(); break;
          case 2: EP2_IN_Callback(); break;
          case 3: EP3_IN_Callback(); break;
          case 4: EP4_IN_Callback(); break;
          default: break;
        }
        break;
      case UIS_TOKEN_SETUP:
        // SDCC will take IRAM if array of function pointer is used.
        switch (callIndex) {
          case 0: EP0_SETUP_Callback(); break;
          case 1: EP1_SETUP_Callback(); break;
          case 2: EP2_SETUP_Callback(); break;
          case 3: EP3_SETUP_Callback(); break;
          case 4: EP4_SETUP_Callback(); break;
          default: break;
        }
        break;
    }
    UIF_TRANSFER = 0;                       // clear interrupt flag
  }
    
  // Device mode USB bus reset
  if(UIF_BUS_RST) {
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;

    USB_DEV_AD = 0x00;
    UIF_SUSPEND = 0;
    UIF_TRANSFER = 0;
    UIF_BUS_RST = 0;                        // clear interrupt flag

    CDC_reset();
  }
    
  // USB bus suspend / wake up
  if (UIF_SUSPEND) {
    UIF_SUSPEND = 0;
    if ( !(USB_MIS_ST & bUMS_SUSPEND) ) USB_INT_FG = 0xFF;  // clear interrupt flag
  }
}
#pragma restore

void USB_init(void) {
  USB_CTRL    = bUC_DEV_PU_EN               // USB internal pull-up enable
              | bUC_INT_BUSY                // Return NAK if USB INT flag not clear
              | bUC_DMA_EN;                 // DMA enable
  UDEV_CTRL   = bUD_PD_DIS                  // Disable UDP/UDM pulldown resistor
              | bUD_PORT_EN;                // Enable port, full-speed
  USB_INT_EN |= bUIE_SUSPEND                // Enable device hang interrupt
              | bUIE_TRANSFER               // Enable USB transfer completion interrupt
              | bUIE_BUS_RST;               // Enable device mode USB bus reset interrupt

  USB_INT_FG |= 0x1F;                       // Clear interrupt flag
  IE_USB      = 1;                          // Enable USB interrupt
  EA          = 1;                          // Enable global interrupts

  UEP0_DMA    = (uint16_t)Ep0Buffer;        // EP0 data transfer address
  UEP1_DMA    = (uint16_t)Ep1Buffer;        // EP1 data transfer address
  UEP2_DMA    = (uint16_t)Ep2Buffer;        // EP2 data transfer address

  UEP0_CTRL   = UEP_R_RES_ACK               // EP0 Manual flip, OUT transaction returns ACK
              | UEP_T_RES_NAK;              // EP0 IN transaction returns NAK
  UEP1_CTRL   = bUEP_AUTO_TOG               // EP1 Auto flip sync flag
              | UEP_T_RES_NAK;              // EP1 IN transaction returns NAK
  UEP2_CTRL   = bUEP_AUTO_TOG               // EP2 Auto flip sync flag
              | UEP_T_RES_NAK               // EP2 IN transaction returns NAK
              | UEP_R_RES_ACK;              // EP2 OUT transaction returns ACK

  UEP2_3_MOD = bUEP2_RX_EN | bUEP2_TX_EN;   // EP2 double buffer (0x0C)
  UEP4_1_MOD = bUEP1_TX_EN;                 // EP1 TX enable (0x40)
}
