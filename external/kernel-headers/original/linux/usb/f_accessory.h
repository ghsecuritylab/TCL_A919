
#ifndef __LINUX_USB_F_ACCESSORY_H
#define __LINUX_USB_F_ACCESSORY_H

/* Use Google Vendor ID when in accessory mode */
#define USB_ACCESSORY_VENDOR_ID 0x18D1


/* Product ID to use when in accessory mode */
#define USB_ACCESSORY_PRODUCT_ID 0x2D00

/* Product ID to use when in accessory mode and adb is enabled */
#define USB_ACCESSORY_ADB_PRODUCT_ID 0x2D01

/* Indexes for strings sent by the host via ACCESSORY_SEND_STRING */
#define ACCESSORY_STRING_MANUFACTURER   0
#define ACCESSORY_STRING_MODEL          1
#define ACCESSORY_STRING_DESCRIPTION    2
#define ACCESSORY_STRING_VERSION        3
#define ACCESSORY_STRING_URI            4
#define ACCESSORY_STRING_SERIAL         5

#define ACCESSORY_GET_PROTOCOL  51

#define ACCESSORY_SEND_STRING   52

#define ACCESSORY_START         53

/* ioctls for retrieving strings set by the host */
#define ACCESSORY_GET_STRING_MANUFACTURER   _IOW('M', 1, char[256])
#define ACCESSORY_GET_STRING_MODEL          _IOW('M', 2, char[256])
#define ACCESSORY_GET_STRING_DESCRIPTION    _IOW('M', 3, char[256])
#define ACCESSORY_GET_STRING_VERSION        _IOW('M', 4, char[256])
#define ACCESSORY_GET_STRING_URI            _IOW('M', 5, char[256])
#define ACCESSORY_GET_STRING_SERIAL         _IOW('M', 6, char[256])

#endif /* __LINUX_USB_F_ACCESSORY_H */
