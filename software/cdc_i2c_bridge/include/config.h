// ===================================================================================
// User configurations
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_BUZZER          P15       // buzzer pin
#define PIN_SDA             P16       // I2C SDA
#define PIN_SCL             P17       // I2C SCL

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DD    // PID (shared CDC)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA 

// USB descriptor strings
#define MANUFACTURER_STR    'w','a','g','i','m','i','n','a','t','o','r'
#define PRODUCT_STR         'I','2','C','-','B','r','i','d','g','e'
#define SERIAL_STR          'g','i','t','h','u','b','.','c','o','m','/', \
                            'w','a','g','i','m','i','n','a','t','o','r'
#define INTERFACE_STR       'C','D','C',' ','S','e','r','i','a','l'
