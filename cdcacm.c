/*
 * This file is part of the MSX Keyboard Subsystem Emulator project.
 *
 * Copyright (C) 2022 Evandro Souza <evandro.r.souza@gmail.com>
 *
 * Based on CDCACM example of:
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include "system.h"
#include "cdcacm.h"
#include "hr_timer.h"
#include "serial_no.h"


//Global variables
#if USE_USB == true
usbd_device *usb_dev;
int usb_configured;
//static int cdcacm_gdb_dtr = 1;
bool nak_cleared[6];
uint8_t usbd_control_buffer[2 * USBD_DATA_BUFFER_SIZE];	// Buffer to be used for control requests.
#endif	//#if USE_USB == true
extern struct sring uart_tx_ring;							//Declared on serial.c
extern struct sring uart_rx_ring;							//Declared on serial.c
extern struct sring con_tx_ring;							//Declared on serial.c
extern struct sring con_rx_ring;							//Declared on serial.c
extern char serial_no[LEN_SERIAL_No + 1];			//Declared as uint8_t on serial_no.c


#if USE_USB == true	
static const char *usb_strings[] = {
	"Evandro Rodrigues de Souza Technologies",
	"MSX keyboard subsystem emulator",
	serial_no,
	"Emulator Equipment Console",               //  Console Port
	"Emulator Equipment Console ACM Port",      //  Console ACM Port
	"Emulator Equipment Console DataPort",      //  Console DATA Port
	"Emulator Equipment USB <-> Serial",        //  Serial Port
	"Emulator Equipment USB-Serial ACM Port",   //  Serial ACM Port
	"Emulator Equipment USB-Serial DataPort",   //  Serial DATA Port
};

#define NUM_STRINGS (sizeof(usb_strings) / sizeof(usb_strings[0]))

enum usb_strings_index {  //  Index of USB strings.  Must sync with *usb_strings[], starts from 1.
	USB_STRINGS_MANUFACTURER = 1,
	USB_STRINGS_PRODUCT,
	USB_STRINGS_SERIAL_NUMBER,
	USB_STRINGS_CONSOLE,
	USB_STRINGS_CON_COMM,
	USB_STRINGS_CON_DATA,
	USB_STRINGS_UART,
	USB_STRINGS_UART_COMM,
	USB_STRINGS_UART_DATA,
};

static const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,							//  18: Length of this descriptor.
	.bDescriptorType = USB_DT_DEVICE,						//  This is a Device Descriptor
#if USB21_INTERFACE == true
	.bcdUSB = 0x0210,  //  USB Version 2.1.  Need to handle special requests e.g. BOS.
#else
	.bcdUSB = 0x0200,  //  USB Version 2.0.  No need to handle special requests e.g. BOS.
#endif  //  #if USB21_INTERFACE == true
#if (CDC_ONLY_ON_USB == true)
	.bDeviceClass = USB_CLASS_CDC,							//0x02 Set the class to CDC if it's only serial.  Serial interface will not start on Windows when class = 0.
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
#else	//#if (CDC_ONLY_ON_USB == true)
	.bDeviceClass = USB_CLASS_MISCELLANEOUS,		//0xEF Set the class to CDC to miscellaneous.
	.bDeviceSubClass = 2,												//0xEF, 0x02, 0x01 is Interface Association Descriptor
	.bDeviceProtocol = 1,												//0xEF, 0x02, 0x01 is Interface Association Descriptor
#endif	//#if (CDC_ONLY_ON_USB == true)
	.bMaxPacketSize0 = USBD_DATA_BUFFER_SIZE,		//  USB packet size (64)
	.idVendor = USB_VID,												//  Official USB Vendor ID
	.idProduct = USB_PID,												//  Official USB Product ID
	.bcdDevice = 0x0100,												//  Device Release number 1.0
	.iManufacturer = USB_STRINGS_MANUFACTURER,	//  Name of manufacturer (index of string descriptor)
	.iProduct = USB_STRINGS_PRODUCT,						//  Name of product (index of string descriptor)
	.iSerialNumber = USB_STRINGS_SERIAL_NUMBER,	//  Serial number (index of string descriptor)
	.bNumConfigurations = 1,										//  How many configurations we support
};

/*
 * This notification endpoint isn't implemented. According to CDC spec its
 * optional, but its absence causes a NULL pointer dereference in Linux
 * cdc_acm driver.
 */
// Console ACM interface
//  CON Endpoint Descriptors
static const struct usb_endpoint_descriptor con_comm_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,						//7 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_ENDPOINT,					//5 (as defined in usbstd.h)
	.bEndpointAddress = EP_CON_COMM,						//0x82 Console USB IN Control Endpoint//EP_CON_COMM=0x83 originally
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,//0x03 (as defined in usbstd.h)
	.wMaxPacketSize = COMM_PACKET_SIZE,  				//16 - Smaller than others
	.bInterval = 255,
}};

static const struct usb_endpoint_descriptor con_data_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,						//7 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_ENDPOINT,					//5 (as defined in usbstd.h)
	.bEndpointAddress = EP_CON_DATA_OUT,				//0x01 Console USB OUT Data Endpoint//EP_CON_DATA_OUT=0x01 originally
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,			//0x02 (as defined in usbstd.h)
	.wMaxPacketSize = USBD_DATA_BUFFER_SIZE,		//64
	.bInterval = 1,
}, {
	.bLength = USB_DT_ENDPOINT_SIZE,						//7 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_ENDPOINT,					//5 (as defined in usbstd.h)
	.bEndpointAddress = EP_CON_DATA_IN,					//0x01 Console USB OUT Data Endpoint//EP_CON_DATA_IN=0x82 originally
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,			//0x02 (as defined in usbstd.h)
	.wMaxPacketSize = USBD_DATA_BUFFER_SIZE,		//64
	.bInterval = 1,
}};

//  CON Functional Descriptor
static const struct {
	struct usb_cdc_header_descriptor header;
	struct usb_cdc_call_management_descriptor call_mgmt;
	struct usb_cdc_acm_descriptor acm;
	struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed)) con_cdcacm_functional_descriptors = {
	.header = {
		.bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
		.bDescriptorType = CS_INTERFACE,					//0x24 (as defined in cdc.h)
		.bDescriptorSubtype = USB_CDC_TYPE_HEADER,//0x00 (as defined in cdc.h)
		.bcdCDC = 0x0110,
	},
	.call_mgmt = {
		.bFunctionLength =
			sizeof(struct usb_cdc_call_management_descriptor),
		.bDescriptorType = CS_INTERFACE,					//0x24 (as defined in cdc.h)
		.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,//0x01 (as defined in cdc.h)
		.bmCapabilities = 0,
		.bDataInterface = INTF_CON_DATA,  				//  DATA Interface
	},
	.acm = {
		.bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
		.bDescriptorType = CS_INTERFACE,					//0x24 (as defined in cdc.h)
		.bDescriptorSubtype = USB_CDC_TYPE_ACM,		//0x02 (as defined in cdc.h)
		.bmCapabilities = 2,											// SET_LINE_CODING supported (BMP uses for both GDB and uart)
	},
	.cdc_union = {
		.bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
		.bDescriptorType = CS_INTERFACE,					//0x24 (as defined in cdc.h)
		.bDescriptorSubtype = USB_CDC_TYPE_UNION,	//0x06 (as defined in cdc.h)
		.bControlInterface = INTF_CON_COMM,       //  COMM Interface
		.bSubordinateInterface0 = INTF_CON_DATA,  //  DATA Interface
	 },
};

//  CON Interface Descriptor (Comm)
static const struct usb_interface_descriptor con_comm_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,						//9 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_INTERFACE,				//4 (as defined in usbstd.h)
	.bInterfaceNumber = INTF_CON_COMM,  				//  CDC ACM interface ID is 0
	.bAlternateSetting = 0,											//No alternate setting
	.bNumEndpoints = 1,													//1 EP: CDC ACM Endpoint
	.bInterfaceClass = USB_CLASS_CDC,						//0x02 (as defined in cdc.h)
	.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,	//0x02 (as defined in cdc.h)
	.bInterfaceProtocol = USB_CDC_PROTOCOL_AT,	//0x01 (as defined in cdc.h)
	.iInterface = USB_STRINGS_CON_COMM,       	//  Name of CDC ACM interface (index of string descriptor)

	.endpoint = con_comm_endp,

	.extra = &con_cdcacm_functional_descriptors,
	.extralen = sizeof(con_cdcacm_functional_descriptors),
}};

//  CON Interface Descriptor (Data)
static const struct usb_interface_descriptor con_data_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,						//9 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_INTERFACE,				//4 (as defined in usbstd.h)
	.bInterfaceNumber = INTF_CON_DATA,  				//  CDC ACM interface ID is 1
	.bAlternateSetting = 0,											//No alternate setting
	.bNumEndpoints = 2,													//2 EP's: Data OUT (for received data) and Data IN (for sent data)
	.bInterfaceClass = USB_CLASS_DATA,					//0x0A (as defined in cdc.h)
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = USB_STRINGS_CON_DATA,	      	//  Name of CDC ACM interface (index of string descriptor)

	.endpoint = con_data_endp,  								//  CDC ACM Endpoint
}};

#if !(CDC_ONLY_ON_USB == true)
//  CON Interface Association Descriptor - CDC Interfaces
static const struct usb_iface_assoc_descriptor con_iface_assoc = {  //  Copied from BMP.  Mandatory for composite device.
	.bLength = USB_DT_INTERFACE_ASSOCIATION_SIZE,//8 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_INTERFACE_ASSOCIATION,//11 (as defined in usbstd.h)
	.bFirstInterface = INTF_CON_COMM, //  First associated interface (INTF_CON_COMM and INTF_CON_DATA)
	.bInterfaceCount = 2,          		//  Total number of associated interfaces (INTF_CON_COMM and INTF_CON_DATA), ID must be consecutive.
	.bFunctionClass = USB_CLASS_CDC,						//0x02 This is a USB CDC (Comms Device Class) interface
	.bFunctionSubClass = USB_CDC_SUBCLASS_ACM,	//0x02 That implements ACM (Abstract Control Model)
	.bFunctionProtocol = USB_CDC_PROTOCOL_AT,		//0x01 Using the AT protocol
	.iFunction = USB_STRINGS_CONSOLE, 					//  Name of Console Port
};
#endif	//#if (CDC_ONLY_ON_USB == true)


// UART ACM interface
//  UART Endpoint Descriptors
static const struct usb_endpoint_descriptor uart_comm_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,						//7 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_ENDPOINT,					//5 (as defined in usbstd.h)
	.bEndpointAddress = EP_UART_COMM,						//0x84 UART USB Comm Endpoint
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,//0x03 (as defined in usbstd.h)
	.wMaxPacketSize = COMM_PACKET_SIZE,  				// Smaller than others: 16
	.bInterval = 255,
} };

static const struct usb_endpoint_descriptor uart_data_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,						//7 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_ENDPOINT,					//5 (as defined in usbstd.h)
	.bEndpointAddress = EP_UART_DATA_OUT,				//3 UART USB OUT Data Endpoint
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,			//0x02 (as defined in usbstd.h)
	.wMaxPacketSize = USBD_DATA_BUFFER_SIZE,		//64
	.bInterval = 1,
}, {
	.bLength = USB_DT_ENDPOINT_SIZE,						//7 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_ENDPOINT,					//5 (as defined in usbstd.h)
	.bEndpointAddress = EP_UART_DATA_IN,				//0x83 UART USB IN Data Endpoint
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,			//0x02 (as defined in usbstd.h)
	.wMaxPacketSize = USBD_DATA_BUFFER_SIZE,		//64
	.bInterval = 1,
} };

//  UART Functional Descriptor
static const struct {
	struct usb_cdc_header_descriptor header;
	struct usb_cdc_call_management_descriptor call_mgmt;
	struct usb_cdc_acm_descriptor acm;
	struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed)) uart_cdcacm_functional_descriptors = {
	.header = {
		.bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
		.bDescriptorType = CS_INTERFACE,					//0x24 (as defined in cdc.h)
		.bDescriptorSubtype = USB_CDC_TYPE_HEADER,//0x00 (as defined in cdc.h)
		.bcdCDC = 0x0110,
	},
	.call_mgmt = {
		.bFunctionLength = sizeof(struct usb_cdc_call_management_descriptor),
		.bDescriptorType = CS_INTERFACE,					//0x24 (as defined in cdc.h)
		.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,//0x01 (as defined in cdc.h)
		.bmCapabilities = 0,
		.bDataInterface = INTF_UART_DATA,  				//  DATA Interface
	},
	.acm = {
		.bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
		.bDescriptorType = CS_INTERFACE,					//0x24 (as defined in cdc.h)
		.bDescriptorSubtype = USB_CDC_TYPE_ACM,		//0x02 (as defined in cdc.h)
		.bmCapabilities = 2,											// SET_LINE_CODING supported (BMP uses for both GDB and uart)
	},
	.cdc_union = {
		.bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
		.bDescriptorType = CS_INTERFACE,					//0x24 (as defined in cdc.h)
		.bDescriptorSubtype = USB_CDC_TYPE_UNION,	//0x06 (as defined in cdc.h)
		.bControlInterface = INTF_UART_COMM,      //  COMM Interface
		.bSubordinateInterface0 = INTF_UART_DATA, //  DATA Interface
	 }
};


//  UART Interface Descriptor (comm)
static const struct usb_interface_descriptor uart_comm_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,						//9 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_INTERFACE,				//4 (as defined in usbstd.h)
	.bInterfaceNumber = INTF_UART_COMM,  				//  CDC ACM interface ID is 2
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_CDC,						//0x02 (as defined in cdc.h)
	.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,	//0x02 (as defined in cdc.h)
	.bInterfaceProtocol = USB_CDC_PROTOCOL_AT,	//0x01 (as defined in cdc.h)
	.iInterface = USB_STRINGS_UART_COMM,       	//  Name of CDC ACM interface (index of string descriptor)

	.endpoint = uart_comm_endp,
	.extra = &uart_cdcacm_functional_descriptors,
	.extralen = sizeof(uart_cdcacm_functional_descriptors),
} };

//  UART Interface Descriptor (data)
static const struct usb_interface_descriptor uart_data_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,						//9 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_INTERFACE,				//4 (as defined in usbstd.h)
	.bInterfaceNumber = INTF_UART_DATA,  				//  CDC ACM interface ID is 3
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_DATA,					//0x0A (as defined in cdc.h)
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = USB_STRINGS_UART_DATA,	      //  Name of CDC ACM interface (index of string descriptor)

	.endpoint = uart_data_endp,  								//  CDC ACM Endpoint
} };

#if !(CDC_ONLY_ON_USB == true)
//  UART Interface Association Descriptor - CDC Interfaces
static const struct usb_iface_assoc_descriptor uart_iface_assoc = //  Copied from BMP.  Mandatory for composite device.
{
	.bLength = USB_DT_INTERFACE_ASSOCIATION_SIZE,//8 (as defined in usbstd.h)
	.bDescriptorType = USB_DT_INTERFACE_ASSOCIATION,//11 (as defined in usbstd.h)
	.bFirstInterface = INTF_UART_COMM,//  First associated interface (INTF_UART_COMM and INTF_UART_DATA)
	.bInterfaceCount = 2,          		//  Total number of associated interfaces (INTF_UART_COMM and INTF_UART_DATA), ID must be consecutive.
	.bFunctionClass = USB_CLASS_CDC,						//0x02 This is a USB CDC (Comms Device Class) interface
	.bFunctionSubClass = USB_CDC_SUBCLASS_ACM,	//0x02 That implements ACM (Abstract Control Model)
	.bFunctionProtocol = USB_CDC_PROTOCOL_AT,		//0x01 Using the AT protocol
	.iFunction = USB_STRINGS_UART,			  			//  Name of Serial Port
};
#endif	//#if (CDC_ONLY_ON_USB == true)


// Both Console and UART ACM interfaces
//  USB Configuration Descriptor (Both interfaces)
static const struct usb_interface interfaces[] = {
{
	.num_altsetting = 1,
#if !(CDC_ONLY_ON_USB == true)
	.iface_assoc = &con_iface_assoc,						//Mandatory for composite device with multiple interfaces. static const struct usb_iface_assoc_descriptor con_iface_assoc
#endif	//#if !(CDC_ONLY_ON_USB == true)
	.altsetting = con_comm_iface,								//Index must sync with INTF_CON_COMM.
}, {
	.num_altsetting = 1,
	.altsetting = con_data_iface,  							//Index must sync with INTF_CON_DATA.
},
{
	.num_altsetting = 1,
#if !(CDC_ONLY_ON_USB == true)
	.iface_assoc = &uart_iface_assoc,						//Mandatory for composite device with multiple interfaces. static const struct usb_iface_assoc_descriptor uart_iface_assoc
#endif	//#if !(CDC_ONLY_ON_USB == true)
	.altsetting = uart_comm_iface,							//Index must sync with INTF_UART_COMM.
}, {
	.num_altsetting = 1,
	.altsetting = uart_data_iface,  						//Index must sync with INTF_UART_DATA.
},};


static const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = sizeof(interfaces)    /	\
										sizeof(interfaces[0]),		//  We will have 4 interfaces
	.bConfigurationValue = 1,										//  This is the configuration ID 1
	.iConfiguration = 0,												//  Configuration string (0 means none)
	.bmAttributes = USB_CONFIG_ATTR_DEFAULT |	\
									USB_CONFIG_ATTR_SELF_POWERED,//  Self-powered: it doesn't draw power from USB bus.
	.bMaxPower = 5,															//  Specifies how much bus current a device requires: 10 mA. (2 x .bMaxPower)

	.interface = interfaces,										//  List of all interfaces
};



static enum usbd_request_return_codes cdcacm_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf,
		uint16_t *len, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	switch (req->bRequest) {
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
		/*
		 * This Linux cdc_acm driver requires this to be implemented
		 * even though it's optional in the CDC spec, and we don't
		 * advertise it in the ACM functional descriptor.
		 */
		char local_buf[10];
		struct usb_cdc_notification *notif = (void *)local_buf;

		/* We echo signals back to host as notification. */
		notif->bmRequestType = 0xA1;
		notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
		notif->wValue = 0;
		notif->wIndex = 0;
		notif->wLength = 2;
		local_buf[8] = req->wValue & 3;
		local_buf[9] = 0;
		// usbd_ep_write_packet(EP_CON_COMM, buf, sizeof(local_buf));
		return USBD_REQ_HANDLED;
		}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding))
			return USBD_REQ_NOTSUPP;
		return USBD_REQ_HANDLED;
	}
	return USBD_REQ_NOTSUPP;
}


static void cdcacm_con_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)usbd_dev;
	(void)ep;
	char buf[USBD_DATA_BUFFER_SIZE];
	uint16_t len, result = 0;

	len = (uint16_t)usbd_ep_read_packet(usbd_dev, EP_CON_DATA_OUT, buf, USBD_DATA_BUFFER_SIZE);
	if (len) 
	{
		for(uint16_t i = 0; (i < len) && (result != 0xFFFF); i++)
			while( (result = ring_put_ch(&con_rx_ring, buf[i])) == 0xFFFF) __asm("nop");
		if(result > X_OFF_TRIGGER)
		{
			//Put EP in nak. On con_rx_ring read, check con_rx_ring room (X_ON_TRIGGER) to clear nak.
			nak_cleared[EP_CON_DATA_OUT] = false;
			usbd_ep_nak_set(usbd_dev, EP_CON_DATA_OUT, 1);
		}
	}
}


static void cdcacm_con_data_tx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)usbd_dev;
	(void)ep;
	char buf[USBD_DATA_BUFFER_SIZE];
	uint16_t i, len, max_transf, qty_accepted, local_getptr;

	len = (SERIAL_RING_BUFFER_SIZE - con_tx_ring.get_ptr + con_tx_ring.put_ptr) & (SERIAL_RING_BUFFER_SIZE - 1);//(&con_tx_ring);
	if(len)
	{
		max_transf = (len > (USBD_DATA_BUFFER_SIZE - 1)) ? (USBD_DATA_BUFFER_SIZE - 1) : len;
		local_getptr = con_tx_ring.get_ptr;
		for(i = 0; i < max_transf; i++)
		{
			buf[i] = con_tx_ring.data[local_getptr++];
			local_getptr &= (SERIAL_RING_BUFFER_SIZE - 1);
		}
		qty_accepted = usbd_ep_write_packet(usb_dev, EP_CON_DATA_IN, buf, max_transf);
		//This following two passes warranties that con_tx_ring.get_ptr to be an atomic update.
		con_tx_ring.get_ptr = (con_tx_ring.get_ptr + qty_accepted) & (SERIAL_RING_BUFFER_SIZE - 1);
		len -= qty_accepted;
	}
}


static void cdcacm_uart_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)usbd_dev;
	(void)ep;
	char buf[USBD_DATA_BUFFER_SIZE + 1];
	uint16_t len, result = 0;

	len = (uint16_t)usbd_ep_read_packet(usbd_dev, EP_UART_DATA_OUT, buf, USBD_DATA_BUFFER_SIZE);
	if (len) {
		for(uint16_t i = 0; (i < len) && (result != 0xFFFF); i++)
			while( (result = uart_tx_ring_dma_send_ch(buf[i])) == 0xFFFF) __asm("nop");
		if(result > X_OFF_TRIGGER)
		{
			//Put EP in nak. On uart_tx_ring read, check uart_rx_ring room (X_ON_TRIGGER) to clear nak.
			nak_cleared[EP_UART_DATA_OUT] = false;
			usbd_ep_nak_set(usbd_dev, EP_UART_DATA_OUT, 1);
		}	//if(result > X_OFF_TRIGGER)
	}	//if (len)
}


static void cdcacm_uart_data_tx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)usbd_dev;
	(void)ep;
	char buf[USBD_DATA_BUFFER_SIZE];
	uint16_t i, len, max_transf, qty_accepted, local_getptr;

	len = (SERIAL_RING_BUFFER_SIZE - uart_rx_ring.get_ptr + uart_rx_ring.put_ptr) & (SERIAL_RING_BUFFER_SIZE - 1);//(&con_tx_ring);
	if(len)
	{
		max_transf = (len > (USBD_DATA_BUFFER_SIZE - 1)) ? (USBD_DATA_BUFFER_SIZE - 1) : len;
		local_getptr = uart_rx_ring.get_ptr;
		for(i = 0; i < max_transf; i++)
		{
			buf[i] = uart_rx_ring.data[local_getptr++];
			local_getptr &= (SERIAL_RING_BUFFER_SIZE - 1);
		}
		qty_accepted = usbd_ep_write_packet(usb_dev, EP_UART_DATA_IN, buf, max_transf);
		//This following two passes warranties that uart_rx_ring.get_ptr to be an atomic update.
		uart_rx_ring.get_ptr = (uart_rx_ring.get_ptr + qty_accepted) & (SERIAL_RING_BUFFER_SIZE - 1);
	}
	else
		usbd_ep_nak_set(usbd_dev, EP_UART_DATA_IN, 1);
}


static void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	usb_configured = wValue;

	usbd_ep_setup(usbd_dev, EP_CON_COMM, USB_ENDPOINT_ATTR_INTERRUPT, COMM_PACKET_SIZE, NULL);
	usbd_ep_setup(usbd_dev, EP_CON_DATA_OUT, USB_ENDPOINT_ATTR_BULK, USBD_DATA_BUFFER_SIZE, cdcacm_con_data_rx_cb);
	usbd_ep_setup(usbd_dev, EP_CON_DATA_IN, USB_ENDPOINT_ATTR_BULK, USBD_DATA_BUFFER_SIZE, cdcacm_con_data_tx_cb);
	usbd_ep_setup(usbd_dev, EP_UART_COMM, USB_ENDPOINT_ATTR_INTERRUPT, COMM_PACKET_SIZE, NULL);
	usbd_ep_setup(usbd_dev, EP_UART_DATA_OUT, USB_ENDPOINT_ATTR_BULK, USBD_DATA_BUFFER_SIZE, cdcacm_uart_data_rx_cb);
	usbd_ep_setup(usbd_dev, EP_UART_DATA_IN, USB_ENDPOINT_ATTR_BULK, USBD_DATA_BUFFER_SIZE, cdcacm_uart_data_tx_cb);

	usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				cdcacm_control_request);
}


//It is used to start a communication pipe to allow the EP_??_DATA_IN callback continues send to host as needed.
//It returns ring->get_ptr updated.
void first_put_ring_content_onto_ep(struct sring *ring, uint8_t ep)
//uint16_t first_put_ring_content_onto_ep(struct sring ring, uint8_t ep)
{
	if(usb_configured)
	{
		char buf[USBD_DATA_BUFFER_SIZE];
		uint16_t i, len, max_transf, qty_accepted, local_getptr;

		len = (SERIAL_RING_BUFFER_SIZE - ring->get_ptr + ring->put_ptr) & (SERIAL_RING_BUFFER_SIZE - 1);//(&con_tx_ring);
		local_getptr = ring->get_ptr;
		if(len)
		{
			max_transf = (len > (USBD_DATA_BUFFER_SIZE - 1)) ? (USBD_DATA_BUFFER_SIZE - 1) : len;
			for(i = 0; i < max_transf; i++)
			{
				buf[i] = ring->data[local_getptr++];
				local_getptr &= (SERIAL_RING_BUFFER_SIZE - 1);
			}
			usbd_ep_nak_set(usb_dev, ep, 0);	//disable nak on ep
			qty_accepted = usbd_ep_write_packet(usb_dev, ep, buf, max_transf);
			//This following two passes warranties that ring->get_ptr to be an atomic update.
			ring->get_ptr = (ring->get_ptr + qty_accepted) & (SERIAL_RING_BUFFER_SIZE - 1);
		}
	}	//if(usb_configured)
}


static void usb_reset(void)
{
# if (MCU == STM32F103)
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_OPENDRAIN, GPIO12);
	gpio_clear(GPIOA, GPIO12);
	for (uint32_t i = 0; i < 0x500000; i++) __asm__("nop");
# endif	//# if (MCU == STM32F103)
# if (MCU == STM32F401)
# endif	//# if (MCU == STM32F401)
}


void set_nak_endpoint(uint8_t ep)
{
	usbd_ep_nak_set(usb_dev, ep, 1);
	nak_cleared[ep] = false;
}


void clear_nak_endpoint(uint8_t ep)
{
	usbd_ep_nak_set(usb_dev, ep, 0);
	nak_cleared[ep] = true;
}


void cdcacm_init(void)
{
	usb_reset();

	usb_dev = usbd_init(&USB_DRIVER, &dev, &config,
											usb_strings, NUM_STRINGS,
											usbd_control_buffer, sizeof(usbd_control_buffer));

#if MCU == STM32F401
	//Disable VBUS sensing => https://github.com/libopencm3/libopencm3/pull/1256#
	OTG_FS_GCCFG |= OTG_GCCFG_NOVBUSSENS | OTG_GCCFG_PWRDWN;
	OTG_FS_GCCFG &= ~(OTG_GCCFG_VBUSBSEN | OTG_GCCFG_VBUSASEN);
#if USART_PORT == USART1
	//Now, if STM32F401 and USART1, recover A9 and A10 back to serial 1:
	gpio_set_af(GPIO_BANK_USART1_TX, GPIO_AF7, GPIO_USART1_TX | GPIO_USART1_RX);
#endif	//#if USART_PORT == USART1
#endif	//#if MCU == STM32F401

	usbd_register_set_config_callback(usb_dev, cdcacm_set_config);

	nvic_set_priority(USB_NVIC, IRQ_PRI_USB);
	nvic_enable_irq(USB_NVIC);
}


/*************************************************************************************************/
/******************************************** ISR ************************************************/
/*************************************************************************************************/
USB_ISR
{
	usbd_poll(usb_dev);
}	//USB_ISR
#endif	//#if USE_USB == true


