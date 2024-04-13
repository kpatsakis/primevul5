static int usb_host_set_config(USBHostDevice *s, int config)
{
    usb_host_release_interfaces(s);

    int ret = ioctl(s->fd, USBDEVFS_SETCONFIGURATION, &config);
 
    dprintf("husb: ctrl set config %d ret %d errno %d\n", config, ret, errno);
    
    if (ret < 0)
        return ctrl_error();
 
    usb_host_claim_interfaces(s, config);
    return 0;
}