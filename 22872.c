static char *composite_default_mfr(struct usb_gadget *gadget)
{
	return kasprintf(GFP_KERNEL, "%s %s with %s", init_utsname()->sysname,
			 init_utsname()->release, gadget->name);
}