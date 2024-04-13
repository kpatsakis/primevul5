static struct usb_gadget_strings **get_containers_gs(
		struct usb_gadget_string_container *uc)
{
	return (struct usb_gadget_strings **)uc->stash;
}