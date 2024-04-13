next_desc(struct usb_descriptor_header **t, u8 desc_type)
{
	for (; *t; t++) {
		if ((*t)->bDescriptorType == desc_type)
			return t;
	}
	return NULL;
}