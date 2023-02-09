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
#define USB_PRODUCT_ID      0x05DC    // PID (shared vendor class with libusb)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA 

// USB string descriptors
#define MANUFACTURER_STR    'w','a','g','i','m','i','n','a','t','o','r'
#define PRODUCT_STR         'V','E','N','-','I','2','C','-','B','r','i','d','g','e'
#define SERIAL_STR          'C','H','5','5','x'
#define INTERFACE_STR       'V','e','n','d','o','r',' ','B','u','l','k'

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! EXPERIMENTAL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Windows Compatible ID (WCID) code for automated driver installation.
// Theoretically, no manual installation of a driver for Windows OS is necessary.
// However, since I (un)fortunately do not have a Windows system, this function 
// is untested. The vendor code should not be identical to other vendor codes used,
// the device must be declared as USB 2.0 and the maximum packet size of the EP0 
// must be 64 bytes. Uncomment this define to enable experimental WCID feature.
#define WCID_VENDOR_CODE    0x37
