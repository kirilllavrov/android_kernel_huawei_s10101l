/*
 * linux/driver/usb/serial/hi_usb_ser.c
 *
 * Copyright (c) 2011 Hisi technology corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation;version 2 of the License.
 *
 * NOTE:Only use this driver for devices that provide AT/Diag port and all other generic serials.
 * and CDC-ACM port should probably be driven by option.ko.
 */

#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/slab.h>
#include "usb-wwan.h"

#define DRIVER_AUTHOR "Hisi Inc"
#define DRIVER_DESC "Hisi USB Serial driver"

#ifdef CONFIG_LTE_RMNET
#define MAX_PORT_NUMBER     16
#endif

static const struct usb_device_id id_table[] = {
	{USB_DEVICE(0x058b, 0x0041)},	/*  */
    {USB_DEVICE(0x12d1, 0x1506)},   
	{ }				/* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, id_table);

static struct usb_driver hi_gs_driver = {
	.name			= "hi_gs",
	.probe			= usb_serial_probe,
	.disconnect		= usb_serial_disconnect,
	.id_table		= id_table,
	.suspend		= usb_serial_suspend,
	.resume			= usb_serial_resume,
	.supports_autosuspend	= true,
};

#ifdef CONFIG_LTE_RMNET
static struct usb_wwan_port_param hi_gs_port_param[MAX_PORT_NUMBER] = {
	[1] = { 7, 65536, 32, 2048 },	/* For high-speed data port, need more buffer */
};
#endif

static int hi_gs_probe(struct usb_serial *serial, const struct usb_device_id *id)
{
	struct usb_wwan_intf_private *data;


	data = serial->private = kzalloc(sizeof(struct usb_wwan_intf_private),
					 GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	spin_lock_init(&data->susp_lock);
	return 0;
}

#ifdef CONFIG_LTE_RMNET
static int hi_gs_attach(struct usb_serial *serial)
{
	struct usb_wwan_intf_private *wwan_priv =
		(struct usb_wwan_intf_private *)serial->private;

	if (serial->minor < MAX_PORT_NUMBER)
		wwan_priv->port_para = hi_gs_port_param[serial->minor];
	else
		dev_err(&serial->dev->dev, "Serial minor(%d) is invalid.", serial->minor);

	usb_wwan_startup(serial);
}
#endif

//set bulk size to 8k to improve download modem image speed
#ifdef CONFIG_BALONG_FLASHLESS_POWER
 #define USB_BULK_OUT_SIZE   (8*1024)
#endif

static struct usb_serial_driver hi_gs_device = {
	.driver = {
		.owner          = THIS_MODULE,
		.name          = "hi_gs",
	},
	.description          = "Hisi USB generic serial",
	.id_table              = id_table,
	.usb_driver          = &hi_gs_driver,
	.num_ports          = 1,
	.probe                 = hi_gs_probe,
	.open		     = usb_wwan_open,
	.close		     = usb_wwan_close,
	.write		     = usb_wwan_write,
	.write_room	     = usb_wwan_write_room,
	.chars_in_buffer     = usb_wwan_chars_in_buffer,
#ifdef CONFIG_LTE_RMNET
	.attach		     = hi_gs_attach,
#else
	.attach		     = usb_wwan_startup,
#endif
	.disconnect	     = usb_wwan_disconnect,
	.release	            = usb_wwan_release,
#ifdef CONFIG_BALONG_FLASHLESS_POWER
	.bulk_out_size = USB_BULK_OUT_SIZE,
#endif

#ifdef CONFIG_PM
	.suspend	            = usb_wwan_suspend,
	.resume		     = usb_wwan_resume,
#endif
};

static int __init hi_gs_init(void)
{
	int retval;
	retval = usb_serial_register(&hi_gs_device);
	if (retval)
		return retval;

	retval = usb_register(&hi_gs_driver);
	if (retval) {
		usb_serial_deregister(&hi_gs_device);
		return retval;
	}

	return 0;
}

static void __exit hi_gs_exit(void)
{
	usb_deregister(&hi_gs_driver);
	usb_serial_deregister(&hi_gs_device);
}

module_init(hi_gs_init);
module_exit(hi_gs_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL v2");

