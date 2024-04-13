static int usb_host_set_interface(USBHostDevice *s, int iface, int alt)
{
    struct usbdevfs_setinterface si;
    int ret;

    si.interface  = iface;
    si.altsetting = alt;
    ret = ioctl(s->fd, USBDEVFS_SETINTERFACE, &si);
    
    dprintf("husb: ctrl set iface %d altset %d ret %d errno %d\n", 
    	iface, alt, ret, errno);
    
    if (ret < 0)
        return ctrl_error();

    usb_linux_update_endp_table(s);
    return 0;
}