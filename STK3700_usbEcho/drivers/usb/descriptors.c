/***************************************************************************//**
 * @file descriptors.c
 * @brief USB descriptors   modified by Lukasz Uszko
 * @version 3.20.5
 *******************************************************************************/


#include "em_common.h"
#include "cdc.h"
#include "usbconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int  SetupCmd(const USB_Setup_TypeDef *setup);
extern void StateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);

EFM32_ALIGN(4)
static const USB_DeviceDescriptor_TypeDef deviceDesc __attribute__ ((aligned(4)))=
{
  .bLength            = USB_DEVICE_DESCSIZE,
  .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
  .bcdUSB             = 0x0200,
  .bDeviceClass       = USB_CLASS_CDC,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize0    = USB_EP0_SIZE,
  .idVendor           = 0x2544,
  .idProduct          = 0x0003,
  .bcdDevice          = 0x0000,
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 0,
  .bNumConfigurations = 1
};

#define CONFIG_DESCSIZE ( USB_CONFIG_DESCSIZE                   + \
                          (USB_INTERFACE_DESCSIZE * 2)          + \
                          (USB_ENDPOINT_DESCSIZE * NUM_EP_USED) + \
                          USB_CDC_HEADER_FND_DESCSIZE           + \
                          USB_CDC_CALLMNG_FND_DESCSIZE          + \
                          USB_CDC_ACM_FND_DESCSIZE              + \
                          5 )

EFM32_ALIGN(4)
static const uint8_t configDesc[] __attribute__ ((aligned(4)))=
{
  /*** Configuration descriptor ***/
  USB_CONFIG_DESCSIZE,    /* bLength                                   */
  USB_CONFIG_DESCRIPTOR,  /* bDescriptorType                           */
  CONFIG_DESCSIZE,        /* wTotalLength (LSB)                        */
  USB_CONFIG_DESCSIZE>>8, /* wTotalLength (MSB)                        */
  2,                      /* bNumInterfaces                            */
  1,                      /* bConfigurationValue                       */
  0,                      /* iConfiguration                            */
  CONFIG_DESC_BM_RESERVED_D7 |   /* bmAttrib: Self powered             */
  CONFIG_DESC_BM_SELFPOWERED,
  CONFIG_DESC_MAXPOWER_mA( 100 ),/* bMaxPower: 100 mA                  */

  /*** Communication Class Interface descriptor (interface no. 0)    ***/
  USB_INTERFACE_DESCSIZE, /* bLength               */
  USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
  CDC_CTRL_INTERFACE_NO,  /* bInterfaceNumber      */
  0,                      /* bAlternateSetting     */
  1,                      /* bNumEndpoints         */
  USB_CLASS_CDC,          /* bInterfaceClass       */
  USB_CLASS_CDC_ACM,      /* bInterfaceSubClass    */
  0,                      /* bInterfaceProtocol    */
  0,                      /* iInterface            */

  /*** CDC Header Functional descriptor ***/
  USB_CDC_HEADER_FND_DESCSIZE, /* bFunctionLength  */
  USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
  USB_CLASS_CDC_HFN,      /* bDescriptorSubtype    */
  0x20,                   /* bcdCDC spec.no LSB    */
  0x01,                   /* bcdCDC spec.no MSB    */

  /*** CDC Call Management Functional descriptor ***/
  USB_CDC_CALLMNG_FND_DESCSIZE, /* bFunctionLength */
  USB_CS_INTERFACE_DESCRIPTOR,  /* bDescriptorType */
  USB_CLASS_CDC_CMNGFN,   /* bDescriptorSubtype    */
  0,                      /* bmCapabilities        */
  1,                      /* bDataInterface        */

  /*** CDC Abstract Control Management Functional descriptor ***/
  USB_CDC_ACM_FND_DESCSIZE, /* bFunctionLength     */
  USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
  USB_CLASS_CDC_ACMFN,    /* bDescriptorSubtype    */
  0x02,                   /* bmCapabilities        */
  /* The capabilities that this configuration supports:                   */
  /* D7..D4: RESERVED (Reset to zero)                                     */
  /* D3: 1 - Device supports the notification Network_Connection.         */
  /* D2: 1 - Device supports the request Send_Break                       */
  /* D1: 1 - Device supports the request combination of Set_Line_Coding,  */
  /*         Set_Control_Line_State, Get_Line_Coding, and the             */
  /*         notification Serial_State.                                   */
  /* D0: 1 - Device supports the request combination of Set_Comm_Feature, */
  /*         Clear_Comm_Feature, and Get_Comm_Feature.                    */

  /*** CDC Union Functional descriptor ***/
  5,                      /* bFunctionLength       */
  USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
  USB_CLASS_CDC_UNIONFN,  /* bDescriptorSubtype    */
  0,                      /* bControlInterface,      itf. no. 0 */
  1,                      /* bSubordinateInterface0, itf. no. 1 */

  /*** CDC Notification endpoint descriptor ***/
  USB_ENDPOINT_DESCSIZE,  /* bLength               */
  USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
  CDC_EP_NOTIFY,              /* bEndpointAddress (IN) */
  USB_EPTYPE_INTR,        /* bmAttributes          */
  CDC_BULK_EP_SIZE,           /* wMaxPacketSize (LSB)  */
  0,                      /* wMaxPacketSize (MSB)  */
  0xFF,                   /* bInterval             */

  /*** Data Class Interface descriptor (interface no. 1)                ***/
  USB_INTERFACE_DESCSIZE, /* bLength               */
  USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
  CDC_DATA_INTERFACE_NO,   /* bInterfaceNumber      */
  0,                      /* bAlternateSetting     */
  2,                      /* bNumEndpoints         */
  USB_CLASS_CDC_DATA,     /* bInterfaceClass       */
  0,                      /* bInterfaceSubClass    */
  0,                      /* bInterfaceProtocol    */
  0,                      /* iInterface            */

  /*** CDC Data interface endpoint descriptors ***/
  USB_ENDPOINT_DESCSIZE,  /* bLength               */
  USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
  CDC_EP_DATA_IN,             /* bEndpointAddress (IN) */
  USB_EPTYPE_BULK,        /* bmAttributes          */
  CDC_BULK_EP_SIZE,           /* wMaxPacketSize (LSB)  */
  0,                      /* wMaxPacketSize (MSB)  */
  0,                      /* bInterval             */

  USB_ENDPOINT_DESCSIZE,  /* bLength               */
  USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
  CDC_EP_DATA_OUT,            /* bEndpointAddress (OUT)*/
  USB_EPTYPE_BULK,        /* bmAttributes          */
  CDC_BULK_EP_SIZE,           /* wMaxPacketSize (LSB)  */
  0,                      /* wMaxPacketSize (MSB)  */
  0                       /* bInterval             */
};

STATIC_CONST_STRING_DESC_LANGID( langID, 0x04, 0x09 );
/*
 *
 *
 *
 *
 */

STATIC_CONST_STRING_DESC( iManufacturer, 'L','u','k','t','e','c','h',' ','I','n','c','.' );
STATIC_CONST_STRING_DESC( iProduct     , 'S','i','g','n','a','l',' ','G','e', \
                                        'n','e','r','a','t','o','r',' ', );
STATIC_CONST_STRING_DESC( iSerialNumber, '0','0','0','0','1','2',             \
                                         '3','4','5','6','7','8' );

static const void * const strings[] =


{
  &langID,
  &iManufacturer,
  &iProduct,
  //&iSerialNumber
};

/* Endpoint buffer sizes */
/* 1 = single buffer, 2 = double buffering, 3 = triple buffering ...  */
/* Use double buffering on the BULK endpoints.                        */
static uint8_t bufferingMultiplier[ NUM_EP_USED  + 1 ] = { 1, 1, 2, 2 };

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = CDC_StateChangeEvent,
  .setupCmd        = CDC_SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

const USBD_Init_TypeDef initstruct =
{
  .deviceDescriptor    = &deviceDesc,
  .configDescriptor    = configDesc,
  .stringDescriptors   = strings,
  .numberOfStrings     = sizeof(strings)/sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = bufferingMultiplier,
  .reserved            = 0
};

#ifdef __cplusplus
}
#endif
