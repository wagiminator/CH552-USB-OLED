// ===================================================================================
// USB Descriptors and Definitions
// ===================================================================================

#pragma once
#include "usb.h"

// ===================================================================================
// USB Endpoint Addresses and Sizes
// ===================================================================================
#define EP0_SIZE    DEFAULT_ENDP0_SIZE
#define EP1_SIZE    DEFAULT_ENDP1_SIZE
#define EP2_SIZE    MAX_PACKET_SIZE

#define EP0_ADDR    0
#define EP1_ADDR    (EP0_ADDR + EP0_SIZE + 2)
#define EP2_ADDR    (EP1_ADDR + EP1_SIZE + 2)

// ===================================================================================
// Device and Configuration Descriptors
// ===================================================================================
extern __code USB_DEV_DESCR DevDescr;
extern __code USB_CFG_DESCR_CDC CfgDescr;

// ===================================================================================
// String Descriptors
// ===================================================================================
extern __code uint16_t LangDescr[];
extern __code uint16_t SerDescr[];
extern __code uint16_t ProdDescr[];
extern __code uint16_t CDC_Descr[];
extern __code uint16_t ManufDescr[];

#define USB_STR_DESCR_i0    (uint8_t*)LangDescr
#define USB_STR_DESCR_i1    (uint8_t*)ManufDescr
#define USB_STR_DESCR_i2    (uint8_t*)ProdDescr
#define USB_STR_DESCR_i3    (uint8_t*)SerDescr
#define USB_STR_DESCR_i4    (uint8_t*)CDC_Descr
#define USB_STR_DESCR_i5    (uint8_t*)SerDescr
#define USB_STR_DESCR_i6    (uint8_t*)SerDescr
#define USB_STR_DESCR_i7    (uint8_t*)SerDescr
#define USB_STR_DESCR_i8    (uint8_t*)SerDescr
#define USB_STR_DESCR_i9    (uint8_t*)SerDescr
#define USB_STR_DESCR_ix    (uint8_t*)SerDescr
