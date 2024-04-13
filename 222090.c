static int usb_host_release_interfaces(USBHostDevice *s)
{
    int ret, i;

    dprintf("husb: releasing interfaces\n");

    for (i = 0; i < s->ninterfaces; i++) {
        ret = ioctl(s->fd, USBDEVFS_RELEASEINTERFACE, &i);
        if (ret < 0) {
            perror("husb: failed to release interface");
            return 0;
        }
    }

    return 1;
}