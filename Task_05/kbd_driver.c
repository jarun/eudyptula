#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");

static struct usb_device_id kbd_table[] =
{
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
			USB_INTERFACE_SUBCLASS_BOOT,
			USB_INTERFACE_PROTOCOL_KEYBOARD) },
	{}
};

MODULE_DEVICE_TABLE (usb, kbd_table);

static int __init kbd_init(void)
{
	pr_info("kbd_init called.\n");
	return 0;
}

static void __exit kbd_exit(void)
{
	pr_info("kbd_exit called.\n");
	return;
}

module_init(kbd_init);
module_exit(kbd_exit);
