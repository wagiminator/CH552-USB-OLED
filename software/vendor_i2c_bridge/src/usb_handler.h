// ===================================================================================
// USB Handler for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>
#include "usb_descr.h"

// ===================================================================================
// Endpoint Buffer
// ===================================================================================
__xdata __at (EP0_ADDR) uint8_t EP0_buffer[EP0_BUF_SIZE];     
__xdata __at (EP1_ADDR) uint8_t EP1_buffer[EP1_BUF_SIZE * 2];

#define USB_setupBuf ((PUSB_SETUP_REQ)EP0_buffer)
extern uint8_t SetupReq;

// ===================================================================================
// Custom External USB Handler Functions
// ===================================================================================
uint8_t VEN_control(void);
void VEN_setup(void);
void VEN_reset(void);
void VEN_EP1_IN(void);
void VEN_EP1_OUT(void);

// ===================================================================================
// USB Handler Defines
// ===================================================================================
// Custom USB handler functions
#define USB_INIT_handler    VEN_setup         // init custom endpoints
#define USB_RESET_handler   VEN_reset         // custom USB reset handler
#define USB_CTRL_NS_handler VEN_control       // handle custom non-standard requests

// Endpoint callback functions
#define EP0_SETUP_callback  USB_EP0_SETUP
#define EP0_IN_callback     USB_EP0_IN
#define EP0_OUT_callback    USB_EP0_OUT
#define EP1_IN_callback     VEN_EP1_IN
#define EP1_OUT_callback    VEN_EP1_OUT

// ===================================================================================
// Functions
// ===================================================================================
void USB_interrupt(void);
void USB_init(void);
