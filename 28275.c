 int __usb_get_extra_descriptor(char *buffer, unsigned size,
			       unsigned char type, void **ptr)
 {
 	struct usb_descriptor_header *header;
 
 	while (size >= sizeof(struct usb_descriptor_header)) {
 		header = (struct usb_descriptor_header *)buffer;
 
		if (header->bLength < 2) {
 			printk(KERN_ERR
 				"%s: bogus descriptor, type %d length %d\n",
 				usbcore_name,
				header->bDescriptorType,
				header->bLength);
 			return -1;
 		}
 
		if (header->bDescriptorType == type) {
 			*ptr = header;
 			return 0;
 		}

		buffer += header->bLength;
		size -= header->bLength;
	}
	return -1;
}
