// ===================================================================================
// USB Handler for CH551, CH552 and CH554                                     * v1.5 *
// ===================================================================================

#pragma once
#include <stdint.h>
#include "ch554.h"
#include "usb_descr.h"

// ===================================================================================
// Variables
// ===================================================================================
#define USB_SetupBuf     ((PUSB_SETUP_REQ)EP0_buffer)
extern volatile uint8_t  USB_SetupReq, USB_SetupTyp;
extern volatile uint16_t USB_SetupLen;
extern volatile __bit    USB_ENUM_OK;
extern __code uint8_t*   USB_pDescr;

// ===================================================================================
// Custom External USB Handler Functions
// ===================================================================================
uint8_t VEN_control(void);
void VEN_setup(void);
void VEN_EP0_IN(void);
void VEN_EP1_IN(void);
void VEN_EP1_OUT(void);

// ===================================================================================
// USB Handler Defines
// ===================================================================================
// Custom USB handler functions
#define USB_INIT_endpoints        VEN_setup       // init custom endpoints
#define USB_VENDOR_SETUP_handler  VEN_control     // handle vendor setup requests
#define USB_VENDOR_IN_handler     VEN_EP0_IN      // handle vendor in transfers

// Endpoint callback functions
#define EP0_SETUP_callback        USB_EP0_SETUP
#define EP0_IN_callback           USB_EP0_IN
#define EP0_OUT_callback          USB_EP0_OUT
#define EP1_IN_callback           VEN_EP1_IN
#define EP1_OUT_callback          VEN_EP1_OUT

// ===================================================================================
// Functions
// ===================================================================================
void USB_init(void);
void USB_interrupt(void);
void USB_EP0_copyDescr(uint8_t len);
