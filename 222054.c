static void usb_host_handle_destroy(USBDevice *dev)
{
    USBHostDevice *s = (USBHostDevice *)dev;

    s->closing = 1;

    qemu_set_fd_handler(s->fd, NULL, NULL, NULL);

    hostdev_unlink(s);

    async_complete(s);

    if (s->fd >= 0)
        close(s->fd);

    qemu_free(s);
}