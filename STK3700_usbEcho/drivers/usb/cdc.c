/**************************************************************************//**
 * @file cdc.c
 * @brief USB CDC (Communication Device Class) driver. Lukasz Uszko
 * @version 3.20.5
 ******************************************************************************/

#include "em_device.h"
#include "em_common.h"
#include "em_cmu.h"
#include "emusb/em_usb.h"
#include "bsp.h"
#include "cdc.h"

/**************************************************************************//**

 * Use the file LuktechCDC.inf to install a USB serial port device driver
 * on the host PC.
 *
 *
 *****************************************************************************/

/*** Typedef's and defines. ***/

#define CDC_USB_RX_BUF_SIZE  CDC_BULK_EP_SIZE /* Packet size when receiving on USB */
#define CDC_USB_TX_BUF_SIZE  127    /* Packet size when transmitting on USB.  */

/* Calculate a timeout in ms corresponding to 5 char times on current     */
/* baudrate. Minimum timeout is set to 10 ms.                             */
#define CDC_RX_TIMEOUT    EFM32_MAX(10U, 50000 / (cdcLineCoding.dwDTERate))

/* The serial port LINE CODING data structure, used to carry information  */
/* about serial port baudrate, parity etc. between host and device.       */
EFM32_PACK_START(1)typedef struct {
	uint32_t dwDTERate; /** Baudrate                            */
	uint8_t bCharFormat; /** Stop bits, 0=1 1=1.5 2=2            */
	uint8_t bParityType; /** 0=None 1=Odd 2=Even 3=Mark 4=Space  */
	uint8_t bDataBits; /** 5, 6, 7, 8 or 16                    */
	uint8_t dummy; /** To ensure size is a multiple of 4 bytes.*/
}__attribute__ ((packed)) cdcLineCoding_TypeDef;
EFM32_PACK_END()

/*** Function prototypes. ***/

static int usbDataReceived(USB_Status_TypeDef status, uint32_t xferred,
		uint32_t remaining);
static int LineCodingReceived(USB_Status_TypeDef status, uint32_t xferred,
		uint32_t remaining);

/*** Variables ***/

/*
 * The LineCoding variable must be 4-byte aligned as it is used as USB
 * transmit and receive buffer
 */
EFM32_ALIGN(4)EFM32_PACK_START(1)static cdcLineCoding_TypeDef __attribute__ ((aligned(4))) cdcLineCoding =
		{ 115200, 0, 0, 8, 0 };
EFM32_PACK_END()

STATIC_UBUF(usbTxBuffer, CDC_USB_RX_BUF_SIZE); /* USB  buffers.   */
STATIC_UBUF(usbRxBuffer, CDC_USB_RX_BUF_SIZE);

static volatile bool usbRxTransferActive = false;
static volatile bool usbTxTransferActive = false;
static bool usbCdcConfigured= false;

static uint32_t usbBytesReceived;

/**************************************************************************//**
 * @brief CDC device initialization.
 * waits until CDC Class is configured correctly.
 *****************************************************************************/
extern const USBD_Init_TypeDef initstruct; //defined in descriptors.c

bool CDC_Init(void) {
	if (USBD_Init(&initstruct) == USB_STATUS_OK) { /* Start USB. */
		int msElapsed =0;
	    while (msElapsed<100&&!usbCdcConfigured){ //wait 10s
	    	USBTIMER_DelayMs( 100 );
	    	msElapsed++;
	    }
	}
    if(usbCdcConfigured)return true;
    return false;
}

/**************************************************************************//**
 * @brief
 *   Handle USB setup commands. Implements CDC class specific commands.
 *
 * @param[in] setup Pointer to the setup packet received.
 *
 * @return USB_STATUS_OK if command accepted.
 *         USB_STATUS_REQ_UNHANDLED when command is unknown, the USB device
 *         stack will handle the request.
 *****************************************************************************/
int CDC_SetupCmd(const USB_Setup_TypeDef *setup) {
	int retVal = USB_STATUS_REQ_UNHANDLED;

	if ((setup->Type == USB_SETUP_TYPE_CLASS)
			&& (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)) {
		switch (setup->bRequest) {
		case USB_CDC_GETLINECODING:
			/********************/
			if ((setup->wValue == 0)
					&& (setup->wIndex == CDC_CTRL_INTERFACE_NO)&& /* Interface no. */
					( setup->wLength == 7 ) && /* Length of cdcLineCoding */
					( setup->Direction == USB_SETUP_DIR_IN ) ){
					/* Send current settings to USB host. */
					USBD_Write(0, (void*) &cdcLineCoding, 7, NULL);
					retVal = USB_STATUS_OK;
				}
				break;

				case USB_CDC_SETLINECODING:
				/********************/
				if ( ( setup->wValue == 0 ) &&
						( setup->wIndex == CDC_CTRL_INTERFACE_NO ) && /* Interface no. */
						( setup->wLength == 7 ) && /* Length of cdcLineCoding */
						( setup->Direction != USB_SETUP_DIR_IN ) )
				{
					/* Get new settings from USB host. */
					USBD_Read(0, (void*) &cdcLineCoding, 7, LineCodingReceived);
					retVal = USB_STATUS_OK;
				}
				break;

				case USB_CDC_SETCTRLLINESTATE:
				/********************/
				if ( ( setup->wIndex == CDC_CTRL_INTERFACE_NO ) && /* Interface no.  */
						( setup->wLength == 0 ) ) /* No data        */
				{
					/* Do nothing ( Non compliant behaviour !! ) */
					retVal = USB_STATUS_OK;
				}
				break;
			}
		}

	return retVal;
}

/**************************************************************************//**
 * @brief
 *   Callback function called each time the USB device state is changed.
 *   Starts CDC operation when device has been configured by USB host.
 *
 * @param[in] oldState The device state the device has just left.
 * @param[in] newState The new device state.
 *****************************************************************************/
void CDC_StateChangeEvent(USBD_State_TypeDef oldState,
		USBD_State_TypeDef newState) {
	if (newState == USBD_STATE_CONFIGURED) {
		/* We have been configured, start CDC functionality ! */

		if (oldState == USBD_STATE_SUSPENDED) /* Resume ?   */
		{
		}
		usbCdcConfigured=true;

	}

	else if ((oldState == USBD_STATE_CONFIGURED)
			&& (newState != USBD_STATE_SUSPENDED)) {
		/* We have been de-configured, stop CDC functionality */
		//USBTIMER_Stop(CDC_TIMER_ID);
		usbCdcConfigured=false;
	}

	else if (newState == USBD_STATE_SUSPENDED) {
		/* We have been suspended, stop CDC functionality */
		/* Reduce current consumption to below 2.5 mA.    */
		//USBTIMER_Stop(CDC_TIMER_ID);
		usbCdcConfigured=false;
	}
}

/**************************************************************************//**
 * @brief Callback function called whenever a new packet with data is received
 *        on USB.
 *
 * @param[in] status    Transfer status code.
 * @param[in] xferred   Number of bytes transferred.
 * @param[in] remaining Number of bytes not transferred.
 *
 * @return USB_STATUS_OK.
 *****************************************************************************/
static int usbDataReceived(USB_Status_TypeDef status, uint32_t xferred,
		uint32_t remaining) {
	(void) remaining; /* Unused parameter */
	if ((status == USB_STATUS_OK) && (xferred > 0)) {
		usbRxTransferActive = false;
		usbBytesReceived = xferred;
		BSP_LedToggle(0);
	}
	return USB_STATUS_OK;
}

/**************************************************************************//**
 * @brief Callback function called whenever a packet with data has been
 *        transmitted on USB
 *
 * @param[in] status    Transfer status code.
 * @param[in] xferred   Number of bytes transferred.
 * @param[in] remaining Number of bytes not transferred.
 *
 * @return USB_STATUS_OK.
 *****************************************************************************/
static int usbDataTransmitted(USB_Status_TypeDef status, uint32_t xferred,
		uint32_t remaining) {
	(void) xferred; /* Unused parameter */
	(void) remaining; /* Unused parameter */
	if (status == USB_STATUS_OK) {
		usbTxTransferActive = false;
		BSP_LedToggle(1);
	}
	return USB_STATUS_OK;
}

/**************************************************************************//**
 * @brief Get an packet from USB with optional timeout.
 *
 * @param[in] buf
 *   Pointer to data storage space. Memory space pointed is ended with '\0'
 *
 * @param[in] msTimeout
 *   Transmission timeout in milliseconds, no timeout if zero.
 *
 * @return
 *   True if a transmission took place.
 *****************************************************************************/

bool usbReadData(uint8_t *buf, int msTimeout) {

	if (!buf||usbRxTransferActive)
		return false;
	usbRxTransferActive = true;
	USBD_Read(CDC_EP_DATA_OUT, (void*) usbRxBuffer,CDC_USB_RX_BUF_SIZE,
			usbDataReceived);
	if (msTimeout > 0) {

		while (usbRxTransferActive && msTimeout>0) {
			USBTIMER_DelayMs(1);
			msTimeout--;
		}

		if (msTimeout <= 0) {
			USBD_AbortTransfer(CDC_EP_DATA_OUT);
			return false;
		}
	}

	else {
		while (usbRxTransferActive)
			; //wait for data without timer

	}
	usbRxBuffer[usbBytesReceived]='\0';
	memcpy(buf, usbRxBuffer, usbBytesReceived+1);
	return true;
}

/**************************************************************************//**
 * @brief Send an packet to USB port.
 *
 * @param[in] data
 *   Pointer to data buffer.
 *
 * @param[in] len
 *   length of sent data.
 *
 * @return
 *   True if a transmission took place.
 *****************************************************************************/

bool usbWriteData(uint8_t *data, uint8_t len) {

	if (!data||!len||usbTxTransferActive)
		return false;
	if (len > CDC_USB_TX_BUF_SIZE) {
		len = CDC_USB_TX_BUF_SIZE;
	}
	memcpy(usbTxBuffer, data, len);
	usbTxTransferActive = true;
	USBD_Write(CDC_EP_DATA_IN, usbTxBuffer, len, usbDataTransmitted);
	while (usbTxTransferActive)
		;
	return true;
}

/**************************************************************************//**
 * @brief
 *   Callback function called when the data stage of a CDC_SET_LINECODING
 *   setup command has completed.
 *
 * @param[in] status    Transfer status code.
 * @param[in] xferred   Number of bytes transferred.
 * @param[in] remaining Number of bytes not transferred.
 *
 * @return USB_STATUS_OK if data accepted.
 *         USB_STATUS_REQ_ERR if data calls for modes we can not support.
 *****************************************************************************/
static int LineCodingReceived(USB_Status_TypeDef status, uint32_t xferred,
		uint32_t remaining) {
	uint32_t frame = 0;
	(void) remaining;

	/* We have received new serial port communication settings from USB host */
	if ((status == USB_STATUS_OK) && (xferred == 7)) {
		/* Check bDataBits, valid values are: 5, 6, 7, 8 or 16 bits */
		if (cdcLineCoding.bDataBits == 5)
			frame |= USART_FRAME_DATABITS_FIVE;

		else if (cdcLineCoding.bDataBits == 6)
			frame |= USART_FRAME_DATABITS_SIX;

		else if (cdcLineCoding.bDataBits == 7)
			frame |= USART_FRAME_DATABITS_SEVEN;

		else if (cdcLineCoding.bDataBits == 8)
			frame |= USART_FRAME_DATABITS_EIGHT;

		else if (cdcLineCoding.bDataBits == 16)
			frame |= USART_FRAME_DATABITS_SIXTEEN;

		else
			return USB_STATUS_REQ_ERR;

		/* Check bParityType, valid values are: 0=None 1=Odd 2=Even 3=Mark 4=Space  */
		if (cdcLineCoding.bParityType == 0)
			frame |= USART_FRAME_PARITY_NONE;

		else if (cdcLineCoding.bParityType == 1)
			frame |= USART_FRAME_PARITY_ODD;

		else if (cdcLineCoding.bParityType == 2)
			frame |= USART_FRAME_PARITY_EVEN;

		else if (cdcLineCoding.bParityType == 3)
			return USB_STATUS_REQ_ERR;

		else if (cdcLineCoding.bParityType == 4)
			return USB_STATUS_REQ_ERR;

		else
			return USB_STATUS_REQ_ERR;

		/* Check bCharFormat, valid values are: 0=1 1=1.5 2=2 stop bits */
		if (cdcLineCoding.bCharFormat == 0)
			frame |= USART_FRAME_STOPBITS_ONE;

		else if (cdcLineCoding.bCharFormat == 1)
			frame |= USART_FRAME_STOPBITS_ONEANDAHALF;

		else if (cdcLineCoding.bCharFormat == 2)
			frame |= USART_FRAME_STOPBITS_TWO;

		else
			return USB_STATUS_REQ_ERR;

		return USB_STATUS_OK;
	}
	return USB_STATUS_REQ_ERR;
}

